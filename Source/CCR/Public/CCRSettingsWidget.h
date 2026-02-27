#pragma once

#include "CoreMinimal.h"
#include "CCRPerformanceGovernorSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "CCRSettingsWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRSettingsWidgetClosed);

/**
 * UCCRSettingsWidget
 *
 * C++ base class for the UMG settings screen.
 * Blueprint WBP_CCRSettings should derive from this class.
 *
 * Exposes the full UCCRSettingsSubsystem surface as BlueprintCallable
 * getters/setters so that Blueprint sliders and toggles can read and write
 * settings without needing any code.
 *
 * Reads are safe at any time; writes are forwarded to UCCRSettingsSubsystem
 * which persists them and fires OnSettingsChanged so other subsystems
 * (audio, performance governor) pick up the new values automatically.
 *
 * NativeConstruct fires OnSettingsOpened() so the Blueprint implementation
 * can pre-populate sliders to the current values.
 *
 * Close() hides the widget and fires OnSettingsClosed() so the owning
 * UCCRPauseWidget can react (e.g. re-show itself).
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// ---- Lifecycle ----

	/**
	 * Called when the settings widget is shown.
	 * Override in Blueprint to populate sliders / toggle states from current values.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Settings")
	void OnSettingsOpened();

	/**
	 * Called just before the widget is hidden via Close().
	 * Override in Blueprint to play a close animation.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Settings")
	void OnSettingsClosed();

	/** Hide this widget and notify listeners. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void Close();

	// ---- Volume getters ----

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	float GetMusicVolume() const;

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	float GetVOVolume() const;

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	float GetSFXVolume() const;

	// ---- Volume setters ----

	/** Set music volume [0..1] and persist immediately. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetMusicVolume(float Volume);

	/** Set voice-over volume [0..1] and persist immediately. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetVOVolume(float Volume);

	/** Set SFX volume [0..1] and persist immediately. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetSFXVolume(float Volume);

	// ---- Subtitles ----

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	bool GetSubtitlesAlwaysOn() const;

	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetSubtitlesAlwaysOn(bool bEnabled);

	// ---- Performance tier ----

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	bool IsPerformanceTierOverridden() const;

	/** Returns the active override tier, or ECCRPerformanceTier::Mid if not overridden. */
	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	ECCRPerformanceTier GetPerformanceTierOverride() const;

	/**
	 * Override the performance tier and persist.
	 * Pass ECCRPerformanceTier::Mid to use auto-detection instead (calls
	 * ClearPerformanceTierOverride internally when the current detected tier is Mid).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetPerformanceTierOverride(ECCRPerformanceTier Tier);

	/** Remove any manual override and return to auto-detected tier. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void ClearPerformanceTierOverride();

	// ---- Accessibility ----

	/** Returns the global UI font-scale multiplier (default 1.0). */
	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	float GetFontScale() const;

	/** Set font scale [0.5..2.0] and persist. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetFontScale(float Scale);

	/** Returns true when reduced-motion mode is active. */
	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	bool GetReducedMotion() const;

	/** Enable / disable reduced-motion mode and persist. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetReducedMotion(bool bEnabled);

	/** Returns true when high-contrast mode is active. */
	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	bool GetHighContrast() const;

	/** Enable / disable high-contrast mode and persist. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetHighContrast(bool bEnabled);

	/** Fired when Close() is called; UCCRPauseWidget subscribes to this. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Settings")
	FOnCCRSettingsWidgetClosed OnClosed;
};
