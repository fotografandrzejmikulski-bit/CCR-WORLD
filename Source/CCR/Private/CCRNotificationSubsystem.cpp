#include "CCRNotificationSubsystem.h"

void UCCRNotificationSubsystem::ShowNotification(
	const FText& Message,
	float DurationSec,
	ECCRNotificationType Type)
{
	OnNotificationRequested.Broadcast(Message, DurationSec, Type);
}
