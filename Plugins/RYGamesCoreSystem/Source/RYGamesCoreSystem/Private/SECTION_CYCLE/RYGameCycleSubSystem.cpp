// All CopyRight From YulRyongGameStudio //


#include "SECTION_CYCLE/RYGameCycleSubSystem.h"
#include "SECTION_UTIL/Marco/RYCoreLogMarco.h"
#include "SECTION_UTIL/RYCoreLogMessageSystem.h"
#include "RYLog.h"

//======================================================================================================================
#pragma region Getter

	//--------------------
	// Getter
	//--------------------

URYGameCycleSubSystem* URYGameCycleSubSystem::Get(const UObject* Object)
{
	// 시도하려는 오브젝트 확인
	if (IsValid(Object) == false)
	{
		CYCLESYSTEM_WARN(TEXT("Get : Object fail"));
		return nullptr;
	}
	
	// 월드 컨텍스트로부터 월드를 가져옵니다.
	const UWorld* World = Object->GetWorld();
	if (IsValid(World) == false)
	{
		CYCLESYSTEM_WARN_RECORD(Object, TEXT("Get : World fail"));
		return nullptr;
	}
	
	// 월드로부터 게임 인스턴스를 가져옵니다.
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		CYCLESYSTEM_WARN_RECORD(World, TEXT("Get : GameInstance fail"));
		return nullptr;
	}

	// 게임 인스턴스로부터 게임 인스턴스 서브 시스템을 가져옵니다.
	URYGameCycleSubSystem* GameCycleSubSystem = GameInstance->GetSubsystem<URYGameCycleSubSystem>();
	if (IsValid(GameCycleSubSystem) == false)
	{
		CYCLESYSTEM_WARN_RECORD(GameInstance, TEXT("Get : GameCycleSubSystem fail"));
		return nullptr;
	}
	
	CYCLESYSTEM_LOG_RECORD(GameCycleSubSystem, TEXT("Get : Success"));
	return GameCycleSubSystem;
}

#pragma endregion
//======================================================================================================================
#pragma region LifeCycle
	//--------------------
	// Life Cycle
	//--------------------

bool URYGameCycleSubSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}


void URYGameCycleSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	CYCLESYSTEM_LOG(TEXT("Initialize : Success"));
}

void URYGameCycleSubSystem::Deinitialize()
{
	// 모든 델리게이트 바인딩 해제
	OnGameCycleChangedDelegate.Clear();
	
	CYCLESYSTEM_LOG(TEXT("Deinitialize : Success"));
	
	Super::Deinitialize();
}

#pragma endregion
//======================================================================================================================
#pragma region GameCycle
	//--------------------
	// Game Cycle
	//--------------------

void URYGameCycleSubSystem::SubscribeGameCycleDelegate(UObject* Object, const FName FunctionName)
{
	if (!IsValid(Object))
	{
		CYCLESYSTEM_WARN_RECORD(this, TEXT("SubscribeGameCycleDelegate : Object fail"));
		return;
	}
	
	if (FunctionName.IsNone())
	{
		CYCLESYSTEM_WARN_RECORD(this, TEXT("SubscribeGameCycleDelegate : FunctionName fail"));
		return;
	}
	
	// 스크립트 델리게이트 생성 및 설정
	FScriptDelegate Delegate;
	Delegate.BindUFunction(Object, FunctionName);
	
	// 등록 (중복 방지 적용)
	OnGameCycleChangedDelegate.AddUnique(Delegate);
	
	CYCLESYSTEM_LOG_RECORD(this, TEXT("SubscribeGameCycleDelegate : Success"));
}

#pragma endregion
//======================================================================================================================
