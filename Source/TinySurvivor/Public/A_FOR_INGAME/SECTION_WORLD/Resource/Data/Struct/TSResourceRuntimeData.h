// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSResourceDynamicData.h"
#include "TSResourceRuntimeData.generated.h"


USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceRuntimeData
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	FTSResourceDynamicData DynamicData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	int32 StaticDataID = -1;
	
};
