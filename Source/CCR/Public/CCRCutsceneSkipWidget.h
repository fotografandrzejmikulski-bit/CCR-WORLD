#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRCutsceneSkipWidget.generated.h"

/**
 * UCCRCutsceneSkipWidget
 *
 * Standalone "Skip" button overlay shown during skippable cinematics.
 * Blueprint WBP_CCRCutsceneSkip should derive from this class.
 *
 * ACCRGameHUD creates and adds this widget at CCRZOrder::Cinematic (= 2)
 * and shows it whenever the narrative enters a Cinematic node with bSkippable = true.
 *
 * Lifecycle:
 *   1. HUD subscribes to UCCRNarrativeRuntimeSubsystem::OnNodeChanged.
 *   2. On a Cinematic node with bSkippable = true, HUD calls SetVisibility(Visible).
 *   3. Blueprint shows an animated "Tap to skip" prompt.
 *   4. Player taps → Blueprint calls SkipCinematic().
 *   5. SkipCinematic() calls UCCRCinematicSubsystem::SkipCinematic() and hides the widget.
 *   6. After cinematic ends (skipped or natural), the HUD sets the widget Collapsed.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRCutsceneSkipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Called when a skippable cinematic begins.
	 * Override in Blueprint to show a "Tap to skip" animation.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Cinematic")
	void OnCinematicStarted();

	/**
	 * Called when the cinematic ends (naturally or via skip).
	 * Override in Blueprint to hide the skip prompt.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Cinematic")
	void OnCinematicEnded();

	/**
	 * Skip the currently playing cinematic.
	 * Delegates to UCCRCinematicSubsystem::SkipCinematic() and collapses the widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Cinematic")
	void SkipCinematic();

private:
	UFUNCTION()
	void HandleNodeChanged(FName NodeId);
};
