// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Enum/InventoryType/TSInventoryCompType.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Struct/InventorySlot/TSInventorySlotData.h"
#include "TSInventoryCompData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSInventoryCompData
{
	GENERATED_BODY()
	
	// 인벤토리 타입 (save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	ETSInventoryCompType InventoryType = ETSInventoryCompType::None;
	
	// 인벤토리의 등급 (Save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	int32 InventoryRarity = 0;
	
	// 슬롯 배열 (Save On)
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	TArray<FTSInventorySlotData> InventoryDataArray = {};
};
