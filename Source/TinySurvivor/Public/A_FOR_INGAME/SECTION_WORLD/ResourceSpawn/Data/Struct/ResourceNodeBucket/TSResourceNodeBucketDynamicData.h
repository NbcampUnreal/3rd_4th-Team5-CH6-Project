// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSResourceNodeBucketDynamicData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeBucketDynamicData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	FTransform BucketLastTransform;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	bool IsBucketIsDead = false;
};
