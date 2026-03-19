using UnrealBuildTool;

public class RYGameplayInterface : ModuleRules
{
    public RYGameplayInterface(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] 
        {
            // 코어
            "Core", "CoreUObject", "Engine",
            
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}