// TSInventoryMasterComponent.cpp

#include "Inventory/TSInventoryMasterComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Character/TSCharacter.h"
// TODO: 아이템 컨테이너 매니저 include 추가
// #include "Manager/TSItemContainerManager.h"

UTSInventoryMasterComponent::UTSInventoryMasterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UTSInventoryMasterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTSInventoryMasterComponent, HotkeyInventory);
	DOREPLIFETIME(UTSInventoryMasterComponent, EquipmentInventory);
	DOREPLIFETIME(UTSInventoryMasterComponent, BagInventory);
	DOREPLIFETIME(UTSInventoryMasterComponent, ActiveHotkeyIndex);
}

void UTSInventoryMasterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		// 핫키 인벤토리 초기화
		HotkeyInventory.InventoryType = EInventoryType::HotKey;
		HotkeyInventory.InventorySlotContainer.SetNum(HotkeySlotCount);
		for (int32 i = 0; i < HotkeySlotCount; ++i)
		{
			HotkeyInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
		}

		// 장비 인벤토리 초기화
		EquipmentInventory.InventoryType = EInventoryType::Equipment;
		EquipmentInventory.InventorySlotContainer.SetNum(EquipmentSlotCount);
		for (int32 i = 0; i < EquipmentSlotTypes.Num() && i < EquipmentSlotCount; ++i)
		{
			EquipmentInventory.InventorySlotContainer[i].SlotType = EquipmentSlotTypes[i];
		}

		// 가방 인벤토리 (슬롯은 가방 장착 시 생성)
		BagInventory.InventoryType = EInventoryType::BackPack;

		UE_LOG(LogTemp, Log, TEXT("Inventory initialized: Hotkey=%d, Equipment=%d, Bag=Disabled"),
		       HotkeySlotCount, EquipmentSlotCount);
	}
}

// ========================================
// 리플리케이션 콜백
// ========================================

void UTSInventoryMasterComponent::OnRep_HotkeyInventory()
{
	for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
	{
		OnInventoryUpdated.Broadcast(EInventoryType::HotKey, i);
	}
}

void UTSInventoryMasterComponent::OnRep_EquipmentInventory()
{
	for (int32 i = 0; i < EquipmentInventory.InventorySlotContainer.Num(); ++i)
	{
		OnInventoryUpdated.Broadcast(EInventoryType::Equipment, i);
	}
}

void UTSInventoryMasterComponent::OnRep_BagInventory()
{
	for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
	{
		OnInventoryUpdated.Broadcast(EInventoryType::BackPack, i);
	}
}

void UTSInventoryMasterComponent::OnRep_ActiveHotkeyIndex()
{
	if (ActiveHotkeyIndex >= 0 && ActiveHotkeyIndex < HotkeyInventory.InventorySlotContainer.Num())
	{
		OnHotkeyActivated.Broadcast(ActiveHotkeyIndex, HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex]);
		EquipActiveHotkeyItem();
	}
	else
	{
		UnequipCurrentItem();
	}
}

// ========================================
// Server RPC
// ========================================

void UTSInventoryMasterComponent::ServerSwapSlots_Implementation(
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	Internal_SwapSlots(FromInventoryType, FromSlotIndex, ToInventoryType, ToSlotIndex, bIsFullStack);
}

bool UTSInventoryMasterComponent::ServerSwapSlots_Validate(
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	// 기본 인덱스 체크
	if (!IsValidSlotIndex(FromInventoryType, FromSlotIndex) ||
		!IsValidSlotIndex(ToInventoryType, ToSlotIndex))
	{
		return false;
	}

	if (FromInventoryType == EInventoryType::BackPack && !CanAccessBagInventory())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client tried to access disabled bag (From)"));
		return false;
	}

	if (ToInventoryType == EInventoryType::BackPack && !CanAccessBagInventory())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client tried to access disabled bag (To)"));
		return false;
	}

	return true;
}

void UTSInventoryMasterComponent::ServerTransferItem_Implementation(
	UTSInventoryMasterComponent* TargetInventory,
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	Internal_TransferItem(TargetInventory, FromInventoryType, FromSlotIndex, ToInventoryType, ToSlotIndex,
	                      bIsFullStack);
}

