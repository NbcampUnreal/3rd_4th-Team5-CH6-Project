// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/TalbeStaticData/TSResourceSpawnTableStaticData.h"
#include "TSResourceSpawnTableCachingHelperLibrary.generated.h"

class UDataTable;

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnTableCachingHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	static void CacheResourceSpawnTable_Lib(UDataTable* InTable, TMap<FGameplayTag, FTSResourceSpawnTableStaticData>& InTempMap);
	
};
