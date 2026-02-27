#include "CCRAudioSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRSettingsSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// ---------------------------------------------------------------------------
// Initialize / Deinitialize
// ---------------------------------------------------------------------------

void UCCRAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Load persisted audio volumes from settings
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		MusicVolume = Settings->GetMusicVolume();
		VOVolume    = Settings->GetVOVolume();
		SFXVolume   = Settings->GetSFXVolume();
	}

	UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (NRS)
	{
		NRS->OnNodeChanged.AddDynamic(this, &UCCRAudioSubsystem::OnNodeChanged);
	}
}

void UCCRAudioSubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
		if (NRS)
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRAudioSubsystem::OnNodeChanged);
		}
	}

	StopVO();
	StopMusic(0.f);

	Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// OnNodeChanged — auto-stop VO on narrative advancement
// ---------------------------------------------------------------------------

void UCCRAudioSubsystem::OnNodeChanged(FName NodeId)
{
	// Stop any playing VO when the narrative moves to a new node.
	// The caller (Blueprint or code) should call PlayVO(NodeId) if the new
	// node has registered VO.
	StopVO();

	// Auto-play VO for the new node if a cue is registered under NodeId
	if (VOCues.Contains(NodeId))
	{
		PlayVO(NodeId);
	}
}

// ---------------------------------------------------------------------------
// Music
// ---------------------------------------------------------------------------

void UCCRAudioSubsystem::RegisterMusicCue(FName Key, USoundBase* Cue)
{
	if (Key.IsNone() || !Cue) return;
	MusicCues.Add(Key, Cue);
}

void UCCRAudioSubsystem::PlayMusic(FName CueKey, float FadeTimeSec)
{
	if (CueKey == CurrentMusicKey && MusicComponent && MusicComponent->IsPlaying()) return;

	USoundBase** Found = MusicCues.Find(CueKey);
	if (!Found || !(*Found)) return;

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	// Fade out existing music component.
	// Mark it to auto-destroy once the fade finishes so it cleans up without
	// us needing to hold a pointer to it across the fade duration.
	if (MusicComponent && MusicComponent->IsPlaying())
	{
		MusicComponent->bAutoDestroy = true;
		MusicComponent->FadeOut(FadeTimeSec, 0.f);
		MusicComponent = nullptr; // Safe: component owns its own lifecycle now
	}

	// Spawn and fade in new music
	MusicComponent = UGameplayStatics::SpawnSound2D(
		World,
		*Found,
		MusicVolume,
		/*PitchMultiplier=*/1.f,
		/*StartTime=*/0.f,
		/*ConcurrencySettings=*/nullptr,
		/*bPersistAcrossLevelTransitions=*/true,
		/*bAutoDestroy=*/false); // We manage this component's lifecycle

	if (MusicComponent)
	{
		MusicComponent->FadeIn(FadeTimeSec, MusicVolume);
	}

	CurrentMusicKey = CueKey;
}

void UCCRAudioSubsystem::StopMusic(float FadeTimeSec)
{
	if (MusicComponent)
	{
		if (FadeTimeSec > 0.f)
			MusicComponent->FadeOut(FadeTimeSec, 0.f);
		else
			MusicComponent->Stop();

		MusicComponent = nullptr;
	}
	CurrentMusicKey = NAME_None;
}

void UCCRAudioSubsystem::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.f, 1.f);
	if (MusicComponent)
	{
		MusicComponent->SetVolumeMultiplier(MusicVolume);
	}
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		Settings->SetMusicVolume(MusicVolume);
	}
}

// ---------------------------------------------------------------------------
// VO
// ---------------------------------------------------------------------------

void UCCRAudioSubsystem::RegisterVOCue(FName Key, USoundBase* Cue)
{
	if (Key.IsNone() || !Cue) return;
	VOCues.Add(Key, Cue);
}

void UCCRAudioSubsystem::PlayVO(FName CueKey)
{
	StopVO();

	USoundBase** Found = VOCues.Find(CueKey);
	if (!Found || !(*Found)) return;

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	VOComponent = UGameplayStatics::SpawnSound2D(
		World,
		*Found,
		VOVolume,
		/*PitchMultiplier=*/1.f,
		/*StartTime=*/0.f,
		/*ConcurrencySettings=*/nullptr,
		/*bPersistAcrossLevelTransitions=*/false,
		/*bAutoDestroy=*/true);
}

void UCCRAudioSubsystem::StopVO()
{
	if (VOComponent)
	{
		VOComponent->Stop();
		VOComponent = nullptr;
	}
}

void UCCRAudioSubsystem::SetVOVolume(float Volume)
{
	VOVolume = FMath::Clamp(Volume, 0.f, 1.f);
	if (VOComponent)
	{
		VOComponent->SetVolumeMultiplier(VOVolume);
	}
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		Settings->SetVOVolume(VOVolume);
	}
}

// ---------------------------------------------------------------------------
// SFX
// ---------------------------------------------------------------------------

void UCCRAudioSubsystem::PlaySFX(USoundBase* Cue)
{
	if (!Cue) return;
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	UGameplayStatics::PlaySound2D(World, Cue, SFXVolume);
}

void UCCRAudioSubsystem::PlaySFXAtLocation(USoundBase* Cue, FVector Location)
{
	if (!Cue) return;
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	UGameplayStatics::PlaySoundAtLocation(World, Cue, Location, SFXVolume);
}

void UCCRAudioSubsystem::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.f, 1.f);
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		Settings->SetSFXVolume(SFXVolume);
	}
}

// ---------------------------------------------------------------------------
// Cinematic ducking
// ---------------------------------------------------------------------------

void UCCRAudioSubsystem::DuckMusicForCinematic(bool bDuck)
{
	if (bMusicDucked == bDuck) return; // already in the desired state
	bMusicDucked = bDuck;

	if (MusicComponent)
	{
		const float TargetVolume = bDuck
			? MusicVolume * FMath::Clamp(CinematicDuckVolume, 0.f, 1.f)
			: MusicVolume;

		// Fade the music component to the target volume over a short time
		// so the transition is smooth rather than an abrupt jump.
		MusicComponent->AdjustVolume(CinematicDuckFadeDuration, TargetVolume);
	}
}
