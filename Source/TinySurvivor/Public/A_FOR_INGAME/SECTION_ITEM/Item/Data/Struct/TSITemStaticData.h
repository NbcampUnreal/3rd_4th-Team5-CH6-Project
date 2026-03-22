// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/DataTable/TSItemBaseInfoTable.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/DataTable/TSItemInventoryInfoTable.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/DataTable/TSItemUIInfoTable.h"
#include "TSITemStaticData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSITemStaticData
{
	GENERATED_BODY()
	
	// 아이템 기본 정보 (ID, 클래스, 데이터 에셋)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	FTSItemBaseInfoTable ItemBaseInfoTable;
	
	// 아이템 인벤토리 관련 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	FTSItemInventoryInfoTable ItemInventoryInfoTable;
	
	// 아이템 UI 관련 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TS | Item")
	FTSItemUIInfoTable ItemUIInfoTable;
};
