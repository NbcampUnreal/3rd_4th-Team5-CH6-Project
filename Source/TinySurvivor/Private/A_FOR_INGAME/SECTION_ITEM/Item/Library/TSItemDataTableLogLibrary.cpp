// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/Library/TSItemDataTableLogLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/DataAsset/TSItemInfoDataAsset.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSITemStaticData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/Base/TSItemActorBase.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Enum/InventoryType/TSInventoryCompType.h"
#include "A_FOR_COMMON/Log/TSSystemLogHeader.h"

void UTSItemDataTableLogLibrary::LogStaticItemData_Lib(FTSITemStaticData* InITemStaticData)
{
	if (!InITemStaticData) return;
	
	UE_LOG(ItemDataMgrLog, Log, TEXT("==== Starting Static Item Data Log ===="));
	LogBaseInfoData_Lib(&InITemStaticData->ItemBaseInfoTable);
	LogInventoryInfoData_Lib(&InITemStaticData->ItemInventoryInfoTable);
	LogUIInfoData_Lib(&InITemStaticData->ItemUIInfoTable);
	UE_LOG(ItemDataMgrLog, Log, TEXT("==== End of Static Item Data Log ===="));
}

void UTSItemDataTableLogLibrary::LogBaseInfoData_Lib(FTSItemBaseInfoTable* InItemBaseInfoTable)
{
	if (!InItemBaseInfoTable) return;
	
	UE_LOG(ItemDataMgrLog, Log, TEXT("[Base Info] ItemID: %d"), InItemBaseInfoTable->ItemID);
	
	FString ItemClassName = InItemBaseInfoTable->ItemClass ? InItemBaseInfoTable->ItemClass->GetName() : TEXT("NULL");
	UE_LOG(ItemDataMgrLog, Log, TEXT("[Base Info] ItemClass: %s"), *ItemClassName);
	
	FString DataAssetName = InItemBaseInfoTable->ItemDataAsset ? InItemBaseInfoTable->ItemDataAsset->GetName() : TEXT("NULL");
	UE_LOG(ItemDataMgrLog, Log, TEXT("[Base Info] ItemDataAsset: %s"), *DataAssetName);
}

void UTSItemDataTableLogLibrary::LogInventoryInfoData_Lib(FTSItemInventoryInfoTable* InItemInventoryInfoTable)
{
	if (!InItemInventoryInfoTable) return;
	
	UE_LOG(ItemDataMgrLog, Log, TEXT("[Inventory Info] ItemID: %d"), InItemInventoryInfoTable->ItemID);
	UE_LOG(ItemDataMgrLog, Log, TEXT("[Inventory Info] MaxStackSize: %d"), InItemInventoryInfoTable->MaxStackSize);

	// Enum Array Log
	for (const ETSInventoryCompType& CompType : InItemInventoryInfoTable->MatchInventoryCompType)
	{
		const UEnum* EnumPtr = StaticEnum<ETSInventoryCompType>();
		FString TypeName = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(CompType)) : TEXT("Invalid");
		UE_LOG(ItemDataMgrLog, Log, TEXT("[Inventory Info] MatchInventoryCompType: %s"), *TypeName);
	}

	// Class/Object Array Log
	for (const ETSInventorySlotType& SlotType : InItemInventoryInfoTable->MatchInventorySlotType)
	{
		const UEnum* EnumPtr = StaticEnum<ETSInventorySlotType>();
		FString TypeName = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(SlotType)) : TEXT("Invalid");
		UE_LOG(ItemDataMgrLog, Log, TEXT("[Inventory Info] MatchInventorySlotType: %s"), *TypeName);
	}
}

void UTSItemDataTableLogLibrary::LogUIInfoData_Lib(FTSItemUIInfoTable* InItemUIInfoTable)
{
	if (!InItemUIInfoTable) return;
	
	UE_LOG(ItemDataMgrLog, Log, TEXT("[UI Info] ItemID: %d"), InItemUIInfoTable->ItemID);
	UE_LOG(ItemDataMgrLog, Log, TEXT("[UI Info] ItemName: %s"), *InItemUIInfoTable->ItemName.ToString());
	UE_LOG(ItemDataMgrLog, Log, TEXT("[UI Info] ItemDescription: %s"), *InItemUIInfoTable->ItemDescription.ToString());
	
	FString IconPath = InItemUIInfoTable->ItemIcon.IsNull() ? TEXT("Empty") : InItemUIInfoTable->ItemIcon.ToString();
	UE_LOG(ItemDataMgrLog, Log, TEXT("[UI Info] ItemIcon Path: %s"), *IconPath);
}

