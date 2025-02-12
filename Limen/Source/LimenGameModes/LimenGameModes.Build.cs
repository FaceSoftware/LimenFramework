using UnrealBuildTool;

public class LimenGameModes : ModuleRules
{
    public LimenGameModes(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "LimenGameFramework",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                
                "LimenCore",
                "LimenWidgets",
                "LimenSaveSystem",
                "LimenPlayers", 
                "ProceduralMaps",
                "Limen", 
                "LimenAbilitySystem",
                "LimenInteraction",
                "LimenGameSaves",
                "LimenLevels",
                "LimenModals",
            }
        );
    }
}