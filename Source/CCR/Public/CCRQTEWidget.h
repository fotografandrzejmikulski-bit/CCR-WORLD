#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "Blueprint/UserWidget.h"
#include "CCRQTEWidget.generated.h"

/**
 * UCCRQTEWidget
 *
 * C++ base class for the UMG QTE (Quick Time Event) widget.
 * Blueprint WBP_CCRQTE should derive from this class.
 *
 * Displays a progress arc/bar while the player holds the screen.
 * The owning ACCRTouchController drives progress via UpdateProgress().
 *
 * NativeConstruct subscribes to UCCRNarrativeRuntimeSubsystem::OnNodeChanged
 * so that OnQTEPromptReady is called whenever the narrative enters a QTE node,
 * letting Blueprint display the correct gesture icon (hold-circle, swipe arrow
 * with direction) before ACCRTouchController fires OnQTEStarted.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRQTEWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Called when the narrative enters a QTE node.
	 * Override in Blueprint to display the gesture-specific icon / direction arrow
	 * before the QTE timer begins.
	 * @param GestureType   LongPress, Tap, or Swipe.
	 * @param SwipeDir      Required swipe direction (Any when GestureType != Swipe).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|QTE")
	void OnQTEPromptReady(ECCRGestureType GestureType, ECCRSwipeDirection SwipeDir);

	/**
	 * Called every tick while QTE is active.
	 * @param Progress  0.0 = just started, 1.0 = success threshold reached.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|QTE")
	void UpdateProgress(float Progress);

	/** Called when QTE starts – show the widget. */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|QTE")
	void OnQTEStarted(float TimeWindowSec);

	/** Called when QTE ends. */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|QTE")
	void OnQTEEnded(bool bSuccess);

private:
	UFUNCTION()
	void HandleNodeChanged(FName NodeId);
};
