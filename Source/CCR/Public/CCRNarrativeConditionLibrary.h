#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CCRNarrativeConditionLibrary.generated.h"

/**
 * UCCRNarrativeConditionLibrary
 *
 * Blueprint Function Library providing convenient condition-check helpers for
 * use in BP dialogue nodes, UI, and event graphs.
 *
 * All functions query the appropriate CCR subsystem via the WorldContextObject
 * and are safe to call from any Blueprint context (widget, actor, etc.).
 *
 * Available checks:
 *   - World-state flags / floats / ints
 *   - Inventory items
 *   - Achievements
 *   - Current narrative position (chunk / node / phase)
 */
UCLASS()
class CCR_API UCCRNarrativeConditionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ---- World State ----

	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static bool WorldStateFlag(const UObject* WorldContextObject, FName Key);

	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static float WorldStateFloat(const UObject* WorldContextObject, FName Key, float DefaultValue = 0.f);

	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static int32 WorldStateInt(const UObject* WorldContextObject, FName Key, int32 DefaultValue = 0);

	// ---- Inventory ----

	/** Returns true when the player has at least one of the given item. */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static bool HasItem(const UObject* WorldContextObject, FName ItemId);

	/** Returns true when the player has at least Count of the given item. */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static bool HasItems(const UObject* WorldContextObject, FName ItemId, int32 Count);

	/** Returns the current item count. */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static int32 ItemCount(const UObject* WorldContextObject, FName ItemId);

	// ---- Achievements ----

	/** Returns true when the achievement has been unlocked. */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static bool IsAchievementUnlocked(const UObject* WorldContextObject, FName AchievementId);

	// ---- Narrative position ----

	/** Returns the current chunk ID. */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static FName CurrentChunkId(const UObject* WorldContextObject);

	/** Returns the current node ID. */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static FName CurrentNodeId(const UObject* WorldContextObject);

	/** Returns true when the current chunk matches ChunkId. */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static bool IsInChunk(const UObject* WorldContextObject, FName ChunkId);

	// ---- Game phase ----

	/**
	 * Returns the current ECCRGamePhase as an integer for BP switch nodes.
	 * Cast the result to ECCRGamePhase in Blueprint.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Condition",
	          meta = (WorldContext = "WorldContextObject"))
	static uint8 GetGamePhaseAsInt(const UObject* WorldContextObject);
};
