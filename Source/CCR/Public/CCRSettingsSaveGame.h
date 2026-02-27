#pragma once

#include "CoreMinimal.h"
#include "CCRPerformanceGovernorSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "CCRSettingsSaveGame.generated.h"

/**
 * UCCRSettingsSaveGame
 *
 * Stores player preferences that must persist across sessions independently
 * of narrative progress (separate slot from the world-state save).
 *
 * Slot: "CCRSettings"
 */
UCLASS(BlueprintType)
class CCR_API UCCRSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UCCRSettingsSaveGame();

	// ---- Audio ----

	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	float MusicVolume = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	float VOVolume = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	float SFXVolume = 1.f;

	// ---- Performance ----

	/**
	 * When true the player has explicitly chosen a tier; ignore auto-detect.
	 * When false (default) UCCRPerformanceGovernorSubsystem auto-detects.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	bool bOverridePerformanceTier = false;

	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	ECCRPerformanceTier PerformanceTierOverride = ECCRPerformanceTier::Mid;

	// ---- Accessibility ----

	/** Show subtitles even when VO is enabled */
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	bool bSubtitlesAlwaysOn = false;

	/**
	 * Global UI font-scale multiplier.
	 * Clamped to [0.5, 2.0] by UCCRSettingsSubsystem::SetFontScale.
	 * Default 1.0 = native font size.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	float FontScale = 1.f;

	/** Suppress non-essential parallax, screen-shake, and flicker animations. */
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	bool bReducedMotion = false;

	/** Use high-contrast colour overrides in dialogue, subtitles, and HUD. */
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Settings")
	bool bHighContrast = false;
};
