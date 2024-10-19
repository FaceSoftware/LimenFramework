using UnrealBuildTool;

public class ProceduralMaps : ModuleRules
{
    public ProceduralMaps(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                
                "LimenLevelTransitions", 
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
                "NavigationSystem",
                "AdpcmAudioDecoder",
                
                "LimenCore",
                "LimenProceduralGeneration",
            }
        );
    }
}