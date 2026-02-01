// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TSLampInventory.h"


// Sets default values for this component's properties
UTSLampInventory::UTSLampInventory()
{
	PrimaryComponentTick.bCanEverTick = false;

	HotkeySlotCount = 0;
	InitialBagSlotCount = 1;
	MaxBagSlotCount = 1;
	SlotAccessType = ESlotAccessType::ReadWrite;
}

void UTSLampInventory::Internal_TransferItem(UTSInventoryMasterComponent* SourceInventory,
                                             UTSInventoryMasterComponent* TargetInventory,
                                             EInventoryType FromInventoryType, int32 FromSlotIndex,
                                             EInventoryType ToInventoryType, int32 ToSlotIndex,
                                             bool bIsFullStack, ATSPlayerController* RequestingPlayer)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Internal_TransferItem called on client!"));
		return;
	}
	if (SourceInventory->GetSlot(FromInventoryType, FromSlotIndex).ItemData.StaticDataID
		!= MaintenanceCostID)
	{
		return;
	}

	Super::Internal_TransferItem(SourceInventory, TargetInventory, FromInventoryType, FromSlotIndex, ToInventoryType,
	                             ToSlotIndex,
	                             bIsFullStack, RequestingPlayer);
	SetFuelSlot();
}

bool UTSLampInventory::CanPlaceItemInSlot(int32 StaticDataID, EInventoryType InventoryType, int32 SlotIndex,
                                          bool IsTarget)
{
	if (Super::CanPlaceItemInSlot(StaticDataID, InventoryType, SlotIndex, IsTarget))
	{
		if (StaticDataID != MaintenanceCostID)
		{
			return false;
		}
		if (IsTarget)
		{
			if (GetSlot(InventoryType, SlotIndex).CurrentStackSize >= MaintenanceCostQty)
			{
				return false;
			}
		}
	}
	OnFuelTransferred.Broadcast();
	return true;
}

void UTSLampInventory::ClearSlot(FSlotStructMaster& Slot)
{
	Super::ClearSlot(Slot);
	Slot.ItemData.StaticDataID = MaintenanceCostID;
	Slot.MaxStackSize = MaintenanceCostQty;
}

void UTSLampInventory::SetFuelSlot()
{
	BagInventory.InventorySlotContainer[0].ItemData.StaticDataID = MaintenanceCostID;
	BagInventory.InventorySlotContainer[0].MaxStackSize = MaintenanceCostQty;
}
