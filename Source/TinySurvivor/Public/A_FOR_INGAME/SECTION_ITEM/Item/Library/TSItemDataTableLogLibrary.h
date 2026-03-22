// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSItemDataTableLogLibrary.generated.h"

struct FTSITemStaticData;
struct FTSItemUIInfoTable;
struct FTSItemInventoryInfoTable;
struct FTSItemBaseInfoTable;

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSItemDataTableLogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static void LogStaticItemData_Lib(FTSITemStaticData* InITemStaticData);
		
	static void LogBaseInfoData_Lib(FTSItemBaseInfoTable* InItemBaseInfoTable);
	
	static void LogInventoryInfoData_Lib(FTSItemInventoryInfoTable* InItemInventoryInfoTable);
	
	static void LogUIInfoData_Lib(FTSItemUIInfoTable* InItemUIInfoTable);
	
};
