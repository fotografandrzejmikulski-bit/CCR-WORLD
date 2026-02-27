#include "CCRSaveSlotManagerSubsystem.h"
#include "CCR.h"
#include "CCRWorldStateSaveGameV2.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRPlayTimeSubsystem.h"
#include "CCRResumeSubsystem.h"
#include "Kismet/GameplayStatics.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

FString UCCRSaveSlotManagerSubsystem::GetSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("CCRSave_%d"), SlotIndex);
}

// ---------------------------------------------------------------------------
// GetSlotInfo
// ---------------------------------------------------------------------------

FCCRSaveSlotInfo UCCRSaveSlotManagerSubsystem::GetSlotInfo(int32 SlotIndex) const
{
	FCCRSaveSlotInfo Info;
	Info.SlotIndex = SlotIndex;

	const FString SlotName = GetSlotName(SlotIndex);
	const bool bExists = UGameplayStatics::DoesSaveGameExist(SlotName, /*UserIndex=*/0);
	Info.bExists = bExists;

	if (bExists)
	{
		UCCRWorldStateSaveGameV2* Save = Cast<UCCRWorldStateSaveGameV2>(
			UGameplayStatics::LoadGameFromSlot(SlotName, /*UserIndex=*/0));
		if (Save)
		{
			Info.ChunkId         = Save->CurrentChunkId;
			Info.NodeId          = Save->CurrentNodeId;
			Info.PlayTimeSec     = Save->PlayTimeSec;
			Info.SaveDateTimeString = Save->SaveDateTime;
		}
	}

	return Info;
}

TArray<FCCRSaveSlotInfo> UCCRSaveSlotManagerSubsystem::GetAllSlotInfos() const
{
	TArray<FCCRSaveSlotInfo> Infos;
	Infos.Reserve(SlotCount);
	for (int32 i = 0; i < SlotCount; ++i)
	{
		Infos.Add(GetSlotInfo(i));
	}
	return Infos;
}

// ---------------------------------------------------------------------------
// SaveToSlot
// ---------------------------------------------------------------------------

bool UCCRSaveSlotManagerSubsystem::SaveToSlot(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= SlotCount) return false;

	UGameInstance* GI = GetGameInstance();
	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	UCCRWorldStateSubsystemV2* WSM     = GI->GetSubsystem<UCCRWorldStateSubsystemV2>();
	if (!NRS || !WSM) return false;

	UCCRWorldStateSaveGameV2* Save = Cast<UCCRWorldStateSaveGameV2>(
		UGameplayStatics::CreateSaveGameObject(UCCRWorldStateSaveGameV2::StaticClass()));
	if (!Save) return false;

	// Narrative position
	Save->CurrentAxisId  = NRS->GetCurrentAxisId();
	Save->CurrentChunkId = NRS->GetCurrentChunkId();
	Save->CurrentNodeId  = NRS->GetCurrentNodeId();

	// World state
	WSM->ExportState(
		Save->WSKeys, Save->WSTypes, Save->WSIndices,
		Save->WSFlagBits, Save->WSFloats, Save->WSInts);

	// Metadata
	if (UCCRPlayTimeSubsystem* PT = GI->GetSubsystem<UCCRPlayTimeSubsystem>())
	{
		Save->PlayTimeSec = PT->GetPlayTimeSec();
	}
	Save->SaveDateTime = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));

	// Player spatial
	if (UWorld* World = GI->GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC && PC->GetPawn())
		{
			FCCRPlayerSpatialSave Spatial;
			Spatial.LevelName       = World->GetMapName();
			Spatial.PlayerTransform = PC->GetPawn()->GetActorTransform();
			const FRotator CtrlRot  = PC->GetControlRotation();
			Spatial.CameraYaw       = CtrlRot.Yaw;
			Spatial.CameraPitch     = CtrlRot.Pitch;
			Spatial.bHasSpatial     = true;
			Save->PlayerSpatial     = Spatial;
		}
	}

	const FString SlotName = GetSlotName(SlotIndex);
	// Override the save slot name so it writes to the correct slot
	Save->SaveSlotName = SlotName;

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(Save, SlotName, /*UserIndex=*/0);
	if (bSuccess)
	{
		OnSaveSlotChanged.Broadcast(SlotIndex);
	}
	return bSuccess;
}

// ---------------------------------------------------------------------------
// LoadFromSlot
// ---------------------------------------------------------------------------

bool UCCRSaveSlotManagerSubsystem::LoadFromSlot(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= SlotCount) return false;

	const FString SlotName = GetSlotName(SlotIndex);
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, /*UserIndex=*/0)) return false;

	// Delegate to UCCRResumeSubsystem but using this slot's name.
	// UCCRResumeSubsystem always uses the default slot (CCRSaveSlots::WorldState).
	// For multi-slot loading we replicate the resume logic here.
	UCCRWorldStateSaveGameV2* Save = Cast<UCCRWorldStateSaveGameV2>(
		UGameplayStatics::LoadGameFromSlot(SlotName, /*UserIndex=*/0));
	if (!Save) return false;

	UGameInstance* GI = GetGameInstance();

	// Restore world state
	if (UCCRWorldStateSubsystemV2* WSM = GI->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		WSM->ImportState(
			Save->WSKeys, Save->WSTypes, Save->WSIndices,
			Save->WSFlagBits, Save->WSFloats, Save->WSInts);
	}

	// Resume narrative via UCCRResumeSubsystem (it handles level opening + chunk loading)
	// We copy the save data into the default slot so Resume reads the correct data.
	Save->SaveSlotName = CCRSaveSlots::WorldState;
	UGameplayStatics::SaveGameToSlot(Save, CCRSaveSlots::WorldState, /*UserIndex=*/0);

	if (UCCRResumeSubsystem* Resume = GI->GetSubsystem<UCCRResumeSubsystem>())
	{
		return Resume->ResumeFromDefaultSlot();
	}
	return false;
}

// ---------------------------------------------------------------------------
// DeleteSlot
// ---------------------------------------------------------------------------

bool UCCRSaveSlotManagerSubsystem::DeleteSlot(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= SlotCount) return false;

	const FString SlotName = GetSlotName(SlotIndex);
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, /*UserIndex=*/0)) return false;

	const bool bSuccess = UGameplayStatics::DeleteGameInSlot(SlotName, /*UserIndex=*/0);
	if (bSuccess)
	{
		OnSaveSlotChanged.Broadcast(SlotIndex);
	}
	return bSuccess;
}
