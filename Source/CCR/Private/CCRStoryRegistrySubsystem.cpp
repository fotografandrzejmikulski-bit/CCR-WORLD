#include "CCRStoryRegistrySubsystem.h"
#include "CCRStoryChunk.h"

void UCCRStoryRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	BuildRegistry();
}

void UCCRStoryRegistrySubsystem::BuildRegistry()
{
	UAssetManager& AM = UAssetManager::Get();
	static const FPrimaryAssetType ChunkType(TEXT("CCRStoryChunk"));

	TArray<FPrimaryAssetId> AssetIds;
	AM.GetPrimaryAssetIdList(ChunkType, AssetIds);

	ChunkRegistry.Reset();
	ChunkRegistry.Reserve(AssetIds.Num());

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		// For already-loaded chunks, use the authoritative ChunkId from the object.
		// For unloaded chunks, fall back to the PrimaryAssetName as the key;
		// call RefreshRegistry() after async loads to fix up any mismatches.
		if (UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(AM.GetPrimaryAssetObject(AssetId)))
		{
			ChunkRegistry.Add(Chunk->ChunkId, AssetId);
		}
		else
		{
			// Fallback: derive ChunkId from the asset name portion of the PrimaryAssetId
			ChunkRegistry.Add(FName(*AssetId.PrimaryAssetName.ToString()), AssetId);
		}
	}
}

void UCCRStoryRegistrySubsystem::RefreshRegistry()
{
	// Re-examine any entry whose PrimaryAssetObject is now loaded so its
	// authoritative ChunkId can replace the fallback name-derived key.
	UAssetManager& AM = UAssetManager::Get();

	TArray<FName> StaleKeys;
	for (auto& Pair : ChunkRegistry)
	{
		if (UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(AM.GetPrimaryAssetObject(Pair.Value)))
		{
			if (Chunk->ChunkId != Pair.Key)
			{
				StaleKeys.Add(Pair.Key);
			}
		}
	}

	for (const FName& OldKey : StaleKeys)
	{
		FPrimaryAssetId AssetId;
		if (const FPrimaryAssetId* Found = ChunkRegistry.Find(OldKey))
		{
			AssetId = *Found;
		}
		ChunkRegistry.Remove(OldKey);
		if (AssetId.IsValid())
		{
			if (UCCRStoryChunk* Chunk = Cast<UCCRStoryChunk>(AM.GetPrimaryAssetObject(AssetId)))
			{
				ChunkRegistry.Add(Chunk->ChunkId, AssetId);
			}
		}
	}
}

FPrimaryAssetId UCCRStoryRegistrySubsystem::GetAssetIdForChunk(FName ChunkId) const
{
	const FPrimaryAssetId* Found = ChunkRegistry.Find(ChunkId);
	return Found ? *Found : FPrimaryAssetId();
}

TArray<FName> UCCRStoryRegistrySubsystem::GetAllChunkIds() const
{
	TArray<FName> Keys;
	ChunkRegistry.GetKeys(Keys);
	return Keys;
}
