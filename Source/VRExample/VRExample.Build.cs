// 2021-2022 Alexander Smirnov

using UnrealBuildTool;

public class VRExample : ModuleRules
{
	public VRExample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		//DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}
