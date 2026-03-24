// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Library/TSResourceSpawnTableDebugLibrary.h"
#include "A_FOR_COMMON/Log/TSSystemLogHeader.h"

void UTSResourceSpawnTableDebugLibrary::PrintDebugInfoSpawnTable_Lib(FTSResourceSpawnTableStaticData& Info)
{
	UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("=================PrintResourceSpawnTable================="));
	UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("RegionTag: %s"), *Info.RegionTag.ToString());
	UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("ResourceSpawnPerRegionDataArray Num: %d"), Info.ResourceSpawnPerRegionDataArray.Num());

	for (const FTSResourceSpawnPerRegionDatatable& Data : Info.ResourceSpawnPerRegionDataArray)
	{
		UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("---------------------------------------------------------"));
		UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("RegionTag: %s"), *Data.RegionTag.ToString());
		UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("bIsGeneralNode: %s"), Data.bIsGeneralNode ? TEXT("true") : TEXT("false"));
		UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("ResourceID: %d"), Data.ResourceID);
		UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("SpawnCount: %d"), Data.SpawnCount);
	}

	UE_LOG(ResourceSpawnDataMgrLog, Log, TEXT("========================================================="));
}