bool UTSInventoryMasterComponent::ServerTransferItem_Validate(
	UTSInventoryMasterComponent* TargetInventory,
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	if (!TargetInventory || !IsValidSlotIndex(FromInventoryType, FromSlotIndex))
	{
		return false;
	}

	if (FromInventoryType == EInventoryType::BackPack && !CanAccessBagInventory())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client tried to transfer from disabled bag"));
		return false;
	}

	if (ToInventoryType == EInventoryType::BackPack && !TargetInventory->CanAccessBagInventory())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client tried to transfer to disabled bag"));
		return false;
	}

	return true;
}

void UTSInventoryMasterComponent::ServerDropItemToWorld_Implementation(
	EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	if (!IsValidSlotIndex(InventoryType, SlotIndex))
		return;

	FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
		return;

	FSlotStructMaster& Slot = Inventory->InventorySlotContainer[SlotIndex];

	if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
		return;

	int32 DropQuantity = (Quantity <= 0) ? Slot.CurrentStackSize : FMath::Min(Quantity, Slot.CurrentStackSize);

	// TODO: 스폰 매니저에 아이템 액터 스폰 요청
	UE_LOG(LogTemp, Log, TEXT("Dropping item: %d x%d"), Slot.StaticDataID, DropQuantity);

	Slot.CurrentStackSize -= DropQuantity;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot(Slot);
	}
}

bool UTSInventoryMasterComponent::ServerDropItemToWorld_Validate(
	EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	if (InventoryType == EInventoryType::BackPack && !CanAccessBagInventory())
	{
		return false;
	}

	return IsValidSlotIndex(InventoryType, SlotIndex);
}

void UTSInventoryMasterComponent::ServerActivateHotkeySlot_Implementation(int32 SlotIndex)
{
	if (SlotIndex < -1 || SlotIndex >= HotkeyInventory.InventorySlotContainer.Num())
		return;

	ActiveHotkeyIndex = SlotIndex;
}

bool UTSInventoryMasterComponent::ServerActivateHotkeySlot_Validate(int32 SlotIndex)
{
	return SlotIndex >= -1 && SlotIndex < HotkeySlotCount;
}

// ========================================
// Internal 함수
// ========================================

