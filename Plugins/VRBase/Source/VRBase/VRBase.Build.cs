// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VRBase : ModuleRules
{
	public VRBase(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"NetCore",
				"CoreUObject"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"HeadMountedDisplay",
				"NavigationSystem",
				"EnhancedInput",
				"InputCore",
				"SlateCore",
				"Niagara",
				"Slate",
				"UMG"
			}
		);
	}
}
