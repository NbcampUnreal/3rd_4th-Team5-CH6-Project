// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/Data/Enum/TSInGameCycleMode.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "Subsystems/WorldSubsystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceSpawnControlSystemRunTimeData.h"
#include "TSResourceSpawnControlSystem.generated.h"

/**
 * 자원 스폰에 관한 중추를 담당하는 상위 시스템
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnControlSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:
	static UTSResourceSpawnControlSystem* Get(const UObject* InWorldContextObject);
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	UTSResourceSpawnControlSystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 인게임_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인게임_사이클 (자원 초기화 이후 로직)
	//━━━━━━━━━━━━━━━━━━━━	
		
protected:
	// 인 게임 사이클 델리게이트 바인딩 함수 
	UFUNCTION()
	void OnReceivedInGameCycleDelegate_internal(ETSInGameCycleMode InGameCycleMode, FTSSaveMasterData& InData);
	
	// new 호출 시
	void CallWhenNewModeIsCalled_internal();
	
	// load 호출 시
	void CallWhenLoadModeIsCalled_internal(FTSSaveMasterData& InData);
	
	// play 호출 시 
	void CallWhenPlayModeIsCalled_internal();
	
#pragma endregion
//======================================================================================================================	
#pragma region 자원_초기화
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	// 자원 스폰을 위한 초기 동작을 지시
	void InitResourceSpawnNodeAndBuckets();
	
public:
	// 자원 스폰 초기 동작이 완료되었다고 알림 받음 (UTSResourceNodeAndBucketGetHelperSystem가 자길 할 일 하고 호출)
	void InitResourceSpawnNodeAndBucketsCompleteReceive();
	
#pragma endregion
//======================================================================================================================	
#pragma region 자원_관리
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	
#pragma endregion
//======================================================================================================================	
#pragma region 데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 데이터
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	
	// key 영역 태그, value 영역 내 버킷과 노드 모음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | ResourceSpawn")
	TMap<FGameplayTag, FTSResourceSpawnControlSystemPerRegionRunTimeData> ResourceSpawnControlSystemPerRegionRunTimeData;
	
#pragma endregion
//======================================================================================================================	
	
		
	
	
	
};
