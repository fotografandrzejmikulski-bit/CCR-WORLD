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
	// ClearAll() is intentionally a no-op in the C++ base class.
	// Inventory counts are stored as world-state integers (key: "INV_<ItemId>")
	// managed by UCCRWorldStateSubsystemV2, which does not expose an iteration
	// API.  To clear all inventory, either:
	//   (a) Call UCCRWorldStateSubsystemV2::ImportState() with a clean state, or
	//   (b) Override this function in a Blueprint or C++ subclass that maintains
	//       its own registered item list and calls SetInt("INV_<Id>", 0) for each.
	UE_LOG(LogTemp, Warning,
		TEXT("UCCRInventorySubsystem::ClearAll() was called but is not implemented "
			 "in the C++ base class.  Override in a subclass or use "
			 "UCCRWorldStateSubsystemV2::ImportState() to reset all inventory counts."));
}
