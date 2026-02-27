#include "CCRMainMenuWidget.h"
#include "CCR.h"
#include "CCRGameMode.h"
#include "CCRResumeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UCCRMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const bool bHasSave = UGameplayStatics::DoesSaveGameExist(
		CCRSaveSlots::WorldState, /*UserIndex=*/0);
	OnMainMenuReady(bHasSave);
}

void UCCRMainMenuWidget::NewGame()
{
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameMode* GM = World->GetAuthGameMode<ACCRGameMode>())
		{
			GM->StartNewGame();
		}
	}
}

void UCCRMainMenuWidget::Continue()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRResumeSubsystem* Resume = GI->GetSubsystem<UCCRResumeSubsystem>();
	if (Resume && Resume->ResumeFromDefaultSlot())
	{
		return;
	}

	// No save exists; fall back to starting a new game.
	NewGame();
}

void UCCRMainMenuWidget::QuitGame()
{
	// UKismetSystemLibrary::QuitGame handles all platforms (including Android).
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, /*bIgnorePlatformRestrictions=*/false);
	}
}
