// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSItemDynamicData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSItemDynamicData
{
	GENERATED_BODY()
	
	// 현재 스택 수 
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	int32 CurrentStack = 0;
	
};
