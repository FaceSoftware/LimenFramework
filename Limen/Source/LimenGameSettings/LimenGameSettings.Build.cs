using UnrealBuildTool;

public class LimenGameSettings : ModuleRules
{
    public LimenGameSettings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "DeveloperSettings",
                
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
                
                "LimenCore",
                "LimenPlayers",
                "LimenNotifications",
                "LimenSaveSystem",
                "LimenStorage", 
                "LimenLocomotion", 
                "LimenHints",
            }
        );
    }
}