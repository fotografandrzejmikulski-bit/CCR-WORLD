// CCRGooglePlay.Target.cs
//
// Build target for the Google Play Store release.
// Produces an Android App Bundle (AAB) in Shipping configuration.
//
// How to build:
//   RunUAT BuildCookRun \
//     -project="CCR.uproject" \
//     -targetplatform=Android \
//     -target=CCRGooglePlay \
//     -configuration=Shipping \
//     -cook -build -stage -package -archive \
//     -archivedirectory="Builds/GooglePlay"
//
// The resulting .aab file is located in:
//   Builds/GooglePlay/Android_Shipping/
// Upload that file to the Google Play Console via Internal Testing → Production.

using UnrealBuildTool;
using System.Collections.Generic;

public class CCRGooglePlayTarget : TargetRules
{
	public CCRGooglePlayTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("CCR");

		// ---- Shipping optimisations ----
		// Disable runtime logging so no sensitive strings leak in release builds.
		bUseLoggingInShipping = false;

		// Strip debug symbols from the final package to reduce AAB size.
		bUsePDBFiles = false;

		// Enable link-time code generation for the best possible runtime speed.
		bAllowLTCG = true;

		// ---- Android-specific ----
		// The per-config INI file Config/Android/ShippingAndroidGame.ini
		// sets bEnableBundle=True, bEnableGooglePlaySupport=True and bumps
		// the StoreVersion automatically — no extra code needed here.
	}
}
