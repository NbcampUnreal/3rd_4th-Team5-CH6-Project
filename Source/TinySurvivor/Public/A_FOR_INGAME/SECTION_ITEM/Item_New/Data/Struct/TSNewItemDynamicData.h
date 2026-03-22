// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSNewItemDynamicData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSNewItemDynamicData
{
	GENERATED_BODY()
	
	// 현재 스택 수 
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	int32 CurrentStack = 0;
	
};
