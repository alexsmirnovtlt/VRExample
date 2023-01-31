// 2021-2022 Alexander Smirnov

using UnrealBuildTool;
using System.Collections.Generic;

public class VRExampleEditorTarget : TargetRules
{
	public VRExampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "VRExample" } );
	}
}
