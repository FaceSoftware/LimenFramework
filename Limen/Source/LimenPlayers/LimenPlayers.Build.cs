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
                "LimenItemSmith",
                "Limen", 
                "LimenLevelTransitions",
                "LimenStorage",
                "LimenArchives", 
                "LimenGameSaves", 
                "LimenModals",
                "LimenProxyInventory",
            }
        );
    }
}