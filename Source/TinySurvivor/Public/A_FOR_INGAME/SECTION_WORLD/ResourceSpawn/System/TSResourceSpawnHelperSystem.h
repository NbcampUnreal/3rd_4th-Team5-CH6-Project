// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_INGAMECYCLE/Data/Enum/TSInGameCycleMode.h"
#include "A_FOR_INGAME/SECTION_SAVELOAD/Data/Struct/TSSaveMasterData.h"
#include "Subsystems/WorldSubsystem.h"
#include "TSResourceSpawnHelperSystem.generated.h"

/**
 * 자원 소환 담당 서브 매니저 : 자원 스폰 컨트롤 시스템에게 지시를 받아서 특정 자원을 소환하는 역할  
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnHelperSystem : public UTickableWorldSubsystem
{
	
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:
	static UTSResourceSpawnHelperSystem* Get(const UObject* InWorldContextObject);
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	UTSResourceSpawnHelperSystem();
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
};
