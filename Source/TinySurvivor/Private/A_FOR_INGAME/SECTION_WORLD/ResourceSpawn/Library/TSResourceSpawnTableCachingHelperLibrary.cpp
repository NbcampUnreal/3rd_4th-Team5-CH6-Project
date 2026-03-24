// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Library/TSResourceSpawnTableCachingHelperLibrary.h"

void UTSResourceSpawnTableCachingHelperLibrary::CacheResourceSpawnTable_Lib(UDataTable* InTable, TMap<FGameplayTag, FTSResourceSpawnTableStaticData>& InTempMap)
{
	if (!IsValid(InTable)) return;
	
	TArray<FTSResourceSpawnPerRegionDatatable*> Rows;
	InTable->GetAllRows<FTSResourceSpawnPerRegionDatatable>(TEXT("ResourceSpawnTableLoad"), Rows);
	
	for (FTSResourceSpawnPerRegionDatatable* Row : Rows)
	{
		if (!Row) continue;
		auto& FoundData =  InTempMap.FindOrAdd(Row->RegionTag);
		FoundData.RegionTag = Row->RegionTag;
		FoundData.ResourceSpawnPerRegionDataArray.Add(*Row);
	}
}
