using UnrealBuildTool;

public class LimenHints : ModuleRules
{
    public LimenHints(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "LimenCore",
                "LimenWidgets",
                "LimenTriggers",
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
                "DeveloperSettings"
            }
        );
    }
}