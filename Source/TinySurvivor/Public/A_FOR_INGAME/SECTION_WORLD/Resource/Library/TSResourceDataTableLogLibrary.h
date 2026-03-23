// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSResourceDataTableLogLibrary.generated.h"

struct FTSResourceLootInfoTable;
struct FTSResourceUIInfoTable;
struct FTSResourceStaticData;
struct FTSResourceBaseInfoTable;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSResourceDataTableLogLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void LogResourceStaticData_Lib(FTSResourceStaticData* InResourceData);
	static void LogResourceBaseInfoData_Lib(FTSResourceBaseInfoTable* InResourceBaseInfoTable);
	static void LogResourceUIInfoData_Lib(FTSResourceUIInfoTable* InResourceUIInfoTable);
	static void LogResourceLootInfoData_Lib(FTSResourceLootInfoTable* InResourceLootInfoTable);
	
};
