#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRPauseWidget.generated.h"

/**
 * UCCRPauseWidget
 *
 * C++ base class for the UMG pause menu widget.
 * Blueprint WBP_CCRPause should derive from this class.
 *
 * Provides Resume, ReturnToMainMenu, and QuitGame actions with Blueprint-
 * implementable handlers so that UI animation and sound can be
 * driven from Blueprint while the C++ base handles the actual logic.
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

	// ---- Blueprint-implementable notifications ----

	/** Called when this widget is shown (game paused). */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|Pause")
	void OnPaused();

	/** Called when the game is about to resume. */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|Pause")
	void OnResumed();

	/** Main menu level name (set in Blueprint or defaults) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Pause")
	FName MainMenuLevelName = TEXT("MainMenu");
};
