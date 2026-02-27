#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundBase.h"
#include "CCRAudioSubsystem.generated.h"

/**
 * UCCRAudioSubsystem
 *
 * Centralised audio controller for CCR WORLD.
 *
 * Responsibilities:
 *   Music  – manage two crossfading "music layers" (ambient + intensity).
 *             Fades out the current track and fades in the new one.
 *   VO     – play a per-node voice-over cue (one-shot, stopped on NodeChanged).
 *   SFX    – fire-and-forget one-shot sound effects.
 *
 * Music and VO cues are referenced via FName keys mapped to USoundBase
 * soft references so that only the active cues are in memory.
 *
 * Blueprint subclasses can override the cue library via
 * RegisterMusicCue() / RegisterVOCue() at startup.
 */
UCLASS(BlueprintType, Blueprintable)
class CCR_API UCCRAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ---- Delegates ----

	/**
	 * Broadcast when the current VO cue finishes playing naturally.
	 * Not fired when VO is stopped early via StopVO().
	 * Blueprint dialogue widgets can bind to this to auto-advance or
	 * hide the "tap to continue" prompt when VO finishes.
	 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCCRVOFinished);

	UPROPERTY(BlueprintAssignable, Category = "CCR|Audio")
	FOnCCRVOFinished OnVOFinished;

	// ---- Music API ----

	/**
	 * Crossfade to a new music track identified by CueKey.
	 * If the same key is already playing, this is a no-op.
	 * @param CueKey     Identifier registered via RegisterMusicCue().
	 * @param FadeTimeSec Duration of the crossfade in seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void PlayMusic(FName CueKey, float FadeTimeSec = 1.5f);

	/** Fade out all music. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void StopMusic(float FadeTimeSec = 1.5f);

	/**
	 * Returns the cue key of the currently playing music track.
	 * Returns NAME_None when no music is playing.
	 */
	UFUNCTION(BlueprintPure, Category = "CCR|Audio")
	FName GetCurrentMusicId() const;

	/**
	 * Register a music cue so it can be referenced by key at runtime.
	 * Call during Game Instance or Subsystem init.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void RegisterMusicCue(FName Key, USoundBase* Cue);

	// ---- VO API ----

	/**
	 * Play a voice-over cue for a narrative node.
	 * Any previously playing VO is stopped first.
	 * @param CueKey Identifier registered via RegisterVOCue().
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void PlayVO(FName CueKey);

	/** Stop the current VO immediately. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void StopVO();

	/**
	 * Register a VO cue. Typically called when a StoryChunk is loaded,
	 * using the NodeId as the key.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void RegisterVOCue(FName Key, USoundBase* Cue);

	// ---- SFX API ----

	/** Fire-and-forget SFX at the player's location. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void PlaySFX(USoundBase* Cue);

	/** Fire-and-forget SFX at a world location. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void PlaySFXAtLocation(USoundBase* Cue, FVector Location);

	// ---- Cinematic audio ducking ----

	/**
	 * Duck or restore music volume for cinematic playback.
	 * When bDuck is true, music is faded to CinematicDuckVolume.
	 * When false, it is restored to the full MusicVolume.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void DuckMusicForCinematic(bool bDuck);

	/**
	 * Music volume multiplier applied while a cinematic is playing.
	 * Default 0.3 leaves ambient music audible but clearly secondary.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Audio",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CinematicDuckVolume = 0.3f;

	/**
	 * Duration in seconds of the music volume fade when entering or leaving
	 * cinematic ducking.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCR|Audio",
		meta = (ClampMin = "0.0"))
	float CinematicDuckFadeDuration = 0.5f;

	// ---- Volume control ----

	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void SetVOVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "CCR|Audio")
	void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintPure, Category = "CCR|Audio")
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintPure, Category = "CCR|Audio")
	float GetVOVolume() const { return VOVolume; }

	UFUNCTION(BlueprintPure, Category = "CCR|Audio")
	float GetSFXVolume() const { return SFXVolume; }

private:
	UFUNCTION()
	void OnNodeChanged(FName NodeId);

	UFUNCTION()
	void OnSettingsChanged();

	/** Bound to VOComponent::OnAudioFinished; fires OnVOFinished delegate. */
	UFUNCTION()
	void HandleVOFinished();

	// ---- Cue registries ----
	UPROPERTY()
	TMap<FName, TObjectPtr<USoundBase>> MusicCues;

	UPROPERTY()
	TMap<FName, TObjectPtr<USoundBase>> VOCues;

	// ---- Runtime audio component handles ----
	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UAudioComponent> VOComponent = nullptr;

	FName CurrentMusicKey;

	// ---- Volume state ----
	float MusicVolume = 1.f;
	float VOVolume    = 1.f;
	float SFXVolume   = 1.f;

	bool bMusicDucked = false;
};
