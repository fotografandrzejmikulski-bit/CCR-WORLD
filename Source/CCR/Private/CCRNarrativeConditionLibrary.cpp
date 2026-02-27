#include "CCRNarrativeConditionLibrary.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRInventorySubsystem.h"
#include "CCRAchievementSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRGameState.h"
#include "Engine/World.h"

// ---------------------------------------------------------------------------
// Internal helper
// ---------------------------------------------------------------------------

static UGameInstance* CCR_GetGI(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	UWorld* World = WorldContextObject->GetWorld();
	return World ? World->GetGameInstance() : nullptr;
}

// ---------------------------------------------------------------------------
// World State
// ---------------------------------------------------------------------------

bool UCCRNarrativeConditionLibrary::WorldStateFlag(const UObject* WorldContextObject, FName Key)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return false;
	if (UCCRWorldStateSubsystemV2* WSM = GI->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		return WSM->GetFlag(Key);
	}
	return false;
}

float UCCRNarrativeConditionLibrary::WorldStateFloat(
	const UObject* WorldContextObject, FName Key, float DefaultValue)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return DefaultValue;
	if (UCCRWorldStateSubsystemV2* WSM = GI->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		return WSM->GetFloat(Key, DefaultValue);
	}
	return DefaultValue;
}

int32 UCCRNarrativeConditionLibrary::WorldStateInt(
	const UObject* WorldContextObject, FName Key, int32 DefaultValue)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return DefaultValue;
	if (UCCRWorldStateSubsystemV2* WSM = GI->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		return WSM->GetInt(Key, DefaultValue);
	}
	return DefaultValue;
}

// ---------------------------------------------------------------------------
// Inventory
// ---------------------------------------------------------------------------

bool UCCRNarrativeConditionLibrary::HasItem(const UObject* WorldContextObject, FName ItemId)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return false;
	if (UCCRInventorySubsystem* Inv = GI->GetSubsystem<UCCRInventorySubsystem>())
	{
		return Inv->HasItem(ItemId);
	}
	return false;
}

bool UCCRNarrativeConditionLibrary::HasItems(const UObject* WorldContextObject, FName ItemId, int32 Count)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return false;
	if (UCCRInventorySubsystem* Inv = GI->GetSubsystem<UCCRInventorySubsystem>())
	{
		return Inv->HasItems(ItemId, Count);
	}
	return false;
}

int32 UCCRNarrativeConditionLibrary::ItemCount(const UObject* WorldContextObject, FName ItemId)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return 0;
	if (UCCRInventorySubsystem* Inv = GI->GetSubsystem<UCCRInventorySubsystem>())
	{
		return Inv->GetItemCount(ItemId);
	}
	return 0;
}

// ---------------------------------------------------------------------------
// Achievements
// ---------------------------------------------------------------------------

bool UCCRNarrativeConditionLibrary::IsAchievementUnlocked(const UObject* WorldContextObject, FName AchievementId)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return false;
	if (UCCRAchievementSubsystem* Ach = GI->GetSubsystem<UCCRAchievementSubsystem>())
	{
		return Ach->IsAchievementUnlocked(AchievementId);
	}
	return false;
}

// ---------------------------------------------------------------------------
// Narrative position
// ---------------------------------------------------------------------------

FName UCCRNarrativeConditionLibrary::CurrentChunkId(const UObject* WorldContextObject)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return NAME_None;
	if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		return NRS->GetCurrentChunkId();
	}
	return NAME_None;
}

FName UCCRNarrativeConditionLibrary::CurrentNodeId(const UObject* WorldContextObject)
{
	UGameInstance* GI = CCR_GetGI(WorldContextObject);
	if (!GI) return NAME_None;
	if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		return NRS->GetCurrentNodeId();
	}
	return NAME_None;
}

bool UCCRNarrativeConditionLibrary::IsInChunk(const UObject* WorldContextObject, FName ChunkId)
{
	return CurrentChunkId(WorldContextObject) == ChunkId;
}

// ---------------------------------------------------------------------------
// Game phase
// ---------------------------------------------------------------------------

uint8 UCCRNarrativeConditionLibrary::GetGamePhaseAsInt(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return 0;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return 0;
	if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
	{
		return static_cast<uint8>(GS->GetGamePhase());
	}
	return 0;
}
