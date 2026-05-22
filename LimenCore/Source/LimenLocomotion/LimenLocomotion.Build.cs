using UnrealBuildTool;

public class LimenLocomotion : ModuleRules
{
    public LimenLocomotion(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "LimenCore",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "GameplayCameras",
                "EngineCameras",
                "AIModule",
                "NetCore",
                
                "LimenCore",
                "PhysicsCore",
            }
        );
    }
}