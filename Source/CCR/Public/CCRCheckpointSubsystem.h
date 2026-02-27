#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRCheckpointSubsystem.generated.h"

struct FCCRPlayerSpatialSave;

/**
 * UCCRCheckpointSubsystem
 *
 * Listens for OnCheckpointRequested events from UCCRNarrativeRuntimeSubsystem.
 * Writes the save game only when MarkSafeWindow() is called, to avoid saving
 * mid-cinematic or mid-animation.
 *
 * SetActiveSpawnTag() lets level Blueprint (or overlap triggers placed near
 * ACCRSpawnPoint actors) record which spawn point the player is currently
 * closest to.  That tag is written to FCCRPlayerSpatialSave::SpawnTag and used
 * by UCCRResumeSubsystem to re-position the player via UCCRSpawnSubsystem
 * instead of relying on a raw serialised transform.
 */
UCLASS()
class CCR_API UCCRCheckpointSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Signal that the game is in a safe state for saving.
	 * If a checkpoint is pending, it will be committed now.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Checkpoint")
	void MarkSafeWindow();

	/** Manually request a checkpoint at the current narrative position. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Checkpoint")
	void RequestCheckpoint(FName NodeId);

	/**
	 * Set the spawn tag that should be written into the next checkpoint save.
	 * Call this from a level Blueprint overlap event or a trigger actor when
	 * the player enters the zone of an ACCRSpawnPoint.
	 * Pass NAME_None to clear (the raw PlayerTransform will be used instead).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Checkpoint")
	void SetActiveSpawnTag(FName SpawnTag);

	/** Returns the currently active spawn tag (NAME_None if not set). */
	UFUNCTION(BlueprintPure, Category = "CCR|Checkpoint")
	FName GetActiveSpawnTag() const { return ActiveSpawnTag; }

private:
	bool bPendingCheckpoint = false;
	FName PendingNodeId;
	FName ActiveSpawnTag;

	void CommitCheckpoint();

	UFUNCTION()
	void OnCheckpointRequested(FName NodeId);
};
