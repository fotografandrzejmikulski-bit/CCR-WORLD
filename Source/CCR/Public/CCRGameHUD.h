#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCRGameHUD.generated.h"

class UCCRDialogueWidget;
class UCCRQTEWidget;
class UCCRPauseWidget;

/**
 * ACCRGameHUD
 *
 * Manages the in-game widget stack:
 *   - UCCRDialogueWidget  (z-order 0) – dialogue and choice UI
 *   - UCCRQTEWidget       (z-order 1) – QTE progress overlay
 *   - UCCRPauseWidget     (z-order 10) – pause menu (created on demand)
 *
 * Widget class references are soft-pointed so Blueprint sub-classes
 * (WBP_CCRDialogue, WBP_CCRQTE, WBP_CCRPause) can be assigned in the
 * editor without requiring hard C++ dependencies.
 */
UCLASS()
class CCR_API ACCRGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

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

	// ---- Live widget instances ----

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRDialogueWidget* DialogueWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRQTEWidget* QTEWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CCR|HUD")
	UCCRPauseWidget* PauseWidget = nullptr;

	/** Show or hide the dialogue panel */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetDialogueVisible(bool bVisible);

	/** Show or hide the QTE overlay */
	UFUNCTION(BlueprintCallable, Category = "CCR|HUD")
	void SetQTEVisible(bool bVisible);

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
};
