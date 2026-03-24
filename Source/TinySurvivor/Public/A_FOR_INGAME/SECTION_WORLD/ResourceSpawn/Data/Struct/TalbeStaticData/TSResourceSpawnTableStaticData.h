// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/DataTable/TSResourceSpawnPerRegionDatatable.h"
#include "TSResourceSpawnTableStaticData.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceSpawnTableStaticData
{
	GENERATED_BODY()
	
	FGameplayTag RegionTag = FGameplayTag::EmptyTag;
	
	TArray<FTSResourceSpawnPerRegionDatatable> ResourceSpawnPerRegionDataArray;
};
