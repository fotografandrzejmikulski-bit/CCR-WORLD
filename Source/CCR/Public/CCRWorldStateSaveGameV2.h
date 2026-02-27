#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "GameFramework/SaveGame.h"
#include "CCRWorldStateSaveGameV2.generated.h"

/**
 * UCCRWorldStateSaveGameV2
 *
 * Versioned save game object (SaveVersion = 2).
 * Stores narrative position, world state registry + values, and player spatial data.
 */
UCLASS(BlueprintType)
class CCR_API UCCRWorldStateSaveGameV2 : public USaveGame
{
	GENERATED_BODY()

public:
	UCCRWorldStateSaveGameV2();

	// ---- Version ----
	UPROPERTY(VisibleAnywhere, Category = "CCR|Save")
	int32 SaveVersion = 2;

	// ---- Narrative position ----
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Save")
	FName CurrentAxisId;

	UPROPERTY(BlueprintReadWrite, Category = "CCR|Save")
	FName CurrentChunkId;

	UPROPERTY(BlueprintReadWrite, Category = "CCR|Save")
	FName CurrentNodeId;

	// ---- World State registry ----
	UPROPERTY()
	TArray<FName> WSKeys;

	UPROPERTY()
	TArray<ECCRStateValueType> WSTypes;

	UPROPERTY()
	TArray<int32> WSIndices;

	// ---- World State values ----
	UPROPERTY()
	TArray<uint64> WSFlagBits;

	UPROPERTY()
	TArray<float> WSFloats;

	UPROPERTY()
	TArray<int32> WSInts;

	// ---- Player spatial ----
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Save")
	FCCRPlayerSpatialSave PlayerSpatial;

	// ---- Slot metadata ----

	/**
	 * Cumulative gameplay time in seconds at the point of the save.
	 * The calling code is responsible for updating this value;
	 * UCCRSaveSlotManagerSubsystem writes it from UCCRPlayTimeSubsystem if available.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Save")
	float PlayTimeSec = 0.f;

	/**
	 * Human-readable date-time string at the point of the save (local time).
	 * Format: "YYYY-MM-DD HH:MM:SS" (written by UCCRSaveSlotManagerSubsystem).
	 */
	UPROPERTY(BlueprintReadWrite, Category = "CCR|Save")
	FString SaveDateTime;
};
