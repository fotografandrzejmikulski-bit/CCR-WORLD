#include "CCRCreditsWidget.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UCCRCreditsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNarrativeEnded.AddDynamic(this, &UCCRCreditsWidget::HandleNarrativeEnded);
		}
	}
	// OnCreditsBegin() is NOT called here to avoid a premature start; the
	// widget is added to the viewport at Collapsed visibility by ACCRGameHUD
	// and only becomes visible (and starts rolling) when HandleNarrativeEnded fires.
}

void UCCRCreditsWidget::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNarrativeEnded.RemoveDynamic(this, &UCCRCreditsWidget::HandleNarrativeEnded);
		}
	}
	Super::NativeDestruct();
}

void UCCRCreditsWidget::HandleNarrativeEnded()
{
	SetVisibility(ESlateVisibility::Visible);
	OnCreditsBegin();
}

void UCCRCreditsWidget::ReturnToMainMenu()
{
	// Ensure the game is unpaused before returning (it may have been paused if
	// the player paused then reached the end node without unpausing).
	UGameplayStatics::SetGamePaused(this, false);
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
	RemoveFromParent();
}
