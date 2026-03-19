// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TinySurvivor : ModuleRules
{
	public TinySurvivor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			
			// 코어 모듈
			"Core", "CoreUObject", "Engine",
				
			//	입력 모듈
			"InputCore", "EnhancedInput",
			
			// UI 모듈
			"UMG", "Slate", "SlateCore",
			
			// VFX 모듈
			"Niagara",
			
			// SoundsByMaterial 모듈
			"PhysicsCore",

			// AI 모듈
			"AIModule", "NavigationSystem", "StateTreeModule", "GameplayStateTreeModule",
			
			// 구조체 유틸 모듈
			"StructUtils",
			
			// GAS 모듈
			"GameplayAbilities", "GameplayTags", "GameplayTasks", 
			
			// 스팀 + 보이스 모듈
			"OnlineSubsystem", "OnlineSubsystemSteam", "AdvancedSessions",
			
			// Object Pooling 모듈
			"rdInst", "ActorPoolManager",
			
			// DeveloperSettings 모듈 (UDeveloperSettings 관련) 
			"DeveloperSettings",
			
		});

#if WITH_EDITOR
		PrivateDependencyModuleNames.AddRange(new string[] { "SequencerCore", "AITestSuite" });
#endif
	}
}