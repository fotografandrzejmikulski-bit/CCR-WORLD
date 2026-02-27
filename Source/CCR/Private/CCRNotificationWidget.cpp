#include "CCRNotificationWidget.h"
#include "CCRNotificationSubsystem.h"

void UCCRNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	if (UCCRNotificationSubsystem* NS = GI->GetSubsystem<UCCRNotificationSubsystem>())
	{
		NS->OnNotificationRequested.AddDynamic(
			this, &UCCRNotificationWidget::HandleNotificationRequested);
	}
}

void UCCRNotificationWidget::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRNotificationSubsystem* NS = GI->GetSubsystem<UCCRNotificationSubsystem>())
		{
			NS->OnNotificationRequested.RemoveDynamic(
				this, &UCCRNotificationWidget::HandleNotificationRequested);
		}
	}
	Super::NativeDestruct();
}

void UCCRNotificationWidget::HandleNotificationRequested(
	const FText& Message,
	float DurationSec,
	ECCRNotificationType Type)
{
	OnNotificationReceived(Message, DurationSec, Type);
}
