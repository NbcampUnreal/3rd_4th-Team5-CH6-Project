// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSLampInventory.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Runtime/ItemInstance.h"


UTSLampInventory::UTSLampInventory()
{
	PrimaryComponentTick.bCanEverTick = false;

	HotkeySlotCount = 0;
	InitialBagSlotCount = 1;
	MaxBagSlotCount = 1;
	SlotAccessType = ESlotAccessType::ReadWrite;
}

void UTSLampInventory::TransferItem(UTSInventoryMasterComponent* SourceInventory,UTSInventoryMasterComponent* TargetInventory,EInventoryType FromInventoryType, int32 FromSlotIndex,EInventoryType ToInventoryType, int32 ToSlotIndex,bool bIsFullStack, ATSPlayerController* RequestingPlayer)
{
	if (!GetOwner()->HasAuthority()) return;
	if (SourceInventory->GetSlot(FromInventoryType, FromSlotIndex).ItemData.StaticDataID != MaintenanceCostID) return;

	Super::TransferItem(SourceInventory, TargetInventory, FromInventoryType, FromSlotIndex, ToInventoryType, ToSlotIndex, bIsFullStack, RequestingPlayer);
	
	SetFuelSlot();
}

bool UTSLampInventory::CanPlaceItemInSlot_internal(int32 StaticDataID, EInventoryType InventoryType, int32 SlotIndex,bool IsTarget)
{
	if (Super::CanPlaceItemInSlot_internal(StaticDataID, InventoryType, SlotIndex, IsTarget))
	{
		if (StaticDataID != MaintenanceCostID) return false;
		if (IsTarget && GetSlot(InventoryType, SlotIndex).CurrentStackSize >= MaintenanceCostQty) return false;
	}
	
	OnFuelTransferred.Broadcast();
	return true;
}

void UTSLampInventory::ClearSlot_internal(FSlotStructMaster& Slot)
{
	Super::ClearSlot_internal(Slot);
	
	Slot.ItemData.StaticDataID = MaintenanceCostID;
	Slot.MaxStackSize = MaintenanceCostQty;
}

void UTSLampInventory::SetFuelSlot()
{
	BagInventory.InventorySlotContainer[0].ItemData.StaticDataID = MaintenanceCostID;
	BagInventory.InventorySlotContainer[0].MaxStackSize = MaintenanceCostQty;
}
