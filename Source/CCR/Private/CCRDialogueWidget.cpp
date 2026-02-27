#include "CCRDialogueWidget.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRSettingsSubsystem.h"
#include "CCRStoryChunk.h"

void UCCRDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	NRS->OnNodeChanged.AddDynamic(this, &UCCRDialogueWidget::HandleNodeChanged);
	NRS->OnChoicePresented.AddDynamic(this, &UCCRDialogueWidget::HandleChoicePresented);
}

void UCCRDialogueWidget::NativeDestruct()
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
		if (NRS)
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRDialogueWidget::HandleNodeChanged);
			NRS->OnChoicePresented.RemoveDynamic(this, &UCCRDialogueWidget::HandleChoicePresented);
		}
	}
	Super::NativeDestruct();
}

void UCCRDialogueWidget::HandleNodeChanged(FName NodeId)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode Node;
	if (NRS->GetCurrentNode(Node))
	{
		if (Node.NodeType == ECCRNodeType::Dialogue)
		{
			OnDialogueNode(Node.DialogueText, Node.SpeakerTag);
		}
		else if (Node.NodeType == ECCRNodeType::End)
		{
			OnNarrativeEnd();
		}
	}
}

void UCCRDialogueWidget::HandleChoicePresented(const TArray<FCCRChoiceOption>& Choices)
{
	OnChoicePresented(Choices);
}

void UCCRDialogueWidget::SelectChoice(int32 ChoiceIndex)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (NRS)
	{
		NRS->SelectChoice(ChoiceIndex);
	}
}

void UCCRDialogueWidget::AdvanceDialogue()
{
	// "Tap to continue" – tells the NRS to move to NextAfterDialogue for Dialogue nodes.
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (NRS)
	{
		NRS->AdvanceDialogue();
	}
}

bool UCCRDialogueWidget::ShouldShowSubtitles() const
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return true;

	if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
	{
		return Settings->GetSubtitlesAlwaysOn();
	}
	return true;
}
