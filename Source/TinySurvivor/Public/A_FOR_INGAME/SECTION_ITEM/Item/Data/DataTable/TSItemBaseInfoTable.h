// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TSItemBaseInfoTable.generated.h"

class ATSItemActorBase;
class UTSItemInfoDataAsset;
/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSItemBaseInfoTable : public FTableRowBase
{
	GENERATED_BODY()
	
	// 아이템 ID 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	int32 ItemID = -1;
	
	// 아이템 클래스 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	TSubclassOf<ATSItemActorBase> ItemClass = nullptr;
	
	// 아이템 데이터 에셋 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	TSubclassOf<UTSItemInfoDataAsset> ItemDataAsset = nullptr;
};
