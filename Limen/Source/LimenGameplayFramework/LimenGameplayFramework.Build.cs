using UnrealBuildTool;

public class LimenGameplayFramework : ModuleRules
{
    public LimenGameplayFramework(ReadOnlyTargetRules Target) : base(Target)
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
                "LimenWidgets",
                "LimenItemSmith",
                "LimenPlayers",
            }
        );
    }
}