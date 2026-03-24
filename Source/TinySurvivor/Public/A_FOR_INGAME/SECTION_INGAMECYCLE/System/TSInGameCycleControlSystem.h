// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/Data/Enum/TSInGameCycleMode.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/Data/Struct/TSInGameCycleInitCheckData.h"
#include "TSInGameCycleControlSystem.generated.h"

// 인 게임 사이클에 따른 호출 델리게이트  (반드시 init 초기화 단계 이후 호출)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInGameCycleDelegate, ETSInGameCycleMode, InGamecycleMode, FTSSaveMasterData&, Data);

/**
 * 인게임의 사이클을 책임지는 시스템 
 * 초기화, 세이브 로드 데이터 덮어씌우기, 오토 세이브 요청 시 세이브 데이터 만들어서 넘겨주기 등등. 
 */
UCLASS()
class TINYSURVIVOR_API UTSInGameCycleControlSystem : public UWorldSubsystem
{
	GENERATED_BODY()
//======================================================================================================================	
#pragma region 델리게이트
	
	//━━━━━━━━━━━━━━━━━━━━
	// 델리게이트
	//━━━━━━━━━━━━━━━━━━━━	
public:
	UPROPERTY(BlueprintAssignable) FInGameCycleDelegate InGameCycleDelegate;
	
#pragma endregion
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:

	static UTSInGameCycleControlSystem* Get(const UObject* InWorldContextObject);

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	UTSInGameCycleControlSystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
#pragma endregion
//======================================================================================================================
#pragma region 초기화
	
	//━━━━━━━━━━━━━━━━━━━━
	// 초기화
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	// 초기화 준비 등록, 초기화 완료 알림 함수 (플레이어)
	void InitInGamePlayerRegister(APlayerController* InPlayerController);
	void InitInGamePlayerComplete(APlayerController* InPlayerController);
	
	// 자원 스폰
	FORCEINLINE void InitWorldResourceControlSystemComplete() { InitCheckData.bResourceSpawnControlSystemReadyComplete = true; CheckCanStart_LOAD(); };				// 월드 자원 스폰 컨트롤 시스템 체크
	FORCEINLINE void InitWorldResourceSpawnHelperSystemComplete() { InitCheckData.bResourceSpawnHelperSystemReadyComplete = true; CheckCanStart_LOAD(); };			// 월드 자원 스폰 헬퍼 시스템 체크 
	FORCEINLINE void InitWorldResourceNodeBucketNodeSystemComplete() { InitCheckData.bResourceNodeBucketNodeSystemReadyComplete = true; CheckCanStart_LOAD(); };	// 월드 자원 리소스 버킷 노드 시스템 체크
	FORCEINLINE void InitWorldResourceLogicHelperSystemComplete() { InitCheckData.bResourceSpawnLogicHelperSystemReadyComplete = true; CheckCanStart_LOAD(); }		// 월드 자원 스폰 로직 헬퍼 시스템 체크 
	
protected:
	// 초기화가 전부 되었는지 체크하기 위한 함수 	
	void CheckCanStart_LOAD();
	
	// 초기화가 전부 되었는지 체크하기 위한 데이터 
	FTSInGameCycleReadyCheckData InitCheckData;
	
#pragma endregion
//======================================================================================================================
#pragma region 로드
	
	//━━━━━━━━━━━━━━━━━━━━
	// 로드
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	// 초기화 준비 등록, 초기화 완료 알림 함수 (플레이어)
	void LOAD_GamePlayerRegister(APlayerController* InPlayerController);
	void LOAD_GamePlayerComplete(APlayerController* InPlayerController);
	
	// 자원 스폰
	FORCEINLINE void LOAD_WorldResourceControlSystemComplete() { LOAD_CheckData.bResourceSpawnControlSystemReadyComplete = true; CheckCanStart_PLAYING(); };				// 월드 자원 스폰 컨트롤 시스템 체크
	FORCEINLINE void LOAD_WorldResourceSpawnHelperSystemComplete() { LOAD_CheckData.bResourceSpawnHelperSystemReadyComplete = true; CheckCanStart_PLAYING(); };			// 월드 자원 스폰 헬퍼 시스템 체크 
	FORCEINLINE void LOAD_WorldResourceNodeBucketNodeSystemComplete() { LOAD_CheckData.bResourceNodeBucketNodeSystemReadyComplete = true; CheckCanStart_PLAYING(); };	// 월드 자원 리소스 버킷 노드 시스템 체크
	FORCEINLINE void LOAD_WorldResourceLogicHelperSystemComplete() { LOAD_CheckData.bResourceSpawnLogicHelperSystemReadyComplete = true; CheckCanStart_PLAYING(); }		// 월드 자원 스폰 로직 헬퍼 시스템 체크 
	
	
protected:
	// 로드가 전부 되었는지 체크하기 위한 함수 	
	void CheckCanStart_PLAYING();
	
	// 초기화가 전부 되었는지 체크하기 위한 데이터 
	FTSInGameCycleReadyCheckData LOAD_CheckData;
	
	
#pragma endregion	
//======================================================================================================================
	
};
