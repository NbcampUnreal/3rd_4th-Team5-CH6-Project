// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TinySurvivor : ModuleRules
{
	public TinySurvivor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			// UI
			"UMG",
			"Slate",
			"SlateCore",
			"CommonUI",
			"Niagara",
			// AI
			"AIModule",
			"NavigationSystem",
			//GAS
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks", 
			
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}