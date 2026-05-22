using UnrealBuildTool;

public class LimenAudioSettings : ModuleRules
{
    public LimenAudioSettings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "LimenModularSettings",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "DeveloperSettings",
                
                "FMODStudio",
                
                "LimenCore",
                "LimenSaveSystem",
                "LimenStorage",
            }
        );
    }
}