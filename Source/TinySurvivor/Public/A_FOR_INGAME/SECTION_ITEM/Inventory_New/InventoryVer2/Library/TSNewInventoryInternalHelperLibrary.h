// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Data/Struct/TSNewItemRuntimeData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Data/Struct/InventoryCompData/TSNewInventoryCompData.h"
#include "TSNewInventoryInternalHelperLibrary.generated.h"

/**
 * 인벤토리 내부 사용 전용 라이브러리 
 * :: 인벤토리 클래스에 너무 많은 API가 들어있는 것을 방지하기 위함.
 */
UCLASS()
class TINYSURVIVOR_API UTSNewInventoryInternalHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	// 아무 빈 슬롯 찾기 
	static int32 FindEmptySlot_Lib(const UObject* InWorldContextObject, FTSNewInventoryCompData& InInventoryCompData, FTSNewItemRuntimeData& InItemRuntimeData);
	
	// 들어갈 수 있는 스택 슬롯 찾기 
	static int32 FindStackSlot_Lib(const UObject* InWorldContextObject, FTSNewInventoryCompData& InInventoryCompData, FTSNewItemRuntimeData& InItemRuntimeData);
	
	// 빈 슬롯에 넣기
	static bool AddItemToEmptySlot_Lib(const UObject* InWorldContextObject, FTSNewInventorySlotData& InTargetSlot, FTSNewItemRuntimeData& InItemRuntimeData);
	
	// 스택 슬롯에 넣기 
	static bool AddItemToStackSlot_Lib(const UObject* InWorldContextObject, FTSNewInventorySlotData& InTargetSlot, FTSNewItemRuntimeData& InItemRuntimeData);
	
};
