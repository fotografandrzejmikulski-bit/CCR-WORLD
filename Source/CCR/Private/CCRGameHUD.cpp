#include "CCRGameHUD.h"
#include "CCRDialogueWidget.h"
#include "CCRQTEWidget.h"
#include "CCRPauseWidget.h"
#include "CCRGameState.h"
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
			DialogueWidget->AddToViewport(0);
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
			QTEWidget->AddToViewport(1); // above dialogue
			QTEWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	// Note: PauseWidget is created on demand in TogglePause().

	// ---- Subscribe to game phase changes ----
	if (UWorld* World = GetWorld())
	{
		if (ACCRGameState* GS = World->GetGameState<ACCRGameState>())
		{
			GS->OnGamePhaseChanged.AddDynamic(this, &ACCRGameHUD::HandleGamePhaseChanged);
		}
	}
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
			PauseWidget->AddToViewport(10); // topmost z-order
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
}
