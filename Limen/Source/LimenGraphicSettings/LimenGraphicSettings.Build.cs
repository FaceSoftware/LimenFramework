using UnrealBuildTool;

public class LimenGraphicSettings : ModuleRules
{
    public LimenGraphicSettings(ReadOnlyTargetRules Target) : base(Target)
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
                "RHI",
                
                "LimenCore",
                "LimenSaveSystem",
                "LimenStorage", 
                "LimenModals",
                "LimenWidgets",
                "ApplicationCore",
            }
        );
    }
}