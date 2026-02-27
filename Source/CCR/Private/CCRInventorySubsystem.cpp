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
	// ClearAll() has no base implementation because inventory counts are stored
	// in UCCRWorldStateSubsystemV2 (keys: "INV_<ItemId>"), which does not expose
	// an iteration API for bulk deletion.  This is intentional.
	// To clear inventory, either:
	//   (a) Override this method in a subclass that maintains a registered item
	//       list and calls UCCRWorldStateSubsystemV2::SetInt("INV_<Id>", 0) for each.
	//   (b) Use UCCRWorldStateSubsystemV2::ImportState() to reset the entire world state.
	UE_LOG(LogTemp, Warning,
		TEXT("UCCRInventorySubsystem::ClearAll() has no base-class implementation. "
			 "Override in a subclass or use UCCRWorldStateSubsystemV2::ImportState() "
			 "to reset the world state."));
}
