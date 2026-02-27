#include "CCRPerformanceGovernorSubsystem.h"
#include "CCRSettingsSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "RHI.h"

// ---------------------------------------------------------------------------
// Internal helper
// ---------------------------------------------------------------------------

static void CCR_SetQualityCVar(const TCHAR* Name, int32 Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(Value, ECVF_SetByGameSetting);
	}
}

static void CCR_SetQualityCVarFloat(const TCHAR* Name, float Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(Value, ECVF_SetByGameSetting);
	}
}

// ---------------------------------------------------------------------------
// Initialize / Deinitialize
// ---------------------------------------------------------------------------

void UCCRPerformanceGovernorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Check if the player has manually overridden the performance tier in settings.
	if (UCCRSettingsSubsystem* Settings = GetGameInstance()->GetSubsystem<UCCRSettingsSubsystem>())
	{
		if (Settings->IsPerformanceTierOverridden())
		{
			Tier = Settings->GetPerformanceTierOverride();
		}
		else
		{
			DetectTier();
		}

		// Listen for future setting changes so the tier is re-applied if the
		// player changes the override from the pause menu / settings screen.
		Settings->OnSettingsChanged.AddDynamic(this, &UCCRPerformanceGovernorSubsystem::OnSettingsChanged);
	}
	else
	{
		DetectTier();
	}

	ApplyQualitySettings();
}

void UCCRPerformanceGovernorSubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
		{
			Settings->OnSettingsChanged.RemoveDynamic(this, &UCCRPerformanceGovernorSubsystem::OnSettingsChanged);
		}
	}
	Super::Deinitialize();
}

void UCCRPerformanceGovernorSubsystem::DetectTier()
{
	const FString GPUName = GRHIAdapterName.ToUpper();

	// High-end: Snapdragon 8 Gen series, Adreno 730+, Apple A15+
	if (GPUName.Contains(TEXT("ADRENO 7"))  ||
		GPUName.Contains(TEXT("ADRENO 8"))  ||
		GPUName.Contains(TEXT("APPLE A15")) ||
		GPUName.Contains(TEXT("APPLE A16")) ||
		GPUName.Contains(TEXT("APPLE A17")) ||
		GPUName.Contains(TEXT("MALI-G710")) ||
		GPUName.Contains(TEXT("MALI-G720")))
	{
		Tier = ECCRPerformanceTier::High;
		return;
	}

	// Low-end: older Mali, Adreno 5xx and below
	if (GPUName.Contains(TEXT("ADRENO 5"))  ||
		GPUName.Contains(TEXT("ADRENO 4"))  ||
		GPUName.Contains(TEXT("MALI-G5"))   ||
		GPUName.Contains(TEXT("MALI-G57"))  ||
		GPUName.Contains(TEXT("MALI-T")))
	{
		Tier = ECCRPerformanceTier::Low;
		return;
	}

	// Default: mid tier
	Tier = ECCRPerformanceTier::Mid;
}

int32 UCCRPerformanceGovernorSubsystem::GetMaxPredictedChunks() const
{
	switch (Tier)
	{
	case ECCRPerformanceTier::High: return 3;
	case ECCRPerformanceTier::Low:  return 1;
	default:                        return 2;
	}
}

// ---------------------------------------------------------------------------
// ApplyQualitySettings
// ---------------------------------------------------------------------------

void UCCRPerformanceGovernorSubsystem::ApplyQualitySettings()
{
	// sg.* scalability groups:
	//   0 = Low, 1 = Medium, 2 = High, 3 = Epic
	// r.MobileContentScaleFactor: resolution scale relative to native (1.0 = native).

	switch (Tier)
	{
	case ECCRPerformanceTier::High:
		CCR_SetQualityCVar(TEXT("sg.ShadowQuality"),      3);
		CCR_SetQualityCVar(TEXT("sg.PostProcessQuality"), 3);
		CCR_SetQualityCVar(TEXT("sg.TextureQuality"),     3);
		CCR_SetQualityCVar(TEXT("sg.EffectsQuality"),     3);
		CCR_SetQualityCVar(TEXT("sg.ViewDistanceQuality"),3);
		CCR_SetQualityCVarFloat(TEXT("r.MobileContentScaleFactor"), 1.0f);
		break;

	case ECCRPerformanceTier::Low:
		CCR_SetQualityCVar(TEXT("sg.ShadowQuality"),      0);
		CCR_SetQualityCVar(TEXT("sg.PostProcessQuality"), 0);
		CCR_SetQualityCVar(TEXT("sg.TextureQuality"),     1);
		CCR_SetQualityCVar(TEXT("sg.EffectsQuality"),     1);
		CCR_SetQualityCVar(TEXT("sg.ViewDistanceQuality"),1);
		CCR_SetQualityCVarFloat(TEXT("r.MobileContentScaleFactor"), 0.70f);
		break;

	default: // Mid
		CCR_SetQualityCVar(TEXT("sg.ShadowQuality"),      1);
		CCR_SetQualityCVar(TEXT("sg.PostProcessQuality"), 1);
		CCR_SetQualityCVar(TEXT("sg.TextureQuality"),     2);
		CCR_SetQualityCVar(TEXT("sg.EffectsQuality"),     2);
		CCR_SetQualityCVar(TEXT("sg.ViewDistanceQuality"),2);
		CCR_SetQualityCVarFloat(TEXT("r.MobileContentScaleFactor"), 0.85f);
		break;
	}
}

// ---------------------------------------------------------------------------
// OnSettingsChanged — re-evaluate tier and re-apply when player edits settings
// ---------------------------------------------------------------------------

void UCCRPerformanceGovernorSubsystem::OnSettingsChanged()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	if (UCCRSettingsSubsystem* Settings = GI->GetSubsystem<UCCRSettingsSubsystem>())
	{
		if (Settings->IsPerformanceTierOverridden())
		{
			Tier = Settings->GetPerformanceTierOverride();
		}
		else
		{
			DetectTier();
		}
	}
	ApplyQualitySettings();
}
