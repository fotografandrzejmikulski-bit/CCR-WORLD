#pragma once

#include "CoreMinimal.h"
#include "CCRGameState.h"
#include "Blueprint/UserWidget.h"
#include "CCRLoadingWidget.generated.h"

/**
 * UCCRLoadingWidget
 *
 * C++ base class for the UMG loading-screen overlay.
 * Blueprint WBP_CCRLoading should derive from this class.
 *
 * NativeConstruct subscribes to ACCRGameState::OnGamePhaseChanged so that
 * OnLoadingStarted is called whenever the phase transitions TO Loading, and
 * OnLoadingFinished is called when it transitions away.  The owning
 * ACCRGameHUD also drives visibility via SetLoadingVisible(), ensuring the
 * overlay is always in sync even if the GameState is not yet available when
 * the widget is first added to the viewport.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRLoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Called when the game transitions into ECCRGamePhase::Loading.
	 * Override in Blueprint to start a spinner animation, fade-in, etc.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|Loading")
	void OnLoadingStarted();

	/**
	 * Called when the game transitions out of ECCRGamePhase::Loading.
	 * Override in Blueprint to play a fade-out or hide the spinner.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "CCR|Loading")
	void OnLoadingFinished();

private:
	/** Tracks whether the last known phase was Loading, so OnLoadingFinished
	 *  is only fired when transitioning FROM Loading (not on every phase change). */
	bool bWasLoading = false;

	UFUNCTION()
	void HandleGamePhaseChanged(ECCRGamePhase NewPhase);
};
