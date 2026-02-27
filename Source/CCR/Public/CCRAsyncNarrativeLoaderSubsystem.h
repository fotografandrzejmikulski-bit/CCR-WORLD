#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRAsyncNarrativeLoaderSubsystem.generated.h"

/**
 * UCCRAsyncNarrativeLoaderSubsystem
 *
 * Manages async loading of CCRStoryChunk primary assets.
 * Separates "required" (player is about to need) from "predicted" (likely-next) handles
 * so that predicted assets can be dropped without unloading required ones.
 */
UCLASS()
class CCR_API UCCRAsyncNarrativeLoaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Request async loading of chunks.
	 * @param Required  Chunks the player definitely needs next.
	 * @param Predicted Speculatively preloaded chunks.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Loader")
	void PreloadChunks(
		const TArray<FPrimaryAssetId>& Required,
		const TArray<FPrimaryAssetId>& Predicted);

	/** Release predicted handles (does not touch required handles). */
	UFUNCTION(BlueprintCallable, Category = "CCR|Loader")
	void DropPredicted();

	/** Release all handles. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Loader")
	void DropAll();

	/** Request a GC pass after dropping handles. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Loader")
	void RequestSafeGC();

private:
	TSharedPtr<FStreamableHandle> RequiredHandle;
	TSharedPtr<FStreamableHandle> PredictedHandle;
};
