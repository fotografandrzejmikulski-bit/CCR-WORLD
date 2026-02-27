#include "CCRGameHUD.h"
#include "CCRDialogueWidget.h"
#include "CCRQTEWidget.h"
#include "CCRGameState.h"
#include "Blueprint/UserWidget.h"

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
