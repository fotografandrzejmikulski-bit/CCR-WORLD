#include "CCRPauseWidget.h"
#include "CCRGameState.h"
#include "Kismet/GameplayStatics.h"

void UCCRPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Notify Blueprint that the widget is visible (pause state entered)
	OnPaused();
}

void UCCRPauseWidget::Resume()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	// Update the game phase
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->SetGamePhase(ECCRGamePhase::Narrative);
		}
	}

	// Notify Blueprint before collapsing so it can play an animation
	OnResumed();

	// Unpause and remove widget
	UGameplayStatics::SetGamePaused(this, false);
	RemoveFromParent();
}

void UCCRPauseWidget::ReturnToMainMenu()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UGameplayStatics::SetGamePaused(this, false);
	}
	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}

void UCCRPauseWidget::QuitGame()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->ConsoleCommand(TEXT("quit"));
	}
}
