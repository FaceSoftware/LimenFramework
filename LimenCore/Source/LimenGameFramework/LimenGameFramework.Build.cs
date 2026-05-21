using UnrealBuildTool;

public class LimenGameFramework : ModuleRules
{
    public LimenGameFramework(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "Engine",
                "LimenCore",
                "AIModule",
                "GameplayTags",
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
                "NetCore",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                
                "LimenNotifications",
                "LimenWidgets",
                "LimenLocomotion", 
                "LimenAbilitySystem",
                "LimenLevelTransitions",
                "LimenUpgrades",
            }
        );
    }
}