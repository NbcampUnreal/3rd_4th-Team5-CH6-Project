// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Table/TSResourceBaseInfoTable.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Table/TSResourceLootInfoTable.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/Data/Table/TSResourceUIInfoTable.h"
#include "TSResourceStaticData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceStaticData
{
	GENERATED_BODY()
	
	// 기본 정보 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	FTSResourceBaseInfoTable ResourceBaseInfoTable;
	
	// UI 관련 정보 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	FTSResourceUIInfoTable ResourceUIInfoTable;
	
	// 루팅 관련 정보 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Resource")
	FTSResourceLootInfoTable ResourceLootInfoTable;
};
