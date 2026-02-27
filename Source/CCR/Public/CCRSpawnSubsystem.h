#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRSpawnSubsystem.generated.h"

/** Internal registry entry for UCCRSpawnSubsystem. */
USTRUCT()
struct FCCRSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator;
};

/**
 * UCCRSpawnSubsystem
 *
 * Registry of named spawn points placed in the level.
 *
 * ACCRSpawnPoint actors call RegisterSpawnPoint() / UnregisterSpawnPoint()
 * automatically on BeginPlay / EndPlay, so the registry is always current
 * for the loaded level.
 *
 * UCCRResumeSubsystem::OnLevelLoaded uses GetSpawnTransform() to position
 * the player at the saved SpawnTag instead of relying on a raw saved
 * FCCRPlayerSpatialSave::PlayerTransform, which may become stale after
 * level edits.
 *
 * Priority rule (applied by UCCRResumeSubsystem):
 *   1. SpawnTag is set AND a matching spawn point is registered → use it.
 *   2. SpawnTag is None OR no matching point found AND bHasSpatial → use PlayerTransform.
 *   3. Neither → do nothing (player spawns at default start transform).
 */
UCLASS()
class CCR_API UCCRSpawnSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Register a spawn point.  Called by ACCRSpawnPoint::BeginPlay().
	 * If a point with the same tag already exists it is overwritten so that
	 * loading a level twice does not leave stale entries.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Spawn")
	void RegisterSpawnPoint(FName SpawnTag, FVector Location, FRotator Rotation);

	/**
	 * Unregister a spawn point.  Called by ACCRSpawnPoint::EndPlay().
	 * Safe to call even if the tag was never registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Spawn")
	void UnregisterSpawnPoint(FName SpawnTag);

	/**
	 * Look up the transform for a given SpawnTag.
	 * Returns true and fills OutTransform when the tag is registered.
	 * Returns false when the tag is None or has no registered entry.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Spawn")
	bool GetSpawnTransform(FName SpawnTag, FTransform& OutTransform) const;

	/** Returns true when SpawnTag has a registered spawn point. */
	UFUNCTION(BlueprintPure, Category = "CCR|Spawn")
	bool HasSpawnPoint(FName SpawnTag) const;

	/** Returns all currently registered spawn tags. */
	UFUNCTION(BlueprintPure, Category = "CCR|Spawn")
	TArray<FName> GetAllSpawnTags() const;

	/** Remove all entries (called automatically when the game instance shuts down). */
	virtual void Deinitialize() override;

private:
	TMap<FName, FCCRSpawnEntry> Registry;
};
