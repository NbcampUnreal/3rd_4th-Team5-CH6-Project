// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Data/Enum/InventoryType/TSNewInventoryCompType.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Data/Struct/InventorySlot/TSNewInventorySlotData.h"
#include "TSNewInventoryCompData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSNewInventoryCompData
{
	GENERATED_BODY()
	
	// 인벤토리 타입 (save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	ETSNewInventoryCompType InventoryType = ETSNewInventoryCompType::None;
	
	// 인벤토리의 등급 (Save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	int32 InventoryRarity = 0;
	
	// 슬롯 배열 (Save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	TArray<FTSNewInventorySlotData> InventoryDataArray = {};
};
