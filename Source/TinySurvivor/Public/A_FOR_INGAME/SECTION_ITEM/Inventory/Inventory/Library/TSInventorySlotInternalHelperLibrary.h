// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Struct/InventorySlot/TSInventorySlotData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSInventorySlotInternalHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSInventorySlotInternalHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 이 슬롯이 비었는가?
	static bool IsSlotEmpty_Lib(FTSInventorySlotData& InInventorySlotData);
	
};
