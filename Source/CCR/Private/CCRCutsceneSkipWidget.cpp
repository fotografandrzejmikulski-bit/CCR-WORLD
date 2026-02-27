#include "CCRCutsceneSkipWidget.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRCinematicSubsystem.h"
#include "CCRTypes.h"

void UCCRCutsceneSkipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNodeChanged.AddDynamic(this, &UCCRCutsceneSkipWidget::HandleNodeChanged);
		}
	}
}

void UCCRCutsceneSkipWidget::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRCutsceneSkipWidget::HandleNodeChanged);
		}
	}
	Super::NativeDestruct();
}

void UCCRCutsceneSkipWidget::HandleNodeChanged(FName NodeId)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode Node;
	if (!NRS->GetCurrentNode(Node))
	{
		// Left a cinematic — hide
		SetVisibility(ESlateVisibility::Collapsed);
		OnCinematicEnded();
		return;
	}

	if (Node.NodeType == ECCRNodeType::Cinematic && Node.bSkippable)
	{
		SetVisibility(ESlateVisibility::Visible);
		OnCinematicStarted();
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		if (Node.NodeType != ECCRNodeType::Cinematic)
		{
			OnCinematicEnded();
		}
	}
}

void UCCRCutsceneSkipWidget::SkipCinematic()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	if (UCCRCinematicSubsystem* Cinematic = GI->GetSubsystem<UCCRCinematicSubsystem>())
	{
		Cinematic->SkipCinematic();
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRCutsceneSkipWidget::OnCinematicStarted_Implementation()
{
// Default C++ implementation: no-op.
}

void UCCRCutsceneSkipWidget::OnCinematicEnded_Implementation()
{
// Default C++ implementation: no-op.
}
