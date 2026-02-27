#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCRGameHUD.generated.h"

class UCCRDialogueWidget;
class UCCRQTEWidget;
class UCCRPauseWidget;
class UCCRLoadingWidget;
class UCCRMainMenuWidget;
class UCCRSettingsWidget;
class UCCRNotificationWidget;
class UCCRChapterTransitionWidget;
class UCCRCreditsWidget;
class UCCRCutsceneSkipWidget;
class UCCRObjectiveWidget;
class UCCRInventoryWidget;
class UCCRChapterSelectWidget;
class UCCRStoryChunk;

/**
 * ACCRGameHUD
 *
 * Manages the in-game widget stack:
 *   - UCCRDialogueWidget        (z-order 0)  – dialogue and choice UI
 *   - UCCRQTEWidget             (z-order 1)  – QTE progress overlay
 *   - UCCRCutsceneSkipWidget    (z-order 2)  – skip hint during cinematics
 *   - UCCRNotificationWidget    (z-order 3)  – transient notification toasts
 *   - UCCRObjectiveWidget       (z-order 4)  – current objective display
 *   - UCCRMainMenuWidget        (z-order 5)  – main menu overlay
 *   - UCCRInventoryWidget       (z-order 6)  – inventory screen (toggle)
 *   - UCCRChapterSelectWidget   (z-order 9)  – chapter select/replay screen (toggle)
 *   - UCCRPauseWidget           (z-order 10) – pause menu (created on demand)
 *   - UCCRSettingsWidget        (z-order 11) – settings screen (owned by pause/main-menu)
 *   - UCCRCreditsWidget         (z-order 15) – end-game credits overlay
 *   - UCCRChapterTransitionWidget (z-order 18) – chapter title card
 *   - UCCRLoadingWidget         (z-order 20) – full-screen loading overlay
 *
 * Widget class references are soft-pointed so Blueprint sub-classes
 * (WBP_CCRDialogue, WBP_CCRQTE, WBP_CCRPause, …) can be assigned in the
 * editor without requiring hard C++ dependencies.
 */
UCLASS()
class CCR_API ACCRGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ---- Widget class references (assign Blueprint subclasses in editor) ----

	/** Blueprint subclass of UCCRDialogueWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRDialogueWidget> DialogueWidgetClass;

	/** Blueprint subclass of UCCRQTEWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRQTEWidget> QTEWidgetClass;

	/** Blueprint subclass of UCCRPauseWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRPauseWidget> PauseWidgetClass;

	/** Blueprint subclass of UCCRLoadingWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRLoadingWidget> LoadingWidgetClass;

	/** Blueprint subclass of UCCRMainMenuWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRMainMenuWidget> MainMenuWidgetClass;

	/** Blueprint subclass of UCCRSettingsWidget (used by the pause menu). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRSettingsWidget> SettingsWidgetClass;

	/** Blueprint subclass of UCCRNotificationWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRNotificationWidget> NotificationWidgetClass;

	/** Blueprint subclass of UCCRChapterTransitionWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRChapterTransitionWidget> ChapterTransitionWidgetClass;

	/** Blueprint subclass of UCCRCreditsWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRCreditsWidget> CreditsWidgetClass;

	/** Blueprint subclass of UCCRCutsceneSkipWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRCutsceneSkipWidget> CutsceneSkipWidgetClass;

	/** Blueprint subclass of UCCRObjectiveWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRObjectiveWidget> ObjectiveWidgetClass;

	/** Blueprint subclass of UCCRInventoryWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRInventoryWidget> InventoryWidgetClass;

	/** Blueprint subclass of UCCRChapterSelectWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|HUD")
	TSoftClassPtr<UCCRChapterSelectWidget> ChapterSelectWidgetClass;

	// ---- Live widget instances ----

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRDialogueWidget* DialogueWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRQTEWidget* QTEWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRPauseWidget* PauseWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRLoadingWidget* LoadingWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRMainMenuWidget* MainMenuWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRSettingsWidget* SettingsWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRNotificationWidget* NotificationWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRChapterTransitionWidget* ChapterTransitionWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRCreditsWidget* CreditsWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRCutsceneSkipWidget* CutsceneSkipWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRObjectiveWidget* ObjectiveWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRInventoryWidget* InventoryWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRChapterSelectWidget* ChapterSelectWidget = nullptr;

	/** Show or hide the dialogue panel */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetDialogueVisible(bool bVisible);

	/** Show or hide the QTE overlay */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetQTEVisible(bool bVisible);

	/** Show or hide the loading overlay */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetLoadingVisible(bool bVisible);

	/** Show or hide the main menu */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetMainMenuVisible(bool bVisible);

	/**
	 * Show or hide the standalone settings widget owned by the HUD.
	 * Note: the pause menu also owns its own settings widget instance;
	 * this function controls only the HUD-level one (if any).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetSettingsVisible(bool bVisible);

	/** Show or hide the credits overlay */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetCreditsVisible(bool bVisible);

	/** Toggle the inventory panel (creates it on first use) */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void ToggleInventory();

	/** Show or hide the inventory panel */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetInventoryVisible(bool bVisible);

	/** Returns true when the inventory widget is currently visible */
	UFUNCTION(BlueprintPure, Category = "CCR|HUD")
	bool IsInventoryOpen() const;

	/** Toggle the chapter select screen (creates it on first use) */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void ToggleChapterSelect();

	/** Show or hide the chapter select screen */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetChapterSelectVisible(bool bVisible);

	/** Returns true when the chapter select screen is currently visible */
	UFUNCTION(BlueprintPure, Category = "CCR|HUD")
	bool IsChapterSelectOpen() const;

	/** Toggle the pause menu (creates it on first use) */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void TogglePause();

	/** Returns true when the pause widget is currently visible */
	UFUNCTION(BlueprintPure, Category = "CCR|HUD")
	bool IsPaused() const;

private:
	/** Reacts to ACCRGameState::OnGamePhaseChanged to drive widget visibility. */
	UFUNCTION()
	void HandleGamePhaseChanged(ECCRGamePhase NewPhase);

	/** Reacts to UCCRNarrativeRuntimeSubsystem::OnChunkStarted to show chapter title cards. */
	UFUNCTION()
	void HandleChunkStarted(UCCRStoryChunk* Chunk);
};
