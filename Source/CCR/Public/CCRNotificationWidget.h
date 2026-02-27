#pragma once

#include "CoreMinimal.h"
#include "CCRNotificationSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "CCRNotificationWidget.generated.h"

/**
 * UCCRNotificationWidget
 *
 * C++ base class for the UMG in-game notification / toast overlay.
 * Blueprint WBP_CCRNotification should derive from this class.
 *
 * NativeConstruct subscribes to UCCRNotificationSubsystem::OnNotificationRequested.
 * Each incoming notification fires OnNotificationReceived() which Blueprint
 * implements to play a slide-in animation, display the message with the
 * appropriate icon / colour, then auto-dismiss after DurationSec seconds.
 *
 * The widget is created by ACCRGameHUD and lives at CCRZOrder::Notification.
 * It is always present in the viewport; visibility is driven entirely by
 * Blueprint animations triggered from OnNotificationReceived().
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class CCR_API UCCRNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Called whenever UCCRNotificationSubsystem::ShowNotification() fires.
	 * Override in Blueprint to play a slide-in animation, set text / colour,
	 * and schedule a dismiss after DurationSec.
	 * @param Message      Localised notification text.
	 * @param DurationSec  Suggested on-screen duration in seconds (0 = decide in BP).
	 * @param Type         Visual category (Info, Warning, Checkpoint, Chapter).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "CCR|Notification")
	void OnNotificationReceived(const FText& Message, float DurationSec, ECCRNotificationType Type);

private:
	UFUNCTION()
	void HandleNotificationRequested(const FText& Message, float DurationSec, ECCRNotificationType Type);
};
