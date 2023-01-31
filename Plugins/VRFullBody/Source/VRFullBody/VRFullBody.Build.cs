// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VRFullBody : ModuleRules
{
	public VRFullBody(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"CoreUObject",

				"VRBase",
				"VRBaseMultiplayer",
				"HeadMountedDisplay"
			}
		);
	}
}
