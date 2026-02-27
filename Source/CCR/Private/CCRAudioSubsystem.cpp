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

	// Keep live volumes in sync if the player changes settings from a menu
	// that calls UCCRSettingsSubsystem setters directly.
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		Settings->OnSettingsChanged.AddDynamic(this, &UCCRAudioSubsystem::OnSettingsChanged);
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

		if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
		{
			Settings->OnSettingsChanged.RemoveDynamic(this, &UCCRAudioSubsystem::OnSettingsChanged);
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
// OnSettingsChanged — re-sync volumes when settings are changed externally
// ---------------------------------------------------------------------------

void UCCRAudioSubsystem::OnSettingsChanged()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>();
	if (!Settings) return;

	// Absorb new values without triggering another save (SetMusicVolume etc.
	// would call Settings->SetMusicVolume() and re-fire OnSettingsChanged).
	MusicVolume = Settings->GetMusicVolume();
	VOVolume    = Settings->GetVOVolume();
	SFXVolume   = Settings->GetSFXVolume();

	// Apply immediately to live components
	if (MusicComponent)
	{
		const float AppliedVolume = bMusicDucked
			? MusicVolume * FMath::Clamp(CinematicDuckVolume, 0.f, 1.f)
			: MusicVolume;
		MusicComponent->SetVolumeMultiplier(AppliedVolume);
	}

	if (IsValid(VOComponent))
	{
		VOComponent->SetVolumeMultiplier(VOVolume);
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
		// Respect the current duck state so a newly-started track does not
		// blast at full volume while a cinematic is playing.
		const float EffectiveVolume = bMusicDucked
			? MusicVolume * FMath::Clamp(CinematicDuckVolume, 0.f, 1.f)
			: MusicVolume;
		MusicComponent->FadeIn(FadeTimeSec, EffectiveVolume);
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

FName UCCRAudioSubsystem::GetCurrentMusicId() const
{
	return CurrentMusicKey;
}

void UCCRAudioSubsystem::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.f, 1.f);
	if (MusicComponent)
	{
		// Apply the duck multiplier if a cinematic is currently playing so we
		// don't accidentally restore full volume while the sequence is still running.
		const float AppliedVolume = bMusicDucked
			? MusicVolume * FMath::Clamp(CinematicDuckVolume, 0.f, 1.f)
			: MusicVolume;
		MusicComponent->SetVolumeMultiplier(AppliedVolume);
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

	// Bind a callback so we can fire OnVOFinished when playback ends naturally.
	// Note: bAutoDestroy=true means the component will be GC'd after OnStop fires;
	// we null VOComponent in HandleVOFinished to avoid a dangling UPROPERTY.
	if (IsValid(VOComponent))
	{
		VOComponent->OnAudioFinished.AddDynamic(this, &UCCRAudioSubsystem::HandleVOFinished);
	}
}

void UCCRAudioSubsystem::HandleVOFinished()
{
	// The audio component auto-destroys after this fires; null our pointer
	// to prevent accessing a PendingKill object in subsequent IsValid() checks.
	VOComponent = nullptr;
	OnVOFinished.Broadcast();
}

void UCCRAudioSubsystem::StopVO()
{
	// Use IsValid() because VOComponent uses bAutoDestroy=true: the component
	// can be PendingKill before a GC pass nulls the UPROPERTY pointer.
	// Always null the pointer afterwards so future IsValid() checks are correct.
	if (IsValid(VOComponent))
	{
		// Unbind our finish callback before stopping so that HandleVOFinished
		// (and therefore OnVOFinished) is NOT fired for early-stop cases.
		VOComponent->OnAudioFinished.RemoveDynamic(this, &UCCRAudioSubsystem::HandleVOFinished);
		VOComponent->Stop();
	}
	VOComponent = nullptr;
}

void UCCRAudioSubsystem::SetVOVolume(float Volume)
{
	VOVolume = FMath::Clamp(Volume, 0.f, 1.f);
	if (IsValid(VOComponent))
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
