#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRPerformanceGovernorSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECCRPerformanceTier : uint8
{
	High UMETA(DisplayName = "High"),
	Mid  UMETA(DisplayName = "Mid"),
	Low  UMETA(DisplayName = "Low"),
};

/**
 * UCCRPerformanceGovernorSubsystem
 *
 * Detects device performance tier from the GPU adapter name
 * and exposes limits used by other subsystems (e.g. max predicted chunks).
 * Also applies Unreal scalability CVars to match the detected tier and
 * re-applies them whenever the player changes the performance override
 * in UCCRSettingsSubsystem.
 *
 * UE5 quality CVars applied per tier:
 *   sg.AntiAliasingQuality, sg.FoliageQuality, sg.ReflectionQuality,
 *   sg.GlobalIlluminationQuality, r.AntiAliasingMethod (TSR/TAA/FXAA),
 *   r.Lumen.DiffuseIndirect.Allow, r.Lumen.Reflections.Allow,
 *   r.Nanite.Allow, t.MaxFPS (frame rate ceiling per tier).
 */
UCLASS()
class CCR_API UCCRPerformanceGovernorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, Category = "CCR|Performance")
	ECCRPerformanceTier GetPerformanceTier() const { return Tier; }

	/**
	 * Maximum number of predicted chunks to keep in memory.
	 * High=3, Mid=2, Low=1.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Performance")
	int32 GetMaxPredictedChunks() const;

	/**
	 * Target FPS ceiling for the current tier.
	 * High=60, Mid=60, Low=30.
	 * Matches the t.MaxFPS value applied by ApplyQualitySettings().
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Performance")
	int32 GetTargetFPS() const;

	/**
	 * Push scalability CVar values to the engine to match the current tier.
	 * Covers all UE5 quality groups including Lumen, Nanite, TSR, and FPS cap.
	 * Called automatically on Initialize and whenever settings change.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Performance")
	void ApplyQualitySettings();

private:
	ECCRPerformanceTier Tier = ECCRPerformanceTier::Mid;

	void DetectTier();

	UFUNCTION()
	void OnSettingsChanged();
};
