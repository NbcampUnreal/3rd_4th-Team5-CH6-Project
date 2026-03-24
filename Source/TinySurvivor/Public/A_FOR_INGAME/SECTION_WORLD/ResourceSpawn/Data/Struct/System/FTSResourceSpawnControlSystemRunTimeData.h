// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplaytagContainer.h"
#include "FTSResourceSpawnControlSystemRunTimeData.generated.h"

class ATSResourceBucketActor;
class ATSResourceNodeActor;
struct FTSResourceNodeBucketRuntimeData;
struct FTSResourceNodeRunTimeData;
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
	
	// 스폰 버킷 바구니들 : 세이브 로드 전용으로 쓰자.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TArray<FTSResourceNodeBucketRuntimeData> NodeBucketDataArrayForSaveLoad;
	
	// 스폰 노드들 (바구니에 없는 것들) : 세이브 로드 전용으로 쓰자.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TArray<FTSResourceNodeRunTimeData> NodeDataArrayForSaveLoad;
	
	// 스폰 버킷들 (포인터)
	UPROPERTY() 
	TArray<TObjectPtr<ATSResourceBucketActor>> BucketPtrArray;
	
	// 스폰 도드들 (포인터)
	UPROPERTY() 
	TArray<TObjectPtr<ATSResourceNodeActor>> NodePtrArray;
};
