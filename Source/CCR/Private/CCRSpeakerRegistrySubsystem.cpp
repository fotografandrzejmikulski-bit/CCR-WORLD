#include "CCRSpeakerRegistrySubsystem.h"

void UCCRSpeakerRegistrySubsystem::RegisterSpeaker(FName SpeakerTag, const FCCRSpeakerData& Data)
{
	if (SpeakerTag.IsNone()) return;
	// Emplace overwrites any existing entry so repeated calls are idempotent.
	Registry.Emplace(SpeakerTag, Data);
}

void UCCRSpeakerRegistrySubsystem::UnregisterSpeaker(FName SpeakerTag)
{
	Registry.Remove(SpeakerTag);
}

bool UCCRSpeakerRegistrySubsystem::GetSpeakerData(FName SpeakerTag, FCCRSpeakerData& OutData) const
{
	if (SpeakerTag.IsNone()) return false;

	const FCCRSpeakerData* Found = Registry.Find(SpeakerTag);
	if (!Found) return false;

	OutData = *Found;
	return true;
}

bool UCCRSpeakerRegistrySubsystem::HasSpeaker(FName SpeakerTag) const
{
	if (SpeakerTag.IsNone()) return false;
	return Registry.Contains(SpeakerTag);
}

TArray<FName> UCCRSpeakerRegistrySubsystem::GetAllSpeakerTags() const
{
	TArray<FName> Keys;
	Registry.GetKeys(Keys);
	return Keys;
}

void UCCRSpeakerRegistrySubsystem::Deinitialize()
{
	Registry.Reset();
	Super::Deinitialize();
}
