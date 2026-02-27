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
