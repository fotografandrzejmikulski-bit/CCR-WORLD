#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCRTouchController.generated.h"

/**
 * ACCRTouchController
 *
 * Mobile-first PlayerController handling Long Press QTE.
 *
 * Behaviour:
 *  - TouchBegin: if the current narrative node is a QTE/LongPress, start the timer.
 *  - Tick: measure held time, pulse haptic feedback every ~0.30 s.
 *  - TouchEnd: if time < TimeWindowSec -> failure; success fires after TimeWindowSec.
 */
UCLASS()
class CCR_API ACCRTouchController : public APlayerController
{
	GENERATED_BODY()

public:
	ACCRTouchController();

	// ---- APlayerController overrides ----
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	/** How long the player must hold to succeed the QTE */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|QTE")
	float TimeWindowSec = 2.f;

	/** Interval between haptic pulses while holding */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|QTE")
	float HapticPulseInterval = 0.30f;

private:
	bool  bQTEActive           = false;
	float QTEHeldTime          = 0.f;
	float TimeSinceLastHaptic  = 0.f;

	UFUNCTION()
	void HandleTouchBegin(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
	void HandleTouchEnd(ETouchIndex::Type FingerIndex, FVector Location);

	void StartQTE();
	void EndQTE(bool bSuccess);
};
