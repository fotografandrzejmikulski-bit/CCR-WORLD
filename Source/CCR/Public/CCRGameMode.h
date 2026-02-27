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
 * On PostLogin the game transitions to ECCRGamePhase::MainMenu so that
 * UCCRMainMenuWidget is shown. The player then calls NewGame() or Continue()
 * on the widget, which triggers StartNewGame() or UCCRResumeSubsystem
 * respectively.
 */
UCLASS()
class CCR_API ACCRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACCRGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**
	 * Begin a fresh game from the entry chunk.
	 * Called by UCCRMainMenuWidget::NewGame() but also accessible from Blueprint.
	 * Sets the Loading phase, preloads the entry chunk, and starts the narrative.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Narrative")
	void StartNewGame();

	/** Chunk id to start when beginning a new game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Narrative")
	FName NewGameStartChunkId = TEXT("CHUNK_INTRO");
};
