using UnrealBuildTool;
using System.Collections.Generic;

public class CCRTarget : TargetRules
{
	public CCRTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("CCR");
	}
}
