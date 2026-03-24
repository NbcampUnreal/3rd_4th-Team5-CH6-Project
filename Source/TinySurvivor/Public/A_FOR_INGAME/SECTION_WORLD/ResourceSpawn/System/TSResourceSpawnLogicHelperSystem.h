// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/Data/Enum/TSInGameCycleMode.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "Subsystems/WorldSubsystem.h"
#include "TSResourceSpawnLogicHelperSystem.generated.h"

/**
 * 자원 소환 주기 관리 도움 시스템 : 자원 중추 시스템의 지시를 받아서 자원 소환에 대한 계산을 도와주는 시스템
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnLogicHelperSystem : public UWorldSubsystem
{
	
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:
	static UTSResourceSpawnLogicHelperSystem* Get(const UObject* InWorldContextObject);
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	UTSResourceSpawnLogicHelperSystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
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
	
};
