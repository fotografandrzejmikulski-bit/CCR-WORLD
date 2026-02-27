#include "CCRSceneDirectorSubsystem.h"
#include "CCRGameState.h"
#include "CCRSpawnSubsystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"

// ---------------------------------------------------------------------------
// RequestLevelLoad
// ---------------------------------------------------------------------------

void UCCRSceneDirectorSubsystem::RequestLevelLoad(const FCCRLevelLoadRequest& Request)
{
	if (Request.LevelName.IsEmpty()) return;

	if (Request.bShowLoadingScreen)
	{
		if (UWorld* World = GetGameInstance()->GetWorld())
		{
			if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
			{
				GS->SetGamePhase(ECCRGamePhase::Loading);
			}
		}
	}

	if (Request.bStreaming)
	{
		ExecuteStreamLevelLoad(Request);
	}
	else
	{
		ExecuteFullLevelTravel(Request);
	}
}

// ---------------------------------------------------------------------------
// ExecuteFullLevelTravel
// ---------------------------------------------------------------------------

void UCCRSceneDirectorSubsystem::ExecuteFullLevelTravel(const FCCRLevelLoadRequest& Req)
{
	const FString LevelName = Req.LevelName;
	const FCCRLevelLoadRequest ReqCopy = Req;

	// Register a one-shot PostLoadMapWithWorld callback to position the player
	// and fire OnSceneLoadComplete after the level finishes loading.
	TSharedPtr<FDelegateHandle> SharedHandle = MakeShared<FDelegateHandle>();
	*SharedHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this,
		[this, ReqCopy, SharedHandle](UWorld* LoadedWorld)
		{
			FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(*SharedHandle);
			ApplyPlayerPosition(LoadedWorld, ReqCopy);
			OnSceneLoadComplete.Broadcast(ReqCopy.LevelName);
		});

	UGameplayStatics::OpenLevel(GetGameInstance(), FName(*LevelName));
}

// ---------------------------------------------------------------------------
// ExecuteStreamLevelLoad
// ---------------------------------------------------------------------------

void UCCRSceneDirectorSubsystem::ExecuteStreamLevelLoad(const FCCRLevelLoadRequest& Req)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	const FString LevelName = Req.LevelName;
	const FCCRLevelLoadRequest ReqCopy = Req;

	// ULevelStreamingDynamic loads the sublevel asynchronously.
	bool bOutSuccess = false;
	ULevelStreamingDynamic* StreamLevel = ULevelStreamingDynamic::LoadLevelInstance(
		World,
		LevelName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		bOutSuccess);

	if (!bOutSuccess || !StreamLevel)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UCCRSceneDirectorSubsystem: LoadLevelInstance failed for '%s'"),
			*LevelName);
		OnSceneLoadComplete.Broadcast(LevelName);
		return;
	}

	// Bind a callback for when the level finishes loading and becomes visible.
	UWorld* OwningWorldPtr = World; // capture world for the lambda
	StreamLevel->OnLevelShown.AddWeakLambda(this,
		[this, ReqCopy, OwningWorldPtr]()
		{
			if (ReqCopy.bShowLoadingScreen)
			{
				if (ACCRGameState* GS = OwningWorldPtr->GetGameState<ACCRGameState>())
				{
					GS->SetGamePhase(ECCRGamePhase::Narrative);
				}
			}

			ApplyPlayerPosition(OwningWorldPtr, ReqCopy);
			OnSceneLoadComplete.Broadcast(ReqCopy.LevelName);
		});
}

// ---------------------------------------------------------------------------
// UnloadStreamLevel
// ---------------------------------------------------------------------------

void UCCRSceneDirectorSubsystem::UnloadStreamLevel(const FString& LevelName)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	UGameplayStatics::UnloadStreamLevel(World, FName(*LevelName),
		FLatentActionInfo(), /*bShouldBlockOnUnload=*/false);
}

// ---------------------------------------------------------------------------
// ApplyPlayerPosition
// ---------------------------------------------------------------------------

void UCCRSceneDirectorSubsystem::ApplyPlayerPosition(UWorld* World, const FCCRLevelLoadRequest& Req)
{
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	// bHasTransform is determined after SpawnTag resolution so it only becomes
	// true when we have a valid transform to apply.
	bool bHasTransform = false;
	FTransform FinalTransform = Req.PlayerSpawnTransform;

	if (!Req.SpawnTag.IsNone())
	{
		if (UCCRSpawnSubsystem* SpawnSys = GetGameInstance()->GetSubsystem<UCCRSpawnSubsystem>())
		{
			FTransform SpawnTransform;
			if (SpawnSys->GetSpawnTransform(Req.SpawnTag, SpawnTransform))
			{
				FinalTransform = SpawnTransform;
				bHasTransform = true;
			}
		}
	}

	// Fall back to the explicit spawn transform if no SpawnTag was resolved
	if (!bHasTransform && !Req.PlayerSpawnTransform.Equals(FTransform::Identity))
	{
		bHasTransform = true;
	}

	if (bHasTransform)
	{
		Pawn->SetActorTransform(FinalTransform, false, nullptr, ETeleportType::TeleportPhysics);
		PC->SetControlRotation(FinalTransform.Rotator());
	}
}
