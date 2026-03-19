using UnrealBuildTool;

public class RYCoreLog : ModuleRules
{
    public RYCoreLog(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core","CoreUObject", "Engine", 
                
                
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}