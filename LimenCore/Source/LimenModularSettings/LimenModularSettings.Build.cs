using UnrealBuildTool;

public class LimenModularSettings : ModuleRules
{
    public LimenModularSettings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "DeveloperSettings",
                "UMG",
                
                "LimenCore",
                "LimenWidgets",
                "LimenSaveSystem", 
                "LimenStorage",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );
    }
}