// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Library/TSNewInventoryInternalHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Library/TSNewInventorySlotInternalHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Library/TSNewItemCheckLibrary.h"

int32 UTSNewInventoryInternalHelperLibrary::FindEmptySlot_Lib(const UObject* InWorldContextObject, FTSNewInventoryCompData& InInventoryCompData, FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 인벤토리 배열 체크 
	if (InInventoryCompData.InventoryDataArray.IsEmpty()) return -1;
	
	// 인벤토리 데이터 배열 돌면서 체크
	for (int32 Index = 0; Index < InInventoryCompData.InventoryDataArray.Num(); ++Index)
	{
		// 인덱스 유효성 체크
		if (!InInventoryCompData.InventoryDataArray.IsValidIndex(Index)) continue;
		FTSNewInventorySlotData& SlotData = InInventoryCompData.InventoryDataArray[Index];
		
		// 이미 아이템이 있으면 건너뛰기
		if (UTSNewInventorySlotInternalHelperLibrary::IsSlotEmpty_Lib(SlotData) == false) continue;
		
		// 비었으면 인덱스 반환
		return Index;
	}
	
	return -1;
}

int32 UTSNewInventoryInternalHelperLibrary::FindStackSlot_Lib(const UObject* InWorldContextObject, FTSNewInventoryCompData& InInventoryCompData, FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 인벤토리 배열 체크 
	if (InInventoryCompData.InventoryDataArray.IsEmpty()) return -1;
	
	// 인벤토리 데이터 배열 돌면서 체크
	for (int32 Index = 0; Index < InInventoryCompData.InventoryDataArray.Num(); ++Index)
	{
		// 인덱스 유효성 체크
		if (!InInventoryCompData.InventoryDataArray.IsValidIndex(Index)) continue;
		FTSNewInventorySlotData& SlotData = InInventoryCompData.InventoryDataArray[Index];
		
		// 비었으면 건너뛰기 
		if (UTSNewInventorySlotInternalHelperLibrary::IsSlotEmpty_Lib(SlotData) == true) continue;
		
		// 같은 아이템인지 체크하기
		if (!UTSNewItemCheckLibrary::IsThisSameItem_Lib(SlotData.ItemData, InItemRuntimeData)) continue;
 		
		// 같은 아이템일 경우 인덱스 반환
		return Index;
	}
	
	return -1;
}

bool UTSNewInventoryInternalHelperLibrary::AddItemToEmptySlot_Lib(const UObject* InWorldContextObject, FTSNewInventorySlotData& InTargetSlot, FTSNewItemRuntimeData& InItemRuntimeData)
{
 	// 1. 슬롯의 데이터 통째로 복사
	InTargetSlot.ItemData = InItemRuntimeData;
	
	// 2. 들어온 데이터 레퍼런스로 접근해서 정적 데이터와 스택 수 없애기
	InItemRuntimeData.DynamicData.CurrentStack = -1;
	InItemRuntimeData.StaticDataID = -1;
	
	return true;
}

bool UTSNewInventoryInternalHelperLibrary::AddItemToStackSlot_Lib(const UObject* InWorldContextObject, FTSNewInventorySlotData& InTargetSlot, FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 남은 스택 수와 최대 스택 수 
	int32 RemainStack = 0;
	int32 MaxStackSize = UTSNewItemCheckLibrary::GetMaxStackSize_Lib(InWorldContextObject, InTargetSlot.ItemData);

	// 1. 스택 수 증가 후 
	InTargetSlot.ItemData.DynamicData.CurrentStack += InItemRuntimeData.DynamicData.CurrentStack;

	
	// 2 스택이 넘었는지 체크 -> 남은 스택 값 기억 -> 최대로 클램핑
	if (InTargetSlot.ItemData.DynamicData.CurrentStack >  MaxStackSize)
	{
		RemainStack = InTargetSlot.ItemData.DynamicData.CurrentStack - MaxStackSize;
		InTargetSlot.ItemData.DynamicData.CurrentStack = MaxStackSize;
	}
	
	// 3. 남은 스택이 0 이상이면 그만큼만 들어온 데이터 페런스로 접근해서 깎기
	if (RemainStack > 0)
	{
		InItemRuntimeData.DynamicData.CurrentStack = RemainStack;
	}
	// 3. 남은 스택이 없을 경우 들어온 데이터 레퍼런스로 접근해서 정적 데이터와 스택 수 없애기
	else
	{
		InItemRuntimeData.DynamicData.CurrentStack = 0;
		InItemRuntimeData.StaticDataID = -1;
	}
	
	return true;
}
