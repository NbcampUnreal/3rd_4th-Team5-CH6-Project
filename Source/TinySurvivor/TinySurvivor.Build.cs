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
			"UMG", 
			
			// VFX 모듈
			"Niagara",
			
			// SoundsByMaterial 모듈
			"PhysicsCore",

			// AI 모듈
			"AIModule", "NavigationSystem", 
			
			// 스테이트 트리 모듈 
			"StateTreeModule", "GameplayStateTreeModule",
			
			// GAS 모듈
			"GameplayAbilities", "GameplayTags", "GameplayTasks", 
			
			// 스팀 + 보이스 모듈
			"OnlineSubsystem", "OnlineSubsystemSteam", "AdvancedSessions",
			
			// DeveloperSettings 모듈 (UDeveloperSettings 관련) 
			"DeveloperSettings",
			
			// 게임 플레이 메시지 시스템 모듈
			"GameplayMessageRuntime",
			
			// ry 게임 코어 시스템 모듈
			"RYGamesCoreSystem",
			
		});
	}
}