void UTSInventoryMasterComponent::Internal_SwapSlots(
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Internal_SwapSlots called on client!"));
		return;
	}

	if (!IsValidSlotIndex(FromInventoryType, FromSlotIndex) ||
		!IsValidSlotIndex(ToInventoryType, ToSlotIndex))
	{
		return;
	}

	FInventoryStructMaster* FromInventory = GetInventoryByType(FromInventoryType);
	FInventoryStructMaster* ToInventory = GetInventoryByType(ToInventoryType);

	if (!FromInventory || !ToInventory)
		return;

	FSlotStructMaster& FromSlot = FromInventory->InventorySlotContainer[FromSlotIndex];
	FSlotStructMaster& ToSlot = ToInventory->InventorySlotContainer[ToSlotIndex];

	// 타입 검증
	if (FromSlot.StaticDataID != INDEX_NONE)
	{
		if (!CanPlaceItemInSlot(FromSlot.StaticDataID, ToInventoryType, ToSlotIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot place item %d in target slot"), FromSlot.StaticDataID);
			return;
		}
	}

	if (ToSlot.StaticDataID != INDEX_NONE)
	{
		if (!CanPlaceItemInSlot(ToSlot.StaticDataID, FromInventoryType, FromSlotIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot place item %d in source slot"), ToSlot.StaticDataID);
			return;
		}
	}

	// 스택 처리 시도
	if (TryStackSlots(FromSlot, ToSlot, bIsFullStack))
	{
		// 스택 완료
	}
	else
	{
		// 일반 교환
		if (bIsFullStack)
		{
			// 전체 교환
			FSlotStructMaster Temp = FromSlot;
			FromSlot = ToSlot;
			ToSlot = Temp;
		}
		else
		{
			if (FromSlot.StaticDataID != INDEX_NONE && ToSlot.StaticDataID == INDEX_NONE)
			{
				CopySlotData(FromSlot, ToSlot, 1);
				FromSlot.CurrentStackSize -= 1;

				if (FromSlot.CurrentStackSize <= 0)
				{
					ClearSlot(FromSlot);
				}
			}
		}
	}

	// 가방 상태 업데이트
	if (FromInventoryType == EInventoryType::Equipment ||
		ToInventoryType == EInventoryType::Equipment)
	{
		UpdateBagInventoryState();
	}
}

void UTSInventoryMasterComponent::Internal_TransferItem(
	UTSInventoryMasterComponent* TargetInventory,
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Internal_TransferItem called on client!"));
		return;
	}

	if (!TargetInventory || !IsValidSlotIndex(FromInventoryType, FromSlotIndex))
		return;

	if (!TargetInventory->IsValidSlotIndex(ToInventoryType, ToSlotIndex))
		return;

	FInventoryStructMaster* FromInventory = GetInventoryByType(FromInventoryType);
	FInventoryStructMaster* ToInventory = TargetInventory->GetInventoryByType(ToInventoryType);

	if (!FromInventory || !ToInventory)
		return;

	FSlotStructMaster& FromSlot = FromInventory->InventorySlotContainer[FromSlotIndex];
	FSlotStructMaster& ToSlot = ToInventory->InventorySlotContainer[ToSlotIndex];

	// 타입 검증
	if (FromSlot.StaticDataID != INDEX_NONE)
	{
		if (!TargetInventory->CanPlaceItemInSlot(FromSlot.StaticDataID, ToInventoryType, ToSlotIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot place item in target inventory"));
			return;
		}
	}

	if (ToSlot.StaticDataID != INDEX_NONE)
	{
		if (!CanPlaceItemInSlot(ToSlot.StaticDataID, FromInventoryType, FromSlotIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot place item in source inventory"));
			return;
		}
	}

	// 스택 처리 시도
	if (TryStackSlots(FromSlot, ToSlot, bIsFullStack))
	{
		// 스택 완료
	}
	else
	{
		// 일반 교환
		if (bIsFullStack)
		{
			FSlotStructMaster Temp = FromSlot;
			FromSlot = ToSlot;
			ToSlot = Temp;
		}
		else
		{
			if (FromSlot.StaticDataID != INDEX_NONE && ToSlot.StaticDataID == INDEX_NONE)
			{
				CopySlotData(FromSlot, ToSlot, 1);
				FromSlot.CurrentStackSize -= 1;

				if (FromSlot.CurrentStackSize <= 0)
				{
					ClearSlot(FromSlot);
				}
			}
		}
	}

	// 가방 상태 업데이트 (양쪽)
	if (FromInventoryType == EInventoryType::Equipment)
	{
		UpdateBagInventoryState();
	}
	if (ToInventoryType == EInventoryType::Equipment)
	{
		TargetInventory->UpdateBagInventoryState();
	}
}

// ========================================
// 아이템 추가/제거
// ========================================

bool UTSInventoryMasterComponent::AddItem(int32 StaticDataID, int32 DynamicDataID, int32 Quantity)
{
	if (!GetOwner()->HasAuthority() || StaticDataID == INDEX_NONE || Quantity <= 0)
		return false;

	FItemData ItemInfo;
	if (!GetItemSlotInfo(StaticDataID, ItemInfo))
	{
		return false;
	}

	int32 RemainingQuantity = Quantity;

	// 빈 슬롯 캐싱용
	TArray<int32> EmptyHotkeySlots;
	TArray<int32> EmptyBagSlots;

	// 핫키 순회
	if (ItemInfo.bCanStack)
	{
		for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[i];

			if (Slot.StaticDataID == StaticDataID && Slot.CurrentStackSize < ItemInfo.StackSize)
			{
				// 스택 가능한 슬롯 발견
				int32 CanAdd = FMath::Min(RemainingQuantity, ItemInfo.StackSize - Slot.CurrentStackSize);
				Slot.CurrentStackSize += CanAdd;
				RemainingQuantity -= CanAdd;

				if (RemainingQuantity <= 0)
					return true;
			}
			else if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
			{
				// 빈 슬롯 발견
				EmptyHotkeySlots.Add(i);
			}
		}
	}
	else
	{
		// 스택 불가하면 빈 슬롯만 수집
		for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
		{
			const FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[i];
			if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
			{
				EmptyHotkeySlots.Add(i);
			}
		}
	}

	// 가방 순회 (활성화된 경우만)
	if (CanAccessBagInventory())
	{
		if (ItemInfo.bCanStack)
		{
			for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
			{
				FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];

				if (Slot.StaticDataID == StaticDataID && Slot.CurrentStackSize < ItemInfo.StackSize)
				{
					int32 CanAdd = FMath::Min(RemainingQuantity, ItemInfo.StackSize - Slot.CurrentStackSize);
					Slot.CurrentStackSize += CanAdd;
					RemainingQuantity -= CanAdd;

					if (RemainingQuantity <= 0)
						return true;
				}
				else if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
				{
					EmptyBagSlots.Add(i);
				}
			}
		}
		else
		{
			for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
			{
				const FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];
				if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
				{
					EmptyBagSlots.Add(i);
				}
			}
		}
	}

	// 빈 슬롯 사용 (핫키 우선)
	for (int32 SlotIndex : EmptyHotkeySlots)
	{
		if (RemainingQuantity <= 0) break;

		FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];
		Slot.StaticDataID = StaticDataID;
		Slot.DynamicDataID = DynamicDataID;
		Slot.bCanStack = ItemInfo.bCanStack;
		Slot.MaxStackSize = ItemInfo.StackSize;

		int32 AddAmount = ItemInfo.bCanStack ? FMath::Min(RemainingQuantity, ItemInfo.StackSize) : 1;
		Slot.CurrentStackSize = AddAmount;
		RemainingQuantity -= AddAmount;
	}

	// 가방 빈 슬롯 사용
	for (int32 SlotIndex : EmptyBagSlots)
	{
		if (RemainingQuantity <= 0) break;

		FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[SlotIndex];
		Slot.StaticDataID = StaticDataID;
		Slot.DynamicDataID = DynamicDataID;
		Slot.bCanStack = ItemInfo.bCanStack;
		Slot.MaxStackSize = ItemInfo.StackSize;

		int32 AddAmount = ItemInfo.bCanStack ? FMath::Min(RemainingQuantity, ItemInfo.StackSize) : 1;
		Slot.CurrentStackSize = AddAmount;
		RemainingQuantity -= AddAmount;
	}

	if (RemainingQuantity > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddItem partial: Added %d, Failed %d"),
		       Quantity - RemainingQuantity, RemainingQuantity);
	}

	return RemainingQuantity == 0;
}

