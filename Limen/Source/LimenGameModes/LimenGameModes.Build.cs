using UnrealBuildTool;

public class LimenGameModes : ModuleRules
{
    public LimenGameModes(ReadOnlyTargetRules Target) : base(Target)
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
                "UMG",
                "AdpcmAudioDecoder",
                
                "LimenCore",
                "LimenWidgets",
                "LimenSaveSystem",
                "LimenPlayers", 
                "ProceduralMaps",
                "Limen", 
                "LimenAbilitySystem",
                "LimenInteraction",
                "LimenLevels",
            }
        );
    }
}