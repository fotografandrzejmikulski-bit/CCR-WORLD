// CCRDev.Target.cs
//
// Build target for the developer / personal device build.
// Produces a universal APK in Development configuration.
//
// How to build:
//   RunUAT BuildCookRun \
//     -project="CCR.uproject" \
//     -targetplatform=Android \
//     -target=CCRDev \
//     -configuration=Development \
//     -cook -build -stage -package \
//     -deploy          (automatically installs on a connected device via adb)
//
// The resulting .apk is located in:
//   Binaries/Android/
// Install manually with:
//   adb install -r Binaries/Android/CCR-Android-Development-arm64.apk

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
		// sets bEnableBundle=False, bEnableUniversalAPK=True so adb install works,
		// and enables on-screen debug messages for the developer.
	}
}
