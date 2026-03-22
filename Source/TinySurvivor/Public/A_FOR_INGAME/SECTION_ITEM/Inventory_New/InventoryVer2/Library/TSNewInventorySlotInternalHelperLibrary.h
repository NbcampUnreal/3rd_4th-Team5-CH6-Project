// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Data/Struct/InventorySlot/TSNewInventorySlotData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSNewInventorySlotInternalHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSNewInventorySlotInternalHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 이 슬롯이 비었는가?
	static bool IsSlotEmpty_Lib(FTSNewInventorySlotData& InInventorySlotData);
	
};
