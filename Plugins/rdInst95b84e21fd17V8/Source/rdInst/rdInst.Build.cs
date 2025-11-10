// Copyright (c) 2022 Recourse Design ltd. All rights reserved.

using UnrealBuildTool;

public class rdInst : ModuleRules {
	public rdInst(ReadOnlyTargetRules Target) : base(Target) {

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
				"Core",
				"Engine",
				"Landscape",
				//"ProceduralMeshComponent",
				//"PCG",
				"Foliage",
				"PhysicsCore",
				"Niagara",
				"NavigationSystem",
				"UMG",
				"RHI",
				"RenderCore",
				"GeometryCollectionEngine"
			}
		);
			
		if(Target.bBuildEditor)	{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.Add("LevelEditor");
		}
		
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
