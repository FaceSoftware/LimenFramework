using UnrealBuildTool;

public class LimenStorage : ModuleRules
{
    public LimenStorage(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
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
                
                "LimenCore",
                "LimenSaveSystem",
            }
        );
    }
}