// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/TSCraftingTableInventory.h"


// Sets default values for this component's properties
UTSCraftingTableInventory::UTSCraftingTableInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
	HotkeySlotCount = 0;
	InitialBagSlotCount = MAX_CRAFTING_SLOTS;
}

void UTSCraftingTableInventory::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		UsedSlot.Init(false, MAX_CRAFTING_SLOTS);
	}
}

void UTSCraftingTableInventory::PostInitProperties()
{
	Super::PostInitProperties();
	InitialBagSlotCount = MAX_CRAFTING_SLOTS;
}

int32 UTSCraftingTableInventory::GetorAssignSlotForPlayer(APlayerController* PC)
{
	if (!PC || !GetOwner()->HasAuthority())
	{
		return -1;
	}
	if (int32* ExistingSlot = PlayerSlotMap.Find(PC))
	{
		return *ExistingSlot;
	}

	int32 EmptySlot = GetUnusedSlot();
	if (EmptySlot == -1)
	{
		return -1;
	}
	PlayerSlotMap.Add(PC, EmptySlot);
	UsedSlot[EmptySlot] = true;
	return EmptySlot;
}

void UTSCraftingTableInventory::OnPlayerClosedUI(APlayerController* PC)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	ReleasePlayerSlot(PC);
}

int32 UTSCraftingTableInventory::PlaceCraftResult(APlayerController* PC, int32 ResultItemID, int32 Quantity)
{
	if (!GetOwner()->HasAuthority())
	{
		return -1;
	}
	if (!PC || ResultItemID == 0 || Quantity <= 0)
	{
		return -1;
	}

	int32 SlotIndex = GetorAssignSlotForPlayer(PC);
	if (SlotIndex == -1)
	{
		return -1;
	}

	if (!IsValidSlotIndex(EInventoryType::BackPack, SlotIndex))
	{
		return -1;
	}

	FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[SlotIndex];

	// 이전 아이템 남아있으면 월드에 드랍
	if (Slot.ItemData.StaticDataID != 0 && Slot.CurrentStackSize > 0)
	{
		ServerDropItemToWorld(EInventoryType::BackPack, SlotIndex, Slot.CurrentStackSize);
	}

	FItemInstance Result = FItemInstance(ResultItemID, GetWorld()->GetTimeSeconds());
	FItemData ItemInfo;
	if (!GetItemData(Result.StaticDataID, ItemInfo))
	{
		return -1;
	}
	// 제작 결과물 배치
	Slot.ItemData = Result;
	Slot.CurrentStackSize = Quantity;
	Slot.MaxStackSize = ItemInfo.MaxStack;
	Slot.bCanStack = ItemInfo.IsStackable();
	// 부패 시스템 적용
	if (ItemInfo.IsDecayEnabled())
	{
		Slot.ExpirationTime = GetWorld()->GetTimeSeconds()
			+ ItemInfo.ConsumableData.DecayRate;
	}
	HandleInventoryChanged();

	return SlotIndex;
}

int32 UTSCraftingTableInventory::GetUnusedSlot() const
{
	for (int32 i = 0; i < MAX_CRAFTING_SLOTS; ++i)
	{
		if (!UsedSlot[i])
		{
			return i;
		}
	}
	return -1;
}

void UTSCraftingTableInventory::ReleasePlayerSlot(APlayerController* PC)
{
	if (!PC)
	{
		return;
	}
	int32* SlotIndex = PlayerSlotMap.Find(PC);
	if (!SlotIndex)
	{
		return;
	}
	if (IsSlotEmpty(EInventoryType::BackPack, *SlotIndex))
	{
		UsedSlot[*SlotIndex] = false;
		PlayerSlotMap.Remove(PC);
	}
}
