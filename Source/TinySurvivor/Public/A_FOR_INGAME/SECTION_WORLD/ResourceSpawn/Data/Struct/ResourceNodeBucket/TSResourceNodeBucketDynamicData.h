// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/ResourceNode/TSResourceNodeRuntimeData.h"
#include "TSResourceNodeBucketDynamicData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeBucketDynamicData
{
	GENERATED_BODY()
	
	// 버킷 내부의 노드들
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TArray<FTSResourceNodeRunTimeData> ResourceNodeArray;
	
};
