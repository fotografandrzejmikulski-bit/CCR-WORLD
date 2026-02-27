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

int32 UCCRPerformanceGovernorSubsystem::GetTargetFPS() const
{
	switch (Tier)
	{
	case ECCRPerformanceTier::Low: return 30;
	default:                       return 60;
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
	//
	// UE5-specific notes:
	//   r.AntiAliasingMethod: 0=None, 1=FXAA, 2=TAA, 4=TSR
	//     TSR (value 4) requires UE 5.1+. On earlier UE5.0 builds the engine
	//     silently falls back to TAA, so this value is safe to set unconditionally.
	//   r.Lumen.DiffuseIndirect.Allow / r.Lumen.Reflections.Allow:
	//     Enabled only on the High tier. Lumen requires either software ray-tracing
	//     support (available on Adreno 730+ / Apple A15+) or a capable compute
	//     shader path. On platforms without hardware ray-tracing it uses the
	//     screen-space fallback, which still benefits from the quality setting
	//     while remaining backward-compatible on Mid/Low hardware.
	//   r.Nanite.Allow:
	//     Enabled only on the High tier. Assets must be authored with Nanite
	//     support in UE5 to benefit. Enabling this CVar on assets without Nanite
	//     geometry is safe (the CVar has no effect on non-Nanite meshes) but
	//     is still left off on Mid/Low tiers to avoid any driver-level overhead.
	//   t.MaxFPS: target frame rate ceiling (0 = unlimited).

	switch (Tier)
	{
	case ECCRPerformanceTier::High:
		// Scalability groups — Epic quality
		CCR_SetQualityCVar(TEXT("sg.ShadowQuality"),               3);
		CCR_SetQualityCVar(TEXT("sg.PostProcessQuality"),          3);
		CCR_SetQualityCVar(TEXT("sg.TextureQuality"),              3);
		CCR_SetQualityCVar(TEXT("sg.EffectsQuality"),              3);
		CCR_SetQualityCVar(TEXT("sg.ViewDistanceQuality"),         3);
		CCR_SetQualityCVar(TEXT("sg.AntiAliasingQuality"),         3);
		CCR_SetQualityCVar(TEXT("sg.FoliageQuality"),              3);
		CCR_SetQualityCVar(TEXT("sg.ReflectionQuality"),           3);
		CCR_SetQualityCVar(TEXT("sg.GlobalIlluminationQuality"),   3);
		// Resolution: native
		CCR_SetQualityCVarFloat(TEXT("r.MobileContentScaleFactor"), 1.0f);
		// UE5 Temporal Super Resolution (best upscaling quality)
		CCR_SetQualityCVar(TEXT("r.AntiAliasingMethod"), 4);
		// Lumen GI + Reflections enabled
		CCR_SetQualityCVar(TEXT("r.Lumen.DiffuseIndirect.Allow"), 1);
		CCR_SetQualityCVar(TEXT("r.Lumen.Reflections.Allow"),     1);
		// Nanite enabled
		CCR_SetQualityCVar(TEXT("r.Nanite.Allow"), 1);
		// Target 60 FPS on high-end devices
		CCR_SetQualityCVar(TEXT("t.MaxFPS"), 60);
		break;

	case ECCRPerformanceTier::Low:
		// Scalability groups — Low quality
		CCR_SetQualityCVar(TEXT("sg.ShadowQuality"),               0);
		CCR_SetQualityCVar(TEXT("sg.PostProcessQuality"),          0);
		CCR_SetQualityCVar(TEXT("sg.TextureQuality"),              1);
		CCR_SetQualityCVar(TEXT("sg.EffectsQuality"),              1);
		CCR_SetQualityCVar(TEXT("sg.ViewDistanceQuality"),         1);
		CCR_SetQualityCVar(TEXT("sg.AntiAliasingQuality"),         1);
		CCR_SetQualityCVar(TEXT("sg.FoliageQuality"),              0);
		CCR_SetQualityCVar(TEXT("sg.ReflectionQuality"),           0);
		CCR_SetQualityCVar(TEXT("sg.GlobalIlluminationQuality"),   0);
		// Resolution: 70% of native to maintain performance headroom
		CCR_SetQualityCVarFloat(TEXT("r.MobileContentScaleFactor"), 0.70f);
		// FXAA only — cheapest anti-aliasing
		CCR_SetQualityCVar(TEXT("r.AntiAliasingMethod"), 1);
		// Lumen off — use screen-space reflections fallback
		CCR_SetQualityCVar(TEXT("r.Lumen.DiffuseIndirect.Allow"), 0);
		CCR_SetQualityCVar(TEXT("r.Lumen.Reflections.Allow"),     0);
		// Nanite disabled on low-end to reduce GPU pressure
		CCR_SetQualityCVar(TEXT("r.Nanite.Allow"), 0);
		// Cap at 30 FPS to maintain stable frame pacing on low-end
		CCR_SetQualityCVar(TEXT("t.MaxFPS"), 30);
		break;

	default: // Mid
		// Scalability groups — Medium/High blend
		CCR_SetQualityCVar(TEXT("sg.ShadowQuality"),               1);
		CCR_SetQualityCVar(TEXT("sg.PostProcessQuality"),          1);
		CCR_SetQualityCVar(TEXT("sg.TextureQuality"),              2);
		CCR_SetQualityCVar(TEXT("sg.EffectsQuality"),              2);
		CCR_SetQualityCVar(TEXT("sg.ViewDistanceQuality"),         2);
		CCR_SetQualityCVar(TEXT("sg.AntiAliasingQuality"),         2);
		CCR_SetQualityCVar(TEXT("sg.FoliageQuality"),              1);
		CCR_SetQualityCVar(TEXT("sg.ReflectionQuality"),           1);
		CCR_SetQualityCVar(TEXT("sg.GlobalIlluminationQuality"),   1);
		// Resolution: 85% of native — good balance
		CCR_SetQualityCVarFloat(TEXT("r.MobileContentScaleFactor"), 0.85f);
		// TAA — good quality at moderate cost
		CCR_SetQualityCVar(TEXT("r.AntiAliasingMethod"), 2);
		// Lumen off on mid-tier mobile to preserve frame rate
		CCR_SetQualityCVar(TEXT("r.Lumen.DiffuseIndirect.Allow"), 0);
		CCR_SetQualityCVar(TEXT("r.Lumen.Reflections.Allow"),     0);
		// Nanite off on mid-tier
		CCR_SetQualityCVar(TEXT("r.Nanite.Allow"), 0);
		// Target 60 FPS on mid-range devices
		CCR_SetQualityCVar(TEXT("t.MaxFPS"), 60);
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
