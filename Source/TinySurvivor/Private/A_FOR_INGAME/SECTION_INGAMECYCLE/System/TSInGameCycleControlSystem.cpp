// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSInGameCycleControlSystem* UTSInGameCycleControlSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UTSInGameCycleControlSystem* SpawnControlSystem = World->GetSubsystem<UTSInGameCycleControlSystem>();
	if (!IsValid(SpawnControlSystem)) return nullptr;

	return SpawnControlSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

UTSInGameCycleControlSystem::UTSInGameCycleControlSystem()
{

}

bool UTSInGameCycleControlSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) return false;
	if (UWorld* World = Cast<UWorld>(Outer); !IsValid(World) || !World->IsGameWorld() || World->GetNetMode() == ENetMode::NM_Client) return false;
	return true;
}

void UTSInGameCycleControlSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTSInGameCycleControlSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UTSInGameCycleControlSystem::Deinitialize()
{
	Super::Deinitialize();
}


#pragma endregion
//======================================================================================================================
#pragma region 초기화
	
	//━━━━━━━━━━━━━━━━━━━━
	// 초기화
	//━━━━━━━━━━━━━━━━━━━━	
	
void UTSInGameCycleControlSystem::InitInGamePlayerRegister(APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	
	FTSPlayerReadyData PlayerInitData;
	PlayerInitData.PlayerController = InPlayerController;
	PlayerInitData.IsReady = false;
	
	InitCheckData.AllPlayersReadyCheckData.Add(PlayerInitData);
}

void UTSInGameCycleControlSystem::InitInGamePlayerComplete(APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	
	for (auto& PlayerInitData : InitCheckData.AllPlayersReadyCheckData)
	{
		if (PlayerInitData.PlayerController != InPlayerController) continue;
		PlayerInitData.IsReady = true;
		CheckCanStart_LOAD();
		return;
	}
}

void UTSInGameCycleControlSystem::CheckCanStart_LOAD()
{
	// 플레이어 체크 
	for (auto& PlayerInitData : InitCheckData.AllPlayersReadyCheckData)
	{
		if (InitCheckData.AllPlayersReadyCheckData.IsEmpty()) {UE_LOG(LogTemp, Warning, TEXT("Init_ AllPlayersInitData Is Empty"));return;}
		if (!PlayerInitData.IsReady) {UE_LOG(LogTemp, Warning, TEXT("Init_ Player Not Ready"));return;}
	}
	
	// 자원 스폰 
	if (InitCheckData.bResourceSpawnControlSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnControlSystem Not InitComplete")); return;}		// 월드 자원 스폰 컨트롤 시스템 체크
	if (InitCheckData.bResourceSpawnHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnHelperSystem Not InitComplete")); return;}			// 월드 자원 스폰 헬퍼 시스템 체크 
	if (InitCheckData.bResourceNodeBucketNodeSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceNodeBucketNodeSystem Not InitComplete")); return;}	// 월드 자원 리소스 버킷 노드 시스템 체크
	if (InitCheckData.bResourceSpawnLogicHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnLogicHelperSystem Not InitComplete"));return;} // 월드 자원 스폰 로직 헬퍼 시스템 체크

	UE_LOG(LogTemp, Warning, TEXT("InGameCycle_Init_Complete"));
	
	// 로드 호출 
	
}


#pragma endregion
//======================================================================================================================
#pragma region 로드
	
	//━━━━━━━━━━━━━━━━━━━━
	// 로드
	//━━━━━━━━━━━━━━━━━━━━	
	
void UTSInGameCycleControlSystem::LOAD_GamePlayerRegister(APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	
	FTSPlayerReadyData PlayerInitData;
	PlayerInitData.PlayerController = InPlayerController;
	PlayerInitData.IsReady = false;
	
	LOAD_CheckData.AllPlayersReadyCheckData.Add(PlayerInitData);
}

void UTSInGameCycleControlSystem::LOAD_GamePlayerComplete(APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	
	for (auto& PlayerInitData : LOAD_CheckData.AllPlayersReadyCheckData)
	{
		if (PlayerInitData.PlayerController != InPlayerController) continue;
		PlayerInitData.IsReady = true;
		CheckCanStart_PLAYING();
		return;
	}
}

void UTSInGameCycleControlSystem::CheckCanStart_PLAYING()
{
	// 플레이어 체크 
	for (auto& PlayerInitData : LOAD_CheckData.AllPlayersReadyCheckData)
	{
		if (LOAD_CheckData.AllPlayersReadyCheckData.IsEmpty()) {UE_LOG(LogTemp, Warning, TEXT("LOAD_ AllPlayersInitData Is Empty"));return;}
		if (!PlayerInitData.IsReady) {UE_LOG(LogTemp, Warning, TEXT("LOAD_Player Not Ready"));return;}
	}
	
	// 자원 스폰 
	if (LOAD_CheckData.bResourceSpawnControlSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnControlSystem Not LOAD_Complete")); return;}		// 월드 자원 스폰 컨트롤 시스템 체크
	if (LOAD_CheckData.bResourceSpawnHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnHelperSystem Not LOAD_Complete")); return;}			// 월드 자원 스폰 헬퍼 시스템 체크 
	if (LOAD_CheckData.bResourceNodeBucketNodeSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceNodeBucketNodeSystem Not LOAD_Complete")); return;}	// 월드 자원 리소스 버킷 노드 시스템 체크
	if (LOAD_CheckData.bResourceSpawnLogicHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnLogicHelperSystem Not LOAD_Complete"));return;} // 월드 자원 스폰 로직 헬퍼 시스템 체크

	UE_LOG(LogTemp, Warning, TEXT("InGameCycle_LOAD_Complete"));
}

#pragma endregion	
//======================================================================================================================
	