#pragma once

#include "CoreMinimal.h"
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
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRQTEWidget : public UUserWidget
{
	GENERATED_BODY()

public:
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
};
