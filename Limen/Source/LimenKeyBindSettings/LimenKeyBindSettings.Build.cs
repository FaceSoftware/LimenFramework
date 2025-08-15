using UnrealBuildTool;

public class LimenKeyBindSettings : ModuleRules
{
    public LimenKeyBindSettings(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "Engine",
                "DeveloperSettings",
                "EnhancedInput",
                "InputCore",
                
                "LimenModularSettings", 
                "LimenModals",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Slate",
                "SlateCore",
                "UMG",
                
                "LimenCore",
                "LimenStorage",
                "LimenWidgets",
                "LimenModals",
                "LimenGameFramework",
            }
        );
    }
}