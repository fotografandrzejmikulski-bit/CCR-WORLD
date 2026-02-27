#include "CCRAsyncNarrativeLoaderSubsystem.h"
#include "CCRStoryRegistrySubsystem.h"
#include "Engine/AssetManager.h"

void UCCRAsyncNarrativeLoaderSubsystem::PreloadChunks(
	const TArray<FPrimaryAssetId>& Required,
	const TArray<FPrimaryAssetId>& Predicted)
{
	UAssetManager& AM = UAssetManager::Get();

	if (Required.Num() > 0)
	{
		TArray<FSoftObjectPath> Paths;
		AM.GetPrimaryAssetPathList(Required, Paths);
		if (Paths.Num() > 0)
		{
			RequiredHandle = AM.GetStreamableManager().RequestAsyncLoad(
				Paths,
				FStreamableDelegate::CreateWeakLambda(this, [this]()
				{
					// Refresh the registry so newly loaded chunks are indexed
					// by their authoritative ChunkId rather than the fallback name.
					if (UCCRStoryRegistrySubsystem* Registry =
						GetGameInstance()->GetSubsystem<UCCRStoryRegistrySubsystem>())
					{
						Registry->RefreshRegistry();
					}
				}));
		}
	}

	if (Predicted.Num() > 0)
	{
		// Drop old predicted handle first so memory is freed before new request
		PredictedHandle.Reset();

		TArray<FSoftObjectPath> Paths;
		AM.GetPrimaryAssetPathList(Predicted, Paths);
		if (Paths.Num() > 0)
		{
			PredictedHandle = AM.GetStreamableManager().RequestAsyncLoad(Paths);
		}
	}
}

void UCCRAsyncNarrativeLoaderSubsystem::DropPredicted()
{
	PredictedHandle.Reset();
}

void UCCRAsyncNarrativeLoaderSubsystem::DropAll()
{
	RequiredHandle.Reset();
	PredictedHandle.Reset();
}

void UCCRAsyncNarrativeLoaderSubsystem::RequestSafeGC()
{
	DropAll();
	// GC is deferred to avoid hitches during active gameplay.
	// Callers should invoke this only during loading screens or level transitions.
	GEngine->ForceGarbageCollection(/*bForcePurge=*/false);
}
