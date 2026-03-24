// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceNodeAndBucketGetHelperSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceBucket/TSResourceBucketActor.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceNode/TSResourceNodeActor.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceSpawnControlSystem.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceNodeAndBucketGetHelperSystem* UTSResourceNodeAndBucketGetHelperSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UTSResourceNodeAndBucketGetHelperSystem* NodeAndBucketGetHelperSystem = World->GetSubsystem<UTSResourceNodeAndBucketGetHelperSystem>();
	if (!IsValid(NodeAndBucketGetHelperSystem)) return nullptr;

	return NodeAndBucketGetHelperSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

UTSResourceNodeAndBucketGetHelperSystem::UTSResourceNodeAndBucketGetHelperSystem()
{
}

bool UTSResourceNodeAndBucketGetHelperSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) return false;
	if (UWorld* World = Cast<UWorld>(Outer); !IsValid(World) || !World->IsGameWorld() || World->GetNetMode() == ENetMode::NM_Client) return false;
	return true;
}

void UTSResourceNodeAndBucketGetHelperSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 인게임 사이클 시스템 초기화 이후 초기화 및 델리게이트 구독 
	Collection.InitializeDependency<UTSInGameCycleControlSystem>();
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InGameCycleDelegate.AddDynamic(this, &UTSResourceNodeAndBucketGetHelperSystem::OnReceivedInGameCycleDelegate_internal);
	
}

void UTSResourceNodeAndBucketGetHelperSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UTSResourceNodeAndBucketGetHelperSystem::Deinitialize()
{
	if (IsValid(GetWorld()))
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	}
	
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
	
void UTSResourceNodeAndBucketGetHelperSystem::OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData)
{
	// 자원 스폰 시스템에 의해서 모든 것이 통제당함.
}

void UTSResourceNodeAndBucketGetHelperSystem::CallWhenNewModeIsCalled_internal()
{
	// 인 게임 사이클 시스템에게 알림 
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->NEW_WorldResourceNodeBucketNodeSystemComplete();
}

void UTSResourceNodeAndBucketGetHelperSystem::CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData)
{
	// 인 게임 사이클 시스템에게 알림 
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->LOAD_WorldResourceNodeBucketNodeSystemComplete();
}

void UTSResourceNodeAndBucketGetHelperSystem::CallWhenPlayModeIsCalled_internal()
{
}

#pragma endregion
//======================================================================================================================
#pragma region 노드_버킷_섹션
	
	//━━━━━━━━━━━━━━━━━━━━
	// 노드_버킷_섹션
	//━━━━━━━━━━━━━━━━━━━━

void UTSResourceNodeAndBucketGetHelperSystem::RequestRegisterNode(ATSResourceNodeActor* InNode)
{
	if (!IsValid(InNode)) return;
	auto& NodeArray = NodeList.FindOrAdd(InNode->GetNodeData().StaticData.RegionTag);
	NodeArray.ResourceNodeArray.Add(InNode);
}

void UTSResourceNodeAndBucketGetHelperSystem::RequestRegisterBucket(ATSResourceBucketActor* InBucket)
{
	if (!IsValid(InBucket)) return;
	auto& BucketArray = BucketList.FindOrAdd(InBucket->GetBucketData().StaticData.RegionTag);
	BucketArray.ResourceBucketArray.Add(InBucket);
}


#pragma endregion
//======================================================================================================================	
#pragma region 자원_초기화
	
	//━━━━━━━━━━━━━━━━━━━━
	// 자원_초기화
	//━━━━━━━━━━━━━━━━━━━━
	
void UTSResourceNodeAndBucketGetHelperSystem::InitResourceNodeAndBucketList()
{
	if (!IsValid(GetWorld())) return;
	FTimerHandle InitTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &UTSResourceNodeAndBucketGetHelperSystem::InitResourceNodeAndBucketListComplete, 3.0f, false);
	// 노드와 버킷들은 BeingPlay에서 자신을 등록함. 따라서 모든 등록이 완료되려면 최소 1프레임이 지나야한다. 따라서 안전하게 3초 뒤 기다리고 완료 호출
}

void UTSResourceNodeAndBucketGetHelperSystem::InitResourceNodeAndBucketListComplete()
{
	// 상위 시스템에 다했음을 알림 
	UTSResourceSpawnControlSystem* SpawnControlSystem = UTSResourceSpawnControlSystem::Get(this);
	if (!IsValid(SpawnControlSystem)) return;
	SpawnControlSystem->InitResourceSpawnNodeAndBucketsCompleteReceive();
	
	// 인 게임 사이클 시스템에게도 알림 
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InitWorldResourceNodeBucketNodeSystemComplete();
}

#pragma endregion
//======================================================================================================================	