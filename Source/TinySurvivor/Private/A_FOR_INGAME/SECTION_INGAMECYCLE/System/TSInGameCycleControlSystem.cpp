// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_INGAMECYCLE/System/TSInGameCycleControlSystem.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/System/TSSaveLoadSystem.h"

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
		CheckCanStart_NEW_OR_LOAD();
		return;
	}
}

void UTSInGameCycleControlSystem::CheckCanStart_NEW_OR_LOAD()
{
	// 플레이어 체크 
	for (auto& PlayerInitData : InitCheckData.AllPlayersReadyCheckData)
	{
		if (InitCheckData.AllPlayersReadyCheckData.IsEmpty()) return;
		if (!PlayerInitData.IsReady) return;
	}
	
	// 자원 스폰 
	if (InitCheckData.bResourceSpawnControlSystemReadyComplete == false) return;		// 월드 자원 스폰 컨트롤 시스템 체크
	if (InitCheckData.bResourceSpawnHelperSystemReadyComplete == false)	return;			// 월드 자원 스폰 헬퍼 시스템 체크 
	if (InitCheckData.bResourceNodeBucketNodeSystemReadyComplete == false) return;		// 월드 자원 리소스 버킷 노드 시스템 체크
	if (InitCheckData.bResourceSpawnLogicHelperSystemReadyComplete == false) return;	// 월드 자원 스폰 로직 헬퍼 시스템 체크

	//[향후 init 추가]
	//[향후 init 추가]
	//[향후 init 추가]

	// 모든 init 이후 로직 
	UTSSaveLoadSystem* SaveLoadSystem = UTSSaveLoadSystem::Get(this);
	if (!IsValid(SaveLoadSystem)) return;
	
	// 세이브 시스템이 현재 Master 데이터의 어떤 값이 들어가 있는지 확인 : 만약 -1 이면 로드에 실패 -> 세이브 데이터가 망가졌거나 새로운 게임이라는 뜻임. -> 그냥 새 게임 시작하게 만들면 됨.
	if (SaveLoadSystem->GetLastRequestedLoadSaveID() == -1)
	{
		InGameCycleDelegate.Broadcast(ETSInGameCycleMode::NEW, *SaveLoadSystem->RequestCurrentLoadedDataPtr());
		return;
	}
	
	// -1 이 아니면 세이브 데이터가 있다는 뜻 -> 로드 호출 
	InGameCycleDelegate.Broadcast(ETSInGameCycleMode::LOAD, *SaveLoadSystem->RequestCurrentLoadedDataPtr());
}

#pragma endregion
//======================================================================================================================
#pragma region 뉴
	
	//━━━━━━━━━━━━━━━━━━━━
	// 뉴
	//━━━━━━━━━━━━━━━━━━━━		

void UTSInGameCycleControlSystem::NEW_GamePlayerRegister(APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	
	FTSPlayerReadyData PlayerInitData;
	PlayerInitData.PlayerController = InPlayerController;
	PlayerInitData.IsReady = false;
	
	NEW_CheckData.AllPlayersReadyCheckData.Add(PlayerInitData);
}

void UTSInGameCycleControlSystem::NEW_GamePlayerComplete(APlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	
	for (auto& PlayerInitData : NEW_CheckData.AllPlayersReadyCheckData)
	{
		if (PlayerInitData.PlayerController != InPlayerController) continue;
		PlayerInitData.IsReady = true;
		CheckCanStart_PLAYING_NEW();
		return;
	}
}

