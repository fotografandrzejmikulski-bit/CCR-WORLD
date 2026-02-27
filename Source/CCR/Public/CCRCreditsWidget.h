#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRCreditsWidget.generated.h"

/**
 * UCCRCreditsWidget
 *
 * C++ base class for the end-of-game credits screen.
 * Blueprint WBP_CCRCredits should derive from this class.
 *
 * ACCRGameHUD creates this widget at CCRZOrder::Credits (Collapsed) on BeginPlay.
 * NativeConstruct subscribes to UCCRNarrativeRuntimeSubsystem::OnNarrativeEnded.
 *
 * Lifecycle:
 *   1. ACCRGameHUD creates the widget with AddToViewport(CCRZOrder::Credits) and
 *      immediately sets its visibility to Collapsed.  NativeConstruct fires here
 *      and subscribes to OnNarrativeEnded — no credits animation starts yet.
 *   2. When the narrative reaches an End node, OnNarrativeEnded fires and
 *      HandleNarrativeEnded() makes the widget visible, then calls OnCreditsBegin().
 *   3. Blueprint implements OnCreditsBegin() to start the scrolling credits.
 *   4. When the credits finish, Blueprint calls ReturnToMainMenu().
 *   5. ReturnToMainMenu() unpauses (if needed) and opens the main menu level.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRCreditsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Called when the credits should begin (narrative ended).
	 * Override in Blueprint to start the scrolling credits animation/sequence.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|Credits")
	void OnCreditsBegin();

	/**
	 * Return to the main menu.
	 * Intended to be called from Blueprint when the credits animation finishes
	 * or the player presses a "Skip" button.
	 * Unpause (if needed), then open the main menu level.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Credits")
	void ReturnToMainMenu();

	/** Main menu level name; matches the value used in UCCRPauseWidget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Credits")
	FName MainMenuLevelName = TEXT("MainMenu");

private:
	UFUNCTION()
	void HandleNarrativeEnded();
};
