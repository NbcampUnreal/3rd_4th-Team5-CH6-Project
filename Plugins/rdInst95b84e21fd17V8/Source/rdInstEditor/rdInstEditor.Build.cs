// Copyright (c) 2022 Recourse Design ltd. All rights reserved.

using UnrealBuildTool;

public class rdInstEditor : ModuleRules {
	public rdInstEditor(ReadOnlyTargetRules Target) : base(Target) {

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
				"UnrealEd",
				"LevelEditor",
				"InputCore",
				"ContentBrowser",
				"ContentBrowserData",
				"MainFrame",
				"EditorSubsystem",
				"EditorFramework",
				"PropertyEditor",
				"ToolMenus",
				"SceneOutliner",
				"EditorStyle",
				"ContentBrowser",
				"ContentBrowserData",
				"Projects",
				"ActorPickerMode",
				"AppFramework",
				"ToolWidgets",
				"PlacementMode",
				"UMG",
				"rdInst"
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
