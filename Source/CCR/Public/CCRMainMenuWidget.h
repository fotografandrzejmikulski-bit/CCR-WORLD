#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRMainMenuWidget.generated.h"

class UCCRSettingsWidget;
class UCCRChapterSelectWidget;

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
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|MainMenu")
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

	/**
	 * Open the settings screen.
	 * Creates the settings widget on first use (requires SettingsWidgetClass to be set).
	 * Fires OnSettingsOpened() so Blueprint can hide the main menu body.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|MainMenu")
	void OpenSettings();

	/**
	 * Open the chapter select / chapter replay screen.
	 * Creates the chapter select widget on first use (requires ChapterSelectWidgetClass).
	 * Fires OnChapterSelectOpened() so Blueprint can hide the main menu body.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|MainMenu")
	void OpenChapterSelect();

	// ---- Blueprint-implementable notifications ----

	/**
	 * Called when the settings screen is about to open.
	 * Override in Blueprint to hide the main menu body / play a transition.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|MainMenu")
	void OnSettingsOpened();

	/**
	 * Called when the settings screen is dismissed and the main menu is restored.
	 * Override in Blueprint to re-show the main menu body.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|MainMenu")
	void OnSettingsClosed();

	/**
	 * Called when the chapter select screen is about to open.
	 * Override in Blueprint to hide the main menu body / play a transition.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|MainMenu")
	void OnChapterSelectOpened();

	/**
	 * Called when the chapter select screen is dismissed and the main menu is restored.
	 * Override in Blueprint to re-show the main menu body.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|MainMenu")
	void OnChapterSelectClosed();

	/**
	 * Blueprint subclass of UCCRSettingsWidget to spawn when OpenSettings() is called.
	 * Assign in Blueprint defaults or propagate from the HUD.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|MainMenu")
	TSoftClassPtr<UCCRSettingsWidget> SettingsWidgetClass;

	/**
	 * Blueprint subclass of UCCRChapterSelectWidget to spawn when OpenChapterSelect() is called.
	 * Assign in Blueprint defaults or propagate from the HUD.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|MainMenu")
	TSoftClassPtr<UCCRChapterSelectWidget> ChapterSelectWidgetClass;

	/** Read-only reference to the live settings widget (null until first OpenSettings()). */
	UPROPERTY(BlueprintReadOnly, Category = "CCR|MainMenu")
	UCCRSettingsWidget* SettingsWidget = nullptr;

	/** Read-only reference to the live chapter select widget (null until first OpenChapterSelect()). */
	UPROPERTY(BlueprintReadOnly, Category = "CCR|MainMenu")
	UCCRChapterSelectWidget* ChapterSelectWidget = nullptr;

private:
	UFUNCTION()
	void HandleSettingsClosed();

	UFUNCTION()
	void HandleChapterSelectClosed();
};
