// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Struct/TSInventorySlot.h"

bool FSlotStructMaster::IsSlotEmpty()
{
	return ItemData.StaticDataID == 0 || CurrentStackSize <= 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool FInventoryStructMaster::IsValidSlotIndex(int32 InSlotIndex)
{
	return InventorySlotContainer.IsValidIndex(InSlotIndex);
}

bool FInventoryStructMaster::IsSlotEmpty(int32 InSlotIndex)
{
	if (!InventorySlotContainer.IsValidIndex(InSlotIndex)) return false;
	return InventorySlotContainer[InSlotIndex].IsSlotEmpty();
}

FSlotStructMaster& FInventoryStructMaster::GetSlot(int32 InSlotIndex)
{
	return InventorySlotContainer[InSlotIndex];
}
