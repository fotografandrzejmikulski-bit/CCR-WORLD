#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CCRInventoryItemDefinition.generated.h"

/**
 * UCCRInventoryItemDefinition
 *
 * Primary data asset describing an inventory item in CCR WORLD.
 *
 * Register items in the Asset Manager under PrimaryAssetType "CCRInventoryItem".
 * Each asset's PrimaryAssetLabel should match the FName ItemId used by
 * UCCRInventorySubsystem (e.g. "KEY_CAVE" → asset named DA_Item_KEY_CAVE).
 *
 * Blueprint WBP_CCRInventoryItem and UCCRInventoryWidget use this data to
 * display the item name, icon, and description without hardcoding strings.
 *
 * Usage from Blueprint / C++:
 * @code
 *   FPrimaryAssetId AssetId("CCRInventoryItem", ItemId);
 *   UAssetManager::Get().LoadPrimaryAsset(AssetId, {}, Callback);
 * @endcode
 */
UCLASS(BlueprintType)
class CCR_API UCCRInventoryItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Item ID — must match the FName used in UCCRInventorySubsystem::AddItem(). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Item")
	FName ItemId;

	/** Localised display name shown in the inventory UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Item")
	FText DisplayName;

	/** Longer localised description (one sentence maximum for mobile legibility). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Item")
	FText Description;

	/**
	 * Icon texture displayed in the inventory grid.
	 * Soft-referenced to avoid loading all icons at once.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Item")
	TSoftObjectPtr<UTexture2D> Icon;

	/**
	 * When true, this item can be "used" from the inventory.
	 * Fires OnItemUsed() on the UCCRInventoryWidget when tapped.
	 * The Blueprint or C++ subclass handles the actual use effect.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Item")
	bool bCanUse = false;

	/**
	 * When true, the item is consumed (count decremented by 1) on use.
	 * Requires bCanUse = true.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Item")
	bool bConsumedOnUse = false;

	/**
	 * World-state flag to set when the item is used.
	 * Useful for triggering narrative consequences from inventory use
	 * without additional Blueprint code.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Item")
	FName UseWorldStateFlag;

	// ---- UPrimaryDataAsset ----
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	static const FPrimaryAssetType ItemAssetType;
};
