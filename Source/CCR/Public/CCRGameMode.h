#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCRGameMode.generated.h"

/**
 * ACCRGameMode
 *
 * Default game mode for CCR WORLD.
 * Sets up:
 *   - ACCRTouchController as the default player controller
 *   - ACCRGameHUD as the default HUD
 *   - ACCRGameState as the game state
 *
 * On PostLogin, kicks off the narrative resume flow if a save exists,
 * or starts the opening chunk for a new game.
 */
UCLASS()
class CCR_API ACCRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACCRGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Chunk id to start when no save exists (new game entry point) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Narrative")
	FName NewGameStartChunkId = TEXT("CHUNK_INTRO");
};
