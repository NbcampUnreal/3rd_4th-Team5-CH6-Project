// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Library/TSNewInventorySlotInternalHelperLibrary.h"

bool UTSNewInventorySlotInternalHelperLibrary::IsSlotEmpty_Lib(FTSNewInventorySlotData& InInventorySlotData)
{
	// 동적 데이터 스택 체크
	bool bIsCurrentStackEmpty = (InInventorySlotData.ItemData.DynamicData.CurrentStack <= 0);
	
	// 정적 데이터 ID 체크 
	bool bIsStaticDataIDEmpty = (InInventorySlotData.ItemData.StaticDataID <= 0);
	
	// 모든 체크가 확인 되면 true
	return (bIsCurrentStackEmpty && bIsStaticDataIDEmpty);
}
