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

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRNotificationWidget::OnNotificationReceived_Implementation(const FText& Message, float DurationSec, ECCRNotificationType Type)
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRNotificationWidget::OnNotificationReceived has no C++ or Blueprint implementation."))
#endif
}
