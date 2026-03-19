// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "SECTION_SYSTEM/RYSystemLog.h"
#include "SECTION_UTIL/System/RYLogMessageSystem.h"

// NOTE : Error 는 패키징 단계에서 문제가 생길 수 있으므로 만들지 않습니다.
// 패키징 단계에서 Error 개수가 많으면 플레이에 지장이 없어도 패키징 fail 이 뜹니다.

//======================================================================================================================

	//--------------------
	// 로그 메시지 시스템 
	//--------------------

// 가변 인자를 지원하는 경고 로그 매크로
#define LOGMESSAGESYSTEM_LOG(Format, ...)											\
{																					\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);							\
	UE_LOG(RYLogSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);		\
}

#define LOGMESSAGESYSTEM_WARN(Format, ...)											\
{																					\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);							\
	UE_LOG(RYLogSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);	\
}

// 시스템(AddLogMessage)에도 자동으로 기록
#define LOGMESSAGESYSTEM_LOG_RECORD(WorldContext, Format, ...)					\
{																				\
	FString Step = TEXT("Log");													\
	FString Category = TEXT("LogMessageSystem");								\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);						\
	UE_LOG(RYLogSystem, Log, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);	\
	if (URYLogMessageSystem* Sys = URYLogMessageSystem::Get(WorldContext))		\
	{																			\
		Sys->AddLogMessage(Category, Step, Msg);								\
	}																			\
}

#define LOGMESSAGESYSTEM_WARN_RECORD(WorldContext, Format, ...)						\
{																					\
	FString Step = TEXT("Warning");													\
	FString Category = TEXT("LogMessageSystem");									\
	FString Msg = FString::Printf(Format, ##__VA_ARGS__);							\
	UE_LOG(RYLogSystem, Warning, TEXT("[%s] %s"), *FString(__FUNCTION__), *Msg);	\
	if (URYLogMessageSystem* Sys = URYLogMessageSystem::Get(WorldContext))			\
	{																				\
		Sys->AddLogMessage(Category, Step, Msg);									\
	}																				\
}
//======================================================================================================================