bool UTSInventoryMasterComponent::RemoveItem(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	if (!GetOwner()->HasAuthority() || !IsValidSlotIndex(InventoryType, SlotIndex))
		return false;

	FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
		return false;

	FSlotStructMaster& Slot = Inventory->InventorySlotContainer[SlotIndex];

	if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
		return false;

	if (Quantity <= 0)
	{
		// 전체 제거
		ClearSlot(Slot);
		return true;
	}

	// 부분 제거
	Slot.CurrentStackSize -= Quantity;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot(Slot);
	}

	return true;
}

// ========================================
// 슬롯 조회
// ========================================

FSlotStructMaster UTSInventoryMasterComponent::GetSlot(EInventoryType InventoryType, int32 SlotIndex) const
{
	if (!IsValidSlotIndex(InventoryType, SlotIndex))
		return FSlotStructMaster();

	if (InventoryType == EInventoryType::BackPack && !CanAccessBagInventory())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot access bag: No bag equipped"));
		return FSlotStructMaster();
	}

	const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
		return FSlotStructMaster();

	return Inventory->InventorySlotContainer[SlotIndex];
}

bool UTSInventoryMasterComponent::IsSlotEmpty(EInventoryType InventoryType, int32 SlotIndex) const
{
	FSlotStructMaster Slot = GetSlot(InventoryType, SlotIndex);
	return Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0;
}

