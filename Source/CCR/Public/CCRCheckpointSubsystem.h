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
 */
UCLASS()
class CCR_API UCCRCheckpointSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * Signal that the game is in a safe state for saving.
	 * If a checkpoint is pending, it will be committed now.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Checkpoint")
	void MarkSafeWindow();

	/** Manually request a checkpoint at the current narrative position. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Checkpoint")
	void RequestCheckpoint(FName NodeId);

private:
	bool bPendingCheckpoint = false;
	FName PendingNodeId;

	void CommitCheckpoint();

	UFUNCTION()
	void OnCheckpointRequested(FName NodeId);
};
