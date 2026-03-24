// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TSResourceNodeBucketStaticData.generated.h"

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeBucketStaticData
{
	GENERATED_BODY()
	
	// 어느 구역에 속해 있는가?
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	FGameplayTag RegionTag;
	
	// 무슨 바구니인가?
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	FGameplayTag BucketTag;
	
	// 몇 번째 바구니인가?
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	int32 NodeIndex = 0;
	
};
