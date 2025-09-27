using UnrealBuildTool;

public class LimenGameSaves : ModuleRules
{
    public LimenGameSaves(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "LimenSaveSystem",
                "DeveloperSettings",
                "LimenModals",
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
                "LimenInteraction", 
                "LimenObjectives", 
                "LimenPlayers",
                "LimenTools",
                "LimenWeapons", 
                "LimenHints", 
                "LimenLevels", 
                "LimenModals",
            }
        );
    }
}
