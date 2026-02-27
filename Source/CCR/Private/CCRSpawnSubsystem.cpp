#include "CCRSpawnSubsystem.h"

void UCCRSpawnSubsystem::RegisterSpawnPoint(FName SpawnTag, FVector Location, FRotator Rotation)
{
	if (SpawnTag.IsNone()) return;

	FCCRSpawnEntry Entry;
	Entry.Location = Location;
	Entry.Rotation = Rotation;
	// Emplace overwrites any existing entry for this tag so that loading a
	// level twice does not leave stale duplicate entries.
	Registry.Emplace(SpawnTag, Entry);
}

void UCCRSpawnSubsystem::UnregisterSpawnPoint(FName SpawnTag)
{
	Registry.Remove(SpawnTag);
}

bool UCCRSpawnSubsystem::GetSpawnTransform(FName SpawnTag, FTransform& OutTransform) const
{
	if (SpawnTag.IsNone()) return false;

	const FSpawnEntry* Entry = Registry.Find(SpawnTag);
	if (!Entry) return false;

	OutTransform = FTransform(Entry->Rotation, Entry->Location);
	return true;
}

bool UCCRSpawnSubsystem::HasSpawnPoint(FName SpawnTag) const
{
	if (SpawnTag.IsNone()) return false;
	return Registry.Contains(SpawnTag);
}

TArray<FName> UCCRSpawnSubsystem::GetAllSpawnTags() const
{
	TArray<FName> Keys;
	Registry.GetKeys(Keys);
	return Keys;
}

void UCCRSpawnSubsystem::Deinitialize()
{
	Registry.Reset();
	Super::Deinitialize();
}
