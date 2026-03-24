// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/System/FTSResourceSpawnControlSystemRunTimeData.h"
#include "Subsystems/WorldSubsystem.h"
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
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | ResourceSpawn")
	TMap<FGameplayTag, FTSResourceSpawnControlSystemPerRegionRunTimeData> ResourceSpawnControlSystemPerRegionRunTimeData;
	
#pragma endregion
//======================================================================================================================	
	
		
	
	
	
};
