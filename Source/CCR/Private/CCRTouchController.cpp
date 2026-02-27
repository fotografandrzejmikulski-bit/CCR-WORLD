#include "CCRTouchController.h"
#include "CCRNarrativeRuntimeSubsystem.h"
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

	// Only activate if the current narrative node is a LongPress QTE
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	// We need access to the current node to check its type.
	// The check is done via the subsystem's public accessors.
	// (The subsystem exposes GetCurrentNodeId; chunk lookup is internal.)
	// For now, we attempt QTE – the subsystem's ResolveQTE is a no-op if not in QTE state.
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
}

void ACCRTouchController::EndQTE(bool bSuccess)
{
	bQTEActive  = false;
	QTEHeldTime = 0.f;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		NRS->ResolveQTE(bSuccess);
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
	}

	// Success when hold time reaches the window
	if (QTEHeldTime >= TimeWindowSec)
	{
		EndQTE(true);
	}
}
