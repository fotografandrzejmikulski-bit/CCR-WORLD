#include "CCRSubtitleSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRSettingsSubsystem.h"
#include "CCRTypes.h"
#include "Engine/World.h"

void UCCRSubtitleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
	{
		NRS->OnNodeChanged.AddDynamic(this, &UCCRSubtitleSubsystem::HandleNodeChanged);
	}
}

void UCCRSubtitleSubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->OnNodeChanged.RemoveDynamic(this, &UCCRSubtitleSubsystem::HandleNodeChanged);
		}
	}
	Super::Deinitialize();
}

void UCCRSubtitleSubsystem::HandleNodeChanged(FName NodeId)
{
	UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>();
	const bool bAlwaysOn = Settings ? Settings->GetSubtitlesAlwaysOn() : false;

	if (!bAlwaysOn)
	{
		// Only show subtitles when subtitles-always-on is active.
		// VO-based subtitles are driven externally via ShowSubtitle().
		return;
	}

	UCCRNarrativeRuntimeSubsystem* NRS = GetGameInstance()->GetSubsystem<UCCRNarrativeRuntimeSubsystem>();
	if (!NRS) return;

	FCCRNode Node;
	if (!NRS->GetCurrentNode(Node)) return;

	if (Node.NodeType == ECCRNodeType::Dialogue && !Node.DialogueText.IsEmpty())
	{
		ShowSubtitle(Node.DialogueText, 0.f);
	}
	else
	{
		ClearSubtitle();
	}
}

void UCCRSubtitleSubsystem::ShowSubtitle(const FText& Text, float DurationSec)
{
	CurrentSubtitle = Text;
	OnSubtitleChanged.Broadcast(Text, /*bVisible=*/true);

	if (DurationSec > 0.f)
	{
		ScheduleClear(DurationSec);
	}
	else if (ClearTimer.IsValid())
	{
		// Cancel any pending auto-clear from a previous ShowSubtitle call
		if (UWorld* World = GetGameInstance()->GetWorld())
		{
			World->GetTimerManager().ClearTimer(ClearTimer);
		}
	}
}

void UCCRSubtitleSubsystem::ClearSubtitle()
{
	if (ClearTimer.IsValid())
	{
		if (UWorld* World = GetGameInstance()->GetWorld())
		{
			World->GetTimerManager().ClearTimer(ClearTimer);
		}
	}
	CurrentSubtitle = FText::GetEmpty();
	OnSubtitleChanged.Broadcast(FText::GetEmpty(), /*bVisible=*/false);
}

void UCCRSubtitleSubsystem::ScheduleClear(float DurationSec)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(
		ClearTimer,
		FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			ClearSubtitle();
		}),
		DurationSec,
		/*bLoop=*/false);
}
