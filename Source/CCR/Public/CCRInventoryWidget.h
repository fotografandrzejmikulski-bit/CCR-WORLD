#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCRInventoryItemDefinition.h"
#include "CCRInventoryWidget.generated.h"

/**
 * UCCRInventoryWidget
 *
 * C++ base class for the in-game inventory screen.
 * Blueprint WBP_CCRInventory should derive from this class.
 *
 * NativeConstruct subscribes to UCCRInventorySubsystem::OnItemChanged
 * and calls a full refresh so the item grid is up-to-date when opened.
 *
 * Usage flow:
 *   1. HUD shows this widget (opened from pause menu or dedicated inventory button).
 *   2. NativeConstruct fires OnInventoryRefresh(ItemIds, Counts).
 *   3. Blueprint populates a scroll box with item cards.
 *   4. Player taps an item → Blueprint calls SelectItem(ItemId).
 *   5. SelectItem fires OnItemSelected(Def, Count).
 *   6. Player taps "Use" → Blueprint calls UseItem(ItemId).
 *   7. UseItem consumes the item, applies world-state flag, fires OnItemUsed.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Called when the inventory contents change or the widget is first shown.
	 * Override in Blueprint to rebuild the item grid.
	 * @param ItemIds  All item IDs that have at least 1 item in inventory.
	 * @param Counts   Matching counts for each ItemId.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Inventory")
	void OnInventoryRefresh(const TArray<FName>& ItemIds, const TArray<int32>& Counts);

	/**
	 * Called when the player taps an inventory item.
	 * Override in Blueprint to show the item detail panel.
	 * @param ItemDef  Loaded item definition (may be null if no DA_Item asset exists).
	 * @param Count    Current item count.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Inventory")
	void OnItemSelected(UCCRInventoryItemDefinition* ItemDef, int32 Count);

	/**
	 * Called after UseItem() successfully uses an item.
	 * Override in Blueprint to play a use animation or update the detail panel.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Inventory")
	void OnItemUsed(FName ItemId);

	/** Refresh the inventory display. Fires OnInventoryRefresh. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Inventory")
	void Refresh();

	/**
	 * Select an item to view its details.
	 * Loads the UCCRInventoryItemDefinition for ItemId (if registered in AssetManager)
	 * then fires OnItemSelected.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Inventory")
	void SelectItem(FName ItemId);

	/**
	 * Use an item.
	 * - Checks bCanUse on the item definition.
	 * - Applies the UseWorldStateFlag if set.
	 * - Decrements count if bConsumedOnUse.
	 * - Fires OnItemUsed.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Inventory")
	void UseItem(FName ItemId);

	/** Close (collapse) the inventory widget. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Inventory")
	void Close();

private:
	UFUNCTION()
	void HandleItemChanged(FName ItemId, int32 Delta, int32 NewCount);
};
