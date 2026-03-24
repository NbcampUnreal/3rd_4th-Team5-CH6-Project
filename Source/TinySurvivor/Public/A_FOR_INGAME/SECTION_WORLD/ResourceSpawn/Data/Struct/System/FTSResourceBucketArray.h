// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "FTSResourceBucketArray.generated.h"

class ATSResourceBucketActor;
/**
 *  TSResourceNodeAndBucketGetHelperSystem 에서 쓰는 구조체 
 */
USTRUCT(BlueprintType)
struct FTSResourceBucketArray
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	TArray<TObjectPtr<ATSResourceBucketActor>> ResourceBucketArray;
};