int32 UTSInventoryMasterComponent::FindEmptySlot(EInventoryType InventoryType) const
{
	const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
		return -1;

	for (int32 i = 0; i < Inventory->InventorySlotContainer.Num(); ++i)
	{
		const FSlotStructMaster& Slot = Inventory->InventorySlotContainer[i];
		if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
		{
			return i;
		}
	}

	return -1;
}

// ========================================
// 타입 검증
// ========================================

bool UTSInventoryMasterComponent::CanPlaceItemInSlot(
	int32 StaticDataID,
	EInventoryType InventoryType,
	int32 SlotIndex) const
{
	if (StaticDataID == INDEX_NONE || !IsValidSlotIndex(InventoryType, SlotIndex))
		return false;

	FItemData ItemInfo;
	if (!GetItemSlotInfo(StaticDataID, ItemInfo))
	{
		return false;
	}

	// 인벤토리 타입 체크
	if (!ItemInfo.InventoryType.Contains(InventoryType) && !ItemInfo.InventoryType.Contains(EInventoryType::Any))
	{
		return false;
	}

	// 장비 슬롯인 경우 슬롯 타입도 체크
	if (InventoryType == EInventoryType::Equipment)
	{
		const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
		if (!Inventory)
			return false;

		ESlotType TargetSlotType = Inventory->InventorySlotContainer[SlotIndex].SlotType;

		if (TargetSlotType != ESlotType::Any)
		{
			if (!ItemInfo.SlotType.Contains(TargetSlotType) && !ItemInfo.SlotType.Contains(ESlotType::Any))
			{
				return false;
			}
		}
	}

	return true;
}

// ========================================
// 가방 시스템
// ========================================

bool UTSInventoryMasterComponent::IsBagEquipped() const
{
	for (const FSlotStructMaster& Slot : EquipmentInventory.InventorySlotContainer)
	{
		if (Slot.SlotType == ESlotType::BackPack &&
			Slot.StaticDataID != INDEX_NONE &&
			Slot.CurrentStackSize > 0)
		{
			return true;
		}
	}
	return false;
}

int32 UTSInventoryMasterComponent::GetEquippedBagSlotCount() const
{
	int32 BagItemID = GetEquippedBagItemID();
	if (BagItemID == INDEX_NONE)
		return 0;

	return GetBagSlotCount(BagItemID);
}

void UTSInventoryMasterComponent::UpdateBagInventoryState()
{
	if (!GetOwner()->HasAuthority())
		return;

	bool bBagEquipped = IsBagEquipped();
	int32 BagSlotCount = GetEquippedBagSlotCount();
	int32 CurrentBagSlotCount = BagInventory.InventorySlotContainer.Num();

	if (bBagEquipped && BagSlotCount > 0)
	{
		// 가방 장착됨
		if (CurrentBagSlotCount == 0)
		{
			// 가방 활성화
			BagInventory.InventorySlotContainer.SetNum(BagSlotCount);

			for (int32 i = 0; i < BagSlotCount; ++i)
			{
				BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
			}

			OnBagSizeChanged.Broadcast(BagSlotCount);
			UE_LOG(LogTemp, Log, TEXT("Bag activated: %d slots"), BagSlotCount);
		}
		else if (CurrentBagSlotCount != BagSlotCount)
		{
			// 가방 크기 변경
			if (BagSlotCount < CurrentBagSlotCount)
			{
				// 축소: 아이템 드롭 필요
				for (int32 i = BagSlotCount; i < CurrentBagSlotCount; ++i)
				{
					FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];
					if (Slot.StaticDataID != INDEX_NONE && Slot.CurrentStackSize > 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("Dropping item from bag slot %d"), i);
						// TODO: 월드에 드롭
					}
				}
			}

			int32 OldSize = CurrentBagSlotCount;
			BagInventory.InventorySlotContainer.SetNum(BagSlotCount);

			if (BagSlotCount > OldSize)
			{
				for (int32 i = OldSize; i < BagSlotCount; ++i)
				{
					BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
				}
			}

			OnBagSizeChanged.Broadcast(BagSlotCount);
			UE_LOG(LogTemp, Log, TEXT("Bag resized: %d -> %d"), OldSize, BagSlotCount);
		}
	}
	else
	{
		// 가방 해제
		if (CurrentBagSlotCount > 0)
		{
			// 아이템 드롭 필요
			for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
			{
				FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];
				if (Slot.StaticDataID != INDEX_NONE && Slot.CurrentStackSize > 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Dropping item from bag slot %d (bag unequipped)"), i);
					// TODO: 월드에 드롭
				}
			}

			BagInventory.InventorySlotContainer.Empty();
			OnBagSizeChanged.Broadcast(0);
			UE_LOG(LogTemp, Log, TEXT("Bag deactivated"));
		}
	}
}

