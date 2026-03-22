// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Struct/InventoryCompData/TSInventoryCompData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSItemRuntimeData.h"
#include "TSItemCheckLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSItemCheckLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 아이템이 유효한가?
	static bool IsThisItemValid_Lib(const UObject* InWorldContextObject, FTSItemRuntimeData& InItemRuntimeData);
	
	// 스택이 가능한 아이템인가?
	static bool IsThisITemCanStack_Lib(const UObject* InWorldContextObject, FTSItemRuntimeData& InItemRuntimeData);

	// 최대 스택이 몇인가?
	static int32 GetMaxStackSize_Lib(const UObject* InWorldContextObject, FTSItemRuntimeData& InItemRuntimeData);

	// 이 아이템은 인벤토리에 들엉올 수 있는가? 
	static bool IsThisItemCanPlaceInThisInventory_Lib(const UObject* InWorldContextObject, FTSItemRuntimeData& InItemRuntimeData, FTSInventoryCompData& InInventoryData);
	
	// 같은 아이템인가?
	static bool IsThisSameItem_Lib(FTSItemRuntimeData InFromSlotItemData, FTSItemRuntimeData InItemRuntimeData);
};
