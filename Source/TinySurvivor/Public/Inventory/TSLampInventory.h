// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TSInventoryMasterComponent.h"
#include "TSLampInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFuelTransferred);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSLampInventory : public UTSInventoryMasterComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTSLampInventory();
	void SetMaintenanceCostID(const int32 StaticDataID) { MaintenanceCostID = StaticDataID; }
	void SetMaintenanceCostQty(const int32 MaxStackSize) { MaintenanceCostQty = MaxStackSize; }
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnFuelTransferred OnFuelTransferred;
	
protected:
	virtual void Internal_TransferItem(UTSInventoryMasterComponent* SourceInventory,
	                                   UTSInventoryMasterComponent* TargetInventory, EInventoryType FromInventoryType,
	                                   int32 FromSlotIndex, EInventoryType ToInventoryType, int32 ToSlotIndex,
	                                   bool bIsFullStack = true,
	                                   ATSPlayerController* RequestingPlayer = nullptr) override;
	virtual bool CanPlaceItemInSlot(int32 StaticDataID, EInventoryType InventoryType, int32 SlotIndex, bool IsTarget) override;
	virtual void ClearSlot(FSlotStructMaster& Slot) override;
	
	void SetFuelSlot();
	
	// 연료 아이템 StaticDataID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fuel")
	int32 MaintenanceCostID = 0;
	// 보관 가능한 연료 최대 스택
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fuel")
	int32 MaintenanceCostQty = 0;
};
