// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/Data/Enum/TSInGameCycleMode.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceSpawnControlSystemRunTimeData.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/TalbeStaticData/TSResourceSpawnTableStaticData.h"
#include "Subsystems/WorldSubsystem.h"
#include "TSResourceSpawnCalHelperSystem.generated.h"

/**
 * 자원 소환 담당 계산 서브 매니저 : 자원 스폰 컨트롤 시스템에게 지시를 받아서 특정 자원을 소환을 얼만큼 하는 계산을 담당하는 역할  
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnCalHelperSystem : public UTickableWorldSubsystem
{
	friend class UTSResourceSpawnControlSystem;
	
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:
	static UTSResourceSpawnCalHelperSystem* Get(const UObject* InWorldContextObject);
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	UTSResourceSpawnCalHelperSystem();
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
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
#pragma region 스폰
	//━━━━━━━━━━━━━━━━━━━━
	// 스폰
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	// 자원 스폰 컨트롤 시스템이 데이터를 넘겨줌에 따라 데이터가 허용하는 만큼 스폰을 실시하는 함수 (새 게임일 때만 씀)
	void NEW_StartSpawnResourceBasedOnTableData_internal(FTSResourceSpawnControlSystemPerRegionRunTimeData& InNodeAndBucketPtrData, FTSResourceSpawnTableStaticData& InSpawnTableData);

#pragma endregion
//======================================================================================================================	
};
