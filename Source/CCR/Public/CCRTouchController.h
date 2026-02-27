#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "GameFramework/PlayerController.h"
#include "CCRTouchController.generated.h"

/**
 * ACCRTouchController
 *
 * Mobile-first PlayerController handling all three QTE gesture types:
 *   LongPress – hold for TimeWindowSec to succeed
 *   Tap        – single touch resolves immediately as success
 *   Swipe      – swipe in RequiredSwipeDir (or Any) within TimeWindowSec
 *
 * Behaviour:
 *  - TouchBegin: arm the appropriate gesture mode based on the active QTE node.
 *  - Tick: drive LongPress timer; haptic feedback; swipe timeout.
 *  - TouchEnd: resolve Tap/Swipe; early LongPress release = failure.
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

	/** How long the player must hold to succeed a LongPress QTE */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|QTE")
	float TimeWindowSec = 2.f;

	/** Interval between haptic pulses while holding */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|QTE")
	float HapticPulseInterval = 0.30f;

	/**
	 * Minimum swipe distance in viewport pixels to be recognised as a swipe.
	 * Smaller movements are treated as a tap.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|QTE")
	float SwipeMinDistancePx = 60.f;

private:
	// ---- LongPress state ----
	bool  bQTEActive           = false;
	float QTEHeldTime          = 0.f;
	float TimeSinceLastHaptic  = 0.f;

	// ---- Swipe state ----
	bool   bSwipeTracking      = false;
	FVector2D TouchStartPos    = FVector2D::ZeroVector;
	ECCRSwipeDirection RequiredSwipe = ECCRSwipeDirection::Any;

	UFUNCTION()
	void HandleTouchBegin(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
	void HandleTouchEnd(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
	void HandlePause();

	void StartQTE();
	void EndQTE(bool bSuccess);

	/** Evaluate whether a swipe from Start to End satisfies RequiredSwipe. */
	bool EvaluateSwipe(FVector2D Start, FVector2D End) const;

	/** Push current QTE progress (0..1) to the QTE widget */
	void UpdateQTEWidget(float Progress, bool bVisible);
};
