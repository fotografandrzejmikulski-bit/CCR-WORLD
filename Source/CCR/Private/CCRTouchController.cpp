#include "CCRTouchController.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRGameHUD.h"
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
	}
}

void ACCRTouchController::HandleTouchBegin(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (bQTEActive) return;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	// Only start QTE if the current node is a QTE node
	FCCRNode CurrentNode;
	if (!NRS->GetCurrentNode(CurrentNode)) return;
	if (CurrentNode.NodeType != ECCRNodeType::QTE) return;

	// Sync window duration from the node definition
	TimeWindowSec = CurrentNode.TimeWindowSec;

	StartQTE();
}

void ACCRTouchController::HandleTouchEnd(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (!bQTEActive) return;

	// Released before time window elapsed = failure
	if (QTEHeldTime < TimeWindowSec)
	{
		EndQTE(false);
	}
}

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

void ACCRTouchController::EndQTE(bool bSuccess)
{
	bQTEActive  = false;
	QTEHeldTime = 0.f;

	UpdateQTEWidget(bSuccess ? 1.f : 0.f, /*bVisible=*/false);

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		NRS->ResolveQTE(bSuccess);
	}
}

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

void ACCRTouchController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
