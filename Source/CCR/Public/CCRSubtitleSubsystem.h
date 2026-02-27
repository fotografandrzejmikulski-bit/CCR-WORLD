#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRSubtitleSubsystem.generated.h"

/**
 * FCCRSubtitleEntry
 *
 * A single timed subtitle line.  Used by UCCRSubtitleSubsystem to queue
 * display of subtitle text during VO playback.
 */
USTRUCT(BlueprintType)
struct FCCRSubtitleEntry
{
	GENERATED_BODY()

	/** Localised subtitle text to display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	/** When this subtitle should appear (seconds from the start of VO playback). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTimeSec = 0.f;

	/** How long this subtitle should remain on screen. 0 = until the next entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationSec = 0.f;
};

/**
 * UCCRSubtitleSubsystem
 *
 * Manages automatic subtitle display for voice-over lines.
 *
 * Responsibilities:
 *   1. Listens to UCCRAudioSubsystem::OnVOStarted / OnVOFinished to auto-show
 *      subtitles based on the active VO key.
 *   2. Exposes ShowSubtitle(FText, DurationSec) for manual/fallback display when
 *      no VO is playing but text should still appear (e.g. narrator text with
 *      subtitles-always-on setting).
 *   3. Fires OnSubtitleChanged so UCCRDialogueWidget can reflect the current text
 *      without polling.
 *
 * Subtitle visibility respects UCCRSettingsSubsystem::GetSubtitlesAlwaysOn().
 * When subtitles-always-on is disabled, subtitles are shown only while VO plays.
 */
UCLASS(BlueprintType)
class CCR_API UCCRSubtitleSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Manually show a subtitle for DurationSec seconds.
	 * If DurationSec <= 0, the subtitle remains until ClearSubtitle() is called.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Subtitle")
	void ShowSubtitle(const FText& Text, float DurationSec = 0.f);

	/** Clear the current subtitle immediately. */
	UFUNCTION(BlueprintCallable, Category = "CCR|Subtitle")
	void ClearSubtitle();

	/** Returns the currently displayed subtitle text (empty when none active). */
	UFUNCTION(BlueprintPure, Category = "CCR|Subtitle")
	FText GetCurrentSubtitle() const { return CurrentSubtitle; }

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCCRSubtitleChanged,
	                                             const FText&, SubtitleText,
	                                             bool, bVisible);

	/**
	 * Fired whenever the displayed subtitle changes.
	 * bVisible = false when the subtitle was cleared.
	 * UCCRDialogueWidget subscribes to update its subtitle panel.
	 */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Subtitle")
	FOnCCRSubtitleChanged OnSubtitleChanged;

private:
	FText CurrentSubtitle;
	FTimerHandle ClearTimer;

	/** Automatically show the dialogue text as a subtitle when subtitles-always-on is set. */
	UFUNCTION()
	void HandleNodeChanged(FName NodeId);

	void ScheduleClear(float DurationSec);
};
