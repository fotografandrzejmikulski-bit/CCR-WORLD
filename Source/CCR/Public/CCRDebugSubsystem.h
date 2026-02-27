#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRDebugSubsystem.generated.h"

/**
 * UCCRDebugSubsystem
 *
 * In-game debug / cheat console for CCR WORLD.
 *
 * Provides:
 *   - State dump (print all world-state flags/floats/ints to the log)
 *   - Narrative jump (teleport to any ChunkId + NodeId)
 *   - Inventory manipulation (add/remove items)
 *   - Achievement unlock (unlock by ID without meeting conditions)
 *   - Toggle debug overlay (prints current chapter / node / world-state on HUD)
 *
 * Debug functions log a warning when called in a Shipping build so they are
 * safe to leave registered but have no gameplay impact on final builds.
 *
 * Enable/disable at runtime via bDebugEnabled. Binding to console commands
 * can be done in Blueprint (e.g. ~ key) by calling the UFUNCTION wrappers.
 */
UCLASS(BlueprintType)
class CCR_API UCCRDebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Master enable switch. Set to false in shipping-build Blueprint defaults. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Debug")
	bool bDebugEnabled = true;

	/**
	 * Print the current world-state (all flags, floats, ints) to UE_LOG.
	 * No-op in Shipping or when bDebugEnabled is false.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void DumpWorldState();

	/**
	 * Print the current narrative position (axis, chunk, node) to UE_LOG.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void DumpNarrativeState();

	/**
	 * Jump the narrative to the given chunk + node.
	 * Synchronously loads the chunk if it is already in memory (registry lookup);
	 * calls UE_LOG Warning and returns without changing state if not loaded.
	 * @param ChunkId  Target story chunk identifier.
	 * @param NodeId   Target node inside the chunk (uses chunk entry node if None).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void JumpToNode(FName ChunkId, FName NodeId = NAME_None);

	/**
	 * Set a world-state flag directly (bypass story conditions).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void SetWorldStateFlag(FName Key, bool bValue);

	/**
	 * Set a world-state float directly.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void SetWorldStateFloat(FName Key, float Value);

	/**
	 * Set a world-state int directly.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void SetWorldStateInt(FName Key, int32 Value);

	/**
	 * Add items to inventory directly (bypasses conditions / flags).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void GiveItem(FName ItemId, int32 Count = 1);

	/**
	 * Unlock an achievement directly (fires the normal OnAchievementUnlocked delegate).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Debug")
	void UnlockAchievement(FName AchievementId);

private:
	bool IsAllowed() const;
};