void UTSInGameCycleControlSystem::CheckCanStart_PLAYING_NEW()
{
	UE_LOG(LogTemp, Log, TEXT("================================================================="));

	bool bIsAllPlayerReady = true;
	
	// 플레이어 체크 
	if (NEW_CheckData.AllPlayersReadyCheckData.IsEmpty()) {UE_LOG(LogTemp, Warning, TEXT("NEW_ AllPlayersInitData Is Empty"));bIsAllPlayerReady = false;}
	for (auto& PlayerInitData : NEW_CheckData.AllPlayersReadyCheckData)
	{
		if (!PlayerInitData.IsReady) {UE_LOG(LogTemp, Warning, TEXT("NEW_Player Not Ready"));bIsAllPlayerReady = false;}
		else {UE_LOG(LogTemp, Log, TEXT("NEW_Player Ready"));}
	}
	
	// 자원 스폰 
	if (NEW_CheckData.bResourceSpawnControlSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnControlSystem Not NEW_Complete")); bIsAllPlayerReady = false;;}		 // 월드 자원 스폰 컨트롤 시스템 체크
	else {UE_LOG(LogTemp, Log, TEXT("ResourceSpawnControlSystem NEW_Complete"));}
	if (NEW_CheckData.bResourceSpawnHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnHelperSystem Not NEW_Complete")); bIsAllPlayerReady = false;;}			 // 월드 자원 스폰 헬퍼 시스템 체크 
	else {UE_LOG(LogTemp, Log, TEXT("ResourceSpawnHelperSystem NEW_Complete"));}
	if (NEW_CheckData.bResourceNodeBucketNodeSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceNodeBucketNodeSystem Not NEW_Complete")); bIsAllPlayerReady = false;;}	 // 월드 자원 리소스 버킷 노드 시스템 체크
	else {UE_LOG(LogTemp, Log, TEXT("ResourceNodeBucketNodeSystem NEW_Complete"));}
	if (NEW_CheckData.bResourceSpawnLogicHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnLogicHelperSystem Not NEW_Complete"));bIsAllPlayerReady = false;;} // 월드 자원 스폰 로직 헬퍼 시스템 체크
	else {UE_LOG(LogTemp, Log, TEXT("ResourceSpawnLogicHelperSystem NEW_Complete"));}	
	
	if (bIsAllPlayerReady == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameCycle_NEW Not Ready"));
		UE_LOG(LogTemp, Log, TEXT("================================================================="));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("InGameCycle_NEW_Complete"));
	UE_LOG(LogTemp, Log, TEXT("================================================================="));
	
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
		CheckCanStart_PLAYING_LOAD();
		return;
	}
}

void UTSInGameCycleControlSystem::CheckCanStart_PLAYING_LOAD()
{
	UE_LOG(LogTemp, Log, TEXT("================================================================="));

	bool bIsAllPlayerReady = true;
	
	// 플레이어 체크 
	if (LOAD_CheckData.AllPlayersReadyCheckData.IsEmpty()) {UE_LOG(LogTemp, Warning, TEXT("LOAD_ AllPlayersInitData Is Empty")); bIsAllPlayerReady = false;}
	for (auto& PlayerInitData : LOAD_CheckData.AllPlayersReadyCheckData)
	{
		if (!PlayerInitData.IsReady) {UE_LOG(LogTemp, Log, TEXT("LOAD_Player Not Ready")); bIsAllPlayerReady = false;}
		else {UE_LOG(LogTemp, Log, TEXT("LOAD_Player Ready"));}
	}
	
	// 자원 스폰 
	if (LOAD_CheckData.bResourceSpawnControlSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnControlSystem Not LOAD_Complete")); bIsAllPlayerReady = false;}			// 월드 자원 스폰 컨트롤 시스템 체크
	else {UE_LOG(LogTemp, Log, TEXT("ResourceSpawnControlSystem LOAD_Complete"));}
	if (LOAD_CheckData.bResourceSpawnHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnHelperSystem Not LOAD_Complete")); bIsAllPlayerReady = false;}			// 월드 자원 스폰 헬퍼 시스템 체크 
	else {UE_LOG(LogTemp, Log, TEXT("ResourceSpawnHelperSystem LOAD_Complete"));}
	if (LOAD_CheckData.bResourceNodeBucketNodeSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceNodeBucketNodeSystem Not LOAD_Complete")); bIsAllPlayerReady = false;}		// 월드 자원 리소스 버킷 노드 시스템 체크
	else {UE_LOG(LogTemp, Log, TEXT("ResourceNodeBucketNodeSystem LOAD_Complete"));}
	if (LOAD_CheckData.bResourceSpawnLogicHelperSystemReadyComplete == false) {UE_LOG(LogTemp, Warning, TEXT("ResourceSpawnLogicHelperSystem Not LOAD_Complete"));bIsAllPlayerReady = false;}	// 월드 자원 스폰 로직 헬퍼 시스템 체크
	else {UE_LOG(LogTemp, Log, TEXT("ResourceSpawnLogicHelperSystem LOAD_Complete"));}
	
	if (bIsAllPlayerReady == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameCycle_LOAD Not Ready"));
		UE_LOG(LogTemp, Log, TEXT("================================================================="));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("InGameCycle_LOAD_Complete"));
	UE_LOG(LogTemp, Log, TEXT("================================================================="));

}

#pragma endregion	
//======================================================================================================================
	