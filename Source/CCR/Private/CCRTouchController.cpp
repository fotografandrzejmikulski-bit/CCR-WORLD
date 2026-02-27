#include "CCRTouchController.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRCinematicSubsystem.h"
#include "CCRGameHUD.h"
#include "CCRHapticLibrarySubsystem.h"
#include "CCRQTEWidget.h"
#include "CCRTypes.h"
#include "Components/InputComponent.h"
#include "GameFramework/ForceFeedbackEffect.h"

ACCRTouchController::ACCRTouchController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACCRTouchController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindTouch(IE_Pressed,  this, &ACCRTouchController::HandleTouchBegin);
		InputComponent->BindTouch(IE_Released, this, &ACCRTouchController::HandleTouchEnd);
		InputComponent->BindAction("CCR_Pause", IE_Pressed, this, &ACCRTouchController::HandlePause);
	}
}

// ---------------------------------------------------------------------------
// HandlePause
// ---------------------------------------------------------------------------

void ACCRTouchController::HandlePause()
{
	if (ACCRGameHUD* HUD = Cast<ACCRGameHUD>(GetHUD()))
	{
		HUD->TogglePause();
	}
}

// ---------------------------------------------------------------------------
// HandleTouchBegin
// ---------------------------------------------------------------------------

void ACCRTouchController::HandleTouchBegin(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bQTEActive || bSwipeTracking) return;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode CurrentNode;
	if (!NRS->GetCurrentNode(CurrentNode)) return;

	// Tap during a Dialogue node advances the narrative (tap-to-continue).
	if (CurrentNode.NodeType == ECCRNodeType::Dialogue)
	{
		NRS->AdvanceDialogue();
		return;
	}

	// Tap during a skippable Cinematic node requests a skip.
	if (CurrentNode.NodeType == ECCRNodeType::Cinematic && CurrentNode.bSkippable)
	{
		if (UCCRCinematicSubsystem* Cinematic = GI->GetSubsystem<UCCRCinematicSubsystem>())
		{
			Cinematic->SkipCinematic();
		}
		return;
	}

	if (CurrentNode.NodeType != ECCRNodeType::QTE) return;

	TimeWindowSec = CurrentNode.TimeWindowSec;

	switch (CurrentNode.GestureType)
	{
	case ECCRGestureType::LongPress:
		StartQTE();
		break;

	case ECCRGestureType::Tap:
		// Single tap immediately succeeds
		EndQTE(true);
		break;

	case ECCRGestureType::Swipe:
		// Record start position; direction evaluated on TouchEnd
		bSwipeTracking = true;
		QTEHeldTime    = 0.f;
		RequiredSwipe  = CurrentNode.RequiredSwipeDir;
		TouchStartPos  = FVector2D(Location.X, Location.Y);
		// Show QTE widget immediately so player sees the prompt
		if (ACCRGameHUD* HUD = Cast<ACCRGameHUD>(GetHUD()))
		{
			HUD->SetQTEVisible(true);
			if (HUD->QTEWidget)
			{
				HUD->QTEWidget->OnQTEStarted(TimeWindowSec);
			}
		}
		break;
	}
}

// ---------------------------------------------------------------------------
// HandleTouchEnd
// ---------------------------------------------------------------------------

void ACCRTouchController::HandleTouchEnd(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bSwipeTracking)
	{
		bSwipeTracking = false;
		const FVector2D EndPos(Location.X, Location.Y);
		const bool bSuccess = EvaluateSwipe(TouchStartPos, EndPos);
		EndQTE(bSuccess);
		return;
	}

	if (!bQTEActive) return;

	// LongPress: released before time window elapsed = failure
	if (QTEHeldTime < TimeWindowSec)
	{
		EndQTE(false);
	}
}

// ---------------------------------------------------------------------------
// StartQTE (LongPress only)
// ---------------------------------------------------------------------------

void ACCRTouchController::StartQTE()
{
	bQTEActive          = true;
	QTEHeldTime         = 0.f;
	TimeSinceLastHaptic = 0.f;

	// Show the QTE widget and notify it of the time window before any progress updates
	if (ACCRGameHUD* HUD = Cast<ACCRGameHUD>(GetHUD()))
	{
		HUD->SetQTEVisible(true);
		if (HUD->QTEWidget)
		{
			HUD->QTEWidget->OnQTEStarted(TimeWindowSec);
		}
	}
}

// ---------------------------------------------------------------------------
// EndQTE
// ---------------------------------------------------------------------------

