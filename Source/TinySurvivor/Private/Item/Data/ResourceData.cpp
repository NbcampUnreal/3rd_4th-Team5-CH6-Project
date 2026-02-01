// ResourceData.cpp
#include "Item/Data/ResourceData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogFResourceData, Log, All);

void FResourceData::PrintDebugInfo() const
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogFResourceData, Display, TEXT("============= Resource Debug Info ============="));
	
	// Identifier
	UE_LOG(LogFResourceData, Display, TEXT("---[Identifier]"));
	UE_LOG(LogFResourceData, Display, TEXT("ResourceID: %d"), ResourceID);
	UE_LOG(LogFResourceData, Display, TEXT("MainCategory: %d"), static_cast<int32>(MainCategory));
	UE_LOG(LogFResourceData, Display, TEXT("Name_KR: %s"), *Name_KR.ToString());
	UE_LOG(LogFResourceData, Display, TEXT("Name_EN: %s"), *Name_EN.ToString());
	
	// System
	UE_LOG(LogFResourceData, Display, TEXT("---[System]"));
	UE_LOG(LogFResourceData, Display, TEXT("Rarity: %d"), static_cast<int32>(Rarity));
	UE_LOG(LogFResourceData, Display, TEXT("NodeType: %d"), static_cast<int32>(NodeType));
	UE_LOG(LogFResourceData, Display, TEXT("NodeTier: %d"), static_cast<int32>(NodeTier));
	UE_LOG(LogFResourceData, Display, TEXT("TotalYield: %d"), TotalYield);
	UE_LOG(LogFResourceData, Display, TEXT("MainDropTableID: %d"), MainDropTableID);
	UE_LOG(LogFResourceData, Display, TEXT("MainDropTablePrecent: %f"), MainDropTablePrecent);
	UE_LOG(LogFResourceData, Display, TEXT("MainDropMinNum: %d"), MainDropMinNum);
	UE_LOG(LogFResourceData, Display, TEXT("MainDropMaxNum: %d"), MainDropMaxNum);
	UE_LOG(LogFResourceData, Display, TEXT("SubDropTableID: %d"), SubDropTableID);
	UE_LOG(LogFResourceData, Display, TEXT("SubDropTablePrecent: %f"), SubDropTablePrecent);
	UE_LOG(LogFResourceData, Display, TEXT("SubDropMinNum: %d"), SubDropMinNum);
	UE_LOG(LogFResourceData, Display, TEXT("SubDropMaxNum: %d"), SubDropMaxNum);
	UE_LOG(LogFResourceData, Display, TEXT("RespawnTime: %.2f"), RespawnTime);
	
	// Visual
	UE_LOG(LogFResourceData, Display, TEXT("---[Visual]"));
	FString MeshName = WorldMesh.IsValid() ? WorldMesh->GetName() : TEXT("None");
	UE_LOG(LogFResourceData, Display, TEXT("WorldMesh: %s"), *MeshName);
	
	// Sound
	UE_LOG(LogFResourceData, Display, TEXT("---[Sound]"));
	FString DestroySoundName = DestroySound.IsValid() ? DestroySound->GetName() : TEXT("None");
	UE_LOG(LogFResourceData, Display, TEXT("DestroySound: %s"), *DestroySoundName);
	
	// Spawn
	UE_LOG(LogFResourceData, Display, TEXT("---[Spawn]"));
	FString ActorName = ActorClass ? ActorClass->GetName() : TEXT("None");
	UE_LOG(LogFResourceData, Display, TEXT("ActorClass: %s"), *ActorName);
	
	UE_LOG(LogFResourceData, Display, TEXT("============================================"));
#endif
}