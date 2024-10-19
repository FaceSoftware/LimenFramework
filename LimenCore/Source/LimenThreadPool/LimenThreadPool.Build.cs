using UnrealBuildTool;

public class LimenThreadPool : ModuleRules
{
    public LimenThreadPool(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "DeveloperSettings",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                
                "AutomationTest",
            }
        );

        switch (Target.Type)
        {
            case TargetType.Game:
                PrivateDependencyModuleNames.Add("AutomationUtils");
                break;
            
            case TargetType.Editor:
                PrivateDependencyModuleNames.Add("AutomationUtilsEditor");
                break;
        }
    }
}