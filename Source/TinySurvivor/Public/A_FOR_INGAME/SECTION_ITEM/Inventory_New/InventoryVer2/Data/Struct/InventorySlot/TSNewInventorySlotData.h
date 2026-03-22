// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Data/Enum/InventorySlotType/TSNewInventorySlotType.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Data/Struct/TSNewItemRuntimeData.h"
#include "TSNewInventorySlotData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSNewInventorySlotData
{
	GENERATED_BODY()
	
	// 인벤토리 슬롯 타입 (save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	ETSNewInventorySlotType InventorySlotType = ETSNewInventorySlotType::None;
	
	// 아이템 데이터 (save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	FTSNewItemRuntimeData ItemData;
	
};
