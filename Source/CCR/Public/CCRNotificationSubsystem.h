#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CCRNotificationSubsystem.generated.h"

/**
 * Visual category / urgency level for a CCR notification.
 * Blueprint WBP_CCRNotification uses this to choose the correct
 * colour, icon, and animation style.
 */
UENUM(BlueprintType)
enum class ECCRNotificationType : uint8
{
	Info        UMETA(DisplayName = "Info"),
	Warning     UMETA(DisplayName = "Warning"),
	Checkpoint  UMETA(DisplayName = "Checkpoint"),
	Chapter     UMETA(DisplayName = "Chapter"),
	Achievement UMETA(DisplayName = "Achievement"),
};

/**
 * UCCRNotificationSubsystem
 *
 * Centralised broadcast point for in-game toast / banner notifications.
 * Other subsystems (UCCRCheckpointSubsystem, UCCRNarrativeRuntimeSubsystem)
 * call ShowNotification() to queue a message.  UCCRNotificationWidget
 * subscribes to OnNotificationRequested to drive the UMG animation.
 *
 * No queuing or ordering is done here; the widget is responsible for
 * stacking or replacing notifications as appropriate for the game's UX.
 */
UCLASS(BlueprintType)
class CCR_API UCCRNotificationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Broadcast a notification to any listening UCCRNotificationWidget.
	 * @param Message       Localised text to display.
	 * @param DurationSec   How long the notification should be visible (0 = widget decides).
	 * @param Type          Visual category used to pick colours / icons.
	 */
	UFUNCTION(BlueprintCallable, Category = "CCR|Notification")
	void ShowNotification(const FText& Message, float DurationSec = 3.f,
	                      ECCRNotificationType Type = ECCRNotificationType::Info);

	// ---- Delegate ----

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCCRNotificationRequested,
	                                               const FText&, Message,
	                                               float, DurationSec,
	                                               ECCRNotificationType, Type);

	/** Fired whenever ShowNotification() is called. */
	UPROPERTY(BlueprintAssignable, Category = "CCR|Notification")
	FOnCCRNotificationRequested OnNotificationRequested;
};
