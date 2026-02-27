#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRChapterTransitionWidget.generated.h"

/**
 * UCCRChapterTransitionWidget
 *
 * C++ base class for the UMG chapter-title card shown between major story beats.
 * Blueprint WBP_CCRChapterTransition should derive from this class.
 *
 * ACCRGameHUD creates this widget at CCRZOrder::ChapterTransition and calls
 * ShowChapterTitle() whenever UCCRNarrativeRuntimeSubsystem fires OnChunkStarted
 * for a chunk that has a non-empty ChapterTitle.
 *
 * Lifecycle:
 *   1. ShowChapterTitle(Title, Subtitle) is called.
 *   2. Blueprint fires OnShowChapterTitle() – play a fade-in + hold animation.
 *   3. When the animation ends, Blueprint calls DismissTransition().
 *   4. DismissTransition() fires OnDismissTransition() – play a fade-out.
 *   5. The widget collapses itself after the dismiss animation completes
 *      (or Blueprint can call SetVisibility(Collapsed) directly).
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRChapterTransitionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Show the chapter title card.
	 * Called by ACCRGameHUD when a new chunk with a non-empty ChapterTitle begins.
	 * Fires OnShowChapterTitle() so Blueprint can animate the text in.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|ChapterTransition")
	void ShowChapterTitle(const FText& Title, const FText& Subtitle);

	/**
	 * Begin dismissing the title card.
	 * Intended to be called from Blueprint once the hold animation finishes.
	 * Fires OnDismissTransition() and collapses the widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|ChapterTransition")
	void DismissTransition();

	// ---- Blueprint-implementable events ----

	/**
	 * Called when a new chapter title should appear.
	 * Override in Blueprint to display Title and Subtitle and play a fade-in.
	 * @param Title     Chapter title text (e.g. "Chapter 1 – The Awakening").
	 * @param Subtitle  Optional subtitle (in-world location or thematic tagline).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|ChapterTransition")
	void OnShowChapterTitle(const FText& Title, const FText& Subtitle);

	/**
	 * Called when DismissTransition() is invoked.
	 * Override in Blueprint to play a fade-out animation, then collapse the widget.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|ChapterTransition")
	void OnDismissTransition();
};
