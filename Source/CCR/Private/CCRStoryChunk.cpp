#include "CCRStoryChunk.h"

FPrimaryAssetId UCCRStoryChunk::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("CCRStoryChunk"), ChunkId);
}
