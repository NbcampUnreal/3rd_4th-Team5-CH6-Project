// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/Library/TSItemDataTableLogLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSITemStaticData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Enum/InventoryType/TSInventoryCompType.h"

void UTSItemDataTableLogLibrary::LogStaticItemData_Lib(FTSITemStaticData* InITemStaticData)
{
	if (!InITemStaticData) return;
	
	UE_LOG(LogTemp, Log, TEXT("==== Starting Static Item Data Log ===="));
	LogBaseInfoData_Lib(&InITemStaticData->ItemBaseInfoTable);
	LogInventoryInfoData_Lib(&InITemStaticData->ItemInventoryInfoTable);
	LogUIInfoData_Lib(&InITemStaticData->ItemUIInfoTable);
	UE_LOG(LogTemp, Log, TEXT("==== End of Static Item Data Log ===="));
}

void UTSItemDataTableLogLibrary::LogBaseInfoData_Lib(FTSItemBaseInfoTable* InItemBaseInfoTable)
{
	if (!InItemBaseInfoTable) return;
	
	UE_LOG(LogTemp, Log, TEXT("[Base Info] ItemID: %d"), InItemBaseInfoTable->ItemID);
	UE_LOG(LogTemp, Log, TEXT("[Base Info] ItemClass: %s"), *InItemBaseInfoTable->ItemClass.ToString());
	UE_LOG(LogTemp, Log, TEXT("[Base Info] ItemDataAsset: %s"), *InItemBaseInfoTable->ItemDataAsset.ToString());
}

void UTSItemDataTableLogLibrary::LogInventoryInfoData_Lib(FTSItemInventoryInfoTable* InItemInventoryInfoTable)
{
	if (!InItemInventoryInfoTable) return;
	
	UE_LOG(LogTemp, Log, TEXT("[Inventory Info] ItemID: %d"), InItemInventoryInfoTable->ItemID);
	UE_LOG(LogTemp, Log, TEXT("[Inventory Info] MaxStackSize: %d"), InItemInventoryInfoTable->MaxStackSize);

	// Enum Array Log
	for (const ETSInventoryCompType& CompType : InItemInventoryInfoTable->MatchInventoryCompType)
	{
		const UEnum* EnumPtr = StaticEnum<ETSInventoryCompType>();
		FString TypeName = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(CompType)) : TEXT("Invalid");
		UE_LOG(LogTemp, Log, TEXT("[Inventory Info] MatchInventoryCompType: %s"), *TypeName);
	}

	// Class/Object Array Log
	for (const ETSInventorySlotType& SlotType : InItemInventoryInfoTable->MatchInventorySlotType)
	{
		const UEnum* EnumPtr = StaticEnum<ETSInventorySlotType>();
		FString TypeName = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(SlotType)) : TEXT("Invalid");
		UE_LOG(LogTemp, Log, TEXT("[Inventory Info] MatchInventorySlotType: %s"), *TypeName);
	}
}

void UTSItemDataTableLogLibrary::LogUIInfoData_Lib(FTSItemUIInfoTable* InItemUIInfoTable)
{
	if (!InItemUIInfoTable) return;
	
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ItemID: %d"), InItemUIInfoTable->ItemID);
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ItemName: %s"), *InItemUIInfoTable->ItemName.ToString());
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ItemDescription: %s"), *InItemUIInfoTable->ItemDescription.ToString());
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ItemIcon Path: %s"), *InItemUIInfoTable->ItemIcon.ToString());
}

