// SpawnerData.cpp
#include "Spawner/Data/SpawnerData.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogFSpawnerData, Log, All);

void FSpawnerData::PrintDebugInfo() const
{
	UE_LOG(LogFSpawnerData, Display, TEXT("============= Spawner Data Debug Info ============="));
	
	// Basic Info
	UE_LOG(LogFSpawnerData, Display, TEXT("---[Basic Info]"));
	UE_LOG(LogFSpawnerData, Display, TEXT("SpawnNodeID: %d"), SpawnNodeID);
	UE_LOG(LogFSpawnerData, Display, TEXT("ResourceNodeID: %d"), ResourceNodeID);
	
	// Location Control
	UE_LOG(LogFSpawnerData, Display, TEXT("\n---[Location Control]"));
	UE_LOG(LogFSpawnerData, Display, TEXT("SpawnMarkerGroup: %d"), SpawnMarkerGroup);
	UE_LOG(LogFSpawnerData, Display, TEXT("AllowedZoneTags Count: %d"), AllowedZoneTags.Num());
	for (int32 i = 0; i < AllowedZoneTags.Num(); ++i)
	{
		UE_LOG(LogFSpawnerData, Display, TEXT("  [%d] %s"), 
			i, *UEnum::GetValueAsString(AllowedZoneTags[i]));
	}
	
	// Spawn Quantity
	UE_LOG(LogFSpawnerData, Display, TEXT("\n---[Spawn Quantity]"));
	UE_LOG(LogFSpawnerData, Display, TEXT("MinSpawnCount: %d"), MinSpawnCount);
	UE_LOG(LogFSpawnerData, Display, TEXT("MaxSpawnCount: %d"), MaxSpawnCount);
	UE_LOG(LogFSpawnerData, Display, TEXT("SpawnProbability: %.2f"), SpawnProbability);
	
	// Respawn Logic
	UE_LOG(LogFSpawnerData, Display, TEXT("\n---[Respawn Logic]"));
	UE_LOG(LogFSpawnerData, Display, TEXT("RespawnErosionMax: %d"), RespawnErosionMax);
	UE_LOG(LogFSpawnerData, Display, TEXT("RespawnInterval: %.2f seconds"), RespawnInterval);
	UE_LOG(LogFSpawnerData, Display, TEXT("RespawnConditionTag: %s"), 
		*UEnum::GetValueAsString(RespawnConditionTag));
	
	// Location Shuffle Condition
	UE_LOG(LogFSpawnerData, Display, TEXT("\n---[Location Shuffle Condition]"));
	UE_LOG(LogFSpawnerData, Display, TEXT("VulnerableToGiantShuffle: %s"), 
		bVulnerableToGiantShuffle ? TEXT("True") : TEXT("False"));
	
	// World Placement
	UE_LOG(LogFSpawnerData, Display, TEXT("\n---[World Placement]"));
	UE_LOG(LogFSpawnerData, Display, TEXT("SpawnRadius: %.2f"), SpawnRadius);
	UE_LOG(LogFSpawnerData, Display, TEXT("MinDistanceBetweenNodes: %.2f"), MinDistanceBetweenNodes);
	UE_LOG(LogFSpawnerData, Display, TEXT("AlignToGround: %s"), 
		bAlignToGround ? TEXT("True") : TEXT("False"));
	
	// Helper Checks
	UE_LOG(LogFSpawnerData, Display, TEXT("\n---[Helper Checks]"));
	UE_LOG(LogFSpawnerData, Display, TEXT("HasValidSpawnCountRange: %s"), 
		HasValidSpawnCountRange() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFSpawnerData, Display, TEXT("ShouldSpawnByProbability: %s"), 
		ShouldSpawnByProbability() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFSpawnerData, Display, TEXT("IsVulnerableToShuffle: %s"), 
		IsVulnerableToShuffle() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFSpawnerData, Display, TEXT("ShouldAlignToGround: %s"), 
		ShouldAlignToGround() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFSpawnerData, Display, TEXT("ValidateSpawnerData: %s"), 
		ValidateSpawnerData() ? TEXT("True") : TEXT("False"));
	UE_LOG(LogFSpawnerData, Display, TEXT("IsSpawnable: %s"), 
		IsSpawnable() ? TEXT("True") : TEXT("False"));
	
	UE_LOG(LogFSpawnerData, Display, TEXT("==================================================="));
}