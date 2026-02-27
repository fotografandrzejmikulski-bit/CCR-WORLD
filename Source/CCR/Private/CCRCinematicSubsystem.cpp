#include "CCRCinematicSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRAudioSubsystem.h"
#include "CCRGameState.h"
#include "CCRTypes.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// ---------------------------------------------------------------------------
// Initialize / Deinitialize
// ---------------------------------------------------------------------------

void UCCRCinematicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (NRS)
	{
		NRS->OnNodeChanged.AddDynamic(this, &UCCRCinematicSubsystem::OnNodeChanged);
	}
}

void UCCRCinematicSubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
		if (NRS)
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRCinematicSubsystem::OnNodeChanged);
		}
	}

	if (SequencePlayer && bPlaying)
	{
		SequencePlayer->Stop();
	}

	Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// OnNodeChanged
// ---------------------------------------------------------------------------

void UCCRCinematicSubsystem::OnNodeChanged(FName NodeId)
{
	UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode Node;
	if (!NRS->GetCurrentNode(Node)) return;
	if (Node.NodeType != ECCRNodeType::Cinematic) return;

	PlaySequenceForCurrentNode();
}

// ---------------------------------------------------------------------------
// PlaySequenceForCurrentNode
// ---------------------------------------------------------------------------

void UCCRCinematicSubsystem::PlaySequenceForCurrentNode()
{
	UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode Node;
	if (!NRS->GetCurrentNode(Node)) return;

	ULevelSequence* Sequence = Cast<ULevelSequence>(Node.CinematicSequence.LoadSynchronous());
	if (!Sequence)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UCCRCinematicSubsystem: CinematicSequence is null for node '%s'. Advancing narrative."),
			*Node.NodeId.ToString());
		NRS->FinishCinematic();
		return;
	}

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		NRS->FinishCinematic();
		return;
	}

	// Set game phase
	if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
	{
		GS->SetGamePhase(ECCRGamePhase::Cinematic);
	}

	// Duck background music for cinematic
	if (UCCRAudioSubsystem* Audio = GetGameInstance()->GetSubsystem<UCCRAudioSubsystem>())
	{
		Audio->DuckMusicForCinematic(true);
	}

	// Destroy any previous sequence actor
	if (IsValid(SequenceActor))
	{
		SequenceActor->Destroy();
		SequenceActor = nullptr;
		SequencePlayer = nullptr;
	}

	// Spawn and configure the sequence actor
	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;

	FLevelSequenceCameraSettings CamSettings;

	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World,
		Sequence,
		Settings,
		SequenceActor);

	if (!SequencePlayer || !SequenceActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UCCRCinematicSubsystem: Failed to create LevelSequencePlayer."));
		NRS->FinishCinematic();
		return;
	}

	// Bind the finish delegate
	SequencePlayer->OnStop.AddDynamic(this, &UCCRCinematicSubsystem::OnSequenceFinished);

	bPlaying = true;
	SequencePlayer->Play();
}

// ---------------------------------------------------------------------------
// OnSequenceFinished
// ---------------------------------------------------------------------------

void UCCRCinematicSubsystem::OnSequenceFinished()
{
	StopAndAdvance();
}

// ---------------------------------------------------------------------------
// SkipCinematic
// ---------------------------------------------------------------------------

void UCCRCinematicSubsystem::SkipCinematic()
{
	if (!bPlaying) return;

	if (SequencePlayer)
	{
		SequencePlayer->Stop();
	}
	// StopAndAdvance is called via OnSequenceFinished delegate after Stop()
}

// ---------------------------------------------------------------------------
// StopAndAdvance
// ---------------------------------------------------------------------------

void UCCRCinematicSubsystem::StopAndAdvance()
{
	bPlaying = false;

	UGameInstance* GI = GetGameInstance();

	// Restore game phase to Narrative
	if (UWorld* World = GI ? GI->GetWorld() : nullptr)
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->SetGamePhase(ECCRGamePhase::Narrative);
		}
	}

	// Restore full music volume
	if (UCCRAudioSubsystem* Audio = GI ? GI->GetSubsystem<UCCRAudioSubsystem>() : nullptr)
	{
		Audio->DuckMusicForCinematic(false);
	}

	// Destroy the sequence actor
	if (IsValid(SequenceActor))
	{
		SequenceActor->Destroy();
		SequenceActor = nullptr;
	}
	SequencePlayer = nullptr;

	// Advance narrative
	if (UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		NRS->FinishCinematic();
	}
}
