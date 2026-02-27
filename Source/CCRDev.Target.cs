// CCRDev.Target.cs
//
// Build target for the developer / personal device build.
// Produces a SELF-CONTAINED universal APK (all game data embedded inside).
// No adb or USB debugging is required — just copy the APK to your device.
//
// ┌──────────────────────────────────────────────────────────────────┐
// │  HOW TO BUILD                                                    │
// │                                                                  │
// │  RunUAT BuildCookRun \                                           │
// │    -project="CCR.uproject" \                                     │
// │    -targetplatform=Android \                                     │
// │    -target=CCRDev \                                              │
// │    -configuration=Development \                                  │
// │    -cook -build -stage -package                                  │
// │                                                                  │
// │  Output APK:                                                     │
// │    Binaries/Android/CCR-Android-Development-arm64.apk            │
// │                                                                  │
// │  HOW TO INSTALL (no adb needed)                                  │
// │                                                                  │
// │  Option A — GitHub Actions artifact:                             │
// │    Open the workflow run on GitHub → Artifacts → download        │
// │    CCR-dev-apk.zip → extract the .apk → tap to install          │
// │                                                                  │
// │  Option B — Google Drive:                                        │
// │    Upload the .apk to Google Drive, open on device, tap Install  │
// │                                                                  │
// │  Option C — USB file transfer:                                   │
// │    Copy .apk via USB to device storage, open with file manager   │
// │                                                                  │
// │  Note: on first install enable "Install unknown apps" in         │
// │  Settings → Apps → (your file manager or browser app)           │
// └──────────────────────────────────────────────────────────────────┘

using UnrealBuildTool;
using System.Collections.Generic;

public class CCRDevTarget : TargetRules
{
	public CCRDevTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("CCR");

		// ---- Development conveniences ----
		// Keep logging fully enabled so in-game issues are visible in adb logcat.
		bUseLoggingInShipping = true;   // irrelevant for Development, but explicit

		// Generate PDB files so stack traces are symbol-resolved on device.
		bUsePDBFiles = true;

		// Disable LTCG for faster iteration builds.
		bAllowLTCG = false;

		// ---- Android-specific ----
		// The per-config INI file Config/Android/DevelopmentAndroidGame.ini
		// sets bEnableBundle=False, bEnableUniversalAPK=True and
		// bPackageDataInsideApk=True so the APK is fully self-contained
		// (no separate OBB push via adb needed).
		// On-screen debug messages are also enabled for the developer.
	}
}
