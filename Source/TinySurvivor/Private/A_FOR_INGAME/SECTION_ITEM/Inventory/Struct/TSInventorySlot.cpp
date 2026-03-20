// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Struct/TSInventorySlot.h"

bool FSlotStructMaster::IsSlotEmpty()
{
	return ItemData.StaticDataID == 0 || CurrentStackSize <= 0;
}

bool FInventoryStructMaster::IsValidSlotIndex(int32 SlotIndex)
{
	return SlotIndex >= 0 && SlotIndex < InventorySlotContainer.Num();
}

FSlotStructMaster& FInventoryStructMaster::GetSlot(int32 SlotIndex)
{
	return InventorySlotContainer[SlotIndex];
}
