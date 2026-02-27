#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRMainMenuWidget.generated.h"

/**
 * UCCRMainMenuWidget
 *
 * C++ base class for the UMG main menu widget.
 * Blueprint WBP_CCRMainMenu should derive from this class.
 *
 * NativeConstruct checks for an existing save slot and fires
 * OnMainMenuReady(bHasSave) so Blueprint can show or hide the
 * "Continue" button accordingly.
 *
 * The game phase is ECCRGamePhase::MainMenu while this widget is active.
 * ACCRGameMode::PostLogin sets that phase on startup; the player's
 * button presses (NewGame / Continue) drive the transition to Loading.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * Called immediately after the widget is constructed.
	 * @param bHasSave  True when a save game exists (show the "Continue" button).
	 * Override in Blueprint to configure the initial UI state.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|MainMenu")
	void OnMainMenuReady(bool bHasSave);

	/**
	 * Start a fresh game from the entry chunk defined in ACCRGameMode.
	 * Transitions the game phase to Loading and begins async chunk streaming.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|MainMenu")
	void NewGame();

	/**
	 * Resume from the default save slot.
	 * Falls back to NewGame() if no save exists.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|MainMenu")
	void Continue();

	/** Quit the application. */
	UFUNCTION(BlueprintCallable, Category = "CCR|MainMenu")
	void QuitGame();
};
