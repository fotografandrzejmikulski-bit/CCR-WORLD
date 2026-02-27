#include "CCRAchievementSubsystem.h"
#include "CCRWorldStateSubsystemV2.h"
#include "CCRNotificationSubsystem.h"

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

FName UCCRAchievementSubsystem::AchievementFlagKey(FName AchievementId)
{
	return FName(*FString::Printf(TEXT("ACH_%s"), *AchievementId.ToString()));
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void UCCRAchievementSubsystem::RegisterAchievement(FName AchievementId, const FCCRAchievementDef& Def)
{
	if (AchievementId.IsNone()) return;
	Definitions.Emplace(AchievementId, Def);
}

// ---------------------------------------------------------------------------
// Unlock
// ---------------------------------------------------------------------------

bool UCCRAchievementSubsystem::UnlockAchievement(FName AchievementId)
{
	if (AchievementId.IsNone()) return false;

	// Already unlocked — no-op
	if (IsAchievementUnlocked(AchievementId)) return false;

	UGameInstance* GI = GetGameInstance();

	// Mark as unlocked in world state so it persists across saves
	if (UCCRWorldStateSubsystemV2* WSM = GI->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		WSM->SetFlag(AchievementFlagKey(AchievementId), true);

		// Mirror into an optional secondary key if requested
		if (const FCCRAchievementDef* Def = Definitions.Find(AchievementId))
		{
			if (!Def->MirrorWorldStateKey.IsNone())
			{
				WSM->SetFlag(Def->MirrorWorldStateKey, true);
			}
		}
	}

	// Broadcast delegate
	const FCCRAchievementDef DefaultDef;
	const FCCRAchievementDef& Def = Definitions.Contains(AchievementId)
		? Definitions[AchievementId]
		: DefaultDef;

	OnAchievementUnlocked.Broadcast(AchievementId, Def);

	// Show a notification
	if (UCCRNotificationSubsystem* NS = GI->GetSubsystem<UCCRNotificationSubsystem>())
	{
		const FText Message = Def.Title.IsEmpty()
			? FText::FromName(AchievementId)
			: Def.Title;
		NS->ShowNotification(Message, 4.f, ECCRNotificationType::Achievement);
	}

	return true;
}

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------

bool UCCRAchievementSubsystem::IsAchievementUnlocked(FName AchievementId) const
{
	if (AchievementId.IsNone()) return false;

	if (UCCRWorldStateSubsystemV2* WSM = GetGameInstance()->GetSubsystem<UCCRWorldStateSubsystemV2>())
	{
		return WSM->GetFlag(AchievementFlagKey(AchievementId));
	}
	return false;
}

TArray<FName> UCCRAchievementSubsystem::GetAllAchievementIds() const
{
	TArray<FName> Keys;
	Definitions.GetKeys(Keys);
	return Keys;
}

bool UCCRAchievementSubsystem::GetAchievementDef(FName AchievementId, FCCRAchievementDef& OutDef) const
{
	if (AchievementId.IsNone()) return false;
	const FCCRAchievementDef* Found = Definitions.Find(AchievementId);
	if (!Found) return false;
	OutDef = *Found;
	return true;
}
