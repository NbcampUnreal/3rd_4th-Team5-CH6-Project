// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"

// NOTE : Error 는 패키징 단계에서 문제가 생길 수 있으므로 만들지 않습니다.
// 패키징 단계에서 Error 개수가 많으면 플레이에 지장이 없어도 패키징 fail 이 뜹니다.

//======================================================================================================================
#pragma region 게임_사이클_시스템

	//--------------------
	// 게임 사이클 시스템 
	//--------------------

// 가변 인자를 지원하는 경고 로그 매크로
#define CYCLESYSTEM_LOG(Format, ...)												\
{																					\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);							\
	UE_LOG(RYGameCycleSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);	\
}

#define CYCLESYSTEM_WARN(Format, ...)													\
{																						\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);								\
	UE_LOG(RYGameCycleSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);	\
}

// 시스템(AddLogMessage)에도 자동으로 기록
#define CYCLESYSTEM_LOG_RECORD(WorldContext, Format, ...)											\
{																									\
	FString Step = TEXT("Log");																		\
	FString Category = TEXT("CycleSystem");															\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYGameCycleSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);					\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#define CYCLESYSTEM_WARN_RECORD(WorldContext, Format, ...)											\
{																									\
	FString Step = TEXT("Warning");																	\
	FString Category = TEXT("CycleSystem");															\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYGameCycleSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);				\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#pragma endregion
//======================================================================================================================

//======================================================================================================================
#pragma region 코어_로그_메시지_시스템

	//--------------------
	// 코어 로그 메시지 시스템 
	//--------------------

// 가변 인자를 지원하는 경고 로그 매크로
#define CORELOGMESSAGESYSTEM_LOG(Format, ...)											\
{																						\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);								\
	UE_LOG(RYCoreLogSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);		\
}

#define CORELOGMESSAGESYSTEM_WARN(Format, ...)											\
{																						\
FString Msg = FString::Printf(Format, ##__VA_ARGS__);									\
UE_LOG(RYCoreLogSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);		\
}

// 시스템(AddLogMessage)에도 자동으로 기록
#define CORELOGMESSAGESYSTEM_LOG_RECORD(WorldContext, Format, ...)									\
{																									\
	FString Step = TEXT("Log");																		\
	FString Category = TEXT("CoreLogMessageSystem");												\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYCoreLogSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);					\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#define CORELOGMESSAGESYSTEM_WARN_RECORD(WorldContext, Format, ...)									\
{																									\
	FString Step = TEXT("Warning");																	\
	FString Category = TEXT("CoreLogMessageSystem");												\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYCoreLogSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);				\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#pragma endregion
//======================================================================================================================

//======================================================================================================================
#pragma region 옵션_시스템

	//--------------------
	// 옵션 시스템 
	//--------------------

// 가변 인자를 지원하는 경고 로그 매크로
#define OPTIONSYSTEM_LOG(Format, ...)											\
{																						\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);								\
	UE_LOG(RYOptionSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);		\
}

#define OPTIONSYSTEM_WARN(Format, ...)											\
{																						\
FString Msg = FString::Printf(Format, ##__VA_ARGS__);									\
UE_LOG(RYOptionSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);		\
}

// 시스템(AddLogMessage)에도 자동으로 기록
#define OPTIONSYSTEM_LOG_RECORD(WorldContext, Format, ...)											\
{																									\
	FString Step = TEXT("Log");																		\
	FString Category = TEXT("OptionSystem");														\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYOptionSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);						\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#define OPTIONSYSTEM_WARN_RECORD(WorldContext, Format, ...)											\
{																									\
	FString Step = TEXT("Warning");																	\
	FString Category = TEXT("OptionSystem");														\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYOptionSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);					\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#pragma endregion
//======================================================================================================================

//======================================================================================================================
#pragma region 세이브_로드_시스템

	//--------------------
	// 세이브 로드 시스템
	//--------------------

// 가변 인자를 지원하는 경고 로그 매크로
#define SAVELOADSYSTEM_LOG(Format, ...)													\
{																						\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);								\
	UE_LOG(RYSaveLoadSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);		\
}

#define SAVELOADSYSTEM_WARN(Format, ...)													\
{																							\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);									\
	UE_LOG(RYSaveLoadSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);		\
}

// 시스템(AddLogMessage)에도 자동으로 기록
#define SAVELOADSYSTEM_LOG_RECORD(WorldContext, Format, ...)										\
{																									\
	FString Step = TEXT("Log");																		\
	FString Category = TEXT("SaveLoadSystem");														\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYSaveLoadSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);					\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#define SAVELOADSYSTEM_WARN_RECORD(WorldContext, Format, ...)										\
{																									\
	FString Step = TEXT("Warning");																	\
	FString Category = TEXT("SaveLoadSystem");														\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYSaveLoadSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);				\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#pragma endregion
//======================================================================================================================

//======================================================================================================================
#pragma region 로비_시스템

	//--------------------
	// 로비 시스템
	//--------------------

// 가변 인자를 지원하는 경고 로그 매크로
#define LOBBYSYSTEM_LOG(Format, ...)													\
{																						\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);								\
	UE_LOG(RYLobbySystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);			\
}

#define LOBBYSYSTEM_WARN(Format, ...)														\
{																							\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);									\
	UE_LOG(RYLobbySystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);			\
}

// 시스템(AddLogMessage)에도 자동으로 기록
#define LOBBYSYSTEM_LOG_RECORD(WorldContext, Format, ...)											\
{																									\
	FString Step = TEXT("Log");																		\
	FString Category = TEXT("LobbySystem");															\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYLobbySystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);						\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#define LOBBYSYSTEM_WARN_RECORD(WorldContext, Format, ...)											\
{																									\
	FString Step = TEXT("Warning");																	\
	FString Category = TEXT("LobbySystem");															\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYLobbySystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);					\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#pragma endregion
//======================================================================================================================

//======================================================================================================================
#pragma region 메인 타이틀 UI 컨트롤러_시스템

	//--------------------
	// 로비 시스템
	//--------------------

// 가변 인자를 지원하는 경고 로그 매크로
#define MAINUICMSYSTEM_LOG(Format, ...)																		\
{																											\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);													\
	UE_LOG(RYMainTitleUIControlModelSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);			\
}

#define MAINUICMSYSTEM_WARN(Format, ...)																		\
{																												\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);														\
	UE_LOG(RYMainTitleUIControlModelSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);			\
}

// 시스템(AddLogMessage)에도 자동으로 기록
#define MAINUICMSYSTEM_LOG_RECORD(WorldContext, Format, ...)										\
{																									\
	FString Step = TEXT("Log");																		\
	FString Category = TEXT("MainTitleUIControlModelSystem");										\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYMainTitleUIControlModelSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);	\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#define MAINUICMSYSTEM_WARN_RECORD(WorldContext, Format, ...)										\
{																									\
	FString Step = TEXT("Warning");																	\
	FString Category = TEXT("MainTitleUIControlModelSystem");										\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);											\
	UE_LOG(RYMainTitleUIControlModelSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);\
	if (URYCoreLogMessageSystem* Sys = URYCoreLogMessageSystem::Get(WorldContext); IsValid(Sys))	\
	{																								\
		Sys->AddCoreLogMessage(Category, Step, Msg);												\
	}																								\
}

#pragma endregion
//======================================================================================================================

