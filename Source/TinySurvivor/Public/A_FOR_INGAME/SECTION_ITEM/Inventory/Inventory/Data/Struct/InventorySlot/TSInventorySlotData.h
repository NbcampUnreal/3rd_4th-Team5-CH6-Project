// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Enum/InventorySlotType/TSInventorySlotType.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSItemRuntimeData.h"
#include "TSInventorySlotData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSInventorySlotData
{
	GENERATED_BODY()
	
	// 인벤토리 슬롯 타입 (save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	ETSInventorySlotType InventorySlotType = ETSInventorySlotType::None;
	
	// 아이템 데이터 (save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	FTSItemRuntimeData ItemData;
	
};
