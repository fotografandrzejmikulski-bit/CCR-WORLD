#include "CCRInventoryWidget.h"
#include "CCRInventorySubsystem.h"
#include "CCRWorldStateSubsystemV2.h"
#include "Engine/AssetManager.h"

void UCCRInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRInventorySubsystem* Inv = GI->GetSubsystem<UCCRInventorySubsystem>())
		{
			Inv->OnItemChanged.AddDynamic(this, &UCCRInventoryWidget::HandleItemChanged);
		}
	}

	Refresh();
}

void UCCRInventoryWidget::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRInventorySubsystem* Inv = GI->GetSubsystem<UCCRInventorySubsystem>())
		{
			Inv->OnItemChanged.RemoveDynamic(this, &UCCRInventoryWidget::HandleItemChanged);
		}
	}
	Super::NativeDestruct();
}

void UCCRInventoryWidget::HandleItemChanged(FName ItemId, int32 Delta, int32 NewCount)
{
	Refresh();
}

void UCCRInventoryWidget::Refresh()
{
	UGameInstance* GI = GetGameInstance();
	UCCRInventorySubsystem* Inv = GI ? GI->GetSubsystem<UCCRInventorySubsystem>() : nullptr;
	if (!Inv) return;

	// Scan all known "INV_<Id>" world-state keys to build the item list.
	// Inventory is backed by world state; we must query for known items.
	// Since we don't maintain a registered item list in the base class, we
	// expose all items via the AssetManager registered item definitions.
	TArray<FName> ItemIds;
	TArray<int32> Counts;

	// Ask AssetManager for all registered CCRInventoryItem primary assets.
	TArray<FPrimaryAssetId> AssetIds;
	UAssetManager::Get().GetPrimaryAssetIdList(UCCRInventoryItemDefinition::ItemAssetType, AssetIds);

	for (const FPrimaryAssetId& Aid : AssetIds)
	{
		const FName ItemId = Aid.PrimaryAssetName;
		const int32 Count  = Inv->GetItemCount(ItemId);
		if (Count > 0)
		{
			ItemIds.Add(ItemId);
			Counts.Add(Count);
		}
	}

	OnInventoryRefresh(ItemIds, Counts);
}

void UCCRInventoryWidget::SelectItem(FName ItemId)
{
	if (ItemId.IsNone()) return;

	UGameInstance* GI = GetGameInstance();
	UCCRInventorySubsystem* Inv = GI ? GI->GetSubsystem<UCCRInventorySubsystem>() : nullptr;
	if (!Inv) return;

	const int32 Count = Inv->GetItemCount(ItemId);

	// Load the item definition async then fire OnItemSelected
	const FPrimaryAssetId AssetId(UCCRInventoryItemDefinition::ItemAssetType, ItemId);
	if (AssetId.IsValid())
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			UAssetManager::Get().GetPrimaryAssetPath(AssetId),
			FStreamableDelegate::CreateWeakLambda(this, [this, AssetId, Count]()
			{
				UCCRInventoryItemDefinition* Def = Cast<UCCRInventoryItemDefinition>(
					UAssetManager::Get().GetPrimaryAssetObject(AssetId));
				OnItemSelected(Def, Count);
			}));
	}
	else
	{
		OnItemSelected(nullptr, Count);
	}
}

void UCCRInventoryWidget::UseItem(FName ItemId)
{
	if (ItemId.IsNone()) return;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	// Load the definition to check bCanUse
	const FPrimaryAssetId AssetId(UCCRInventoryItemDefinition::ItemAssetType, ItemId);
	UCCRInventoryItemDefinition* Def = Cast<UCCRInventoryItemDefinition>(
		UAssetManager::Get().GetPrimaryAssetObject(AssetId));

	if (!Def || !Def->bCanUse) return;

	// Apply world-state flag
	if (!Def->UseWorldStateFlag.IsNone())
	{
		if (UCCRWorldStateSubsystemV2* WSM = GI->GetSubsystem<UCCRWorldStateSubsystemV2>())
		{
			WSM->SetFlag(Def->UseWorldStateFlag, true);
		}
	}

	// Consume if applicable
	if (Def->bConsumedOnUse)
	{
		if (UCCRInventorySubsystem* Inv = GI->GetSubsystem<UCCRInventorySubsystem>())
		{
			Inv->RemoveItem(ItemId, 1);
		}
	}

	OnItemUsed(ItemId);
}

void UCCRInventoryWidget::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRInventoryWidget::OnInventoryRefresh_Implementation(const TArray<FName>& ItemIds, const TArray<int32>& Counts)
{
// Default C++ implementation: no-op.
}

void UCCRInventoryWidget::OnItemSelected_Implementation(UCCRInventoryItemDefinition* ItemDef, int32 Count)
{
// Default C++ implementation: no-op.
}

void UCCRInventoryWidget::OnItemUsed_Implementation(FName ItemId)
{
// Default C++ implementation: no-op.
}
