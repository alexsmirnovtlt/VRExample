// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VRBaseMultiplayer : ModuleRules
{
	public VRBaseMultiplayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		if (Target.bBuildEditor == true) PrivateDependencyModuleNames.Add("UnrealEd");
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"NetCore",
				"InputCore",
				"SlateCore",
				"Slate",
				"DeveloperSettings",
				"VRBase"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"HeadMountedDisplay",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"UMG"
			}
		);
	}
}
