#include "CCRGameHUD.h"
#include "CCR.h"
#include "CCRDialogueWidget.h"
#include "CCRQTEWidget.h"
#include "CCRPauseWidget.h"
#include "CCRLoadingWidget.h"
#include "CCRMainMenuWidget.h"
#include "CCRSettingsWidget.h"
#include "CCRNotificationWidget.h"
#include "CCRChapterTransitionWidget.h"
#include "CCRCreditsWidget.h"
#include "CCRGameState.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRStoryChunk.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ACCRGameHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	// ---- Dialogue widget ----
	TSubclassOf<UCCRDialogueWidget> DlgClass = DialogueWidgetClass.IsValid()
		? DialogueWidgetClass.Get()
		: nullptr;

	if (!DlgClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRGameHUD: DialogueWidgetClass is not set. "
				 "Assign a Blueprint subclass of UCCRDialogueWidget in the HUD defaults."));
	}
	else
	{
		DialogueWidget = CreateWidget<UCCRDialogueWidget>(PC, DlgClass);
		if (DialogueWidget)
		{
			DialogueWidget->AddToViewport(CCRZOrder::Dialogue);
		}
	}

	// ---- QTE widget ----
	TSubclassOf<UCCRQTEWidget> QTEClass = QTEWidgetClass.IsValid()
		? QTEWidgetClass.Get()
		: nullptr;

	if (!QTEClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRGameHUD: QTEWidgetClass is not set. "
				 "Assign a Blueprint subclass of UCCRQTEWidget in the HUD defaults."));
	}
	else
	{
		QTEWidget = CreateWidget<UCCRQTEWidget>(PC, QTEClass);
		if (QTEWidget)
		{
			QTEWidget->AddToViewport(CCRZOrder::QTE);
			QTEWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	// Note: PauseWidget is created on demand in TogglePause().

	// ---- Loading widget ----
	TSubclassOf<UCCRLoadingWidget> LoadingClass = LoadingWidgetClass.IsValid()
		? LoadingWidgetClass.Get()
		: nullptr;

	if (!LoadingClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRGameHUD: LoadingWidgetClass is not set. "
				 "Assign a Blueprint subclass of UCCRLoadingWidget in the HUD defaults."));
	}
	else
	{
		LoadingWidget = CreateWidget<UCCRLoadingWidget>(PC, LoadingClass);
		if (LoadingWidget)
		{
			LoadingWidget->AddToViewport(CCRZOrder::Loading);
		}
	}

	// ---- Main menu widget ----
	TSubclassOf<UCCRMainMenuWidget> MainMenuClass = MainMenuWidgetClass.IsValid()
		? MainMenuWidgetClass.Get()
		: nullptr;

	if (!MainMenuClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRGameHUD: MainMenuWidgetClass is not set. "
				 "Assign a Blueprint subclass of UCCRMainMenuWidget in the HUD defaults."));
	}
	else
	{
		MainMenuWidget = CreateWidget<UCCRMainMenuWidget>(PC, MainMenuClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport(CCRZOrder::MainMenu);
		}
	}

	// ---- Notification widget ----
	TSubclassOf<UCCRNotificationWidget> NotificationClass = NotificationWidgetClass.IsValid()
		? NotificationWidgetClass.Get()
		: nullptr;

	if (!NotificationClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRGameHUD: NotificationWidgetClass is not set. "
				 "Assign a Blueprint subclass of UCCRNotificationWidget in the HUD defaults."));
	}
	else
	{
		NotificationWidget = CreateWidget<UCCRNotificationWidget>(PC, NotificationClass);
		if (NotificationWidget)
		{
			NotificationWidget->AddToViewport(CCRZOrder::Notification);
		}
	}

	// ---- Chapter transition widget ----
	TSubclassOf<UCCRChapterTransitionWidget> ChapterClass = ChapterTransitionWidgetClass.IsValid()
		? ChapterTransitionWidgetClass.Get()
		: nullptr;

	if (!ChapterClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRGameHUD: ChapterTransitionWidgetClass is not set. "
				 "Assign a Blueprint subclass of UCCRChapterTransitionWidget in the HUD defaults."));
	}
	else
	{
		ChapterTransitionWidget = CreateWidget<UCCRChapterTransitionWidget>(PC, ChapterClass);
		if (ChapterTransitionWidget)
		{
			ChapterTransitionWidget->AddToViewport(CCRZOrder::ChapterTransition);
			ChapterTransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// ---- Credits widget ----
	TSubclassOf<UCCRCreditsWidget> CreditsClass = CreditsWidgetClass.IsValid()
		? CreditsWidgetClass.Get()
		: nullptr;

	if (!CreditsClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ACCRGameHUD: CreditsWidgetClass is not set. "
				 "Assign a Blueprint subclass of UCCRCreditsWidget in the HUD defaults."));
	}
	else
	{
		CreditsWidget = CreateWidget<UCCRCreditsWidget>(PC, CreditsClass);
		if (CreditsWidget)
		{
			CreditsWidget->AddToViewport(CCRZOrder::Credits);
			CreditsWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// ---- Subscribe to game phase changes ----
	// Also apply the current phase immediately so widgets start in the correct
	// visibility state (e.g. dialogue hidden while phase is Loading or Cinematic).
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->OnGamePhaseChanged.AddDynamic(this, &ACCRGameHUD::HandleGamePhaseChanged);
			HandleGamePhaseChanged(GS->GetGamePhase());
		}
	}

	// ---- Subscribe to narrative chunk events ----
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnChunkStarted.AddDynamic(this, &ACCRGameHUD::HandleChunkStarted);
		}
	}
}

void ACCRGameHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->OnGamePhaseChanged.RemoveDynamic(this, &ACCRGameHUD::HandleGamePhaseChanged);
		}
	}
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnChunkStarted.RemoveDynamic(this, &ACCRGameHUD::HandleChunkStarted);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ACCRGameHUD::SetDialogueVisible(bool bVisible)
{
	if (DialogueWidget)
	{
		DialogueWidget->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void ACCRGameHUD::SetQTEVisible(bool bVisible)
{
	if (QTEWidget)
	{
		QTEWidget->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void ACCRGameHUD::SetLoadingVisible(bool bVisible)
{
	if (LoadingWidget)
	{
		LoadingWidget->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void ACCRGameHUD::SetMainMenuVisible(bool bVisible)
{
	if (MainMenuWidget)
	{
		MainMenuWidget->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void ACCRGameHUD::SetSettingsVisible(bool bVisible)
{
	if (SettingsWidget)
	{
		SettingsWidget->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void ACCRGameHUD::SetCreditsVisible(bool bVisible)
{
	if (CreditsWidget)
	{
		CreditsWidget->SetVisibility(
			bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void ACCRGameHUD::TogglePause()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	const bool bCurrentlyPaused = UGameplayStatics::IsGamePaused(GetWorld());

	if (bCurrentlyPaused)
	{
		// Resume: delegate to widget so it can play an out-animation first
		if (PauseWidget)
		{
			PauseWidget->Resume();
		}
		else
		{
			// Fallback: no widget — still restore the game phase so the rest
			// of the game (HUD visibility, audio, etc.) is not left in Paused.
			UE_LOG(LogTemp, Warning,
				TEXT("ACCRGameHUD: TogglePause resume — PauseWidget is null; "
					 "restoring phase to Narrative directly."));
			if (UWorld* World = GetWorld())
			{
				if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
				{
					GS->SetGamePhase(ECCRGamePhase::Narrative);
				}
			}
			UGameplayStatics::SetGamePaused(this, false);
		}
	}
	else
	{
		// Pause: create widget on first use, then pause the game
		if (!PauseWidget)
		{
			TSubclassOf<UCCRPauseWidget> PauseClass = PauseWidgetClass.IsValid()
				? PauseWidgetClass.Get()
				: nullptr;

			if (!PauseClass)
			{
				UE_LOG(LogTemp, Warning,
					TEXT("ACCRGameHUD: PauseWidgetClass is not set. "
						 "Assign a Blueprint subclass of UCCRPauseWidget in the HUD defaults."));
			}
			else
			{
				PauseWidget = CreateWidget<UCCRPauseWidget>(PC, PauseClass);
				if (PauseWidget && SettingsWidgetClass.IsValid())
				{
					// Forward the settings widget class so the pause menu can
					// open it without needing its own UPROPERTY to be set.
					PauseWidget->SettingsWidgetClass = SettingsWidgetClass;
				}
			}
		}

		// Update game phase
		if (UWorld* World = GetWorld())
		{
			if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
			{
				GS->SetGamePhase(ECCRGamePhase::Paused);
			}
		}

		if (PauseWidget)
		{
			PauseWidget->AddToViewport(CCRZOrder::Pause);
		}

		UGameplayStatics::SetGamePaused(this, true);
	}
}

bool ACCRGameHUD::IsPaused() const
{
	return UGameplayStatics::IsGamePaused(GetWorld());
}

// ---------------------------------------------------------------------------
// Phase-driven widget visibility
// ---------------------------------------------------------------------------

void ACCRGameHUD::HandleGamePhaseChanged(ECCRGamePhase NewPhase)
{
	// Main menu overlay: visible only during the MainMenu phase.
	SetMainMenuVisible(NewPhase == ECCRGamePhase::MainMenu);

	// Loading overlay: visible only during the Loading phase.
	SetLoadingVisible(NewPhase == ECCRGamePhase::Loading);

	// Dialogue panel: visible during Narrative and QTE phases only.
	const bool bShowDialogue = (NewPhase == ECCRGamePhase::Narrative ||
	                            NewPhase == ECCRGamePhase::QTE);
	SetDialogueVisible(bShowDialogue);

	// QTE overlay: only during QTE phase (the controller drives per-node show/hide,
	// but if we leave QTE phase externally we force-hide it).
	if (NewPhase != ECCRGamePhase::QTE)
	{
		SetQTEVisible(false);
	}

	// Credits: visible during any end-game state. Visibility is driven by
	// UCCRCreditsWidget::HandleNarrativeEnded which subscribes directly to
	// UCCRNarrativeRuntimeSubsystem::OnNarrativeEnded; no phase-based control needed.
}

// ---------------------------------------------------------------------------
// Chapter transition handler
// ---------------------------------------------------------------------------

void ACCRGameHUD::HandleChunkStarted(UCCRStoryChunk* Chunk)
{
	if (!Chunk) return;
	if (Chunk->ChapterTitle.IsEmpty()) return;

	if (ChapterTransitionWidget)
	{
		ChapterTransitionWidget->ShowChapterTitle(Chunk->ChapterTitle, Chunk->ChapterSubtitle);
	}
}
