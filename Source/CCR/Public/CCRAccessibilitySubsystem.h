#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRAccessibilitySubsystem.generated.h"

/**
 * UCCRAccessibilitySubsystem
 *
 * Centralises all accessibility preferences so that widgets and systems can
 * query them without knowing about the underlying persistence layer.
 *
 * Settings are stored in UCCRSettingsSaveGame (piggybacking on the existing
 * settings save) via UCCRSettingsSubsystem getters/setters.
 * This subsystem provides a single discoverable place to query them and fires
 * OnAccessibilityChanged whenever any accessibility value is modified.
 *
 * Preferences:
 *   FontScale      – global UI font scale multiplier (0.5 – 2.0, default 1.0)
 *   ReducedMotion  – suppress non-essential animations and parallax effects
 *   HighContrast   – use high-contrast colour overrides in dialogue / HUD
 */
UCLASS(BlueprintType)
class CCR_API UCCRAccessibilitySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRAccessibilityChanged);

	/** Fired whenever any accessibility preference is changed and saved. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Accessibility")
	FOnCCRAccessibilityChanged OnAccessibilityChanged;

	// ---- Font scale ----

	/** Global UI font scale multiplier. Clamp range: [0.5, 2.0]. Default: 1.0. */
	UFUNCTION(BlueprintPure, Category = "CCR|Accessibility")
	float GetFontScale() const;

	/** Set font scale and persist. Fires OnAccessibilityChanged. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Accessibility")
	void SetFontScale(float Scale);

	// ---- Reduced motion ----

	/**
	 * When true, widgets should suppress parallax, screen-shake, and non-essential
	 * animations to reduce motion sickness risk.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Accessibility")
	bool IsReducedMotion() const;

	/** Enable or disable reduced-motion mode. Fires OnAccessibilityChanged. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Accessibility")
	void SetReducedMotion(bool bEnabled);

	// ---- High contrast ----

	/**
	 * When true, dialogue text, choice labels and HUD elements should use
	 * high-contrast colour overrides defined in the Blueprint implementation.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Accessibility")
	bool IsHighContrast() const;

	/** Enable or disable high-contrast mode. Fires OnAccessibilityChanged. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Accessibility")
	void SetHighContrast(bool bEnabled);

private:
	UFUNCTION()
	void OnSettingsChanged();
};
