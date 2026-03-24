// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceSpawnControlSystem.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceNodeAndBucketGetHelperSystem.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceSpawnControlSystem* UTSResourceSpawnControlSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UTSResourceSpawnControlSystem* SpawnControlSystem = World->GetSubsystem<UTSResourceSpawnControlSystem>();
	if (!IsValid(SpawnControlSystem)) return nullptr;

	return SpawnControlSystem;
}	

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

UTSResourceSpawnControlSystem::UTSResourceSpawnControlSystem()
{
}

bool UTSResourceSpawnControlSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) return false;
	if (UWorld* World = Cast<UWorld>(Outer); !IsValid(World) || !World->IsGameWorld() || World->GetNetMode() == ENetMode::NM_Client) return false;
	return true;
}

void UTSResourceSpawnControlSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 인게임 사이클 시스템 초기화 이후 초기화 및 델리게이트 구독 
	Collection.InitializeDependency<UTSInGameCycleControlSystem>();
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InGameCycleDelegate.AddDynamic(this, &UTSResourceSpawnControlSystem::OnReceivedInGameCycleDelegate_internal);
}

void UTSResourceSpawnControlSystem::PostInitialize()
{
	Super::PostInitialize();
	
	
}

void UTSResourceSpawnControlSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// 자원 초기화 실시 
	InitResourceSpawnNodeAndBuckets();
}

void UTSResourceSpawnControlSystem::Deinitialize()
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

void UTSResourceSpawnControlSystem::OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData)
{
	switch (InGameCycleMode) 
	{
	case ETSInGameCycleMode::NEW:
		CallWhenNewModeIsCalled_internal();
		break;
		
	case ETSInGameCycleMode::LOAD:
		CallWhenLoadModeIsCalled_internal(InData);
		break;
	
	case ETSInGameCycleMode::PLAY:
		CallWhenPlayModeIsCalled_internal();
		break;
	}
}

void UTSResourceSpawnControlSystem::CallWhenNewModeIsCalled_internal()
{
	UTSResourceNodeAndBucketGetHelperSystem* ResourceNodeAndBucketGetHelperSystem = UTSResourceNodeAndBucketGetHelperSystem::Get(this);
	if (!IsValid(ResourceNodeAndBucketGetHelperSystem)) return;
	
	// 1. 버킷 리스트를 돌면서 region 마다 전부 캐싱  
	ResourceNodeAndBucketGetHelperSystem->BucketList;
	for (auto& [Region, BucketArray] : ResourceNodeAndBucketGetHelperSystem->BucketList)
	{
		FTSResourceSpawnControlSystemPerRegionRunTimeData& PerRegionRunTimeData = ResourceSpawnControlSystemPerRegionRunTimeData.FindOrAdd(Region);
		
		for (auto& Bucket : BucketArray)
		{
			if (IsValid(Bucket)) PerRegionRunTimeData.BucketPtrArray.AddUnique(Bucket);
		}
	}
	
	// 2. 노드 리스트를 돌면서 region 마다 전부 캐싱 
	ResourceNodeAndBucketGetHelperSystem->NodeList;
	for (auto& [Region, NodeArray] : ResourceNodeAndBucketGetHelperSystem->NodeList)
	{
		FTSResourceSpawnControlSystemPerRegionRunTimeData& PerRegionRunTimeData = ResourceSpawnControlSystemPerRegionRunTimeData.FindOrAdd(Region);
		
		for (auto& Node : NodeArray)
		{
			if (IsValid(Node)) PerRegionRunTimeData.NodePtrArray.AddUnique(Node);
		}
	}	
	
	// 자원 노드, 버킷 시스템 할 일 끝. 
	ResourceNodeAndBucketGetHelperSystem->CallWhenNewModeIsCalled_internal();
	
	TSResourceSpawnControlSystemPerRegionRunTimeData.Empty();
	
	
	
}

void UTSResourceSpawnControlSystem::CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData)
{
}

void UTSResourceSpawnControlSystem::CallWhenPlayModeIsCalled_internal()
{
}
#pragma endregion
//======================================================================================================================	
#pragma region 자원_초기화
	
	//━━━━━━━━━━━━━━━━━━━━
	// 자원_초기화
	//━━━━━━━━━━━━━━━━━━━━

void UTSResourceSpawnControlSystem::InitResourceSpawnNodeAndBuckets()
{
	// 노드, 버킷 겟 헬퍼 시스템에게 초기화 실시 
	UTSResourceNodeAndBucketGetHelperSystem* ResourceNodeAndBucketGetHelperSystem = GetWorld()->GetSubsystem<UTSResourceNodeAndBucketGetHelperSystem>();
	if (!IsValid(ResourceNodeAndBucketGetHelperSystem)) return;
	ResourceNodeAndBucketGetHelperSystem->InitResourceNodeAndBucketList();
}

void UTSResourceSpawnControlSystem::InitResourceSpawnNodeAndBucketsCompleteReceive()
{
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InitWorldResourceControlSystemComplete();
}

#pragma endregion
//======================================================================================================================	
#pragma region 자원_관리
	
//━━━━━━━━━━━━━━━━━━━━
// 라이프 사이클
//━━━━━━━━━━━━━━━━━━━━
	
	
#pragma endregion
//======================================================================================================================	