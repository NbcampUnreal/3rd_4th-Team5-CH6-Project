// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RYGamesCoreSystem : ModuleRules
{
	public RYGamesCoreSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] { });
		PrivateIncludePaths.AddRange(new string[] { });
			
		
		PublicDependencyModuleNames.AddRange(new string[] 
		{
			
			// 코어
			"Core", "CoreUObject", "Engine", 
			
			// 세팅
			"DeveloperSettings",
			
			// UI
			"Slate", "SlateCore", "UMG",
			
			// 입력
			"InputCore", "EnhancedInput",
			
			// 윈도우 관련
			"ApplicationCore",
			
			// 사운드
			"AudioMixer",
			
			// 로그
			"RYCoreLog",
			
			// GAS
			"GameplayAbilities", "GameplayTasks", "GameplayTags",

			// NetWork
			"OnlineSubsystem", "OnlineSubsystemUtils", "Networking", "Sockets",
			
			// STEAM
			"SteamShared", "Steamworks", "OnlineSubsystemSteam"
			
		});
		
		PrivateDependencyModuleNames.AddRange(new string[] { });
		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}
