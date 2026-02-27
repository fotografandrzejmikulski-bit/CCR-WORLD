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
	 * Push scalability CVar values to the engine to match the current tier.
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
