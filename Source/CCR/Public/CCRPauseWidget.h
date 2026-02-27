#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRPauseWidget.generated.h"

class UCCRSettingsWidget;
class UCCRChapterSelectWidget;

/**
 * UCCRPauseWidget
 *
 * C++ base class for the UMG pause menu widget.
 * Blueprint WBP_CCRPause should derive from this class.
 *
 * Provides Resume, ReturnToMainMenu, QuitGame, and OpenSettings actions
 * with Blueprint-implementable handlers so that UI animation and sound
 * can be driven from Blueprint while the C++ base handles the actual logic.
 *
 * OpenSettings() lazily creates the settings widget (using SettingsWidgetClass),
 * hides the pause menu body, and subscribes to UCCRSettingsWidget::OnClosed
 * so the pause body is restored when settings are dismissed.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRPauseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// ---- Actions (callable from Blueprint buttons) ----

	/** Resume the game (unpause). */
	UFUNCTION(BlueprintCallable, Category = "CCR|Pause")
	void Resume();

	/** Return to the main menu level. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Pause")
	void ReturnToMainMenu();

	/** Quit the application. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Pause")
	void QuitGame();

	/**
	 * Open the settings screen.
	 * Creates the settings widget on first use (requires SettingsWidgetClass to be set).
	 * Fires OnSettingsOpened() so Blueprint can hide the pause menu body.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Pause")
	void OpenSettings();

	/**
	 * Open the chapter select / chapter replay screen.
	 * Creates the chapter select widget on first use (requires ChapterSelectWidgetClass to be set).
	 * Fires OnChapterSelectOpened() so Blueprint can hide the pause menu body.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Pause")
	void OpenChapterSelect();

	// ---- Blueprint-implementable notifications ----

	/** Called when this widget is shown (game paused). */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Pause")
	void OnPaused();

	/** Called when the game is about to resume. */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Pause")
	void OnResumed();

	/**
	 * Called when the settings screen is about to open.
	 * Override in Blueprint to hide the pause menu body / play a transition.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Pause")
	void OnSettingsOpened();

	/**
	 * Called when the settings screen is dismissed and the pause menu is restored.
	 * Override in Blueprint to re-show the pause menu body / play a transition.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Pause")
	void OnSettingsClosed();

	/**
	 * Called when the chapter select screen is about to open.
	 * Override in Blueprint to hide the pause menu body / play a transition.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Pause")
	void OnChapterSelectOpened();

	/**
	 * Called when the chapter select screen is dismissed and the pause menu is restored.
	 * Override in Blueprint to re-show the pause menu body / play a transition.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Pause")
	void OnChapterSelectClosed();

	/** Main menu level name (set in Blueprint or defaults) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Pause")
	FName MainMenuLevelName = TEXT("MainMenu");

	/**
	 * Blueprint subclass of UCCRSettingsWidget to spawn when OpenSettings() is called.
	 * Can be assigned in the Blueprint defaults or propagated at runtime by ACCRGameHUD.
	 * The HUD's value takes precedence if both are set.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Pause")
	TSoftClassPtr<UCCRSettingsWidget> SettingsWidgetClass;

	/** Read-only reference to the live settings widget (null until first OpenSettings()). */
	UPROPERTY(BlueprintReadOnly, Category = "CCR|Pause")
	UCCRSettingsWidget* SettingsWidget = nullptr;

	/**
	 * Blueprint subclass of UCCRChapterSelectWidget to spawn when OpenChapterSelect() is called.
	 * Assign in Blueprint defaults or propagate from the HUD.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Pause")
	TSoftClassPtr<UCCRChapterSelectWidget> ChapterSelectWidgetClass;

	/** Read-only reference to the live chapter select widget (null until first OpenChapterSelect()). */
	UPROPERTY(BlueprintReadOnly, Category = "CCR|Pause")
	UCCRChapterSelectWidget* ChapterSelectWidget = nullptr;

private:
	UFUNCTION()
	void HandleSettingsClosed();

	UFUNCTION()
	void HandleChapterSelectClosed();
};
