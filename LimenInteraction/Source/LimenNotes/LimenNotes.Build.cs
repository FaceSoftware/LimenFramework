using UnrealBuildTool;

public class LimenNotes : ModuleRules
{
    public LimenNotes(ReadOnlyTargetRules Target) : base(Target)
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
                "LimenCore",
                "LimenInteraction",
                
                "LimenSaveSystem",
            }
        );
    }
}