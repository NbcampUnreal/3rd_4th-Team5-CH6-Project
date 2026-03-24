// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Struct/TSResourceStaticData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TSResourceDataSystem.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceDataSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 게터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	
public:

	static UTSResourceDataSystem* Get(const UObject* InWorldContextObject);
	
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
#pragma region 외부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 외부 API
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	FTSResourceStaticData* GetResourceStaticData(int32 InResourceID);

#pragma endregion
//======================================================================================================================	
#pragma region 내부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 내부 API
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	// 데이터 테이블 캐싱 총괄 함수 
	void InitializeResourceStaticData_internal();
	
#pragma endregion
//======================================================================================================================	
#pragma region 시스템_데이터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 시스템_데이터
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	// 데이터 맵 (조회용 캐싱)
	TMap<int32, FTSResourceStaticData*> ResourceStaticDataMap;
	
	// 데이터 배열 (최적화용 캐싱)
	TArray<FTSResourceStaticData> ResourceStaticDataArray;
	
	bool bWantPrintDeBugLog = false;
	
#pragma endregion
//======================================================================================================================	
	
	
	
	
	
	
	
	
	
	
	
	
};
