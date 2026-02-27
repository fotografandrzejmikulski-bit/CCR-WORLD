#include "CCRSettingsSubsystem.h"
#include "CCR.h"
#include "CCRSettingsSaveGame.h"
#include "Kismet/GameplayStatics.h"

namespace CCRSettingsPrivate
{
	static constexpr int32 UserIndex = 0;
}

// ---------------------------------------------------------------------------
// Initialize
// ---------------------------------------------------------------------------

void UCCRSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadOrCreate();
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void UCCRSettingsSubsystem::LoadOrCreate()
{
	if (UGameplayStatics::DoesSaveGameExist(CCRSaveSlots::Settings, CCRSettingsPrivate::UserIndex))
	{
		ActiveSettings = Cast<UCCRSettingsSaveGame>(
			UGameplayStatics::LoadGameFromSlot(CCRSaveSlots::Settings, CCRSettingsPrivate::UserIndex));
	}

	if (!ActiveSettings)
	{
		ActiveSettings = Cast<UCCRSettingsSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UCCRSettingsSaveGame::StaticClass()));
	}
}

void UCCRSettingsSubsystem::SaveNow()
{
	if (!ActiveSettings) return;
	UGameplayStatics::SaveGameToSlot(ActiveSettings, CCRSaveSlots::Settings, CCRSettingsPrivate::UserIndex);
	OnSettingsChanged.Broadcast();
}

// ---------------------------------------------------------------------------
// Getters
// ---------------------------------------------------------------------------

float UCCRSettingsSubsystem::GetMusicVolume() const
{
	return ActiveSettings ? ActiveSettings->MusicVolume : 1.f;
}

float UCCRSettingsSubsystem::GetVOVolume() const
{
	return ActiveSettings ? ActiveSettings->VOVolume : 1.f;
}

float UCCRSettingsSubsystem::GetSFXVolume() const
{
	return ActiveSettings ? ActiveSettings->SFXVolume : 1.f;
}

bool UCCRSettingsSubsystem::IsPerformanceTierOverridden() const
{
	return ActiveSettings ? ActiveSettings->bOverridePerformanceTier : false;
}

ECCRPerformanceTier UCCRSettingsSubsystem::GetPerformanceTierOverride() const
{
	return ActiveSettings ? ActiveSettings->PerformanceTierOverride : ECCRPerformanceTier::Mid;
}

bool UCCRSettingsSubsystem::GetSubtitlesAlwaysOn() const
{
	return ActiveSettings ? ActiveSettings->bSubtitlesAlwaysOn : false;
}

// ---------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------

void UCCRSettingsSubsystem::SetMusicVolume(float Volume)
{
	if (!ActiveSettings) return;
	ActiveSettings->MusicVolume = FMath::Clamp(Volume, 0.f, 1.f);
	SaveNow();
}

void UCCRSettingsSubsystem::SetVOVolume(float Volume)
{
	if (!ActiveSettings) return;
	ActiveSettings->VOVolume = FMath::Clamp(Volume, 0.f, 1.f);
	SaveNow();
}

void UCCRSettingsSubsystem::SetSFXVolume(float Volume)
{
	if (!ActiveSettings) return;
	ActiveSettings->SFXVolume = FMath::Clamp(Volume, 0.f, 1.f);
	SaveNow();
}

void UCCRSettingsSubsystem::SetPerformanceTierOverride(ECCRPerformanceTier Tier)
{
	if (!ActiveSettings) return;
	ActiveSettings->bOverridePerformanceTier = true;
	ActiveSettings->PerformanceTierOverride  = Tier;
	SaveNow();
}

void UCCRSettingsSubsystem::ClearPerformanceTierOverride()
{
	if (!ActiveSettings) return;
	ActiveSettings->bOverridePerformanceTier = false;
	SaveNow();
}

void UCCRSettingsSubsystem::SetSubtitlesAlwaysOn(bool bEnabled)
{
	if (!ActiveSettings) return;
	ActiveSettings->bSubtitlesAlwaysOn = bEnabled;
	SaveNow();
}
