#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRSaveSlotManagerSubsystem.generated.h"

/**
 * Metadata record for a single save slot shown in a save/load UI.
 */
USTRUCT(BlueprintType)
struct FCCRSaveSlotInfo
{
	GENERATED_BODY()

	/** Slot index (0-based). Matches the UserIndex parameter of USaveGame APIs. */
	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = 0;

	/** True when a save file exists for this slot. */
	UPROPERTY(BlueprintReadOnly)
	bool bExists = false;

	/** Chapter / chunk display name at the time of the save. */
	UPROPERTY(BlueprintReadOnly)
	FName ChunkId;

	/** Node ID at the time of the save. */
	UPROPERTY(BlueprintReadOnly)
	FName NodeId;

	/** Human-readable date + time of the save (local time). */
	UPROPERTY(BlueprintReadOnly)
	FString SaveDateTimeString;

	/** Cumulative play-time in seconds at the time of the save. */
	UPROPERTY(BlueprintReadOnly)
	float PlayTimeSec = 0.f;
};

/**
 * UCCRSaveSlotManagerSubsystem
 *
 * Manages multiple named save slots with human-readable metadata.
 * Built on top of UCCRWorldStateSaveGameV2 (the slot name is parameterised).
 *
 * CCR WORLD uses slot names "CCRSave_0", "CCRSave_1", ..., "CCRSave_N".
 * The settings slot is stored separately and not managed here.
 *
 * API overview:
 *   GetSlotCount()          → total number of manageable slots
 *   GetSlotInfo(SlotIndex)  → metadata for a slot (always returns a struct)
 *   GetAllSlotInfos()       → list of FCCRSaveSlotInfo for all slots
 *   SaveToSlot(SlotIndex)   → write current game state to a slot
 *   LoadFromSlot(SlotIndex) → restore game state from a slot
 *   DeleteSlot(SlotIndex)   → delete a save slot
 *
 * Slot 0 is the "auto-save" slot written by UCCRCheckpointSubsystem.
 * Slots 1..N are manual save slots for the player.
 */
UCLASS(BlueprintType)
class CCR_API UCCRSaveSlotManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Number of save slots (auto-save + manual saves). Default: 4 (slot 0-3). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Save",
	          meta = (ClampMin = "1", ClampMax = "20"))
	int32 SlotCount = 4;

	/** Returns the save slot name string for a given slot index. */
	UFUNCTION(BlueprintPure, Category = "CCR|Save")
	FString GetSlotName(int32 SlotIndex) const;

	/** Returns metadata for the given slot (bExists = false when no save is present). */
	UFUNCTION(BlueprintCallable, Category = "CCR|Save")
	FCCRSaveSlotInfo GetSlotInfo(int32 SlotIndex) const;

	/** Returns metadata for all slots. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Save")
	TArray<FCCRSaveSlotInfo> GetAllSlotInfos() const;

	/**
	 * Save current game state (narrative position + world state + player spatial)
	 * to the given slot.  Returns true on success.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Save")
	bool SaveToSlot(int32 SlotIndex);

	/**
	 * Load game state from the given slot and resume.
	 * Returns false when the slot does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Save")
	bool LoadFromSlot(int32 SlotIndex);

	/**
	 * Delete the save file for the given slot.
	 * Returns false when the slot does not exist.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Save")
	bool DeleteSlot(int32 SlotIndex);

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCRSaveSlotChanged, int32, SlotIndex);

	/** Fired after a save or delete operation changes a slot's state. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Save")
	FOnCCRSaveSlotChanged OnSaveSlotChanged;
};
