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
			"Niagara",
			// AI
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"StructUtils",
			//GAS
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks", 
			// Object Pooling
			"rdInst",
			"ActorPoolManager",
			// DeveloperSettings 모듈 링크 (UDeveloperSettings 관련)
			"DeveloperSettings",
			// SoundsByMaterial
			"PhysicsCore",
			// 스팀 + 보이스
			"OnlineSubsystem", 
			"OnlineSubsystemSteam",
			"AdvancedSessions",
		});

#if WITH_EDITOR
		PrivateDependencyModuleNames.AddRange(new string[] { "SequencerCore", "AITestSuite" });
#endif
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}