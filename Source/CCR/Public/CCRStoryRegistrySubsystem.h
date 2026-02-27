#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRStoryRegistrySubsystem.generated.h"

/**
 * UCCRStoryRegistrySubsystem
 *
 * Builds and maintains a map of ChunkId -> PrimaryAssetId at startup.
 * Used by other subsystems to resolve chunk references to loadable assets.
 */
UCLASS()
class CCR_API UCCRStoryRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ---- UGameInstanceSubsystem ----
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Resolve a ChunkId to its PrimaryAssetId. Returns invalid id if not found. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Registry")
	FPrimaryAssetId GetAssetIdForChunk(FName ChunkId) const;

	/** Returns all known chunk IDs */
	UFUNCTION(BlueprintCallable, Category = "CCR|Registry")
	TArray<FName> GetAllChunkIds() const;

private:
	TMap<FName, FPrimaryAssetId> ChunkRegistry;

	void BuildRegistry();
};
