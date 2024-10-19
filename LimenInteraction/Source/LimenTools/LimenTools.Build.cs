// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LimenTools : ModuleRules
{
	public LimenTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"LimenInteraction",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				
				"LimenCore",
				"LimenAbilitySystem",
				"LimenStorage",
				"LimenSaveSystem",
			}
		);
	}
}
