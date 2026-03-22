// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Data/Struct/TSNewITemStaticData.h"
#include "TSNewItemDataTableLogLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NewItemDataTableLog, Log, All);

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSNewItemDataTableLogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static void LogStaticItemData_Lib(FTSNewITemStaticData* InITemStaticData);
		
	static void LogBaseInfoData_Lib(FTSNewItemBaseInfoTable* InItemBaseInfoTable);
	
	static void LogInventoryInfoData_Lib(FTSNewItemInventoryInfoTable* InItemInventoryInfoTable);
	
	static void LogUIInfoData_Lib(FTSNewItemUIInfoTable* InItemUIInfoTable);
	
};
