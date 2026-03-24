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
}

void UTSResourceSpawnControlSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// 자원 초기화 실시 
	InitResourceSpawnNodeAndBuckets();
}

void UTSResourceSpawnControlSystem::Deinitialize()
{
	Super::Deinitialize();
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