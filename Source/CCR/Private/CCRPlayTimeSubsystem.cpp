#include "CCRPlayTimeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UCCRPlayTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Register a per-frame tick delegate.
	// FTSTicker is the safe way to get per-frame callbacks from a non-actor UObject.
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateWeakLambda(this,
			[this](float DeltaTime) -> bool
			{
				return HandleTick(DeltaTime);
			}));
}

void UCCRPlayTimeSubsystem::Deinitialize()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	Super::Deinitialize();
}

bool UCCRPlayTimeSubsystem::HandleTick(float DeltaTime)
{
	// Accumulate time only when the game is not paused.
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		if (!UGameplayStatics::IsGamePaused(World))
		{
			PlayTimeSec += DeltaTime;
		}
	}
	return true; // keep ticking
}

FString UCCRPlayTimeSubsystem::GetPlayTimeFormatted() const
{
	const int32 TotalSeconds = FMath::FloorToInt(PlayTimeSec);
	const int32 Hours   = TotalSeconds / 3600;
	const int32 Minutes = (TotalSeconds % 3600) / 60;
	const int32 Seconds = TotalSeconds % 60;
	return FString::Printf(TEXT("%02d:%02d:%02d"), Hours, Minutes, Seconds);
}

void UCCRPlayTimeSubsystem::SetPlayTimeSec(float Seconds)
{
	PlayTimeSec = FMath::Max(0.f, Seconds);
}
