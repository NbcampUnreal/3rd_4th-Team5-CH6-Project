// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "TSResourceNodeStaticData.generated.h"

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeStaticData
{
	GENERATED_BODY()
	
	// 어느 구역에 속해 있는가?
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	FGameplayTag RegionTag;
	
	// 몇 번째 스폰 노드인가?
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	int32 NodeIndex = 0;

	// 범용 또는 특정 자원만 들어올 수 있다.
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category="TS | Resoruce")
	bool bIsGeneralResource = true;
	
	// 들어올 수 있는 자원 타입
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource", meta=(EditCondition = "bIsGeneralResource == true"))
	FGameplayTagContainer ResourceTagContainer;

	// 들어올 수 있는 특정 자원 
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource", meta=(EditCondition = "bIsGeneralResource == false"))
	int32 ResourceID = -1;
	
};
