// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/Item/TSInventoryHelperLibrary.h"

#include "A_FOR_COMMON/Library/Item/TSItemHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSInventoryMasterComponent.h"

bool UTSInventoryHelperLibrary::ExpandBagInventory_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32& InAdditionalSlots)
{
	if (!IsValid(InInventoryMasterComponent)) return false;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return false; 
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return false;

	
	int32 CurrentSlotCount = InInventoryMasterComponent->BagInventory.InventorySlotContainer.Num();
	int32 NewSlotCount = CurrentSlotCount + InAdditionalSlots;

	if (NewSlotCount > InInventoryMasterComponent->MaxBagSlotCount)
	{
		NewSlotCount = InInventoryMasterComponent->MaxBagSlotCount;

		if (CurrentSlotCount >= InInventoryMasterComponent->MaxBagSlotCount) return false;
	}

	int32 OldSize = CurrentSlotCount;
	InInventoryMasterComponent->BagInventory.InventorySlotContainer.SetNum(NewSlotCount);

	for (int32 i = OldSize; i < NewSlotCount; ++i)
	{
		InInventoryMasterComponent->BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
	}

	InInventoryMasterComponent->OnBagSizeChanged.Broadcast(NewSlotCount);

	return true;
}

FInventoryStructMaster* UTSInventoryHelperLibrary::GetInventoryByType_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, EInventoryType InInventoryType)
{
	if (!IsValid(InInventoryMasterComponent)) return nullptr;
	
	switch (InInventoryType)
	{
	case EInventoryType::HotKey:
		return &InInventoryMasterComponent->HotkeyInventory;
		
	case EInventoryType::Equipment:
		return &InInventoryMasterComponent->EquipmentInventory;
		
	case EInventoryType::BackPack:
		return &InInventoryMasterComponent->BagInventory;
		
	default:
		return nullptr;
	}
}

FSlotStructMaster UTSInventoryHelperLibrary::GetSlot_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent,EInventoryType InInventoryType, int32 InSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return FSlotStructMaster();
	
	FInventoryStructMaster* Inventory = GetInventoryByType_Lib(InInventoryMasterComponent,InInventoryType);
	if (!Inventory || !Inventory->IsValidSlotIndex(InSlotIndex)) return FSlotStructMaster();
	
	return Inventory->GetSlot(InSlotIndex);
	
}

int32 UTSInventoryHelperLibrary::GetItemCount_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 StaticDataID)
{
	if (!IsValid(InInventoryMasterComponent)) return 0;
	
	int32 ResultCount = 0;
	
	// 핫키 인벤토리 탐색
	for (const FSlotStructMaster& Slot : InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer)
	{
		if (Slot.ItemData.StaticDataID == StaticDataID)
		{
			ResultCount += Slot.CurrentStackSize;
		}
	}
	
	// 가방 인벤토리 탐색
	if (InInventoryMasterComponent->BagInventory.InventorySlotContainer.Num() == 0) return ResultCount;
	
	for (const FSlotStructMaster& Slot : InInventoryMasterComponent->BagInventory.InventorySlotContainer)
	{
		if (Slot.ItemData.StaticDataID == StaticDataID)
		{
			ResultCount += Slot.CurrentStackSize;
		}
	}
	return ResultCount;
	
}

FSlotStructMaster UTSInventoryHelperLibrary::GetActiveHotkeySlot_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent)
{
	if (!IsValid(InInventoryMasterComponent)) return FSlotStructMaster();
	
	if (InInventoryMasterComponent->ActiveHotkeyIndex >= 0 && InInventoryMasterComponent->ActiveHotkeyIndex < InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer.Num())
	{
		return InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer[InInventoryMasterComponent->ActiveHotkeyIndex];
	}
	return FSlotStructMaster();
	
}

bool UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent,EInventoryType InInventoryType, int32 InSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return false;
	
	FInventoryStructMaster* Inventory = GetInventoryByType_Lib(InInventoryMasterComponent, InInventoryType);
	if (!Inventory) return false;
	
	return Inventory->IsValidSlotIndex(InSlotIndex);
}

bool UTSInventoryHelperLibrary::IsSlotEmpty_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, EInventoryType InInventoryType, int32 InSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return false;
	if (!IsValidSlotIndex_Lib(InInventoryMasterComponent, InInventoryType, InSlotIndex)) return false;
	if (!GetInventoryByType_Lib(InInventoryMasterComponent,  InInventoryType)->IsValidSlotIndex(InSlotIndex)) return false;
	
	return GetInventoryByType_Lib(InInventoryMasterComponent, InInventoryType)->IsSlotEmpty(InSlotIndex);
	
}

bool UTSInventoryHelperLibrary::CanPlaceItemInSlot_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 StaticDataID, EInventoryType InventoryType, int32 SlotIndex, bool IsTarget)
{
	if (!IsValid(InInventoryMasterComponent)) return false;
	
	if (StaticDataID == 0 || !IsValidSlotIndex_Lib(InInventoryMasterComponent,InventoryType, SlotIndex)) return false;

	// 슬롯 접근 타입 확인
	if (IsTarget && GetSlot_Lib(InInventoryMasterComponent, InventoryType, SlotIndex).SlotAccessType == ESlotAccessType::ReadOnly) return false;

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent,StaticDataID, ItemInfo)) return false;

	// 방어구 아이템 타입 검증
	if (InventoryType == EInventoryType::Equipment)
	{
		if (ItemInfo.Category != EItemCategory::ARMOR) return false;
		
		const FInventoryStructMaster* Inventory = GetInventoryByType_Lib(InInventoryMasterComponent,InventoryType);
		if (!Inventory) return false;
		
		ESlotType TargetSlotType = Inventory->InventorySlotContainer[SlotIndex].SlotType;

		if (InInventoryMasterComponent->EquipmentSlotTypes[TargetSlotType] != ItemInfo.ArmorData.EquipSlot) return false;
	}

	return true;
}
