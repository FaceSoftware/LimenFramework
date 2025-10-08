// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LimenSaveSystem : ModuleRules
{
	public LimenSaveSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"DeveloperSettings",
				"UMG",
				
				"LimenWidgets",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				
				"LimenWidgets",
			}
		);
	}
}
