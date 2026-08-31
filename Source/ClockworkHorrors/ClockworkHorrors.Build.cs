// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

using UnrealBuildTool;

public class ClockworkHorrors : ModuleRules
{
    public ClockworkHorrors(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
           "Core",
           "CoreUObject",
           "Engine",
           "InputCore",
           "AIModule",
           "NavigationSystem",
           "EnhancedInput",
           "Niagara",
           //"AnimGraphRuntime"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore"
        });
    }
}