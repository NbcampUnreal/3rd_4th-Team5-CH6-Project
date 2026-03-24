// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/ResourceNodeBucket/TSResourceNodeBucketRuntimeData.h"
#include "FTSResourceSpawnControlSystemRunTimeData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceSpawnControlSystemPerRegionRunTimeData
{
	GENERATED_BODY()
	
	// 어느 구역인가?
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	FGameplayTag RegionTag;
	
	// 스폰 버킷 바구니들
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TArray<FTSResourceNodeBucketRuntimeData> NodeBucketDataArray;
	
	// 스폰 노드들 (바구니에 없는 것들)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TArray<FTSResourceNodeRunTimeData> NodeDataArray;
	
};
