// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Library/TSNewItemCheckLibrary.h"
#include "A_FOR_COMMON/Library/System/TSSystemGetterLibrary.h"

bool UTSNewItemCheckLibrary::IsThisItemValid_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 아이템 데이터 매니저로부터 정적 데이터 포인터 유효한지 체크
	UTSNewItemDataSubSystem* NewItemDataSubSystem =  UTSSystemGetterLibrary::GetNewItemDataSubsystem(InWorldContextObject);
	if (!IsValid(NewItemDataSubSystem)) return false;
		
	FTSNewITemStaticData* FouNewITemStaticData = NewItemDataSubSystem->GetItemStaticData(InItemRuntimeData.StaticDataID);
	if (!FouNewITemStaticData) return false;
	
	// 다이나믹 데이터에 스택 수가 0 이하인지 체크 (넣을 게 없다는 소리)
	if (InItemRuntimeData.DynamicData.CurrentStack <= 0) return false;
	
	return true;
}

bool UTSNewItemCheckLibrary::IsThisITemCanStack_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 아이템 데이터 매니저로부터 정적 데이터 포인터 유효한지 체크
	UTSNewItemDataSubSystem* NewItemDataSubSystem =  UTSSystemGetterLibrary::GetNewItemDataSubsystem(InWorldContextObject);
	if (!IsValid(NewItemDataSubSystem)) return false;
	
	FTSNewITemStaticData* FouNewITemStaticData = NewItemDataSubSystem->GetItemStaticData(InItemRuntimeData.StaticDataID);
	if (!FouNewITemStaticData) return false;
	
	// 다이나믹 데이터에 스택 수가 0 이상인지 체크
	if (InItemRuntimeData.DynamicData.CurrentStack <= 0) return false;
	
	// 아이템이 스택이 가능한지 확인하기 (1보다 작거나 같으면 스택이 불가능하다고 판단)
	if (FouNewITemStaticData->ItemInventoryInfoTable.MaxStackSize <= 1) return false;
	
	return true;
}

int32 UTSNewItemCheckLibrary::GetMaxStackSize_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 아이템 데이터 매니저로부터 정적 데이터 포인터 유효한지 체크
	UTSNewItemDataSubSystem* NewItemDataSubSystem =  UTSSystemGetterLibrary::GetNewItemDataSubsystem(InWorldContextObject);
	if (!IsValid(NewItemDataSubSystem)) return -1;
	
	FTSNewITemStaticData* FouNewITemStaticData = NewItemDataSubSystem->GetItemStaticData(InItemRuntimeData.StaticDataID);
	if (!FouNewITemStaticData) return false;
	
	// 최대 스택 사이즈 반환
	return FouNewITemStaticData->ItemInventoryInfoTable.MaxStackSize;
}

bool UTSNewItemCheckLibrary::IsThisItemCanPlaceInThisInventory_Lib(const UObject* InWorldContextObject, FTSNewItemRuntimeData& InItemRuntimeData, FTSNewInventoryCompData& InInventoryData)
{
	// 아이템 데이터 매니저로부터 정적 데이터 포인터 유효한지 체크
	UTSNewItemDataSubSystem* NewItemDataSubSystem =  UTSSystemGetterLibrary::GetNewItemDataSubsystem(InWorldContextObject);
	if (!IsValid(NewItemDataSubSystem)) return false;
	
	FTSNewITemStaticData* FouNewITemStaticData = NewItemDataSubSystem->GetItemStaticData(InItemRuntimeData.StaticDataID);
	if (!FouNewITemStaticData) return false;
	
	// 하나라도 들어갈 수 있는 인벤토리 타입이 있는지 체크	(InInventoryData 의 인벤토리 타입이 있는지 체크)
	if (!FouNewITemStaticData->ItemInventoryInfoTable.MatchInventoryCompType.Contains(InInventoryData.InventoryType)) return false;
	
	return true;
}

bool UTSNewItemCheckLibrary::IsThisSameItem_Lib(FTSNewItemRuntimeData InFromSlotItemData, FTSNewItemRuntimeData InItemRuntimeData)
{
	// NOTE : 현재는 그냥 같은 ID 인지 체크하지만 추후 등급이나 강화 수치가 들어갈 경우 비교해야함.
	bool IsSameID = (InFromSlotItemData.StaticDataID == InItemRuntimeData.StaticDataID);
	
	// 모든 체크 같으면 true 반환
	return IsSameID;
}