// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"

// 현재 켜져야할 서브 오버레이 모드 
UENUM(BlueprintType)
enum class ERYMainTitleMode : uint8
{
	None		UMETA(DisplayName = "None"),
	Default		UMETA(DisplayName = "Default"),
	Single		UMETA(DisplayName = "Single"),
	Multi		UMETA(DisplayName = "Multi"),
	NewGame		UMETA(DisplayName = "New Game"),
	LoadGame	UMETA(DisplayName = "Load Game"),
	Option		UMETA(DisplayName = "Option"),
	Credit		UMETA(DisplayName = "Credit"),
	QuitGame	UMETA(DisplayName = "Quit Game")
};

// 생성할 서브 위젯 타입
UENUM(BlueprintType)
enum class ERYMainTitleNoticeType : uint8
{
	None			UMETA(DisplayName = "None"),
	
	NewGame			UMETA(DisplayName = "New Game"),
	LoadGame		UMETA(DisplayName = "Load Game"),
	DeleteGame		UMETA(DisplayName = "Delete Game"),
	
	OptionSet		UMETA(DisplayName = "Option Set"),
	
	ResetAll		UMETA(DisplayName = "ResetAll"),
	ResetGraphic	UMETA(DisplayName = "ResetGraphic"),
	ResetSound		UMETA(DisplayName = "ResetSound"),
	ResetControl	UMETA(DisplayName = "ResetControl"),
	ResetGameplay	UMETA(DisplayName = "ResetGameplay"),

	Streaming		UMETA(DisplayName = "Streaming"),
	
	CreateSession			UMETA(DisplayName = "Create Session"),
	CreateSessionFail		UMETA(DisplayName = "Create Session Fail"),
	
	JoinSession			UMETA(DisplayName = "Join Session"),
	JoinSessionFail		UMETA(DisplayName = "Join Session Fail"),
	
	QuitGame		UMETA(DisplayName = "Quit Game"),
	
};

// 서브 위젯에서 유저가 요청한 타입 정보
UENUM(BlueprintType)
enum class ERYMainTitleNoticeMode : uint8
{
	None		UMETA(DisplayName = "None"),
	Confirm		UMETA(DisplayName = "Confirm"),
	Cancel		UMETA(DisplayName = "Cancel"),
};
