#include "CCRDebugSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRStoryRegistrySubsystem.h"
#include "CCRInventorySubsystem.h"
#include "CCRAchievementSubsystem.h"
#include "CCRStoryChunk.h"
#include "Engine/AssetManager.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

bool UCCRDebugSubsystem::IsAllowed() const
{
#if UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Warning, TEXT("UCCRDebugSubsystem: Debug call ignored in Shipping build."));
	return false;
#else
	if (!bDebugEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCCRDebugSubsystem: bDebugEnabled is false; call ignored."));
		return false;
	}
	return true;
#endif
}

// ---------------------------------------------------------------------------
// DumpWorldState
// ---------------------------------------------------------------------------

void UCCRDebugSubsystem::DumpWorldState()
{
	if (!IsAllowed()) return;

	UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!WSM)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CCRDebug] World state subsystem not available."));
		return;
	}

	// Export all keys and values to the log
	TArray<FName>                OutKeys;
	TArray<ECCRStateValueType>   OutTypes;
	TArray<int32>                OutIndices;
	TArray<uint64>               OutFlagBits;
	TArray<float>                OutFloats;
	TArray<int32>                OutInts;

	WSM->ExportState(OutKeys, OutTypes, OutIndices, OutFlagBits, OutFloats, OutInts);

	UE_LOG(LogTemp, Log, TEXT("[CCRDebug] === World State Dump ==="));
	for (int32 i = 0; i < OutKeys.Num(); ++i)
	{
		switch (OutTypes[i])
		{
		case ECCRStateValueType::Flag:
		{
			const bool bVal = WSM->GetFlag(OutKeys[i]);
			UE_LOG(LogTemp, Log, TEXT("  FLAG  %s = %s"), *OutKeys[i].ToString(), bVal ? TEXT("true") : TEXT("false"));
			break;
		}
		case ECCRStateValueType::Float:
			UE_LOG(LogTemp, Log, TEXT("  FLOAT %s = %f"), *OutKeys[i].ToString(), WSM->GetFloat(OutKeys[i]));
			break;
		case ECCRStateValueType::Int:
			UE_LOG(LogTemp, Log, TEXT("  INT   %s = %d"), *OutKeys[i].ToString(), WSM->GetInt(OutKeys[i]));
			break;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("[CCRDebug] === End Dump ==="));
}

// ---------------------------------------------------------------------------
// DumpNarrativeState
// ---------------------------------------------------------------------------

void UCCRDebugSubsystem::DumpNarrativeState()
{
	if (!IsAllowed()) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CCRDebug] NRS not available."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[CCRDebug] Narrative: axis=%s chunk=%s node=%s"),
		*NRS->GetCurrentAxisId().ToString(),
		*NRS->GetCurrentChunkId().ToString(),
		*NRS->GetCurrentNodeId().ToString());
}

// ---------------------------------------------------------------------------
// JumpToNode
// ---------------------------------------------------------------------------

void UCCRDebugSubsystem::JumpToNode(FName ChunkId, FName NodeId)
{
	if (!IsAllowed()) return;

	UCCRStoryRegistrySubsystem* Registry = GetGameInstance()->GetSubsystem<UCCRStoryRegistrySubsystem>();
	UCCRNarrativeRuntimeSubsystem* NRS   = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!Registry || !NRS) return;

	const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(ChunkId);
	if (!AssetId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CCRDebug] JumpToNode: chunk '%s' not found in registry."), *ChunkId.ToString());
		return;
	}

	UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(UAssetManager::Get().GetPrimaryAssetObject(AssetId));
	if (!Chunk)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CCRDebug] JumpToNode: chunk '%s' is not currently loaded. Load it first."),
			*ChunkId.ToString());
		return;
	}

	NRS->StartChunk(Chunk, NodeId);
}

// ---------------------------------------------------------------------------
// World State setters
// ---------------------------------------------------------------------------

void UCCRDebugSubsystem::SetWorldStateFlag(FName Key, bool bValue)
{
	if (!IsAllowed()) return;
	if (UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		WSM->SetFlag(Key, bValue);
	}
}

void UCCRDebugSubsystem::SetWorldStateFloat(FName Key, float Value)
{
	if (!IsAllowed()) return;
	if (UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		WSM->SetFloat(Key, Value);
	}
}

void UCCRDebugSubsystem::SetWorldStateInt(FName Key, int32 Value)
{
	if (!IsAllowed()) return;
	if (UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		WSM->SetInt(Key, Value);
	}
}

// ---------------------------------------------------------------------------
// Inventory / Achievement cheats
// ---------------------------------------------------------------------------

void UCCRDebugSubsystem::GiveItem(FName ItemId, int32 Count)
{
	if (!IsAllowed()) return;
	if (UCCRInventorySubsystem* Inv = GetGameInstance()->GetSubsystem<UCCRInventorySubsystem>())
	{
		Inv->AddItem(ItemId, Count);
	}
}

void UCCRDebugSubsystem::UnlockAchievement(FName AchievementId)
{
	if (!IsAllowed()) return;
	if (UCCRAchievementSubsystem* Ach = GetGameInstance()->GetSubsystem<UCCRAchievementSubsystem>())
	{
		Ach->UnlockAchievement(AchievementId);
	}
}
