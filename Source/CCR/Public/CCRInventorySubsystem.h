#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRInventorySubsystem.generated.h"

/**
 * UCCRInventorySubsystem
 *
 * Lightweight item-count inventory backed by UCCRWorldStateSubsystemV2.
 *
 * Items are stored as world-state integers under the key "INV_<ItemId>" so
 * that they are automatically persisted and restored via the existing checkpoint
 * / resume flow without any additional save logic.
 *
 * Narrative conditions can therefore branch on inventory directly using
 * ECCRStateValueType::Int with key "INV_<ItemId>".  UCCRInventorySubsystem
 * provides a friendlier API and fires delegates so that the HUD / inventory
 * UI can react to item pickups and removals.
 *
 * Usage from Blueprint:
 * @code
 *   // Pick up 1 key item
 *   InventorySubsystem->AddItem("KEY_CAVE", 1);
 *   // Check before opening door
 *   if (InventorySubsystem->HasItem("KEY_CAVE")) { ... }
 *   // Consume after use
 *   InventorySubsystem->RemoveItem("KEY_CAVE", 1);
 * @endcode
 */
UCLASS(BlueprintType)
class CCR_API UCCRInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Add Count items to the inventory.
	 * Count must be > 0; negative values are ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Inventory")
	void AddItem(FName ItemId, int32 Count = 1);

	/**
	 * Remove Count items from the inventory.
	 * Count must be > 0; negative values are ignored.
	 * Returns true if the removal succeeded (sufficient items were present).
	 * Returns false when there are fewer items than Count; in this case no
	 * items are removed and OnItemRemoveFailed is broadcast.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Inventory")
	bool RemoveItem(FName ItemId, int32 Count = 1);

	/**
	 * Returns the current count of ItemId in the inventory.
	 * Returns 0 when the item has never been added.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Inventory")
	int32 GetItemCount(FName ItemId) const;

	/**
	 * Returns true when at least one unit of ItemId is in the inventory.
	 * Equivalent to GetItemCount(ItemId) > 0.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Inventory")
	bool HasItem(FName ItemId) const;

	/**
	 * Returns true when Count or more units of ItemId are in the inventory.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Inventory")
	bool HasItems(FName ItemId, int32 Count) const;

	/** Clear all items (e.g. after loading a save with no inventory). */
	UFUNCTION(BlueprintCallable, Category = "CCR|Inventory")
	void ClearAll();

	// ---- Delegates ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCCRItemChanged,
	                                               FName, ItemId,
	                                               int32, Delta,
	                                               int32, NewCount);

	/** Fired when items are added (+Delta) or removed (-Delta). */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Inventory")
	FOnCCRItemChanged OnItemChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCCRItemRemoveFailed,
	                                             FName, ItemId,
	                                             int32, RequestedCount);

	/** Fired when RemoveItem() fails because the inventory has insufficient items. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Inventory")
	FOnCCRItemRemoveFailed OnItemRemoveFailed;

private:
	/** Returns the world-state Int key for a given item ID. */
	static FName InventoryKey(FName ItemId);
};
