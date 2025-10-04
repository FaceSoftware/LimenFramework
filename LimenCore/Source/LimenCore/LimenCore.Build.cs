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
				"Core",
				"RenderCore",
				"Renderer",
				"Engine",
				"AIModule",
				"EnhancedInput",
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
				"InputCore",
			}
		);
		
		var ShaderDirectory = Path.Combine(PluginDirectory, "Shaders");
		RuntimeDependencies.Add(Path.Combine(ShaderDirectory, "*.*"));
		AdditionalPropertiesForReceipt.Add("ShaderDirectory", ShaderDirectory);
	}
}