// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Data/Struct/InventoryCompData/TSNewInventoryCompData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Data/Struct/TSNewItemRuntimeData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSNewItemCheckLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSNewItemCheckLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 아이템이 유효한가?
	static bool IsThisItemValid_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData);
	
	// 스택이 가능한 아이템인가?
	static bool IsThisITemCanStack_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData);

	// 최대 스택이 몇인가?
	static int32 GetMaxStackSize_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData);

	// 이 아이템은 인벤토리에 들엉올 수 있는가? 
	static bool IsThisItemCanPlaceInThisInventory_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData, FTSNewInventoryCompData& InInventoryData);
	
	// 같은 아이템인가?
	static bool IsThisSameItem_Lib(FTSNewItemRuntimeData InFromSlotItemData, FTSNewItemRuntimeData InItemRuntimeData);
};
