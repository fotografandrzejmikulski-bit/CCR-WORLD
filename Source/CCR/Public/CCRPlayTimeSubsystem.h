#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRPlayTimeSubsystem.generated.h"

/**
 * UCCRPlayTimeSubsystem
 *
 * Tracks total cumulative gameplay time in seconds across sessions.
 *
 * The time is accumulated while the game is not paused.  On shutdown and on
 * every checkpoint save it is written to UCCRWorldStateSaveGameV2::PlayTimeSec
 * via UCCRCheckpointSubsystem.
 *
 * Design notes:
 *   - Uses Tick-based accumulation (UGameInstanceSubsystem does NOT tick by
 *     default; the subsystem registers a FTicker delegate instead).
 *   - Paused time (UGameplayStatics::IsGamePaused) is excluded automatically.
 *   - The accumulated time is restored from the save game on resume via
 *     SetPlayTimeSec() called by UCCRResumeSubsystem / UCCRSaveSlotManagerSubsystem.
 */
UCLASS(BlueprintType)
class CCR_API UCCRPlayTimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Returns total cumulative gameplay time in seconds. */
	UFUNCTION(BlueprintPure, Category = "CCR|PlayTime")
	float GetPlayTimeSec() const { return PlayTimeSec; }

	/** Returns a formatted string "HH:MM:SS" for display in the UI. */
	UFUNCTION(BlueprintPure, Category = "CCR|PlayTime")
	FString GetPlayTimeFormatted() const;

	/**
	 * Restore the accumulated time (called when loading a save).
	 * @param Seconds   The play time stored in the save game.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|PlayTime")
	void SetPlayTimeSec(float Seconds);

private:
	float PlayTimeSec = 0.f;

	FTSTicker::FDelegateHandle TickHandle;

	bool HandleTick(float DeltaTime);
};
