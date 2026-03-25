// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceSpawnCalHelperSystem.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Enum/TSResourceType.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/System/TSResourceDataSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/System/TSResourceSpawnSystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Actor/ResourceNode/TSResourceNodeActor.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceNodeArray.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceSpawnCalHelperSystem* UTSResourceSpawnCalHelperSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UTSResourceSpawnCalHelperSystem* SpawnHelperSystem = World->GetSubsystem<UTSResourceSpawnCalHelperSystem>();
	if (!IsValid(SpawnHelperSystem)) return nullptr;

	return SpawnHelperSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

UTSResourceSpawnCalHelperSystem::UTSResourceSpawnCalHelperSystem()
{
}

bool UTSResourceSpawnCalHelperSystem::IsTickable() const
{
	return true;
}

TStatId UTSResourceSpawnCalHelperSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTSResourceSpawnHelperSystem, STATGROUP_Tickables);
}

bool UTSResourceSpawnCalHelperSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) return false;
	if (UWorld* World = Cast<UWorld>(Outer); !IsValid(World) || !World->IsGameWorld() || World->GetNetMode() == ENetMode::NM_Client) return false;
	return true;
}

void UTSResourceSpawnCalHelperSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 인게임 사이클 시스템 초기화 이후 초기화 및 델리게이트 구독 
	Collection.InitializeDependency<UTSInGameCycleControlSystem>();
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InGameCycleDelegate.AddDynamic(this, &UTSResourceSpawnCalHelperSystem::OnReceivedInGameCycleDelegate_internal);
}

void UTSResourceSpawnCalHelperSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// 초기화 완료 알림
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InitWorldResourceSpawnCalHelperSystemComplete();
}

void UTSResourceSpawnCalHelperSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UTSResourceSpawnCalHelperSystem::Deinitialize()
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

void UTSResourceSpawnCalHelperSystem::OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData)
{
	// 자원 스폰 시스템에 의해서 모든 것이 통제당함.
}

void UTSResourceSpawnCalHelperSystem::CallWhenNewModeIsCalled_internal()
{
	// 인 게임 사이클 시스템에게 알림 
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->NEW_WorldResourceSpawnCalHelperSystemComplete();
}

void UTSResourceSpawnCalHelperSystem::CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData)
{
}

void UTSResourceSpawnCalHelperSystem::CallWhenPlayModeIsCalled_internal()
{
}

void UTSResourceSpawnCalHelperSystem::NEW_StartSpawnResourceBasedOnTableData_internal(FTSResourceSpawnControlSystemPerRegionRunTimeData& InNodeAndBucketPtrData, FTSResourceSpawnTableStaticData& InSpawnTableData)
{
	// NOTE : 버킷은 쓰지 않음. 새 게임일 경우 고려할 필요가 없음.

	// 범용 map : key 타입, value 노드 배열
	// 유니크 map : key 자원 ID, value 노드 배열
	TMap<ETSResourceType, FTSResourceNodeArray> TempGeneralNodeMap;
	TMap<int32, FTSResourceNodeArray> TempUniqueNodeMap;
	
	// 1단계 : 범용 노드와 유니크 노드 구분 
	for (auto& Node : InNodeAndBucketPtrData.NodePtrArray)
	{
		// 유효성 체크 
		if (!IsValid(Node)) continue;
		
		// 범용 map 에 추가
		if (Node->GetNodeData().StaticData.bIsGeneralResource == true && Node->GetNodeData().StaticData.ResourceType != ETSResourceType::None)
		{
			auto& Value = TempGeneralNodeMap.FindOrAdd(Node->GetNodeData().StaticData.ResourceType);
			Value.ResourceNodeArray.Add(Node);
		}
		// 유니크 map 에 추가
		else if (Node->GetNodeData().StaticData.bIsGeneralResource == false && Node->GetNodeData().StaticData.ResourceID != -1)
		{
			auto& Value = TempUniqueNodeMap.FindOrAdd(Node->GetNodeData().StaticData.ResourceID);
			Value.ResourceNodeArray.Add(Node);
		}
	}
	
	// 자원 데이터, 자원 스폰 시스템 체크 
	UTSResourceDataSystem* ResourceDataSystem = UTSResourceDataSystem::Get(this);
	UTSResourceSpawnSystem* ResourceSpawnSystem = UTSResourceSpawnSystem::Get(this);

	if (!IsValid(ResourceDataSystem)) return;
	if (!IsValid(ResourceSpawnSystem)) return;;
	
	// by 영역 데이터 테이블 순환하며 전부 체크
	for (auto& ResourceSpawnTableData : InSpawnTableData.ResourceSpawnPerRegionDataArray)
	{
		// 공통 : 소환하려는 자원의 정적 데이터가 유효한지 체크
		FTSResourceStaticData* Data = ResourceDataSystem->GetResourceStaticData(ResourceSpawnTableData.ResourceID);
		if (!Data) continue;
		
		// 소환하려는 갯수만큼 반복
		for (int32 SpawnCount = 0; SpawnCount < ResourceSpawnTableData.SpawnCount; ++SpawnCount)
		{
			int32 RandomIndex;
			FTSResourceNodeArray* Value;
		
			// 범용 스폰일 경우 실행하는 코드
			if (ResourceSpawnTableData.bIsGeneralNode == true)
			{
				// 들어갈 수 있는 노드가 있는지 판단 (타입)
				Value = TempGeneralNodeMap.Find(Data->ResourceBaseInfoTable.ResourceType);
				if (!Value) continue;

				// 랜덤 노드 인덱스 받기 
				RandomIndex = FMath::RandHelper(Value->ResourceNodeArray.Num());
				if (!Value->ResourceNodeArray.IsValidIndex(RandomIndex)) continue;
			}
			// 유니크인 경우 실행하는 코드
			else
			{
				// 들어갈 수 있는 노드가 있는지 판단 (ID)
				Value = TempUniqueNodeMap.Find(ResourceSpawnTableData.ResourceID);
				if (!Value) continue;

				// 랜덤 인덱스 받기
				RandomIndex = FMath::RandHelper(Value->ResourceNodeArray.Num());
				if (!Value->ResourceNodeArray.IsValidIndex(RandomIndex)) continue;
			}

			// (공통) 노드 가져오기
			auto& TargetNode = Value->ResourceNodeArray[RandomIndex];
			if (!IsValid(TargetNode)) continue;
		
			// (공통) 스폰 실시 
			bool bSpawnSuccess = ResourceSpawnSystem->SpawnNewResource(ResourceSpawnTableData.ResourceID, TargetNode->GetActorLocation(), TargetNode->GetActorRotation(), TargetNode);
			if (bSpawnSuccess == false) continue;

			// (공통) 배열에서 제거
			Value->ResourceNodeArray.RemoveAt(RandomIndex);
		}
	}
}

#pragma endregion
//======================================================================================================================	