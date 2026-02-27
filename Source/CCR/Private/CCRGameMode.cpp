#include "CCRGameMode.h"
#include "CCRTouchController.h"
#include "CCRGameHUD.h"
#include "CCRGameState.h"
#include "CCRResumeSubsystem.h"
#include "CCRStoryRegistrySubsystem.h"
#include "CCRAsyncNarrativeLoaderSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "Engine/AssetManager.h"

ACCRGameMode::ACCRGameMode()
{
	PlayerControllerClass = ACCRTouchController::StaticClass();
	HUDClass              = ACCRGameHUD::StaticClass();
	GameStateClass        = ACCRGameState::StaticClass();
}

void ACCRGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	// Attempt to resume from an existing save first
	UCCRResumeSubsystem* Resume = GI->GetSubsystem<UCCRResumeSubsystem>();
	if (Resume && Resume->ResumeFromDefaultSlot())
	{
		// Save exists – resume handled by UCCRResumeSubsystem
		return;
	}

	// No save: start a new game from the entry chunk
	if (NewGameStartChunkId.IsNone()) return;

	UCCRStoryRegistrySubsystem* Registry = GI->GetSubsystem<UCCRStoryRegistrySubsystem>();
	UCCRAsyncNarrativeLoaderSubsystem* Loader = GI->GetSubsystem<UCCRAsyncNarrativeLoaderSubsystem>();
	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!Registry || !Loader || !NRS) return;

	const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(NewGameStartChunkId);
	if (!AssetId.IsValid()) return;

	TArray<FPrimaryAssetId> Required;
	Required.Add(AssetId);
	Loader->PreloadChunks(Required, {});

	// Load chunk then start narrative
	FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);
	const FName CapturedChunkId = NewGameStartChunkId;

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AssetPath,
		FStreamableDelegate::CreateWeakLambda(NRS, [NRS, AssetId]()
		{
			UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(
				UAssetManager::Get().GetPrimaryAssetObject(AssetId));
			if (Chunk)
			{
				NRS->StartChunk(Chunk);
			}
		}));
}