void ACCRTouchController::EndQTE(bool bSuccess)
{
	bQTEActive     = false;
	bSwipeTracking = false;
	QTEHeldTime    = 0.f;

	UpdateQTEWidget(bSuccess ? 1.f : 0.f, /*bVisible=*/false);

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	// Play the named haptic pattern for QTE outcome
	if (UCCRHapticLibrarySubsystem* Haptic = GI->GetSubsystem<UCCRHapticLibrarySubsystem>())
	{
		Haptic->PlayPatternOnController(
			bSuccess ? FName(TEXT("QTE_Success")) : FName(TEXT("QTE_Failure")),
			this);
	}

	if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		NRS->ResolveQTE(bSuccess);
	}
}

// ---------------------------------------------------------------------------
// EvaluateSwipe
// ---------------------------------------------------------------------------

bool ACCRTouchController::EvaluateSwipe(FVector2D Start, FVector2D End) const
{
	const FVector2D Delta = End - Start;
	const float Dist = Delta.Size();

	if (Dist < SwipeMinDistancePx)
	{
		// Too short to be a meaningful swipe
		return false;
	}

	if (RequiredSwipe == ECCRSwipeDirection::Any)
	{
		return true;
	}

	// Determine dominant axis and direction.
	// Ties (exactly 45°) are treated as horizontal; document this edge case
	// so designers know diagonal swipes at exactly 45° resolve as Left/Right.
	const bool bHorizontal = FMath::Abs(Delta.X) >= FMath::Abs(Delta.Y);

	switch (RequiredSwipe)
	{
	case ECCRSwipeDirection::Right: return  bHorizontal && Delta.X > 0.f;
	case ECCRSwipeDirection::Left:  return  bHorizontal && Delta.X < 0.f;
	// Y increases downward in screen space
	case ECCRSwipeDirection::Down:  return !bHorizontal && Delta.Y > 0.f;
	case ECCRSwipeDirection::Up:    return !bHorizontal && Delta.Y < 0.f;
	default:                        return true;
	}
}

// ---------------------------------------------------------------------------
// UpdateQTEWidget
// ---------------------------------------------------------------------------

void ACCRTouchController::UpdateQTEWidget(float Progress, bool bVisible)
{
	if (ACCRGameHUD* HUD = Cast<ACCRGameHUD>(GetHUD()))
	{
		HUD->SetQTEVisible(bVisible);
		if (HUD->QTEWidget)
		{
			if (bVisible)
			{
				// Called every haptic pulse interval to drive the progress arc/bar
				HUD->QTEWidget->UpdateProgress(Progress);
			}
			else
			{
				// QTE has ended; pass success state to widget for final animation
				HUD->QTEWidget->OnQTEEnded(Progress >= 1.f);
			}
		}
	}
}

// ---------------------------------------------------------------------------
// Tick
// ---------------------------------------------------------------------------

void ACCRTouchController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ---- Swipe tick (progress + timeout) ----
	// bSwipeTracking and bQTEActive are mutually exclusive: HandleTouchBegin
	// returns early when either is true, so only one path runs per frame.
	// Track elapsed time and push widget progress so the UI reflects the
	// remaining window. Fail the QTE if the player hasn't swiped in time.
	if (bSwipeTracking)
	{
		QTEHeldTime += DeltaTime;
		const float Progress = FMath::Clamp(QTEHeldTime / TimeWindowSec, 0.f, 1.f);
		UpdateQTEWidget(Progress, /*bVisible=*/true);
		if (QTEHeldTime >= TimeWindowSec)
		{
			bSwipeTracking = false;
			EndQTE(false);
		}
		return;
	}

	// ---- LongPress tick ----
	if (!bQTEActive) return;

	QTEHeldTime         += DeltaTime;
	TimeSinceLastHaptic += DeltaTime;

	// Pulse haptic feedback periodically while holding
	if (TimeSinceLastHaptic >= HapticPulseInterval)
	{
		TimeSinceLastHaptic = 0.f;

		// Scale intensity from 0.3 to 1.0 as player approaches TimeWindowSec
		const float Progress  = FMath::Clamp(QTEHeldTime / TimeWindowSec, 0.f, 1.f);
		const float Intensity = FMath::Lerp(0.3f, 1.0f, Progress);

		PlayDynamicForceFeedback(
			Intensity,
			HapticPulseInterval,
			/*bLooping=*/false,
			/*bAffectsLeftLarge=*/true,
			/*bAffectsLeftSmall=*/true,
			/*bAffectsRightLarge=*/true,
			/*bAffectsRightSmall=*/true);

		// Update widget progress
		UpdateQTEWidget(Progress, /*bVisible=*/true);
	}

	// Success when hold time reaches the window
	if (QTEHeldTime >= TimeWindowSec)
	{
		EndQTE(true);
	}
}

