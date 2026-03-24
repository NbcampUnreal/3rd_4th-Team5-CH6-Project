// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TSResourceSpawnPerRegionDatatable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TINYSURVIVOR_API FTSResourceSpawnPerRegionDatatable : public FTableRowBase
{
	GENERATED_BODY()
	
	// 어느 구역인가?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	FGameplayTag RegionTag;
	
	// 범용 노드에 스폰할 것인가? (만약 범용일 경우 스폰은 열거형 타입이 매칭되는 아무 노드에나 스폰이 가능하며, 범용이 아닐 경우 자원 ID가 매칭되는 노드에만 스폰한다.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	bool bIsGeneralNode = true;
	
	// 스폰하려는 자원 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 ResourceID = -1;
	
	// 스폰하려는 자원 갯수 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 SpawnCount = 1;
	
};

