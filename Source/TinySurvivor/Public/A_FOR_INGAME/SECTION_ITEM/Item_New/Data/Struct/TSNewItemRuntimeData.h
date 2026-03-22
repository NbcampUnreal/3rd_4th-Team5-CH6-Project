// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSNewItemDynamicData.h"
#include "TSNewItemRuntimeData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSNewItemRuntimeData
{
	GENERATED_BODY()
	
	// 동적 데이터 (SaveGame)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	FTSNewItemDynamicData DynamicData;
	
	// 정적 데이터 (SaveGame)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Item")
	int32 StaticDataID = -1;
	
};