bool UTSInventoryMasterComponent::CanAccessBagInventory() const
{
	return IsBagEquipped() && BagInventory.InventorySlotContainer.Num() > 0;
}

// ========================================
// 핫키 아이템 장착 시스템
// ========================================

FSlotStructMaster UTSInventoryMasterComponent::GetActiveHotkeySlot() const
{
	if (ActiveHotkeyIndex >= 0 && ActiveHotkeyIndex < HotkeyInventory.InventorySlotContainer.Num())
	{
		return HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];
	}
	return FSlotStructMaster();
}

bool UTSInventoryMasterComponent::HasItemEquipped() const
{
	FSlotStructMaster ActiveSlot = GetActiveHotkeySlot();
	return ActiveSlot.StaticDataID != INDEX_NONE && ActiveSlot.CurrentStackSize > 0;
}

void UTSInventoryMasterComponent::EquipActiveHotkeyItem()
{
	if (ActiveHotkeyIndex < 0 || ActiveHotkeyIndex >= HotkeyInventory.InventorySlotContainer.Num())
	{
		UnequipCurrentItem();
		return;
	}

	const FSlotStructMaster& ActiveSlot = HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];

	if (ActiveSlot.StaticDataID == INDEX_NONE || ActiveSlot.CurrentStackSize <= 0)
	{
		UnequipCurrentItem();
		return;
	}

	// 이전 아이템 해제
	UnequipCurrentItem();

	// GameplayAbility 실행
	UAbilitySystemComponent* ASC = GetASC();
	// TODO: 장착 어빌리티 실행

	UE_LOG(LogTemp, Log, TEXT("Equipped: %d (Slot %d)"),
	       ActiveSlot.StaticDataID, ActiveHotkeyIndex);
}

void UTSInventoryMasterComponent::UnequipCurrentItem()
{
	if (!HasItemEquipped())
		return;

	UAbilitySystemComponent* ASC = GetASC();
	// TODO: 장착 해제 어빌리티 실행

	FSlotStructMaster ActiveSlot = GetActiveHotkeySlot();
	UE_LOG(LogTemp, Log, TEXT("Unequipped: %d"), ActiveSlot.StaticDataID);
}

// ========================================
// 헬퍼 함수 - 슬롯 조작
// ========================================

void UTSInventoryMasterComponent::ClearSlot(FSlotStructMaster& Slot)
{
	Slot.StaticDataID = INDEX_NONE;
	Slot.DynamicDataID = INDEX_NONE;
	Slot.CurrentStackSize = 0;
}

void UTSInventoryMasterComponent::CopySlotData(
	const FSlotStructMaster& Source,
	FSlotStructMaster& Target,
	int32 Quantity)
{
	Target.StaticDataID = Source.StaticDataID;
	Target.DynamicDataID = Source.DynamicDataID;
	Target.bCanStack = Source.bCanStack;
	Target.MaxStackSize = Source.MaxStackSize;
	Target.CurrentStackSize = (Quantity < 0) ? Source.CurrentStackSize : Quantity;
}

