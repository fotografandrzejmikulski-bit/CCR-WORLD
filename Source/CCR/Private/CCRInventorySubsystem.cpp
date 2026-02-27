#include "CCRInventorySubsystem.h"
#include "CCRWorldStateSubsystemV2.h"

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

FName UCCRInventorySubsystem::InventoryKey(FName ItemId)
{
	return FName(*FString::Printf(TEXT("INV_%s"), *ItemId.ToString()));
}

// ---------------------------------------------------------------------------
// Add
// ---------------------------------------------------------------------------

void UCCRInventorySubsystem::AddItem(FName ItemId, int32 Count)
{
	if (ItemId.IsNone() || Count <= 0) return;

	UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!WSM) return;

	const FName Key = InventoryKey(ItemId);
	const int32 OldCount = WSM->GetInt(Key);
	const int32 NewCount = OldCount + Count;
	WSM->SetInt(Key, NewCount);

	KnownItems.Add(ItemId);
	OnItemChanged.Broadcast(ItemId, Count, NewCount);
}

// ---------------------------------------------------------------------------
// Remove
// ---------------------------------------------------------------------------

bool UCCRInventorySubsystem::RemoveItem(FName ItemId, int32 Count)
{
	if (ItemId.IsNone() || Count <= 0) return false;

	UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!WSM) return false;

	const FName Key = InventoryKey(ItemId);
	const int32 OldCount = WSM->GetInt(Key);

	if (OldCount < Count)
	{
		OnItemRemoveFailed.Broadcast(ItemId, Count);
		return false;
	}

	const int32 NewCount = OldCount - Count;
	WSM->SetInt(Key, NewCount);
	OnItemChanged.Broadcast(ItemId, -Count, NewCount);
	return true;
}

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------

int32 UCCRInventorySubsystem::GetItemCount(FName ItemId) const
{
	if (ItemId.IsNone()) return 0;

	UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!WSM) return 0;

	return WSM->GetInt(InventoryKey(ItemId));
}

bool UCCRInventorySubsystem::HasItem(FName ItemId) const
{
	return GetItemCount(ItemId) > 0;
}

bool UCCRInventorySubsystem::HasItems(FName ItemId, int32 Count) const
{
	return GetItemCount(ItemId) >= Count;
}

// ---------------------------------------------------------------------------
// ClearAll
// ---------------------------------------------------------------------------

void UCCRInventorySubsystem::ClearAll()
{
	UGameInstance* GI = GetGameInstance();
	UCCRWorldStateSubsystemV2* WSM = GI ? GI->GetSubsystem<UCCRWorldStateSubsystemV2>() : nullptr;

	if (WSM)
	{
		for (const FName& ItemId : KnownItems)
		{
			const int32 OldCount = WSM->GetInt(InventoryKey(ItemId));
			if (OldCount != 0)
			{
				WSM->SetInt(InventoryKey(ItemId), 0);
				OnItemChanged.Broadcast(ItemId, -OldCount, 0);
			}
		}
	}

	KnownItems.Reset();
}
