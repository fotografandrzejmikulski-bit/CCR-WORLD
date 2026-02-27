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
		// TODO: Replace with build-time registry for shipping builds to avoid synchronous asset lookups.
		// For the Vertical Slice, GetPrimaryAssetObject returns the already-loaded object (if loaded),
		// so this is non-blocking when assets are cooked and pre-loaded by the AssetManager scan.
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
