#include "CCRPerformanceGovernorSubsystem.h"
#include "RHI.h"

void UCCRPerformanceGovernorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	DetectTier();
}

void UCCRPerformanceGovernorSubsystem::DetectTier()
{
	const FString GPUName = GRHIAdapterName.ToUpper();

	// High-end: Snapdragon 8 Gen series, Adreno 730+, Apple A15+
	if (GPUName.Contains(TEXT("ADRENO 7"))  ||
		GPUName.Contains(TEXT("ADRENO 8"))  ||
		GPUName.Contains(TEXT("APPLE A15")) ||
		GPUName.Contains(TEXT("APPLE A16")) ||
		GPUName.Contains(TEXT("APPLE A17")) ||
		GPUName.Contains(TEXT("MALI-G710")) ||
		GPUName.Contains(TEXT("MALI-G720")))
	{
		Tier = ECCRPerformanceTier::High;
		return;
	}

	// Low-end: older Mali, Adreno 5xx and below
	if (GPUName.Contains(TEXT("ADRENO 5"))  ||
		GPUName.Contains(TEXT("ADRENO 4"))  ||
		GPUName.Contains(TEXT("MALI-G5"))   ||
		GPUName.Contains(TEXT("MALI-G57"))  ||
		GPUName.Contains(TEXT("MALI-T")))
	{
		Tier = ECCRPerformanceTier::Low;
		return;
	}

	// Default: mid tier
	Tier = ECCRPerformanceTier::Mid;
}

int32 UCCRPerformanceGovernorSubsystem::GetMaxPredictedChunks() const
{
	switch (Tier)
	{
	case ECCRPerformanceTier::High: return 3;
	case ECCRPerformanceTier::Low:  return 1;
	default:                        return 2;
	}
}
