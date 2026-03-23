// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSResourceDynamicData.generated.h"

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceDynamicData
{
	GENERATED_BODY()
	
	// 현재 남은 수량 
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	int32 CurrentAmount = 0;
	
};
