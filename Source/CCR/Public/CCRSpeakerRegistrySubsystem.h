#pragma once

#include "CoreMinimal.h"
#include "CCRTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRSpeakerRegistrySubsystem.generated.h"

/**
 * UCCRSpeakerRegistrySubsystem
 *
 * Maps SpeakerTag (FName) → FCCRSpeakerData at runtime.
 *
 * Populate the registry at game startup (e.g. from a Blueprint GameInstance
 * subclass, or from a DataTable-driven setup actor) by calling
 * RegisterSpeaker() for every speaker in the project.
 *
 * UCCRDialogueWidget::ResolveSpeakerData() queries this subsystem so that
 * Blueprint dialogue-UI implementations can display the correct display name
 * and portrait without embedding raw FText or texture references in every node.
 *
 * UCCRAudioSubsystem reads FCCRSpeakerData::VOKeyPrefix when constructing VO
 * cue keys for dialogue nodes, allowing per-speaker audio libraries.
 */
UCLASS(BlueprintType, Blueprintable)
class CCR_API UCCRSpeakerRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Register a speaker.
	 * If a speaker with the same tag already exists it is overwritten, so
	 * calling this function multiple times (e.g. on hot-reload) is safe.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Speaker")
	void RegisterSpeaker(FName SpeakerTag, const FCCRSpeakerData& Data);

	/**
	 * Remove a speaker from the registry.
	 * Safe to call even if the tag was never registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Speaker")
	void UnregisterSpeaker(FName SpeakerTag);

	/**
	 * Look up speaker data by tag.
	 * Returns true and fills OutData when the tag is registered.
	 * Returns false when the tag is None or not present in the registry.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Speaker")
	bool GetSpeakerData(FName SpeakerTag, FCCRSpeakerData& OutData) const;

	/** Returns true when the tag has a registered entry. */
	UFUNCTION(BlueprintPure, Category = "CCR|Speaker")
	bool HasSpeaker(FName SpeakerTag) const;

	/** Returns all currently registered speaker tags. */
	UFUNCTION(BlueprintPure, Category = "CCR|Speaker")
	TArray<FName> GetAllSpeakerTags() const;

	/** Remove all entries (called automatically on subsystem shutdown). */
	virtual void Deinitialize() override;

private:
	TMap<FName, FCCRSpeakerData> Registry;
};
