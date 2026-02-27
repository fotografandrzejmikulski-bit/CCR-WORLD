#include "CCRQTEWidget.h"
#include "CCRNarrativeRuntimeSubsystem.h"

void UCCRQTEWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	NRS->OnNodeChanged.AddDynamic(this, &UCCRQTEWidget::HandleNodeChanged);
}

void UCCRQTEWidget::NativeDestruct()
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
		if (NRS)
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRQTEWidget::HandleNodeChanged);
		}
	}
	Super::NativeDestruct();
}

void UCCRQTEWidget::HandleNodeChanged(FName NodeId)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode Node;
	if (!NRS->GetCurrentNode(Node)) return;
	if (Node.NodeType != ECCRNodeType::QTE) return;

	// Notify Blueprint so it can display the correct gesture icon before the
	// QTE timer starts (e.g. a hold-circle for LongPress, a directional arrow
	// for Swipe, or a simple tap icon for Tap).
	OnQTEPromptReady(Node.GestureType, Node.RequiredSwipeDir);
}

