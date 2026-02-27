#include "CCRSettingsWidget.h"
#include "CCRSettingsSubsystem.h"

void UCCRSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OnSettingsOpened();
}

void UCCRSettingsWidget::Close()
{
	OnSettingsClosed();
	OnClosed.Broadcast();
	SetVisibility(ESlateVisibility::Collapsed);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static UCCRSettingsSubsystem* CCR_GetSettings(const UWidget* Widget)
{
	UGameInstance* GI = Widget ? Widget->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<UCCRSettingsSubsystem>() : nullptr;
}

// ---------------------------------------------------------------------------
// Volume getters
// ---------------------------------------------------------------------------

float UCCRSettingsWidget::GetMusicVolume() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetMusicVolume();
	return 1.f;
}

float UCCRSettingsWidget::GetVOVolume() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetVOVolume();
	return 1.f;
}

float UCCRSettingsWidget::GetSFXVolume() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetSFXVolume();
	return 1.f;
}

// ---------------------------------------------------------------------------
// Volume setters
// ---------------------------------------------------------------------------

void UCCRSettingsWidget::SetMusicVolume(float Volume)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetMusicVolume(Volume);
}

void UCCRSettingsWidget::SetVOVolume(float Volume)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetVOVolume(Volume);
}

void UCCRSettingsWidget::SetSFXVolume(float Volume)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetSFXVolume(Volume);
}

// ---------------------------------------------------------------------------
// Subtitles
// ---------------------------------------------------------------------------

bool UCCRSettingsWidget::GetSubtitlesAlwaysOn() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetSubtitlesAlwaysOn();
	return false;
}

void UCCRSettingsWidget::SetSubtitlesAlwaysOn(bool bEnabled)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetSubtitlesAlwaysOn(bEnabled);
}

// ---------------------------------------------------------------------------
// Performance tier
// ---------------------------------------------------------------------------

bool UCCRSettingsWidget::IsPerformanceTierOverridden() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->IsPerformanceTierOverridden();
	return false;
}

ECCRPerformanceTier UCCRSettingsWidget::GetPerformanceTierOverride() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetPerformanceTierOverride();
	return ECCRPerformanceTier::Mid;
}

void UCCRSettingsWidget::SetPerformanceTierOverride(ECCRPerformanceTier Tier)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetPerformanceTierOverride(Tier);
}

void UCCRSettingsWidget::ClearPerformanceTierOverride()
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->ClearPerformanceTierOverride();
}

// ---------------------------------------------------------------------------
// Accessibility
// ---------------------------------------------------------------------------

float UCCRSettingsWidget::GetFontScale() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetFontScale();
	return 1.f;
}

void UCCRSettingsWidget::SetFontScale(float Scale)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetFontScale(Scale);
}

bool UCCRSettingsWidget::GetReducedMotion() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetReducedMotion();
	return false;
}

void UCCRSettingsWidget::SetReducedMotion(bool bEnabled)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetReducedMotion(bEnabled);
}

bool UCCRSettingsWidget::GetHighContrast() const
{
	if (const UCCRSettingsSubsystem* S = CCR_GetSettings(this)) return S->GetHighContrast();
	return false;
}

void UCCRSettingsWidget::SetHighContrast(bool bEnabled)
{
	if (UCCRSettingsSubsystem* S = CCR_GetSettings(this)) S->SetHighContrast(bEnabled);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRSettingsWidget::OnSettingsOpened_Implementation()
{
// Default C++ implementation: no-op.
}

void UCCRSettingsWidget::OnSettingsClosed_Implementation()
{
// Default C++ implementation: no-op.
}
