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
                
                // Nvidia
                "NISCore",
                "NISShaders",
                "NISBlueprint",
                "DLSS",
                "DLSSUtility",
                "DLSSBlueprint",
                "Streamline",
                "StreamlineCore",
                "StreamlineShaders",
                "StreamlineD3D12RHI",
                "StreamlineRHI",
                "StreamlineBlueprint",
                
                "LimenCore",
                "LimenSaveSystem",
                "LimenStorage",
            }
        );
    }
}