#pragma once

#include "CoreMinimal.h"
#include "CCRPerformanceGovernorSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRSettingsSubsystem.generated.h"

/**
 * UCCRSettingsSubsystem
 *
 * Loads player preferences from "CCRSettings" slot on Initialize and
 * saves them back whenever they change.
 *
 * Other subsystems (UCCRAudioSubsystem, UCCRPerformanceGovernorSubsystem)
 * read their initial values from here instead of using hard-coded defaults.
 *
 * Blueprint and C++ callers use the Set* functions to change a value;
 * the subsystem persists immediately and notifies interested parties via
 * OnSettingsChanged.
 */
UCLASS(BlueprintType)
class CCR_API UCCRSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ---- Delegates ----
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRSettingsChanged);

	/** Fired whenever any setting is saved. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Settings")
	FOnCCRSettingsChanged OnSettingsChanged;

	// ---- Getters ----

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	float GetMusicVolume() const;

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	float GetVOVolume() const;

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	float GetSFXVolume() const;

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	bool IsPerformanceTierOverridden() const;

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	ECCRPerformanceTier GetPerformanceTierOverride() const;

	UFUNCTION(BlueprintPure, Category = "CCR|Settings")
	bool GetSubtitlesAlwaysOn() const;

	// ---- Setters (each triggers an immediate save) ----

	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetVOVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetPerformanceTierOverride(ECCRPerformanceTier Tier);

	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void ClearPerformanceTierOverride();

	UFUNCTION(BlueprintCallable, Category = "CCR|Settings")
	void SetSubtitlesAlwaysOn(bool bEnabled);

private:
	UPROPERTY()
	TObjectPtr<class UCCRSettingsSaveGame> ActiveSettings = nullptr;

	void LoadOrCreate();
	void SaveNow();
};
