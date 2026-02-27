#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRWorldStateSubsystemV2.generated.h"

// ---------------------------------------------------------------------------
// Internal registry entry
// ---------------------------------------------------------------------------

USTRUCT()
struct FCCRStateEntry
{
	GENERATED_BODY()

	UPROPERTY()
	ECCRStateValueType Type = ECCRStateValueType::Flag;

	/** Index into the matching storage array (FlagBits word index for flags) */
	UPROPERTY()
	int32 Index = 0;
};

// ---------------------------------------------------------------------------
// UCCRWorldStateSubsystemV2
//
// Data-driven, cache-friendly World State Manager.
// Storage:
//   Flags  – packed into a TArray<uint64> bitset
//   Floats – TArray<float>
//   Ints   – TArray<int32>
// ---------------------------------------------------------------------------

UCLASS()
class CCR_API UCCRWorldStateSubsystemV2 : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ---- UGameInstanceSubsystem ----
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ---- Flag API ----
	UFUNCTION(BlueprintCallable, Category = "CCR|WorldState")
	bool GetFlag(FName Key) const;

	UFUNCTION(BlueprintCallable, Category = "CCR|WorldState")
	void SetFlag(FName Key, bool bValue);

	// ---- Condition evaluation ----

	/**
	 * Evaluates a list of FCCRCondition against the current world state.
	 * Returns true when ALL conditions pass (AND-semantics).
	 * An empty condition list always returns true.
	 * Use this instead of duplicating the switch logic in each consumer class.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|WorldState")
	bool EvaluateConditions(const TArray<FCCRCondition>& Conditions) const;

	// ---- Float API ----
	UFUNCTION(BlueprintCallable, Category = "CCR|WorldState")
	float GetFloat(FName Key, float DefaultValue = 0.f) const;

	UFUNCTION(BlueprintCallable, Category = "CCR|WorldState")
	void SetFloat(FName Key, float Value);

	// ---- Int API ----
	UFUNCTION(BlueprintCallable, Category = "CCR|WorldState")
	int32 GetInt(FName Key, int32 DefaultValue = 0) const;

	UFUNCTION(BlueprintCallable, Category = "CCR|WorldState")
	void SetInt(FName Key, int32 Value);

	// ---- Serialization ----

	/** Fill out arrays for saving */
	void ExportState(
		TArray<FName>& OutKeys,
		TArray<ECCRStateValueType>& OutTypes,
		TArray<int32>& OutIndices,
		TArray<uint64>& OutFlagBits,
		TArray<float>& OutFloats,
		TArray<int32>& OutInts) const;

	/** Restore state from saved arrays */
	void ImportState(
		const TArray<FName>& InKeys,
		const TArray<ECCRStateValueType>& InTypes,
		const TArray<int32>& InIndices,
		const TArray<uint64>& InFlagBits,
		const TArray<float>& InFloats,
		const TArray<int32>& InInts);

private:
	/** Key -> (Type, Index) registry */
	TMap<FName, FCCRStateEntry> Registry;

	/** Bitset storage for flags (64 flags per word) */
	TArray<uint64> FlagBits;

	/** Float value storage */
	TArray<float> Floats;

	/** Int value storage */
	TArray<int32> Ints;

	// helpers
	int32 FindOrAddFlag(FName Key);
	int32 FindOrAddFloat(FName Key);
	int32 FindOrAddInt(FName Key);
};
