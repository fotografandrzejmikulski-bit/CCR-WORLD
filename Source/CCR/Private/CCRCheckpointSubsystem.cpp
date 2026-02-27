#include "CCRCheckpointSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRWorldStateSaveGameV2.h"
#include "Kismet/GameplayStatics.h"

void UCCRCheckpointSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Subscribe to narrative checkpoint events
	if (UCCRNarrativeRuntimeSubsystem* NRS =
		GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		NRS->OnCheckpointRequested.AddDynamic(this, &UCCRCheckpointSubsystem::OnCheckpointRequested);
	}
}

void UCCRCheckpointSubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnCheckpointRequested.RemoveDynamic(this, &UCCRCheckpointSubsystem::OnCheckpointRequested);
		}
	}
	Super::Deinitialize();
}

void UCCRCheckpointSubsystem::OnCheckpointRequested(FName NodeId)
{
	bPendingCheckpoint = true;
	PendingNodeId = NodeId;
}

void UCCRCheckpointSubsystem::RequestCheckpoint(FName NodeId)
{
	bPendingCheckpoint = true;
	PendingNodeId = NodeId;
}

void UCCRCheckpointSubsystem::MarkSafeWindow()
{
	if (bPendingCheckpoint)
	{
		CommitCheckpoint();
	}
}

void UCCRCheckpointSubsystem::SetActiveSpawnTag(FName SpawnTag)
{
	ActiveSpawnTag = SpawnTag;
}

void UCCRCheckpointSubsystem::CommitCheckpoint()
{
	bPendingCheckpoint = false;

	UCCRNarrativeRuntimeSubsystem* NRS =
		GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	UCCRWorldStateSubsystemV2* WSM =
		GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>();

	if (!NRS || !WSM) return;

	UCCRWorldStateSaveGameV2* Save = Cast<UCCRWorldStateSaveGameV2>(
		UGameplayStatics::CreateSaveGameObject(UCCRWorldStateSaveGameV2::StaticClass()));
	if (!Save) return;

	// Narrative position
	Save->CurrentAxisId  = NRS->GetCurrentAxisId();
	Save->CurrentChunkId = NRS->GetCurrentChunkId();
	Save->CurrentNodeId  = PendingNodeId.IsNone() ? NRS->GetCurrentNodeId() : PendingNodeId;

	// World state
	WSM->ExportState(
		Save->WSKeys,
		Save->WSTypes,
		Save->WSIndices,
		Save->WSFlagBits,
		Save->WSFloats,
		Save->WSInts);

	// Player spatial
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC && PC->GetPawn())
		{
			FCCRPlayerSpatialSave Spatial;
			Spatial.LevelName       = World->GetMapName();
			Spatial.PlayerTransform = PC->GetPawn()->GetActorTransform();

			const FRotator CtrlRot = PC->GetControlRotation();
			Spatial.CameraYaw   = CtrlRot.Yaw;
			Spatial.CameraPitch = CtrlRot.Pitch;
			Spatial.SpawnTag    = ActiveSpawnTag;
			Spatial.bHasSpatial = true;

			Save->PlayerSpatial = Spatial;
		}
	}

	UGameplayStatics::SaveGameToSlot(Save, Save->SaveSlotName, Save->UserIndex);
}
