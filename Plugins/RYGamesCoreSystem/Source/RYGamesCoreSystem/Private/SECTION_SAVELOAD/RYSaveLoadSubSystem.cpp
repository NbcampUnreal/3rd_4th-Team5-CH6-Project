// All CopyRight From YulRyongGameStudio //


#include "SECTION_SAVELOAD/RYSaveLoadSubSystem.h"
#include "SECTION_CYCLE/RYGameCycleLibrary.h"
#include "SECTION_CYCLE/RYGameCycleSubSystem.h"

//======================================================================================================================
#pragma region GetSystem
	//--------------------
	// Get System
	//--------------------

URYSaveLoadSubSystem* URYSaveLoadSubSystem::Get(const UObject* Object)
{
	if (!IsValid(Object))
	{
		// 로그
		return nullptr;
	}
	
	// 월드 컨텍스트로부터 월드를 가져옵니다.
	const UWorld* World = Object->GetWorld();
	if (!IsValid(World))
	{
		// 로그
		return nullptr;
	}
	
	// 월드로부터 게임 인스턴스를 가져옵니다.
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		// 로그
		return nullptr;
	}
	
	// 로그
	
	// 게임 인스턴스로부터 게임 인스턴스 서브 시스템을 가져옵니다.
	return GameInstance->GetSubsystem<URYSaveLoadSubSystem>();
}

bool URYSaveLoadSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

#pragma endregion
//======================================================================================================================
#pragma region LifeCycle
	//--------------------
	// Life Cycle
	//--------------------

void URYSaveLoadSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 반드시 게임 사이클 시스템 이후에 초기화 
	Collection.InitializeDependency(URYGameCycleSubSystem::StaticClass());
	
	// 게임 사이클 델리게이크 구독
	URYGameCycleLibrary::SubscribeGameCycleDelegate(this, TEXT("CallOnGameCycleMode"));
}

void URYSaveLoadSubSystem::Deinitialize()
{
	// 모든 델리게이트 바인딩 해제
	
	Super::Deinitialize();
	
	// 로그
}
	
#pragma endregion
//======================================================================================================================
#pragma region GameCycle
	//--------------------
	// Game Cycle
	//--------------------

void URYSaveLoadSubSystem::CallOnGameCycleMode(ERYGameCycleMode NewGameCycleMode)
{
	switch (NewGameCycleMode)
	{
	case ERYGameCycleMode::None:
		break;
	
	case ERYGameCycleMode::Init:
		break;
		
	case ERYGameCycleMode::MainTitle:
		break;
		
	case ERYGameCycleMode::GameStart:
		break;
		
	case ERYGameCycleMode::GamePause:
		break;
		
	case ERYGameCycleMode::GameEnd:
		break;
		
	case ERYGameCycleMode::ManualSave:
		CallOnManualSaveMode();
		break;
		
	case ERYGameCycleMode::AutoSave:
		CallOnAutoSaveMode();
		break;
		
	case ERYGameCycleMode::ERROR:
		break;

	}
}

void URYSaveLoadSubSystem::CallOnManualSaveMode()
{
}

void URYSaveLoadSubSystem::CallOnAutoSaveMode()
{
}

#pragma endregion
//======================================================================================================================


