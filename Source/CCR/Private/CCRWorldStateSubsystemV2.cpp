#include "CCRWorldStateSubsystemV2.h"

void UCCRWorldStateSubsystemV2::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Pre-allocate storage to avoid small reallocations at runtime
	FlagBits.Reserve(4);
	Floats.Reserve(32);
	Ints.Reserve(32);
	Registry.Reserve(128);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

int32 UCCRWorldStateSubsystemV2::FindOrAddFlag(FName Key)
{
	if (FCCRStateEntry* Entry = Registry.Find(Key))
	{
		ensureMsgf(Entry->Type == ECCRStateValueType::Flag,
			TEXT("UCCRWorldStateSubsystemV2: type mismatch for key '%s'"), *Key.ToString());
		return Entry->Index;
	}
	// Count existing flags to derive the next absolute bit index
	int32 FlagCount = 0;
	for (auto& Pair : Registry)
	{
		if (Pair.Value.Type == ECCRStateValueType::Flag) ++FlagCount;
	}
	const int32 WordIndex = FlagCount / 64;
	while (FlagBits.Num() <= WordIndex) FlagBits.Add(0ULL);

	FCCRStateEntry Entry;
	Entry.Type = ECCRStateValueType::Flag;
	Entry.Index = FlagCount; // absolute bit index
	Registry.Add(Key, Entry);
	return FlagCount;
}

int32 UCCRWorldStateSubsystemV2::FindOrAddFloat(FName Key)
{
	if (FCCRStateEntry* Entry = Registry.Find(Key))
	{
		ensureMsgf(Entry->Type == ECCRStateValueType::Float,
			TEXT("UCCRWorldStateSubsystemV2: type mismatch for key '%s'"), *Key.ToString());
		return Entry->Index;
	}
	const int32 Idx = Floats.Num();
	Floats.Add(0.f);
	FCCRStateEntry Entry;
	Entry.Type = ECCRStateValueType::Float;
	Entry.Index = Idx;
	Registry.Add(Key, Entry);
	return Idx;
}

int32 UCCRWorldStateSubsystemV2::FindOrAddInt(FName Key)
{
	if (FCCRStateEntry* Entry = Registry.Find(Key))
	{
		ensureMsgf(Entry->Type == ECCRStateValueType::Int,
			TEXT("UCCRWorldStateSubsystemV2: type mismatch for key '%s'"), *Key.ToString());
		return Entry->Index;
	}
	const int32 Idx = Ints.Num();
	Ints.Add(0);
	FCCRStateEntry Entry;
	Entry.Type = ECCRStateValueType::Int;
	Entry.Index = Idx;
	Registry.Add(Key, Entry);
	return Idx;
}

// ---------------------------------------------------------------------------
// Flag
// ---------------------------------------------------------------------------

bool UCCRWorldStateSubsystemV2::GetFlag(FName Key) const
{
	const FCCRStateEntry* Entry = Registry.Find(Key);
	if (!Entry || Entry->Type != ECCRStateValueType::Flag) return false;
	const int32 WordIdx = Entry->Index / 64;
	const int32 BitIdx  = Entry->Index % 64;
	if (!FlagBits.IsValidIndex(WordIdx)) return false;
	return (FlagBits[WordIdx] & (1ULL << BitIdx)) != 0;
}

void UCCRWorldStateSubsystemV2::SetFlag(FName Key, bool bValue)
{
	const int32 AbsBit = FindOrAddFlag(Key);
	const int32 WordIdx = AbsBit / 64;
	const int32 BitIdx  = AbsBit % 64;
	if (bValue)
		FlagBits[WordIdx] |=  (1ULL << BitIdx);
	else
		FlagBits[WordIdx] &= ~(1ULL << BitIdx);
}

// ---------------------------------------------------------------------------
// Float
// ---------------------------------------------------------------------------

float UCCRWorldStateSubsystemV2::GetFloat(FName Key, float DefaultValue) const
{
	const FCCRStateEntry* Entry = Registry.Find(Key);
	if (!Entry || Entry->Type != ECCRStateValueType::Float) return DefaultValue;
	if (!Floats.IsValidIndex(Entry->Index)) return DefaultValue;
	return Floats[Entry->Index];
}

void UCCRWorldStateSubsystemV2::SetFloat(FName Key, float Value)
{
	const int32 Idx = FindOrAddFloat(Key);
	Floats[Idx] = Value;
}

// ---------------------------------------------------------------------------
// Int
// ---------------------------------------------------------------------------

int32 UCCRWorldStateSubsystemV2::GetInt(FName Key, int32 DefaultValue) const
{
	const FCCRStateEntry* Entry = Registry.Find(Key);
	if (!Entry || Entry->Type != ECCRStateValueType::Int) return DefaultValue;
	if (!Ints.IsValidIndex(Entry->Index)) return DefaultValue;
	return Ints[Entry->Index];
}

void UCCRWorldStateSubsystemV2::SetInt(FName Key, int32 Value)
{
	const int32 Idx = FindOrAddInt(Key);
	Ints[Idx] = Value;
}

// ---------------------------------------------------------------------------
// Serialization
// ---------------------------------------------------------------------------

void UCCRWorldStateSubsystemV2::ExportState(
	TArray<FName>& OutKeys,
	TArray<ECCRStateValueType>& OutTypes,
	TArray<int32>& OutIndices,
	TArray<uint64>& OutFlagBits,
	TArray<float>& OutFloats,
	TArray<int32>& OutInts) const
{
	OutKeys.Reset(Registry.Num());
	OutTypes.Reset(Registry.Num());
	OutIndices.Reset(Registry.Num());

	for (const auto& Pair : Registry)
	{
		OutKeys.Add(Pair.Key);
		OutTypes.Add(Pair.Value.Type);
		OutIndices.Add(Pair.Value.Index);
	}

	OutFlagBits = FlagBits;
	OutFloats   = Floats;
	OutInts     = Ints;
}

void UCCRWorldStateSubsystemV2::ImportState(
	const TArray<FName>& InKeys,
	const TArray<ECCRStateValueType>& InTypes,
	const TArray<int32>& InIndices,
	const TArray<uint64>& InFlagBits,
	const TArray<float>& InFloats,
	const TArray<int32>& InInts)
{
	Registry.Reset();
	FlagBits = InFlagBits;
	Floats   = InFloats;
	Ints     = InInts;

	const int32 Num = InKeys.Num();
	for (int32 i = 0; i < Num; ++i)
	{
		FCCRStateEntry Entry;
		Entry.Type  = InTypes.IsValidIndex(i)   ? InTypes[i]   : ECCRStateValueType::Flag;
		Entry.Index = InIndices.IsValidIndex(i) ? InIndices[i] : 0;
		Registry.Add(InKeys[i], Entry);
	}
}
