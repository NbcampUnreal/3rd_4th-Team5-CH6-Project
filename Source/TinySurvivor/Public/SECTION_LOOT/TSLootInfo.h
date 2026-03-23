// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSLootInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSLootInfo
{
	GENERATED_BODY()
	
	// 나올 확률 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Loot")
	float LootProbability = 0.0f;
	
	// 나올 아이템 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Loot")
	int32 LootItemID = -1;
	
};
