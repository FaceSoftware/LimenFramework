using UnrealBuildTool;

public class LimenLevelCollisionProxy : ModuleRules
{
    public LimenLevelCollisionProxy(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "LevelEditor",
                "ContentBrowser",
                "Slate",
                "SlateCore",
                "EditorFramework",
                "EditorScriptingUtilities",
                "AssetTools",
                "Blutility",
                
                "LimenCore",
                "LimenEditor"
            }
        );
    }
}