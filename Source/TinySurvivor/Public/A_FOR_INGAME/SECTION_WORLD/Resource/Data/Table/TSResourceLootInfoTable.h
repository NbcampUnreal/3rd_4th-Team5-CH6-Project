// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "SECTION_LOOT/TSLootInfo.h"
#include "TSResourceLootInfoTable.generated.h"

/**
 * 
 */
USTRUCT(blueprinttype)
struct TINYSURVIVOR_API FTSResourceLootInfoTable : public FTableRowBase
{
	GENERATED_BODY()
	
	FTSResourceLootInfoTable();
	
	// 자원 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 ResourceID = -1;
	
	// 총 수량 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 TotalCount = 1;
	
	// 체력 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	int32 Health = 100;
	
	// 루팅 가능한 아이템 목록 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	TArray<FTSLootInfo> LootInfoArray;
	
	// 가능한 상호작용 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Resource")
	TArray<FGameplayTag> PossibleInteractTags;
	
};
