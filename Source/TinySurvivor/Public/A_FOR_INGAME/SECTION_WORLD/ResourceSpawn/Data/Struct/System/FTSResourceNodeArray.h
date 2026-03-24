// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "FTSResourceNodeArray.generated.h"

class ATSResourceNodeActor;

/**
 * TSResourceNodeAndBucketGetHelperSystem 에서 쓰는 구조체 
 * UTSResourceSpawnCalHelperSystem 에서도 씀.
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeArray
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TArray<TObjectPtr<ATSResourceNodeActor>> ResourceNodeArray;
};
