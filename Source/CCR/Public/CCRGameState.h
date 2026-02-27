#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CCRGameState.generated.h"

UENUM(BlueprintType)
enum class ECCRGamePhase : uint8
{
	MainMenu    UMETA(DisplayName = "MainMenu"),
	Loading     UMETA(DisplayName = "Loading"),
	Narrative   UMETA(DisplayName = "Narrative"),
	Cinematic   UMETA(DisplayName = "Cinematic"),
	QTE         UMETA(DisplayName = "QTE"),
	Paused      UMETA(DisplayName = "Paused"),
};

/**
 * ACCRGameState
 *
 * Tracks the current high-level game phase so that UI and systems
 * can react appropriately (e.g. hide choice buttons during cinematics).
 */
UCLASS()
class CCR_API ACCRGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CCR")
	void SetGamePhase(ECCRGamePhase NewPhase);

	UFUNCTION(BlueprintPure, Category = "CCR")
	ECCRGamePhase GetGamePhase() const { return CurrentPhase; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, ECCRGamePhase, NewPhase);

	UPROPERTY(BlueprintAssignable, Category = "CCR")
	FOnGamePhaseChanged OnGamePhaseChanged;

private:
	ECCRGamePhase CurrentPhase = ECCRGamePhase::Loading;
};
