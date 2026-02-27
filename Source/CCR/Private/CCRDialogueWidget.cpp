#include "CCRDialogueWidget.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRSettingsSubsystem.h"
#include "CCRSpeakerRegistrySubsystem.h"
#include "CCRTypewriterHelper.h"
#include "CCRStoryChunk.h"

void UCCRDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	// Create the typewriter helper so Blueprint implementations of
	// OnDialogueNode can call TypewriterHelper->Start() without additional setup.
	TypewriterHelper = NewObject<UCCRTypewriterHelper>(this);

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
	// Stop any running typewriter reveal so the previous line doesn't keep ticking.
	if (TypewriterHelper)
	{
		TypewriterHelper->Stop();
	}

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

bool UCCRDialogueWidget::ResolveSpeakerData(FName SpeakerTag, FCCRSpeakerData& OutData) const
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return false;

	if (UCCRSpeakerRegistrySubsystem* Registry = GI->GetSubsystem<UCCRSpeakerRegistrySubsystem>())
	{
		return Registry->GetSpeakerData(SpeakerTag, OutData);
	}
	return false;
}
