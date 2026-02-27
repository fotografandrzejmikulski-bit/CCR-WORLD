#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRResumeSubsystem.generated.h"

/**
 * UCCRResumeSubsystem
 *
 * Handles loading a save and resuming the game from the default save slot.
 * Flow: load save -> import WSM state -> open level -> apply spatial -> start narrative.
 */
UCLASS()
class CCR_API UCCRResumeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Load the default save slot and resume the game.
	 * Returns false if no save exists.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Resume")
	bool ResumeFromDefaultSlot();

private:
	/** Called after the level finishes loading to apply spatial data and start narrative. */
	void OnLevelLoaded();

	FName PendingChunkId;
	FName PendingNodeId;
};
