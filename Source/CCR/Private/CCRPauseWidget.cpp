#include "CCRPauseWidget.h"
#include "CCR.h"
#include "CCRSettingsWidget.h"
#include "CCRChapterSelectWidget.h"
#include "CCRGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
	// UKismetSystemLibrary::QuitGame handles all platforms (including Android).
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, /*bIgnorePlatformRestrictions=*/false);
	}
}

void UCCRPauseWidget::OpenSettings()
{
	// Lazily create the settings widget on first use.
	if (!SettingsWidget)
	{
		TSubclassOf<UCCRSettingsWidget> SettingsClass = SettingsWidgetClass.IsValid()
			? SettingsWidgetClass.Get()
			: nullptr;

		if (!SettingsClass)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("UCCRPauseWidget: SettingsWidgetClass is not set. "
					 "Assign a Blueprint subclass of UCCRSettingsWidget in the pause widget defaults."));
			return;
		}

		APlayerController* PC = GetOwningPlayer();
		if (!PC) return;

		SettingsWidget = CreateWidget<UCCRSettingsWidget>(PC, SettingsClass);
		if (!SettingsWidget) return;

		// Subscribe so we can restore the pause menu when settings close.
		SettingsWidget->OnClosed.AddDynamic(this, &UCCRPauseWidget::HandleSettingsClosed);
		SettingsWidget->AddToViewport(CCRZOrder::Settings);
	}
	else
	{
		SettingsWidget->SetVisibility(ESlateVisibility::Visible);
	}

	// Notify Blueprint to hide the pause menu body.
	OnSettingsOpened();
}

void UCCRPauseWidget::HandleSettingsClosed()
{
	// Settings widget hides itself; restore the pause menu body.
	OnSettingsClosed();
}

void UCCRPauseWidget::OpenChapterSelect()
{
	if (!ChapterSelectWidget)
	{
		TSubclassOf<UCCRChapterSelectWidget> ChapterClass = ChapterSelectWidgetClass.IsValid()
			? ChapterSelectWidgetClass.Get()
			: nullptr;

		if (!ChapterClass)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("UCCRPauseWidget: ChapterSelectWidgetClass is not set. "
					 "Assign a Blueprint subclass of UCCRChapterSelectWidget in the pause widget defaults."));
			return;
		}

		APlayerController* PC = GetOwningPlayer();
		if (!PC) return;

		ChapterSelectWidget = CreateWidget<UCCRChapterSelectWidget>(PC, ChapterClass);
		if (!ChapterSelectWidget) return;

		ChapterSelectWidget->OnClosed.AddDynamic(this, &UCCRPauseWidget::HandleChapterSelectClosed);
		ChapterSelectWidget->AddToViewport(CCRZOrder::ChapterSelect);
	}
	else
	{
		ChapterSelectWidget->SetVisibility(ESlateVisibility::Visible);
	}

	// Notify Blueprint to hide the pause menu body.
	OnChapterSelectOpened();
}

void UCCRPauseWidget::HandleChapterSelectClosed()
{
	// Chapter select widget hides itself; restore the pause menu body.
	OnChapterSelectClosed();
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRPauseWidget::OnPaused_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRPauseWidget::OnPaused has no C++ or Blueprint implementation."))
#endif
}

void UCCRPauseWidget::OnResumed_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRPauseWidget::OnResumed has no C++ or Blueprint implementation."))
#endif
}

void UCCRPauseWidget::OnSettingsOpened_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRPauseWidget::OnSettingsOpened has no C++ or Blueprint implementation."))
#endif
}

void UCCRPauseWidget::OnSettingsClosed_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRPauseWidget::OnSettingsClosed has no C++ or Blueprint implementation."))
#endif
}

void UCCRPauseWidget::OnChapterSelectOpened_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRPauseWidget::OnChapterSelectOpened has no C++ or Blueprint implementation."))
#endif
}

void UCCRPauseWidget::OnChapterSelectClosed_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRPauseWidget::OnChapterSelectClosed has no C++ or Blueprint implementation."))
#endif
}
