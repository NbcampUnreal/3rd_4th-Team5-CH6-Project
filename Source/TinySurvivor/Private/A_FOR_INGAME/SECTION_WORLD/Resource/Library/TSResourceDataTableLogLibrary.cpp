// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/Resource/Library/TSResourceDataTableLogLibrary.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Actor/A_Base/TSResourceActorBase.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/DataAsset/TSResourceInfoDataAsset.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Struct/TSResourceStaticData.h"

void UTSResourceDataTableLogLibrary::LogResourceStaticData_Lib(FTSResourceStaticData* InResourceData)
{
	if (!InResourceData) return;
	
	UE_LOG(LogTemp, Log, TEXT("==== Starting Static Resource Data Log ===="));
	LogResourceBaseInfoData_Lib(&InResourceData->ResourceBaseInfoTable);
	LogResourceUIInfoData_Lib(&InResourceData->ResourceUIInfoTable);
	LogResourceLootInfoData_Lib(&InResourceData->ResourceLootInfoTable);
	UE_LOG(LogTemp, Log, TEXT("==== End of Static Resource Data Log ===="));
}

void UTSResourceDataTableLogLibrary::LogResourceBaseInfoData_Lib(FTSResourceBaseInfoTable* InResourceBaseInfoTable)
{
	if (!InResourceBaseInfoTable) return;
	
	UE_LOG(LogTemp, Log, TEXT("[Base Info] ResourceID: %d"), InResourceBaseInfoTable->ResourceID);
	
	FString ResourceClassName = InResourceBaseInfoTable->ResourceClass ? InResourceBaseInfoTable->ResourceClass->GetName() : TEXT("NULL");
	UE_LOG(LogTemp, Log, TEXT("[Base Info] ResourceClass: %s"), *ResourceClassName);
	
	FString DataAssetName = InResourceBaseInfoTable->ResourceDataAsset ? InResourceBaseInfoTable->ResourceDataAsset->GetName() : TEXT("NULL");
	UE_LOG(LogTemp, Log, TEXT("[Base Info] ResourceDataAsset: %s"), *DataAssetName);
	
}

void UTSResourceDataTableLogLibrary::LogResourceUIInfoData_Lib(FTSResourceUIInfoTable* InResourceUIInfoTable)
{
	if (!InResourceUIInfoTable) return;
	
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ResourceID: %d"), InResourceUIInfoTable->ResourceID);
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ResourceName: %s"), *InResourceUIInfoTable->ResourceName.ToString());
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ResourceDescription: %s"), *InResourceUIInfoTable->ResourceDescription.ToString());
	
	
	FString IconPath = InResourceUIInfoTable->ResourceIcon.IsNull() ? TEXT("Empty") : InResourceUIInfoTable->ResourceIcon.ToString();
	UE_LOG(LogTemp, Log, TEXT("[UI Info] ResourceIcon Path: %s"), *IconPath);
}

void UTSResourceDataTableLogLibrary::LogResourceLootInfoData_Lib(FTSResourceLootInfoTable* InResourceLootInfoTable)
{
	if (!InResourceLootInfoTable) return;
	
	UE_LOG(LogTemp, Log, TEXT("[Loot Info] ResourceID: %d"), InResourceLootInfoTable->ResourceID);
	
	for (const auto& [LootProbability, LootItemID] : InResourceLootInfoTable->LootInfoArray)
	{
		UE_LOG(LogTemp, Log, TEXT("[Loot Info] Item ID: %d | Probability: %f"), LootItemID, LootProbability);
	}
}
