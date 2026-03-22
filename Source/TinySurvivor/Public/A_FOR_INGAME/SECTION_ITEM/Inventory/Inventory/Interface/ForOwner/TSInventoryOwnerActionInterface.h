// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TSInventoryOwnerActionInterface.generated.h"

class UTSBodyInventoryComponent;
class UTSHotKeyInventoryComponent;
class UTSBackPackInventoryComponent;
class UTSStorageInventoryComponent;
class UTSFuelProductInventoryComponent;
class UTSVisualStorageEquipComponent;

// This class does not need to be modified.
UINTERFACE()
class UTSInventoryOwnerActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API ITSInventoryOwnerActionInterface
{
	GENERATED_BODY()

public:
	
	// 핫키 게터
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	UTSHotKeyInventoryComponent* GetHotKeyComponent();
	
	// 백팩 게터
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	UTSBackPackInventoryComponent* GetBackpackComponent();	
	
	// 장착 게터
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	UTSBodyInventoryComponent* GetEquipmentComponent();
	
	// 연료함
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")	
	UTSFuelProductInventoryComponent* GetFuelComponent();
	
	// 보관함
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TS | Inventory")
	UTSStorageInventoryComponent* GetStorageComponent();
};
