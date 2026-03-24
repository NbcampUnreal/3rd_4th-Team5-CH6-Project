// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/System/TSResourceSpawnHelperSystem.h"

#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSResourceSpawnHelperSystem* UTSResourceSpawnHelperSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UTSResourceSpawnHelperSystem* SpawnHelperSystem = World->GetSubsystem<UTSResourceSpawnHelperSystem>();
	if (!IsValid(SpawnHelperSystem)) return nullptr;

	return SpawnHelperSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

UTSResourceSpawnHelperSystem::UTSResourceSpawnHelperSystem()
{
}

bool UTSResourceSpawnHelperSystem::IsTickable() const
{
	return true;
}

TStatId UTSResourceSpawnHelperSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTSResourceSpawnHelperSystem, STATGROUP_Tickables);
}

bool UTSResourceSpawnHelperSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) return false;
	if (UWorld* World = Cast<UWorld>(Outer); !IsValid(World) || !World->IsGameWorld() || World->GetNetMode() == ENetMode::NM_Client) return false;
	return true;
}

void UTSResourceSpawnHelperSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 초기화 완료 알림
	UTSInGameCycleControlSystem* InGameCycleControlSystem = UTSInGameCycleControlSystem::Get(this);	
	if (!IsValid(InGameCycleControlSystem)) return;
	InGameCycleControlSystem->InitWorldResourceSpawnHelperSystemComplete();
}

void UTSResourceSpawnHelperSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UTSResourceSpawnHelperSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UTSResourceSpawnHelperSystem::Deinitialize()
{
	Super::Deinitialize();
}

#pragma endregion
//======================================================================================================================