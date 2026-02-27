#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRAchievementSubsystem.generated.h"

/**
 * Metadata describing a single achievement definition.
 * Register achievements at game-instance startup via
 * UCCRAchievementSubsystem::RegisterAchievement().
 */
USTRUCT(BlueprintType)
struct FCCRAchievementDef
{
	GENERATED_BODY()

	/** Localised display title shown in the achievement notification. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	/** Longer description of how to earn the achievement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/**
	 * Optional portrait / icon texture for the notification widget.
	 * Soft-referenced so it is only loaded when the achievement unlocks.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	/**
	 * Optional world-state flag key to mirror the unlock status into.
	 * When set, UnlockAchievement() also calls WSM->SetFlag(MirrorKey, true).
	 * This allows story conditions to branch on achievement status.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MirrorWorldStateKey;
};

/**
 * UCCRAchievementSubsystem
 *
 * Tracks which named achievements have been unlocked in the current session.
 * Achievement data is automatically persisted via UCCRWorldStateSaveGameV2
 * using a dedicated flag namespace (achievement IDs are stored as world-state
 * flags under the key "ACH_<AchievementId>" so they survive save/load cycles
 * without a separate save format).
 *
 * On unlock:
 *   1. The achievement is recorded in the world state under "ACH_<Id>".
 *   2. If FCCRAchievementDef::MirrorWorldStateKey is set, that key is also set.
 *   3. OnAchievementUnlocked is broadcast.
 *   4. UCCRNotificationSubsystem::ShowNotification() is called automatically
 *      with Type = ECCRNotificationType::Achievement.
 *
 * Blueprint GameInstance override can call RegisterAchievement() for all
 * project achievements in UCCRGameInstance::OnCCRInit().
 */
UCLASS(BlueprintType)
class CCR_API UCCRAchievementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Register an achievement definition.  Safe to call multiple times with
	 * the same ID (overwrites the previous definition).
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Achievement")
	void RegisterAchievement(FName AchievementId, const FCCRAchievementDef& Def);

	/**
	 * Unlock an achievement.
	 * If already unlocked this is a no-op (no duplicate notifications).
	 * Returns true on a fresh unlock, false when already unlocked.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Achievement")
	bool UnlockAchievement(FName AchievementId);

	/** Returns true when the achievement has been unlocked this session or in a prior save. */
	UFUNCTION(BlueprintPure, Category = "CCR|Achievement")
	bool IsAchievementUnlocked(FName AchievementId) const;

	/** Returns all registered achievement IDs. */
	UFUNCTION(BlueprintPure, Category = "CCR|Achievement")
	TArray<FName> GetAllAchievementIds() const;

	/** Returns the definition for an achievement (struct is default-initialised if not found). */
	UFUNCTION(BlueprintCallable, Category = "CCR|Achievement")
	bool GetAchievementDef(FName AchievementId, FCCRAchievementDef& OutDef) const;

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCCRAchievementUnlocked,
	                                             FName, AchievementId,
	                                             const FCCRAchievementDef&, Def);

	/** Broadcast on the first unlock of any achievement. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Achievement")
	FOnCCRAchievementUnlocked OnAchievementUnlocked;

private:
	TMap<FName, FCCRAchievementDef> Definitions;

	/** Derives the world-state flag key used to persist an achievement. */
	static FName AchievementFlagKey(FName AchievementId);
};
