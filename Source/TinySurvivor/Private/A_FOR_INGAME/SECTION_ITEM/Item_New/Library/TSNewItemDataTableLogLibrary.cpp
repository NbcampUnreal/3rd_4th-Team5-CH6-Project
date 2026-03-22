// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Library/TSNewItemDataTableLogLibrary.h"

DEFINE_LOG_CATEGORY(NewItemDataTableLog);

void UTSNewItemDataTableLogLibrary::LogStaticItemData_Lib(FTSNewITemStaticData* InITemStaticData)
{
	if (!InITemStaticData) return;
	
	UE_LOG(NewItemDataTableLog, Log, TEXT("==== Starting Static Item Data Log ===="));
	LogBaseInfoData_Lib(&InITemStaticData->ItemBaseInfoTable);
	LogInventoryInfoData_Lib(&InITemStaticData->ItemInventoryInfoTable);
	LogUIInfoData_Lib(&InITemStaticData->ItemUIInfoTable);
	UE_LOG(NewItemDataTableLog, Log, TEXT("==== End of Static Item Data Log ===="));
}

void UTSNewItemDataTableLogLibrary::LogBaseInfoData_Lib(FTSNewItemBaseInfoTable* InItemBaseInfoTable)
{
	if (!InItemBaseInfoTable) return;
	
	UE_LOG(NewItemDataTableLog, Log, TEXT("[Base Info] ItemID: %d"), InItemBaseInfoTable->ItemID);
	UE_LOG(NewItemDataTableLog, Log, TEXT("[Base Info] ItemClass: %s"), *InItemBaseInfoTable->ItemClass.ToString());
	UE_LOG(NewItemDataTableLog, Log, TEXT("[Base Info] ItemDataAsset: %s"), *InItemBaseInfoTable->ItemDataAsset.ToString());
}

void UTSNewItemDataTableLogLibrary::LogInventoryInfoData_Lib(FTSNewItemInventoryInfoTable* InItemInventoryInfoTable)
{
	if (!InItemInventoryInfoTable) return;
	
	UE_LOG(NewItemDataTableLog, Log, TEXT("[Inventory Info] ItemID: %d"), InItemInventoryInfoTable->ItemID);
	UE_LOG(NewItemDataTableLog, Log, TEXT("[Inventory Info] MaxStackSize: %d"), InItemInventoryInfoTable->MaxStackSize);

	// Enum Array Log
	for (const ETSNewInventoryCompType& CompType : InItemInventoryInfoTable->MatchInventoryCompType)
	{
		const UEnum* EnumPtr = StaticEnum<ETSNewInventoryCompType>();
		FString TypeName = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(CompType)) : TEXT("Invalid");
		UE_LOG(NewItemDataTableLog, Log, TEXT("[Inventory Info] MatchInventoryCompType: %s"), *TypeName);
	}

	// Class/Object Array Log
	for (const ETSNewInventorySlotType& SlotType : InItemInventoryInfoTable->MatchInventorySlotType)
	{
		const UEnum* EnumPtr = StaticEnum<ETSNewInventorySlotType>();
		FString TypeName = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(SlotType)) : TEXT("Invalid");
		UE_LOG(NewItemDataTableLog, Log, TEXT("[Inventory Info] MatchInventorySlotType: %s"), *TypeName);
	}
}

void UTSNewItemDataTableLogLibrary::LogUIInfoData_Lib(FTSNewItemUIInfoTable* InItemUIInfoTable)
{
	if (!InItemUIInfoTable) return;
	
	UE_LOG(NewItemDataTableLog, Log, TEXT("[UI Info] ItemID: %d"), InItemUIInfoTable->ItemID);
	UE_LOG(NewItemDataTableLog, Log, TEXT("[UI Info] ItemName: %s"), *InItemUIInfoTable->ItemName.ToString());
	UE_LOG(NewItemDataTableLog, Log, TEXT("[UI Info] ItemDescription: %s"), *InItemUIInfoTable->ItemDescription.ToString());
	UE_LOG(NewItemDataTableLog, Log, TEXT("[UI Info] ItemIcon Path: %s"), *InItemUIInfoTable->ItemIcon.ToString());
}

