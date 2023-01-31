// 2021-2022 Alexander Smirnov

using UnrealBuildTool;
using System.Collections.Generic;

public class VRExampleTarget : TargetRules
{
	public VRExampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "VRExample" } );
	}
}
