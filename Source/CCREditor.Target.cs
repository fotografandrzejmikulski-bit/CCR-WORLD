using UnrealBuildTool;
using System.Collections.Generic;

public class CCREditorTarget : TargetRules
{
	public CCREditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("CCR");
	}
}
