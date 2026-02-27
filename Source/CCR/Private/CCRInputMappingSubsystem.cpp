#include "CCRInputMappingSubsystem.h"
#include "CCRNarrativeRuntimeSubsystem.h"
#include "CCRCinematicSubsystem.h"
#include "CCRDebugSubsystem.h"
#include "CCRGameHUD.h"
#include "GameFramework/PlayerController.h"

void UCCRInputMappingSubsystem::SimulateAction(ECCRInputAction Action)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	switch (Action)
	{
	case ECCRInputAction::Advance:
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->AdvanceDialogue();
		}
		break;
	}

	case ECCRInputAction::Skip:
	{
		if (UCCRCinematicSubsystem* Cin = GI->GetSubsystem<UCCRCinematicSubsystem>())
		{
			if (Cin->IsCinematicPlaying())
			{
				Cin->SkipCinematic();
			}
		}
		break;
	}

	case ECCRInputAction::Pause:
	{
		if (UWorld* World = GI->GetWorld())
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				if (ACCRGameHUD* HUD = Cast<ACCRGameHUD>(PC->GetHUD()))
				{
					HUD->TogglePause();
				}
			}
		}
		break;
	}

	case ECCRInputAction::QTE_Confirm:
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->ResolveQTE(true);
		}
		break;
	}

	case ECCRInputAction::QTE_Cancel:
	{
		if (UCCRNarrativeRuntimeSubsystem* NRS = GI->GetSubsystem<UCCRNarrativeRuntimeSubsystem>())
		{
			NRS->ResolveQTE(false);
		}
		break;
	}

	case ECCRInputAction::DebugDump:
	{
		if (UCCRDebugSubsystem* Debug = GI->GetSubsystem<UCCRDebugSubsystem>())
		{
			Debug->DumpWorldState();
			Debug->DumpNarrativeState();
		}
		break;
	}
	}
}

FString UCCRInputMappingSubsystem::GetDefaultKeyName(ECCRInputAction Action) const
{
	switch (Action)
	{
	case ECCRInputAction::Advance:     return TEXT("Enter");
	case ECCRInputAction::Skip:        return TEXT("S");
	case ECCRInputAction::Pause:       return TEXT("Escape");
	case ECCRInputAction::QTE_Confirm: return TEXT("Space");
	case ECCRInputAction::QTE_Cancel:  return TEXT("Backspace");
	case ECCRInputAction::DebugDump:   return TEXT("F9");
	default:                           return TEXT("");
	}
}
