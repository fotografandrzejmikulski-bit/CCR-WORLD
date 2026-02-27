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

	// Transition to MainMenu phase so UCCRMainMenuWidget is shown.
	// UCCRMainMenuWidget::NewGame() / Continue() will drive the next transition.
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->SetGamePhase(ECCRGamePhase::MainMenu);
		}
	}
}

void ACCRGameMode::StartNewGame()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	if (NewGameStartChunkId.IsNone()) return;

	UCCRStoryRegistrySubsystem* Registry = GI->GetSubsystem<UCCRStoryRegistrySubsystem>();
	UCCRAsyncNarrativeLoaderSubsystem* Loader = GI->GetSubsystem<UCCRAsyncNarrativeLoaderSubsystem>();
	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!Registry || !Loader || !NRS) return;

	const FPrimaryAssetId AssetId = Registry->GetAssetIdForChunk(NewGameStartChunkId);
	if (!AssetId.IsValid()) return;

	// Transition to Loading phase so the HUD shows the loading overlay
	// while the entry chunk streams in.
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->SetGamePhase(ECCRGamePhase::Loading);
		}
	}

	TArray<FPrimaryAssetId> Required;
	Required.Add(AssetId);
	Loader->PreloadChunks(Required, {});

	FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);

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
