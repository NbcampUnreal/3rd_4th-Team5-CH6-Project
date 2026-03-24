// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "TSSaveMasterData.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSSaveMasterData
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "TS | Save")
	bool IsThisMasterDataIsDontHavAnySavedData = true;
	
	// 플레이어 관련 세이브 정보 
	
	// 자원 관련 세이브 정보 
};
