#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRFadeSubsystem.generated.h"

/**
 * UCCRFadeSubsystem
 *
 * Centralized full-screen fade controller for CCR WORLD.
 *
 * Provides FadeTo() / FadeFrom() / FadeToAndFrom() (a complete "cut" that fades
 * to black, executes an optional callback, then fades back) using
 * APlayerCameraManager::StartCameraFade — the engine's built-in fade API —
 * so it works without any special UMG widget overhead.
 *
 * Common use cases:
 *   - Scene transition (fade to black → open new level → fade from black)
 *   - Game over sequence (fade to black → show game-over HUD)
 *   - Waking-up / knockout effect (rapid fade from/to black)
 *
 * All colour and duration parameters default to sensible values but are
 * fully configurable per call.
 */
UCLASS(BlueprintType)
class CCR_API UCCRFadeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Fade the screen to Color over DurationSec.
	 * When the fade is complete, OnFadedIn fires.
	 * @param DurationSec  Fade duration.  0 = instant.
	 * @param Color        Target screen color (default: black).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Fade")
	void FadeTo(float DurationSec = 0.5f, FLinearColor Color = FLinearColor::Black);

	/**
	 * Fade from Color back to a transparent screen over DurationSec.
	 * When the fade is complete, OnFadedOut fires.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Fade")
	void FadeFrom(float DurationSec = 0.5f, FLinearColor Color = FLinearColor::Black);

	/**
	 * Perform a full "cut" transition:
	 *   1. Fade to Color over FadeInSec.
	 *   2. Hold for HoldSec.
	 *   3. Fire OnCutReady delegate (useful for opening a level mid-cut).
	 *   4. Fade back to transparent over FadeOutSec.
	 *
	 * This is the recommended function for level transitions.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Fade")
	void FadeToAndFrom(float FadeInSec = 0.5f, float HoldSec = 0.5f, float FadeOutSec = 0.5f,
	                   FLinearColor Color = FLinearColor::Black);

	/** Cancel any in-progress fade immediately. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Fade")
	void CancelFade();

	/** Returns true while a fade-in is in progress. */
	UFUNCTION(BlueprintPure, Category = "CCR|Fade")
	bool IsFading() const { return bFading; }

	// ---- Delegates ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRFadeEvent);

	/** Fired when FadeTo() has fully covered the screen. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Fade")
	FOnCCRFadeEvent OnFadedIn;

	/** Fired when FadeFrom() has fully cleared the screen. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Fade")
	FOnCCRFadeEvent OnFadedOut;

	/**
	 * Fired during FadeToAndFrom() after the screen is fully covered and the
	 * hold begins.  Useful for opening a level at the darkest point of the cut.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Fade")
	FOnCCRFadeEvent OnCutReady;

private:
	bool bFading = false;
	FTimerHandle FadeTimer;

	APlayerCameraManager* GetCameraManager() const;
};
