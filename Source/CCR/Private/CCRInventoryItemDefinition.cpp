#include "CCRInventoryItemDefinition.h"

const FPrimaryAssetType UCCRInventoryItemDefinition::ItemAssetType = TEXT("CCRInventoryItem");

FPrimaryAssetId UCCRInventoryItemDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemAssetType, ItemId);
}
