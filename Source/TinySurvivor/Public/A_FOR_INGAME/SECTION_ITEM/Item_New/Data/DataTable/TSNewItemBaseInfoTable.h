// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TSNewItemBaseInfoTable.generated.h"

class ATSNewItemActorBase;
class UTSNewItemInfoDataAsset;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FTSNewItemBaseInfoTable : public FTableRowBase
{
	GENERATED_BODY()
	
	// 아이템 ID 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	int32 ItemID = -1;
	
	// 아이템 클래스 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	TSoftObjectPtr<ATSNewItemActorBase> ItemClass = nullptr;
	
	// 아이템 데이터 에셋 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	TSoftObjectPtr<UTSNewItemInfoDataAsset> ItemDataAsset = nullptr;
};
