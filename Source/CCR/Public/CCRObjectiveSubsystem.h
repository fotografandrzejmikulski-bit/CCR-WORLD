#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRObjectiveSubsystem.generated.h"

/**
 * The completion state of a single objective.
 */
UENUM(BlueprintType)
enum class ECCRObjectiveState : uint8
{
	Hidden    UMETA(DisplayName = "Hidden"),    // not yet revealed to the player
	Active    UMETA(DisplayName = "Active"),    // currently visible / in progress
	Completed UMETA(DisplayName = "Completed"), // finished successfully
	Failed    UMETA(DisplayName = "Failed"),    // failed / obsolete
};

/**
 * Definition + current state of a narrative objective.
 */
USTRUCT(BlueprintType)
struct FCCRObjective
{
	GENERATED_BODY()

	/** Unique objective identifier (e.g. "OBJ_FindKey"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ObjectiveId;

	/** Short localised title displayed in the objective list. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	/** Optional longer description. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/** Current state. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCRObjectiveState State = ECCRObjectiveState::Hidden;

	/**
	 * If set, the objective is backed by a world-state flag.
	 * When UCCRObjectiveSubsystem::Sync() is called, the objective state is
	 * updated from the flag: flag=true → Completed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CompletionWorldStateFlag;
};

/**
 * UCCRObjectiveSubsystem
 *
 * Tracks player objectives (quests / tasks) for CCR WORLD.
 *
 * Objectives are registered at startup (via UCCRGameInstance::OnCCRInit).
 * The subsystem persists objective states as world-state flags ("OBJ_<Id>_STATE")
 * so they survive save/load cycles without a separate save slot.
 *
 * API summary:
 *   RegisterObjective(Def)       Register an objective definition.
 *   SetObjectiveState(Id, State) Manually set a state; fires OnObjectiveChanged.
 *   GetObjectiveState(Id)        Query the current state.
 *   GetAllObjectives()           Returns all objectives (in registration order).
 *   GetActiveObjectives()        Returns only Active objectives.
 *   Sync()                       Re-evaluate all backed objectives from world state.
 */
UCLASS(BlueprintType)
class CCR_API UCCRObjectiveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Register an objective definition. Overwrites if the ID already exists. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Objective")
	void RegisterObjective(const FCCRObjective& Def);

	/** Change the state of an objective. Fires OnObjectiveChanged. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Objective")
	void SetObjectiveState(FName ObjectiveId, ECCRObjectiveState NewState);

	/** Returns the current state of an objective. Returns Hidden when not found. */
	UFUNCTION(BlueprintPure, Category = "CCR|Objective")
	ECCRObjectiveState GetObjectiveState(FName ObjectiveId) const;

	/** Returns all registered objectives in registration order. */
	UFUNCTION(BlueprintPure, Category = "CCR|Objective")
	TArray<FCCRObjective> GetAllObjectives() const;

	/** Returns only objectives in the Active state. */
	UFUNCTION(BlueprintPure, Category = "CCR|Objective")
	TArray<FCCRObjective> GetActiveObjectives() const;

	/**
	 * Re-evaluate all objectives that have a CompletionWorldStateFlag set.
	 * Call after loading a save to restore objective states from world state flags.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Objective")
	void Sync();

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCCRObjectiveChanged,
	                                             const FCCRObjective&, Objective,
	                                             ECCRObjectiveState, NewState);

	/** Fired whenever an objective changes state. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Objective")
	FOnCCRObjectiveChanged OnObjectiveChanged;

private:
	TArray<FName> RegistrationOrder;
	TMap<FName, FCCRObjective> Objectives;

	static FName ObjectiveStateKey(FName ObjectiveId);
};
