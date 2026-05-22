using System.IO;
using UnrealBuildTool;

public class LimenWidgets : ModuleRules
{
    public LimenWidgets(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "InputCore", "UMG"
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
            }
        );

        PublicDefinitions.Add(IsUsingFMODStudio(Target) ? "WITH_FMOD=1" : "WITH_FMOD=0");
    }
    
    public bool IsUsingFMODStudio(ReadOnlyTargetRules InTarget)
    {
        // Possible plugin roots to search
        if (InTarget.ProjectFile == null) return false;
        string[] Roots =
        [
            Path.Combine(InTarget.ProjectFile.Directory.FullName, "Plugins"),
            Path.Combine(InTarget.RelativeEnginePath, "Plugins")
        ];

        foreach (var root in Roots)
        {
            if (!Directory.Exists(root)) continue;
            var Plugins = Directory.GetFiles(root, "FMODStudio.uplugin", SearchOption.AllDirectories);
            if (Plugins.Length > 0)
            {
                return true;
            }
        }

        return false;
    }
}