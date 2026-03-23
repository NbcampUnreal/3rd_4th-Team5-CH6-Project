// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SECTION_LOOT/TSLootInfo.h"
#include "TSResourceLootInfoTable.generated.h"

/**
 * 
 */
USTRUCT(blueprinttype)
struct TINYSURVIVOR_API FTSResourceLootInfoTable : public FTableRowBase
{
	GENERATED_BODY()
	
	// 자원 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 ResourceID = -1;
	
	// 루팅 가능한 아이템 목록 
	TArray<FTSLootInfo> LootInfoArray;
	
};
