// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/System/TSITemSpawnSubSystem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/Base/TSItemActorBase.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/System/TSItemDataSubSystem.h"

//======================================================================================================================	
#pragma region 게터
	
	//━━━━━━━━━━━━━━━━━━━━
	// 게터
	//━━━━━━━━━━━━━━━━━━━━	

UTSITemSpawnSubSystem* UTSITemSpawnSubSystem::Get(const UObject* InWorldContextObject)
{
	if (!IsValid(InWorldContextObject)) return nullptr;
	
	UWorld* World = InWorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
			
	UGameInstance* GameInstance = World->GetGameInstance();

	UTSITemSpawnSubSystem* ItemSpawnSubSystem = GameInstance->GetSubsystem<UTSITemSpawnSubSystem>();
	if (!IsValid(ItemSpawnSubSystem)) return nullptr;

	return ItemSpawnSubSystem;
}

#pragma endregion
//======================================================================================================================	
#pragma region 외부_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 외부 API
	//━━━━━━━━━━━━━━━━━━━━
	
ATSItemActorBase* UTSITemSpawnSubSystem::SpawnNewItem(int32 InSpawnItemID, int32 InWantStackSize, FVector InSpawnLocation)
{
	UTSItemDataSubSystem* ItemDataSubSystem = UTSItemDataSubSystem::Get(this);
	if (!IsValid(ItemDataSubSystem)) return nullptr;
	
	FTSITemStaticData* ItemStaticData = ItemDataSubSystem->GetItemStaticData(InSpawnItemID);
	if (!ItemStaticData) return nullptr;
	
	FTSItemRuntimeData NewItemRuntimeData;
	NewItemRuntimeData.DynamicData.CurrentStack = InWantStackSize;
	NewItemRuntimeData.StaticDataID = InSpawnItemID;
	
	if (!IsValid(GetWorld())) return nullptr;
	if (!IsValid(ItemStaticData->ItemBaseInfoTable.ItemClass)) return nullptr;
	
	ATSItemActorBase* SpawnedItemActor = GetWorld()->SpawnActor<ATSItemActorBase>(ItemStaticData->ItemBaseInfoTable.ItemClass, InSpawnLocation, FRotator::ZeroRotator);
	if (!IsValid(SpawnedItemActor)) return nullptr;
	
	SpawnedItemActor->SetItemRuntimeData(NewItemRuntimeData);
	return SpawnedItemActor;
}

ATSItemActorBase* UTSITemSpawnSubSystem::DropItemFromSomeWhere(FTSItemRuntimeData& InItemRuntimeData, FVector InDropLocation)
{
	UTSItemDataSubSystem* ItemDataSubSystem = UTSItemDataSubSystem::Get(this);
	if (!IsValid(ItemDataSubSystem)) return nullptr;
	
	FTSITemStaticData* ItemStaticData = ItemDataSubSystem->GetItemStaticData(InItemRuntimeData.StaticDataID);
	if (!ItemStaticData) return nullptr;
	
	if (!IsValid(GetWorld())) return nullptr;
	if (!IsValid(ItemStaticData->ItemBaseInfoTable.ItemClass)) return nullptr;
	
	ATSItemActorBase* SpawnedItemActor = GetWorld()->SpawnActor<ATSItemActorBase>(ItemStaticData->ItemBaseInfoTable.ItemClass, InDropLocation, FRotator::ZeroRotator);
	if (!IsValid(SpawnedItemActor)) return nullptr;
	
	SpawnedItemActor->SetItemRuntimeData(InItemRuntimeData);
	return SpawnedItemActor;
}

#pragma endregion
//======================================================================================================================	