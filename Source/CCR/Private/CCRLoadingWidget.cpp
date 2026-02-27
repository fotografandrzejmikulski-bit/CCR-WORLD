#include "CCRLoadingWidget.h"

void UCCRLoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->OnGamePhaseChanged.AddDynamic(this, &UCCRLoadingWidget::HandleGamePhaseChanged);
			// Apply current phase immediately in case we were added mid-load
			HandleGamePhaseChanged(GS->GetGamePhase());
		}
	}
}

void UCCRLoadingWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->OnGamePhaseChanged.RemoveDynamic(this, &UCCRLoadingWidget::HandleGamePhaseChanged);
		}
	}
	Super::NativeDestruct();
}

void UCCRLoadingWidget::HandleGamePhaseChanged(ECCRGamePhase NewPhase)
{
	const bool bNowLoading = (NewPhase == ECCRGamePhase::Loading);
	if (bNowLoading == bWasLoading) return;

	bWasLoading = bNowLoading;

	if (bNowLoading)
	{
		OnLoadingStarted();
	}
	else
	{
		OnLoadingFinished();
	}
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRLoadingWidget::OnLoadingStarted_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRLoadingWidget::OnLoadingStarted has no C++ or Blueprint implementation."))
#endif
}

void UCCRLoadingWidget::OnLoadingFinished_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRLoadingWidget::OnLoadingFinished has no C++ or Blueprint implementation."))
#endif
}
