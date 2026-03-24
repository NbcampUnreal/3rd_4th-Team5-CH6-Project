// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/ResourceSpawn/Data/Struct/TalbeStaticData/TSResourceSpawnTableStaticData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSResourceSpawnTableDebugLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceSpawnTableDebugLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	static void PrintDebugInfoSpawnTable_Lib(FTSResourceSpawnTableStaticData& Info);
};
