using UnrealBuildTool;

public class LimenPlayers : ModuleRules
{
    public LimenPlayers(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                
                "LimenSaveSystem",
                "LimenInteraction",
                "LimenObjectives",
                "LimenNotifications", 
                "LimenWeapons", 
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
                "EnhancedInput",
                "UMG",
                "AIModule",
                
                "LimenCore",
                "LimenLocomotion",
                "LimenAbilitySystem",
                "LimenWidgets",
                "LimenTools",
                "LimenDoors",
                "LimenUpgrades",
                "Limen", 
                "LimenLevelTransitions",
                "LimenStorage",
                "LimenArchives",
                "LimenModals",
                "LimenHints", 
                "LimenKeyBindSettings",
            }
        );
    }
}