// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RYGamePlaySystem : ModuleRules
{
	public RYGamePlaySystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] { });
		PrivateIncludePaths.AddRange(new string[] { });
			
		
		PublicDependencyModuleNames.AddRange(new string[] 
		{
			
			// 코어
			"Core", "CoreUObject", "Engine",
			
			// UI
			"Slate", "SlateCore", "UMG",
			
			// 입력
			"InputCore", "EnhancedInput",
			
			// GAS
			"GameplayAbilities", "GameplayTasks", "GameplayTags",
			
			// RYCore
			"RYGamesCoreSystem",
			
			// Gameplay
			"RYGameplayInterface", "RYGameplayLog",
			
			// Json
			"Json", "JsonUtilities",
			
			// http
			"HTTP",
			
			
		});
		
		PrivateDependencyModuleNames.AddRange(new string[] { });
		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}
