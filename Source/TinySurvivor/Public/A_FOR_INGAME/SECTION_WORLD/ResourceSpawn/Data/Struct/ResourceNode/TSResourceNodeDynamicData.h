// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Struct/TSResourceRuntimeData.h"
#include "TSResourceNodeDynamicData.generated.h"

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceNodeDynamicData
{
	GENERATED_BODY()
	
	// 현재 자원을 가지고 있는지 확인하기 위한 변수 
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	bool bIsSpawnedAnyResource = false;
	
	// 자원 노드가 소환해야하는 자원 런타임 데이터
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Resource")
	FTSResourceRuntimeData LastSavedResourceRuntimeData; 
	
};
