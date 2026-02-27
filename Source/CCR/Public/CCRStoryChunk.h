#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "Engine/DataAsset.h"
#include "CCRStoryChunk.generated.h"

/**
 * UCCRStoryChunk
 *
 * Primary Data Asset representing one narrative chunk (axis section).
 * Scanned by the Asset Manager from /Game/CCR/Story.
 */
UCLASS(BlueprintType)
class CCR_API UCCRStoryChunk : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Unique identifier for this chunk */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Story")
	FName ChunkId;

	/** Narrative axis this chunk belongs to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Story")
	FName AxisId;

	/** First node to execute when entering this chunk */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Story")
	FName EntryNodeId;

	/** All narrative nodes contained in this chunk */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Story")
	TArray<FCCRNode> Nodes;

	// UPrimaryDataAsset override
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
