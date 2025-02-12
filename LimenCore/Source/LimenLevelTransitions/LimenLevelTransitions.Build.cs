using UnrealBuildTool;

public class LimenLevelTransitions : ModuleRules
{
    public LimenLevelTransitions(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "DeveloperSettings",
                
                "LimenCore", 
                "LimenWidgets",
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
                "RenderCore",
                "EnhancedInput",
                
                "LimenCore",
                "LimenWidgets",
            }
        );
    }
}