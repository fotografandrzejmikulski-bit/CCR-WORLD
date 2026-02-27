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
	// Ensure the game is unpaused before returning
	UGameplayStatics::SetGamePaused(this, false);
	// Remove from parent BEFORE OpenLevel so we're not trying to update a widget
	// inside an in-flight level transition.
	RemoveFromParent();
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRCreditsWidget::OnCreditsBegin_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRCreditsWidget::OnCreditsBegin has no C++ or Blueprint implementation."))
#endif
}
