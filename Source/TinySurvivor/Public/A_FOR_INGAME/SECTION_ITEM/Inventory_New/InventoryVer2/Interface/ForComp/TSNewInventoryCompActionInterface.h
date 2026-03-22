// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Data/Struct/TSNewItemRuntimeData.h"
#include "UObject/Interface.h"
#include "TSNewInventoryCompActionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTSNewInventoryCompActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  인벤토리 외부 API 설계도
 */
class TINYSURVIVOR_API ITSNewInventoryCompActionInterface
{
	GENERATED_BODY()
	
public:
	// 아이템 습득
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool AddItemToInventory(FTSNewItemRuntimeData InItemRuntimeData);
    
	// 아이템 제거
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool RemoveItemFromInventory(FTSNewItemRuntimeData InItemRuntimeData);
    
	// 아이템 드랍
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool DropItemToFromInventory(FTSNewItemRuntimeData InItemRuntimeData);
    
	// 아이템 스왑
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool SwapItemFromThisInventoryToAnotherInventory(FTSNewItemRuntimeData InItemRuntimeData);
	
};
