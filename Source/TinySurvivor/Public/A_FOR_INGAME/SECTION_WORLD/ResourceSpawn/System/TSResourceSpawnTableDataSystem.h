// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/TalbeStaticData/TSResourceSpawnTableStaticData.h"
#include "TSResourceSpawnTableDataSystem.generated.h"

/**
 * 영역마다 자원 스폰에 관한 테이블을 캐싱하는 데이터 매니저
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnTableDataSystem : public UGameInstanceSubsystem
{
	friend class UTSResourceSpawnControlSystem;
	
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:

	static UTSResourceSpawnTableDataSystem* Get(const UObject* InWorldContextObject);

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 내부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 내부 API
	//━━━━━━━━━━━━━━━━━━━━	

protected:
	// 데이터 테이블 캐싱 총괄 함수 
	void InitializeResourceSpawnTableStaticData_internal();
	
#pragma endregion
//======================================================================================================================	
#pragma region 시스템_데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 시스템_데이터
	//━━━━━━━━━━━━━━━━━━━━
	
	// 데이터 맵 (조회용 캐싱)
	TMap<FGameplayTag, FTSResourceSpawnTableStaticData*> ResourceSpawnLogicDataMap;
	
	// 데이터 배열 (최적화용 캐싱)
	TArray<FTSResourceSpawnTableStaticData> ResourceSpawnLogicDataArray;
	
	bool bWantPrintDeBugLog = false;	
	
#pragma endregion
//======================================================================================================================	
};
