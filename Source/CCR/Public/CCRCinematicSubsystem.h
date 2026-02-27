#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "CCRCinematicSubsystem.generated.h"

/**
 * UCCRCinematicSubsystem
 *
 * Handles Level Sequence playback for CCR Cinematic nodes.
 *
 * Flow:
 *  1. Listens to UCCRNarrativeRuntimeSubsystem::OnNodeChanged.
 *  2. When the active node is ECCRNodeType::Cinematic, spawns an
 *     ALevelSequenceActor, plays the soft-referenced ULevelSequence,
 *     and sets the game phase to ECCRGamePhase::Cinematic.
 *  3. When playback finishes (OnStop delegate), calls
 *     UCCRNarrativeRuntimeSubsystem::FinishCinematic() and restores
 *     the game phase to ECCRGamePhase::Narrative.
 *
 * The subsystem also exposes SkipCinematic() to let players skip
 * non-critical cinematics.
 */
UCLASS()
class CCR_API UCCRCinematicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Skip the currently playing cinematic (if one is active).
	 * Stops playback immediately and advances the narrative.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Cinematic")
	void SkipCinematic();

	/** Returns true while a cinematic is actively playing. */
	UFUNCTION(BlueprintPure, Category = "CCR|Cinematic")
	bool IsCinematicPlaying() const { return bPlaying; }

private:
	UFUNCTION()
	void OnNodeChanged(FName NodeId);

	/** Called by the LevelSequencePlayer when playback finishes. */
	UFUNCTION()
	void OnSequenceFinished();

	void PlaySequenceForCurrentNode();
	void StopAndAdvance();

	UPROPERTY()
	ALevelSequenceActor* SequenceActor = nullptr;

	UPROPERTY()
	ULevelSequencePlayer* SequencePlayer = nullptr;

	bool bPlaying = false;
};