bool UTSInventoryMasterComponent::TryStackSlots(
	FSlotStructMaster& FromSlot,
	FSlotStructMaster& ToSlot,
	bool bIsFullStack)
{
	if (FromSlot.StaticDataID == INDEX_NONE || ToSlot.StaticDataID == INDEX_NONE ||
		FromSlot.StaticDataID != ToSlot.StaticDataID)
	{
		return false;
	}

	FItemData ItemInfo;
	if (!GetItemSlotInfo(FromSlot.StaticDataID, ItemInfo))
	{
		return false;
	}

	if (!ItemInfo.bCanStack)
		return false;

	int32 MaxStack = ItemInfo.StackSize;

	if (bIsFullStack)
	{
		int32 CanAdd = FMath::Min(FromSlot.CurrentStackSize, MaxStack - ToSlot.CurrentStackSize);
		if (CanAdd > 0)
		{
			ToSlot.CurrentStackSize += CanAdd;
			FromSlot.CurrentStackSize -= CanAdd;

			if (FromSlot.CurrentStackSize <= 0)
			{
				ClearSlot(FromSlot);
			}
			return true;
		}
	}
	else
	{
		if (ToSlot.CurrentStackSize < MaxStack)
		{
			ToSlot.CurrentStackSize += 1;
			FromSlot.CurrentStackSize -= 1;

			if (FromSlot.CurrentStackSize <= 0)
			{
				ClearSlot(FromSlot);
			}
			return true;
		}
	}

	return false;
}

// ========================================
// 헬퍼 함수 - 인벤토리
// ========================================

FInventoryStructMaster* UTSInventoryMasterComponent::GetInventoryByType(EInventoryType InventoryType)
{
	switch (InventoryType)
	{
	case EInventoryType::HotKey:
		return &HotkeyInventory;
	case EInventoryType::Equipment:
		return &EquipmentInventory;
	case EInventoryType::BackPack:
		return &BagInventory;
	default:
		return nullptr;
	}
}

const FInventoryStructMaster* UTSInventoryMasterComponent::GetInventoryByType(EInventoryType InventoryType) const
{
	switch (InventoryType)
	{
	case EInventoryType::HotKey:
		return &HotkeyInventory;
	case EInventoryType::Equipment:
		return &EquipmentInventory;
	case EInventoryType::BackPack:
		return &BagInventory;
	default:
		return nullptr;
	}
}

bool UTSInventoryMasterComponent::IsValidSlotIndex(EInventoryType InventoryType, int32 SlotIndex) const
{
	const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
		return false;

	return SlotIndex >= 0 && SlotIndex < Inventory->InventorySlotContainer.Num();
}

// ========================================
// 헬퍼 함수 - 아이템 정보
// ========================================

bool UTSInventoryMasterComponent::GetItemSlotInfo(int32 StaticDataID, FItemData& OutData) const
{
	// TODO: 아이템 컨테이너 매니저에서 조회
	// return ItemContainerManager->GetItemSlotInfo(StaticDataID, OutData);

	OutData.InventoryType = {EInventoryType::Any};
	OutData.SlotType = {ESlotType::Any};
	OutData.bCanStack = false;
	OutData.StackSize = 1;

	return true;
}

int32 UTSInventoryMasterComponent::GetEquippedBagItemID() const
{
	for (const FSlotStructMaster& Slot : EquipmentInventory.InventorySlotContainer)
	{
		if (Slot.SlotType == ESlotType::BackPack &&
			Slot.StaticDataID != INDEX_NONE &&
			Slot.CurrentStackSize > 0)
		{
			return Slot.StaticDataID;
		}
	}
	return INDEX_NONE;
}

int32 UTSInventoryMasterComponent::GetBagSlotCount(int32 BagItemID) const
{
	if (BagItemID == INDEX_NONE)
		return 0;

	// TODO: 아이템 컨테이너 매니저에서 조회
	// return ItemContainerManager->GetBagSlotCount(BagItemID);

	return 20;
}

UAbilitySystemComponent* UTSInventoryMasterComponent::GetASC()
{
	ATSCharacter* PC = Cast<ATSCharacter>(GetOwner());
	if (!PC)
		return nullptr;

	UAbilitySystemComponent* ASC = PC->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("No AbilitySystemComponent found"));
		return nullptr;
	}
	return ASC;
}
