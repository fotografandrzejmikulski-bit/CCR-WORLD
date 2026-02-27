#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCRSpawnPoint.generated.h"

/**
 * ACCRSpawnPoint
 *
 * A zero-overhead marker actor that registers its location and rotation with
 * UCCRSpawnSubsystem under a designer-assigned SpawnTag.
 *
 * Usage:
 *   1. Place an ACCRSpawnPoint (or a Blueprint subclass) in the level.
 *   2. Set SpawnTag to a unique FName, e.g. "SPAWN_CaveEntrance".
 *   3. Save the same tag in FCCRPlayerSpatialSave::SpawnTag when checkpointing.
 *   4. UCCRResumeSubsystem::OnLevelLoaded will call
 *      UCCRSpawnSubsystem::GetSpawnTransform(SpawnTag) to position the player.
 *
 * The actor has no collision and no visible mesh by default; it is represented
 * by the Billboard component visible only in the editor.  Designers can optionally
 * attach a mesh to the Blueprint subclass for visual reference.
 *
 * BeginPlay  → registers with UCCRSpawnSubsystem.
 * EndPlay    → unregisters from UCCRSpawnSubsystem.
 *
 * Multiple actors with the same SpawnTag in the same level are technically
 * allowed (the last one to call BeginPlay wins), but that is a designer error;
 * a warning is emitted in that case.
 */
UCLASS(BlueprintType, Blueprintable, HideCategories = (Rendering, Physics, LOD, "Tags", "AssetUserData"))
class CCR_API ACCRSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ACCRSpawnPoint();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Unique identifier used to look up this spawn point at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CCR|Spawn")
	FName SpawnTag;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<class UBillboardComponent> SpriteComponent;
#endif
};
