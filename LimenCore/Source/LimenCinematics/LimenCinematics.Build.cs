using UnrealBuildTool;

public class LimenCinematics : ModuleRules
{
    public LimenCinematics(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "MovieScene",
                
                "LimenCore", 
                "LimenSaveSystem",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Engine",
                "Slate",
                "SlateCore", 
                "LimenWidgets", 
                "LevelSequence",
                "MovieScene"
            }
        );
    }
}