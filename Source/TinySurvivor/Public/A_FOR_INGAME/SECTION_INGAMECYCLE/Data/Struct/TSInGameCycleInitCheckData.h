// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSPlayerReadyData.h"
#include "TSInGameCycleInitCheckData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSInGameCycleReadyCheckData
{
	GENERATED_BODY()
	
	// 플레이어들 준비 완료 
	TArray<FTSPlayerReadyData>	AllPlayersReadyCheckData;
	
	// 자원 스폰 
	bool bResourceSpawnControlSystemReadyComplete = false;		// 월드 자원 스폰 컨트롤 시스템 준비 완료 
	bool bResourceSpawnHelperSystemReadyComplete = false;		// 월드 자원 스폰 헬퍼 시스템 준비 완료
	bool bResourceNodeBucketNodeSystemReadyComplete = false;	// 월드 자원 리소스 버킷 노드 시스템 준비 완료
	bool bResourceSpawnLogicHelperSystemReadyComplete = false;	// 월드 자원 스폰 로직 헬퍼 시스템 준비 완료 
	
};
