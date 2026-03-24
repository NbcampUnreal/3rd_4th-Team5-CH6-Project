// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceSpawnLogicHelperSystem.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceSpawnLogicHelperSystem* UTSResourceSpawnLogicHelperSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UTSResourceSpawnLogicHelperSystem* SpawnLogicHelperSystem = World->GetSubsystem<UTSResourceSpawnLogicHelperSystem>();
	if (!IsValid(SpawnLogicHelperSystem)) return nullptr;

	return SpawnLogicHelperSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

UTSResourceSpawnLogicHelperSystem::UTSResourceSpawnLogicHelperSystem()
{
}

bool UTSResourceSpawnLogicHelperSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) return false;
	if (UWorld* World = Cast<UWorld>(Outer); !IsValid(World) || !World->IsGameWorld() || World->GetNetMode() == ENetMode::NM_Client) return false;
	return true;
}

void UTSResourceSpawnLogicHelperSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 인게임 사이클 시스템 초기화 이후 초기화 및 델리게이트 구독 
	Collection.InitializeDependency<UTSInGameCycleControlSystem>();
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InGameCycleDelegate.AddDynamic(this, &UTSResourceSpawnLogicHelperSystem::OnReceivedInGameCycleDelegate_internal);
}

void UTSResourceSpawnLogicHelperSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// 초기화 완료 알림 
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InitWorldResourceLogicHelperSystemComplete();
}

void UTSResourceSpawnLogicHelperSystem::Deinitialize()
{
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (IsValid(InGameCycleControlSystem))
	{
		InGameCycleControlSystem->InGameCycleDelegate.RemoveAll(this);
	}
	
	Super::Deinitialize();
}



#pragma endregion
//======================================================================================================================	
#pragma region 인게임_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인게임_사이클 (자원 초기화 이후 로직)
	//━━━━━━━━━━━━━━━━━━━━	

void UTSResourceSpawnLogicHelperSystem::OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData)
{
	// 자원 스폰 시스템에 의해서 모든 것이 통제당함.
}

void UTSResourceSpawnLogicHelperSystem::CallWhenNewModeIsCalled_internal()
{
	// 인 게임 사이클 시스템에게 알림 
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->NEW_WorldResourceLogicHelperSystemComplete();
}

void UTSResourceSpawnLogicHelperSystem::CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData)
{
}

void UTSResourceSpawnLogicHelperSystem::CallWhenPlayModeIsCalled_internal()
{
}

#pragma endregion
//======================================================================================================================