// BuildingData.cpp
#include "Item/Data/BuildingData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogFBuildingData, Log, All);

void FBuildingData::PrintDebugInfo() const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogFBuildingData, Display, TEXT("============= Building Debug Info ============="));
	
	// Identifier
	UE_LOG(LogFBuildingData, Display, TEXT("---[Identifier]"));
	UE_LOG(LogFBuildingData, Display, TEXT("BuildingID: %d"), BuildingID);
	UE_LOG(LogFBuildingData, Display, TEXT("MainCategory: %d"), static_cast<uint8>(MainCategory));
	UE_LOG(LogFBuildingData, Display, TEXT("Name_KR: %s"), *Name_KR.ToString());
	UE_LOG(LogFBuildingData, Display, TEXT("Name_EN: %s"), *Name_EN.ToString());
	
	// System
	UE_LOG(LogFBuildingData, Display, TEXT("---[System]"));
	UE_LOG(LogFBuildingData, Display, TEXT("Rarity: %d"), static_cast<uint8>(Rarity));
	UE_LOG(LogFBuildingData, Display, TEXT("BuildingType: %d"), static_cast<uint8>(BuildingType));
	UE_LOG(LogFBuildingData, Display, TEXT("PlacementArea: %s"), *PlacementArea.ToString());
	UE_LOG(LogFBuildingData, Display, TEXT("RequiredTier: %d"), static_cast<uint8>(RequiredTier));
	UE_LOG(LogFBuildingData, Display, TEXT("MaxDurability: %d"), MaxDurability);
	UE_LOG(LogFBuildingData, Display, TEXT("IsErosionController: %s"), IsErosionController ? TEXT("True") : TEXT("False"));
	
	//======================================
	// LIGHT SYSTEM DATA
	//======================================
	if (BuildingType == EBuildingType::LIGHT)
	{
		UE_LOG(LogFBuildingData, Display, TEXT("---[Light System]"));
		UE_LOG(LogFBuildingData, Display, TEXT("MaintenanceCostID: %d"), MaintenanceCostID);
		UE_LOG(LogFBuildingData, Display, TEXT("MaintenanceInterval: %.2f"), MaintenanceInterval);
		UE_LOG(LogFBuildingData, Display, TEXT("MaintenanceCostQty: %d"), MaintenanceCostQty);
		UE_LOG(LogFBuildingData, Display, TEXT("LightRadius_Units: %.2f"), LightRadius_Units);
		UE_LOG(LogFBuildingData, Display, TEXT("ErosionReduction: %.2f"), ErosionReduction);
		UE_LOG(LogFBuildingData, Display, TEXT("SanityRecoveryPerSec: %.2f"), SanityRecoveryPerSec);
		UE_LOG(LogFBuildingData, Display, TEXT("MaxMaintenance: %d"), MaxMaintenance);
	}
	
	//======================================
	// STORAGE SYSTEM DATA
	//======================================
	if (BuildingType == EBuildingType::STORAGE)
	{
		UE_LOG(LogFBuildingData, Display, TEXT("---[Storage System]"));
		UE_LOG(LogFBuildingData, Display, TEXT("StorageSlots: %d"), StorageSlots);
	}
	
	// Visual
	UE_LOG(LogFBuildingData, Display, TEXT("---[Visual]"));
	UE_LOG(LogFBuildingData, Display, TEXT("Icon: %s"), Icon.IsValid() ? *Icon->GetName() : TEXT("None"));
	UE_LOG(LogFBuildingData, Display, TEXT("WorldMesh: %s"), WorldMesh.IsValid() ? *WorldMesh->GetName() : TEXT("None"));
	
	// 한글/영문 설명 추가
	UE_LOG(LogFBuildingData, Display, TEXT("Desc_KR: %s"), *Desc_KR.ToString());
	UE_LOG(LogFBuildingData, Display, TEXT("Desc_EN: %s"), *Desc_EN.ToString());
	
	// Spawn
	UE_LOG(LogFBuildingData, Display, TEXT("---[Spawn]"));
	UE_LOG(LogFBuildingData, Display, TEXT("ActorClass: %s"), ActorClass ? *ActorClass->GetName() : TEXT("None"));
	
	UE_LOG(LogFBuildingData, Display, TEXT("=============================================="));
#endif
}