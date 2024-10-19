using UnrealBuildTool;

public class LimenArchives : ModuleRules
{
    public LimenArchives(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                
                "LimenCore", 
                "LimenInteraction",
                "LimenStorage",
                "LimenWidgets",
                "LimenSaveSystem",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG"
            }
        );
    }
}