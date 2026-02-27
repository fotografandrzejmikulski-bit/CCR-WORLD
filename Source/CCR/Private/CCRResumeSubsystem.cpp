#include "CCRResumeSubsystem.h"
#include "CCRWorldStateSaveGameV2.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRStoryRegistrySubsystem.h"
#include "CCRAsyncNarrativeLoaderSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

bool UCCRResumeSubsystem::ResumeFromDefaultSlot()
{
	static const FString SlotName = TEXT("CCRSaveSlot");
	constexpr int32 UserIndex = 0;

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return false;
	}

	UCCRWorldStateSaveGameV2* Save = Cast<UCCRWorldStateSaveGameV2>(
		UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!Save) return false;

	// 1. Import world state
	if (UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		WSM->ImportState(
			Save->WSKeys,
			Save->WSTypes,
			Save->WSIndices,
			Save->WSFlagBits,
			Save->WSFloats,
			Save->WSInts);
	}

	PendingChunkId = Save->CurrentChunkId;
	PendingNodeId  = Save->CurrentNodeId;

	// 2. Open the saved level
	const FString LevelName = Save->PlayerSpatial.bHasSpatial
		? Save->PlayerSpatial.LevelName
		: FString();

	const FCCRPlayerSpatialSave SpatialCopy = Save->PlayerSpatial;

	if (!LevelName.IsEmpty())
	{
	// Track the delegate handle so we can remove only this specific callback
	// even if multiple resume operations are attempted.
	FDelegateHandle LevelLoadHandle;
	LevelLoadHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this,
		[this, SpatialCopy, LevelLoadHandle](UWorld* LoadedWorld) mutable
		{
			FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(LevelLoadHandle);
			OnLevelLoaded();

			// Apply player transform
			if (SpatialCopy.bHasSpatial)
			{
				if (APlayerController* PC = LoadedWorld->GetFirstPlayerController())
				{
					if (APawn* Pawn = PC->GetPawn())
					{
						Pawn->SetActorTransform(SpatialCopy.PlayerTransform, false, nullptr, ETeleportType::TeleportPhysics);
					}
					PC->SetControlRotation(FRotator(SpatialCopy.CameraPitch, SpatialCopy.CameraYaw, 0.f));
				}
			}
		});

		UGameplayStatics::OpenLevel(GetGameInstance(), FName(*LevelName));
	}
	else
	{
		// Level already loaded or no spatial data; start narrative directly
		OnLevelLoaded();
	}

	return true;
}

void UCCRResumeSubsystem::OnLevelLoaded()
{
	if (PendingChunkId.IsNone()) return;

	// Resolve ChunkId -> PrimaryAssetId
	UCCRStoryRegistrySubsystem* Registry = GetGameInstance()->GetSubsystem<UCCRStoryRegistrySubsystem>();
	if (!Registry) return;

	const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(PendingChunkId);
	if (!AssetId.IsValid()) return;

	// Load the chunk then start narrative
	FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);

	const FName CapturedChunkId = PendingChunkId;
	const FName CapturedNodeId  = PendingNodeId;

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AssetPath,
		FStreamableDelegate::CreateWeakLambda(this, [this, AssetId, CapturedNodeId]()
		{
			UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(
				UAssetManager::Get().GetPrimaryAssetObject(AssetId));
			if (!Chunk) return;

			if (UCCRNarrativeRuntimeSubsystem* NRS =
				GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
			{
				NRS->StartChunk(Chunk, CapturedNodeId);
			}
		}));
}
