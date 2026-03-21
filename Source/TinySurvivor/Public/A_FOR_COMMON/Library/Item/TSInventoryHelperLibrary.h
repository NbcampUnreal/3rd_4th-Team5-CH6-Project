// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Struct/TSInventorySlot.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSInventoryHelperLibrary.generated.h"

class UTSInventoryMasterComponent;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSInventoryHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
///////////////////////
	
	static bool ExpandBagInventory_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32& InAdditionalSlots);
	

///////////////////////

	static FInventoryStructMaster* GetInventoryByType_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, EInventoryType InInventoryType);	
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	static FSlotStructMaster GetSlot_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, EInventoryType InInventoryType, int32 InSlotIndex);
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	static int32 GetItemCount_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 StaticDataID);
	
	static FSlotStructMaster GetActiveHotkeySlot_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent);
	
	
///////////////////////
	
	static bool IsValidSlotIndex_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, EInventoryType InInventoryType, int32 InSlotIndex);
	
	static bool IsSlotEmpty_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, EInventoryType InInventoryType, int32 InSlotIndex);
	
	static bool CanPlaceItemInSlot_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 StaticDataID, EInventoryType InventoryType, int32 SlotIndex, bool IsTarget);
	
};
