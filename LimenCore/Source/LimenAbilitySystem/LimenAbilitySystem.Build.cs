using UnrealBuildTool;

public class LimenAbilitySystem : ModuleRules
{
    public LimenAbilitySystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "AIModule",
                
                "LimenStorage",
                "LimenSaveSystem", 
                "LimenCore",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "NetCore",

                "LimenLocomotion", 
                "LimenCore",
            }
        );
    }
}