// BuildingData.cpp
#include "Item/Data/BuildingData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogFBuildingData, Log, All);

void FBuildingData::PrintDebugInfo() const
{
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
	UE_LOG(LogFBuildingData, Display, TEXT("MaintenanceCostID: %d"), MaintenanceCostID);
	UE_LOG(LogFBuildingData, Display, TEXT("MaintenanceInterval: %d"), MaintenanceInterval);
	UE_LOG(LogFBuildingData, Display, TEXT("MaintenanceCostQty: %d"), MaintenanceCostQty);
	
	// Visual
	UE_LOG(LogFBuildingData, Display, TEXT("---[Visual]"));
	UE_LOG(LogFBuildingData, Display, TEXT("Icon: %s"), Icon.IsValid() ? *Icon->GetName() : TEXT("None"));
	UE_LOG(LogFBuildingData, Display, TEXT("WorldMesh: %s"), WorldMesh.IsValid() ? *WorldMesh->GetName() : TEXT("None"));
	
	// Spawn
	UE_LOG(LogFBuildingData, Display, TEXT("---[Spawn]"));
	UE_LOG(LogFBuildingData, Display, TEXT("ActorClass: %s"), ActorClass ? *ActorClass->GetName() : TEXT("None"));
	
	UE_LOG(LogFBuildingData, Display, TEXT("=============================================="));
}