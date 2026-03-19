// All CopyRight From YulRyongGameStudio //


#include "SECTION_CYCLE/RYGameCycleLibrary.h"
#include "SECTION_CYCLE/RYGameCycleSubSystem.h"
#include "SECTION_UTIL/Marco/RYCoreLogMarco.h"
#include "SECTION_UTIL/RYCoreLogMessageSystem.h"
#include "RYLog.h"

// 게임 사이클 매니저의 델리게이트를 구독하는 API
void URYGameCycleLibrary::SubscribeGameCycleDelegate(UObject* Object, const FName FunctionName)
{
	// NOTE : 오브젝트와 함수 이름 유효성 검사는 SubscribeGameCycleDelegate 에서 하고 있다.
	
	URYGameCycleSubSystem* GameCycleSubSystem = URYGameCycleSubSystem::Get(Object);
	if (!IsValid(GameCycleSubSystem))
	{
		CYCLESYSTEM_WARN(TEXT("[URYGameCycleLibrary] SubscribeGameCycleDelegate : Object fail"));
		return;
	}
	
	GameCycleSubSystem->SubscribeGameCycleDelegate(Object, FunctionName);

	CYCLESYSTEM_LOG_RECORD(GameCycleSubSystem, TEXT("[URYGameCycleLibrary] SubscribeGameCycleDelegate : Success"));
}

// 게임 사이클 매니저에게 특정 상황임을 알리는 API
void URYGameCycleLibrary::SetGameCycleMode(const UObject* Object, const ERYGameCycleMode NewGameCycleMode)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
		
	URYGameCycleSubSystem* GameCycleSubSystem = URYGameCycleSubSystem::Get(Object);
	if (!IsValid(GameCycleSubSystem))
	{
		CYCLESYSTEM_WARN(TEXT("[URYGameCycleLibrary] SetGameCycleMode : Object fail"));
		return;
	}
	
	GameCycleSubSystem->SetGameCycleMode(NewGameCycleMode);

	CYCLESYSTEM_LOG_RECORD(GameCycleSubSystem, TEXT("[URYGameCycleLibrary] SetGameCycleMode : Success"));
}

// 게임 사이클 매니저로부터 현재 어떤 상황임을 확인받는 API
ERYGameCycleMode URYGameCycleLibrary::GetGameCycleMode(const UObject* Object)
{
	// NOTE : 오브젝트 검사는 Get 에서 하고 있다.
	
	URYGameCycleSubSystem* GameCycleSubSystem = URYGameCycleSubSystem::Get(Object);
	if (!IsValid(GameCycleSubSystem))
	{
		// 로그
		CYCLESYSTEM_WARN(TEXT("[URYGameCycleLibrary] GetGameCycleMode : Object fail"));
		return ERYGameCycleMode::ERROR;
	}
	
	CYCLESYSTEM_LOG_RECORD(GameCycleSubSystem, TEXT("[URYGameCycleLibrary] GetGameCycleMode : Success"));
	return 	GameCycleSubSystem->GetGameCycleMode();
}
