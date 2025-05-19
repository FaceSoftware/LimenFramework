using UnrealBuildTool;

public class LimenDoors : ModuleRules
{
    public LimenDoors(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                
                "LimenCore",
                "LimenInteraction",
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
                "LimenNotifications",
                "LimenSaveSystem",
            }
        );
    }
}