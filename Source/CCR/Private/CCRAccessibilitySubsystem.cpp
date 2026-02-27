#include "CCRAccessibilitySubsystem.h"
#include "CCRSettingsSubsystem.h"

void UCCRAccessibilitySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Subscribe to settings changes so we can relay them as accessibility changes.
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		Settings->OnSettingsChanged.AddDynamic(this, &UCCRAccessibilitySubsystem::OnSettingsChanged);
	}
}

void UCCRAccessibilitySubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
		{
			Settings->OnSettingsChanged.RemoveDynamic(this, &UCCRAccessibilitySubsystem::OnSettingsChanged);
		}
	}
	Super::Deinitialize();
}

float UCCRAccessibilitySubsystem::GetFontScale() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
		{
			return Settings->GetFontScale();
		}
	}
	return 1.f;
}

void UCCRAccessibilitySubsystem::SetFontScale(float Scale)
{
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		// Clamping is applied by UCCRSettingsSubsystem::SetFontScale ([0.5, 2.0]).
		Settings->SetFontScale(Scale);
		// OnAccessibilityChanged is broadcast via OnSettingsChanged relay.
	}
}

bool UCCRAccessibilitySubsystem::IsReducedMotion() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
		{
			return Settings->GetReducedMotion();
		}
	}
	return false;
}

void UCCRAccessibilitySubsystem::SetReducedMotion(bool bEnabled)
{
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		Settings->SetReducedMotion(bEnabled);
	}
}

bool UCCRAccessibilitySubsystem::IsHighContrast() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
		{
			return Settings->GetHighContrast();
		}
	}
	return false;
}

void UCCRAccessibilitySubsystem::SetHighContrast(bool bEnabled)
{
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		Settings->SetHighContrast(bEnabled);
	}
}

void UCCRAccessibilitySubsystem::OnSettingsChanged()
{
	// Relay any settings change as an accessibility change notification.
	// Widgets that depend on font scale, reduced motion, or high contrast
	// subscribe here rather than watching the broader OnSettingsChanged.
	OnAccessibilityChanged.Broadcast();
}
