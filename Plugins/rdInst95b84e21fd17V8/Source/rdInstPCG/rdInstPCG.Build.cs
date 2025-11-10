// Copyright (c) 2022 Recourse Design ltd. All rights reserved.

using UnrealBuildTool;

public class rdInstPCG : ModuleRules {
	public rdInstPCG(ReadOnlyTargetRules Target) : base(Target) {

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;

		PublicIncludePaths.AddRange(
			new string[] {
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"rdInst",
				"Core",
				"Engine",
				"PCG"
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Slate",
				"SlateCore"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
		);
	}
}
