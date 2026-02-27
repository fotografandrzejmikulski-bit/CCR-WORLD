#include "CCRObjectiveSubsystem.h"
#include "CCRWorldStateSubsystemV2.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

FName UCCRObjectiveSubsystem::ObjectiveStateKey(FName ObjectiveId)
{
	return FName(*FString::Printf(TEXT("OBJ_%s_STATE"), *ObjectiveId.ToString()));
}

// ---------------------------------------------------------------------------
// RegisterObjective
// ---------------------------------------------------------------------------

void UCCRObjectiveSubsystem::RegisterObjective(const FCCRObjective& Def)
{
	if (Def.ObjectiveId.IsNone()) return;

	if (!Objectives.Contains(Def.ObjectiveId))
	{
		RegistrationOrder.Add(Def.ObjectiveId);
	}
	Objectives.Emplace(Def.ObjectiveId, Def);
}

// ---------------------------------------------------------------------------
// SetObjectiveState
// ---------------------------------------------------------------------------

void UCCRObjectiveSubsystem::SetObjectiveState(FName ObjectiveId, ECCRObjectiveState NewState)
{
	FCCRObjective* Obj = Objectives.Find(ObjectiveId);
	if (!Obj) return;

	if (Obj->State == NewState) return;
	Obj->State = NewState;

	// Persist the state as a world-state int so it survives save/load
	if (UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		WSM->SetInt(ObjectiveStateKey(ObjectiveId), static_cast<int32>(NewState));
	}

	OnObjectiveChanged.Broadcast(*Obj, NewState);
}

// ---------------------------------------------------------------------------
// GetObjectiveState
// ---------------------------------------------------------------------------

ECCRObjectiveState UCCRObjectiveSubsystem::GetObjectiveState(FName ObjectiveId) const
{
	const FCCRObjective* Obj = Objectives.Find(ObjectiveId);
	return Obj ? Obj->State : ECCRObjectiveState::Hidden;
}

// ---------------------------------------------------------------------------
// GetAllObjectives / GetActiveObjectives
// ---------------------------------------------------------------------------

TArray<FCCRObjective> UCCRObjectiveSubsystem::GetAllObjectives() const
{
	TArray<FCCRObjective> Result;
	Result.Reserve(RegistrationOrder.Num());
	for (const FName& Id : RegistrationOrder)
	{
		if (const FCCRObjective* Obj = Objectives.Find(Id))
		{
			Result.Add(*Obj);
		}
	}
	return Result;
}

TArray<FCCRObjective> UCCRObjectiveSubsystem::GetActiveObjectives() const
{
	TArray<FCCRObjective> Result;
	for (const FName& Id : RegistrationOrder)
	{
		if (const FCCRObjective* Obj = Objectives.Find(Id))
		{
			if (Obj->State == ECCRObjectiveState::Active)
			{
				Result.Add(*Obj);
			}
		}
	}
	return Result;
}

// ---------------------------------------------------------------------------
// Sync
// ---------------------------------------------------------------------------

void UCCRObjectiveSubsystem::Sync()
{
	UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!WSM) return;

	for (const FName& Id : RegistrationOrder)
	{
		FCCRObjective* Obj = Objectives.Find(Id);
		if (!Obj) continue;

		// Restore persisted state from world-state int
		const int32 StateInt = WSM->GetInt(ObjectiveStateKey(Id));
		if (StateInt > 0)
		{
			const ECCRObjectiveState RestoredState = static_cast<ECCRObjectiveState>(
				FMath::Clamp(StateInt, 0, static_cast<int32>(ECCRObjectiveState::Failed)));

			if (Obj->State != RestoredState)
			{
				Obj->State = RestoredState;
				OnObjectiveChanged.Broadcast(*Obj, RestoredState);
			}
		}

		// Override with completion flag if backed by world state
		if (!Obj->CompletionWorldStateFlag.IsNone())
		{
			if (WSM->GetFlag(Obj->CompletionWorldStateFlag))
			{
				if (Obj->State != ECCRObjectiveState::Completed)
				{
					Obj->State = ECCRObjectiveState::Completed;
					WSM->SetInt(ObjectiveStateKey(Id), static_cast<int32>(ECCRObjectiveState::Completed));
					OnObjectiveChanged.Broadcast(*Obj, ECCRObjectiveState::Completed);
				}
			}
		}
	}
}
