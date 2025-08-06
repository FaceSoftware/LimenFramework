using System.IO;
using UnrealBuildTool;

public class LimenCore : ModuleRules
{
	public LimenCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "RenderCore", "Engine", 
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"EngineSettings",
				"Slate",
				"SlateCore",
				"NavigationSystem",
				"UMG",
				"RHICore",
				"RHI",
			}
		);
		
		var ShaderDirectory = Path.Combine(PluginDirectory, "Shaders");
		RuntimeDependencies.Add(Path.Combine(ShaderDirectory, "*.*"));
		AdditionalPropertiesForReceipt.Add("ShaderDirectory", ShaderDirectory);
	}
}