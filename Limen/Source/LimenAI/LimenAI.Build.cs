using UnrealBuildTool;

public class LimenAI : ModuleRules
{
    public LimenAI(ReadOnlyTargetRules Target) : base(Target)
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
                "AIModule",
                "NavigationSystem",
                
                "LimenCore",
                "LimenPlayers",
                "LimenDoors",
                "LimenInteraction",
                "Limen", 
            }
        );
    }
}