#include "CCRMainMenuWidget.h"
#include "CCR.h"
#include "CCRGameMode.h"
#include "CCRResumeSubsystem.h"
#include "CCRSettingsWidget.h"
#include "CCRChapterSelectWidget.h"
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

void UCCRMainMenuWidget::OpenSettings()
{
	if (!SettingsWidget)
	{
		TSubclassOf<UCCRSettingsWidget> SettingsClass = SettingsWidgetClass.IsValid()
			? SettingsWidgetClass.Get()
			: nullptr;

		if (!SettingsClass)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("UCCRMainMenuWidget: SettingsWidgetClass is not set. "
					 "Assign a Blueprint subclass of UCCRSettingsWidget in the main menu widget defaults."));
			return;
		}

		APlayerController* PC = GetOwningPlayer();
		if (!PC) return;

		SettingsWidget = CreateWidget<UCCRSettingsWidget>(PC, SettingsClass);
		if (!SettingsWidget) return;

		SettingsWidget->OnClosed.AddDynamic(this, &UCCRMainMenuWidget::HandleSettingsClosed);
		SettingsWidget->AddToViewport(CCRZOrder::Settings);
	}
	else
	{
		SettingsWidget->SetVisibility(ESlateVisibility::Visible);
	}

	OnSettingsOpened();
}

void UCCRMainMenuWidget::HandleSettingsClosed()
{
	OnSettingsClosed();
}

void UCCRMainMenuWidget::OpenChapterSelect()
{
	if (!ChapterSelectWidget)
	{
		TSubclassOf<UCCRChapterSelectWidget> ChapterClass = ChapterSelectWidgetClass.IsValid()
			? ChapterSelectWidgetClass.Get()
			: nullptr;

		if (!ChapterClass)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("UCCRMainMenuWidget: ChapterSelectWidgetClass is not set. "
					 "Assign a Blueprint subclass of UCCRChapterSelectWidget in the main menu widget defaults."));
			return;
		}

		APlayerController* PC = GetOwningPlayer();
		if (!PC) return;

		ChapterSelectWidget = CreateWidget<UCCRChapterSelectWidget>(PC, ChapterClass);
		if (!ChapterSelectWidget) return;

		ChapterSelectWidget->OnClosed.AddDynamic(this, &UCCRMainMenuWidget::HandleChapterSelectClosed);
		ChapterSelectWidget->AddToViewport(CCRZOrder::ChapterSelect);
	}
	else
	{
		ChapterSelectWidget->SetVisibility(ESlateVisibility::Visible);
	}

	OnChapterSelectOpened();
}

void UCCRMainMenuWidget::HandleChapterSelectClosed()
{
	OnChapterSelectClosed();
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRMainMenuWidget::OnMainMenuReady_Implementation(bool bHasSave)
{
// Default C++ implementation: no-op.
}

void UCCRMainMenuWidget::OnSettingsOpened_Implementation()
{
// Default C++ implementation: no-op.
}

void UCCRMainMenuWidget::OnSettingsClosed_Implementation()
{
// Default C++ implementation: no-op.
}

void UCCRMainMenuWidget::OnChapterSelectOpened_Implementation()
{
// Default C++ implementation: no-op.
}

void UCCRMainMenuWidget::OnChapterSelectClosed_Implementation()
{
// Default C++ implementation: no-op.
}
