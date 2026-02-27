#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRSceneDirectorSubsystem.generated.h"

/**
 * FCCRLevelLoadRequest
 *
 * Describes a level streaming operation requested via UCCRSceneDirectorSubsystem.
 */
USTRUCT(BlueprintType)
struct FCCRLevelLoadRequest
{
	GENERATED_BODY()

	/** Level package name to load (e.g. "/Game/Maps/ForestGlade"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LevelName;

	/**
	 * When true, use UGameplayStatics::LoadStreamLevel (streaming, no screen transition).
	 * When false, use UGameplayStatics::OpenLevel (full level travel).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStreaming = false;

	/**
	 * When true (default), transition through the Loading phase so the
	 * HUD shows the loading overlay during the travel.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowLoadingScreen = true;

	/** Optional transform to place the player at after the level finishes loading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform PlayerSpawnTransform;

	/** If set and a matching ACCRSpawnPoint exists, use it instead of PlayerSpawnTransform. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpawnTag;
};

/**
 * UCCRSceneDirectorSubsystem
 *
 * Manages level loading, streaming, and scene transitions for CCR WORLD.
 *
 * Responsibilities:
 *   - Provides a single API to travel to a new level or stream a sublevel,
 *     bridging the loading phase, HUD overlay, and spawn positioning.
 *   - Replaces scattered UGameplayStatics::OpenLevel() calls throughout the
 *     game with a coordinated flow that always shows the loading widget and
 *     respects the SpawnTag system.
 *   - Fires OnSceneLoadComplete after the new level is ready and the player
 *     has been positioned.
 *
 * Flow for a full level travel (bStreaming = false):
 *   1. SetGamePhase(Loading) → HUD shows loading overlay.
 *   2. OpenLevel() is called.
 *   3. PostLoadMapWithWorld fires → player is positioned at SpawnTag / transform.
 *   4. OnSceneLoadComplete is broadcast.
 *
 * Flow for sublevel streaming (bStreaming = true):
 *   1. SetGamePhase(Loading) if bShowLoadingScreen.
 *   2. LoadStreamLevel() is called.
 *   3. OnLevelLoaded latent callback → player is positioned if spatial data provided.
 *   4. OnSceneLoadComplete is broadcast, Loading phase cleared.
 */
UCLASS(BlueprintType)
class CCR_API UCCRSceneDirectorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Begin a level load or stream request.
	 * Must be called from the game thread.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Scene")
	void RequestLevelLoad(const FCCRLevelLoadRequest& Request);

	/**
	 * Unload a previously-streamed sublevel.
	 * No-op when the sublevel is not currently loaded.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Scene")
	void UnloadStreamLevel(const FString& LevelName);

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCCRSceneLoadComplete, const FString&, LevelName);

	/**
	 * Fired once the requested level is loaded and the player has been positioned.
	 * Level name matches FCCRLevelLoadRequest::LevelName of the triggering request.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Scene")
	FOnCCRSceneLoadComplete OnSceneLoadComplete;

private:
	FCCRLevelLoadRequest PendingRequest;
	bool bHasPendingRequest = false;

	void ExecuteFullLevelTravel(const FCCRLevelLoadRequest& Req);
	void ExecuteStreamLevelLoad(const FCCRLevelLoadRequest& Req);
	void ApplyPlayerPosition(UWorld* World, const FCCRLevelLoadRequest& Req);
};
