// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Enum/InventorySlotType/TSInventorySlotType.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Enum/InventoryType/TSInventoryCompType.h"
#include "TSItemInventoryInfoTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTSItemInventoryInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	// 생성자에서 타입 배열 관련 기본값 추가함.
	FTSItemInventoryInfoTable();
	
	// 아이템 ID 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	int32 ItemID = -1;
	
	// 들어갈 수 있는 인벤토리 타입 (기본값 : 플레이어 핫키, 가방 / AI 가방 / 보관함 / 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	TArray<ETSInventoryCompType> MatchInventoryCompType;
	
	// 들어갈 수 있는 슬롯 타입 (기본값 : Common)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	TArray<ETSInventorySlotType> MatchInventorySlotType; 
	
	// 최대 스택 가능한 갯수 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = " TS | Item")
	int32 MaxStackSize = 1;
	
};
