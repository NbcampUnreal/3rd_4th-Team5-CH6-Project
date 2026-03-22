// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSItemRuntimeData.h"
#include "TSInventoryCompActionInterface.generated.h"


// This class does not need to be modified.
UINTERFACE()
class UTSInventoryCompActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  인벤토리 외부 API 설계도
 */
class TINYSURVIVOR_API ITSInventoryCompActionInterface
{
	GENERATED_BODY()
	
public:
	// 아이템 습득
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool AddItemToInventory(FTSItemRuntimeData InItemRuntimeData);
    
	// 아이템 제거
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool RemoveItemFromInventory(FTSItemRuntimeData InItemRuntimeData);
    
	// 아이템 드랍
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool DropItemToFromInventory(FTSItemRuntimeData InItemRuntimeData);
    
	// 아이템 스왑
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	bool SwapItemFromThisInventoryToAnotherInventory(FTSItemRuntimeData InItemRuntimeData);
	
};
