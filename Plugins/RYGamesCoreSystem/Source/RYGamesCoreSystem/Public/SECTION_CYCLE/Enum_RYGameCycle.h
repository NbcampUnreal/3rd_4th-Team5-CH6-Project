// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ERYGameCycleMode : uint8
{
	None		UMETA(DisplayName = "None"),
	
	Init		UMETA(DisplayName = "Init"),			// 게임 처음 가동 시
	MainTitle	UMETA(DisplayName = "Main Title"),		// 메인 타이틀 화면 진입 시 
	
	GameStart	UMETA(DisplayName = "Game Start"),		// 게임 시작 시 (싱글, 멀티 포함)
	GamePause	UMETA(DisplayName = "Game Pause"),		// 게임 중지 시 (싱글, 멀티 포함)
	GameEnd		UMETA(DisplayName = "Game End"),		// 게임 종료 시 (싱글, 멀티 포함)
	
	ManualSave	UMETA(DisplayName = "Manual Save"),		// 수동 세이브 요청 시 (싱글, 멀티 포함)
	AutoSave	UMETA(DisplayName = "Auto Save"),		// 오토 세이브 요청 시 (싱글, 멀티 포함)
	
	SAVELOG		UMETA(DisplayName = "SAVELOG"),			// 로그 세이브 요청 시
	ERROR		UMETA(DisplayName = "ERROR")			// 게임 사이클을 돌릴 수 없는 에러 발생 시
};