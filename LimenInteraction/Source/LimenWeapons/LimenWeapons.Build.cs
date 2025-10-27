using UnrealBuildTool;

public class LimenWeapons : ModuleRules
{
    public LimenWeapons(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                
                "LimenInteraction",
                "AIModule",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "AIModule",
                "Niagara",
                "NetCore",
                
                "LimenCore",
                "LimenSaveSystem",
            }
        );
    }
}