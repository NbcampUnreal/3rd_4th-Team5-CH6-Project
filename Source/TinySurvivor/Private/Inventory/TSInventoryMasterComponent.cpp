// TSInventoryMasterComponent.cpp

#include "Inventory/TSInventoryMasterComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Character/TSCharacter.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemData.h"

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

	// 아이템 데이터 서브시스템 초기화
	CachedIDS = GetItemDataSubsystem();
	if (!CachedIDS)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get ItemDataSubsystem!"));
	}

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

		// 가방 인벤토리 초기화
		BagInventory.InventoryType = EInventoryType::BackPack;

		if (InitialBagSlotCount > 0)
		{
			BagInventory.InventorySlotContainer.SetNum(InitialBagSlotCount);
			for (int32 i = 0; i < InitialBagSlotCount; ++i)
			{
				BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
			}
			UE_LOG(LogTemp, Log, TEXT("Inventory initialized: Hotkey=%d, Equipment=%d, Bag=%d"),
			       HotkeySlotCount, EquipmentSlotCount, InitialBagSlotCount);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Inventory initialized: Hotkey=%d, Equipment=%d, Bag=Disabled (0 slots)"),
			       HotkeySlotCount, EquipmentSlotCount);
		}
	}
}

// ========================================
// 리플리케이션 콜백
// ========================================

void UTSInventoryMasterComponent::OnRep_HotkeyInventory()
{
	OnInventoryUpdated.Broadcast(EInventoryType::HotKey);
}

void UTSInventoryMasterComponent::OnRep_EquipmentInventory()
{
	OnInventoryUpdated.Broadcast(EInventoryType::Equipment);
}

void UTSInventoryMasterComponent::OnRep_BagInventory()
{
	OnInventoryUpdated.Broadcast(EInventoryType::BackPack);
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

void UTSInventoryMasterComponent::ServerTransferItem_Implementation(
	UTSInventoryMasterComponent* SourceInventory,
	UTSInventoryMasterComponent* TargetInventory,
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	Internal_TransferItem(SourceInventory, TargetInventory, FromInventoryType, FromSlotIndex, ToInventoryType,
	                      ToSlotIndex,
	                      bIsFullStack);
}

bool UTSInventoryMasterComponent::ServerTransferItem_Validate(
	UTSInventoryMasterComponent* SourceInventory,
	UTSInventoryMasterComponent* TargetInventory,
	EInventoryType FromInventoryType, int32 FromSlotIndex,
	EInventoryType ToInventoryType, int32 ToSlotIndex,
	bool bIsFullStack)
{
	return TargetInventory != nullptr &&
		IsValidSlotIndex(FromInventoryType, FromSlotIndex);
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
	UE_LOG(LogTemp, Log, TEXT("Dropping item: ID=%d x%d"), Slot.StaticDataID, DropQuantity);

	Slot.CurrentStackSize -= DropQuantity;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot(Slot);
	}
}

bool UTSInventoryMasterComponent::ServerDropItemToWorld_Validate(
	EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
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

void UTSInventoryMasterComponent::ServerUseItem_Implementation(int32 SlotIndex)
{
	Internal_UseItem(SlotIndex);
}

bool UTSInventoryMasterComponent::ServerUseItem_Validate(int32 SlotIndex)
{
	return IsValidSlotIndex(EInventoryType::HotKey, SlotIndex);
}

// ========================================
// Internal 함수
// ========================================

void UTSInventoryMasterComponent::Internal_TransferItem(
	UTSInventoryMasterComponent* SourceInventory,
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
	if (!SourceInventory || !TargetInventory)
	{
		return;
	}

	if (!SourceInventory->IsValidSlotIndex(FromInventoryType, FromSlotIndex))
	{
		return;
	}

	if (!TargetInventory->IsValidSlotIndex(ToInventoryType, ToSlotIndex))
	{
		return;
	}

	FInventoryStructMaster* FromInventory = SourceInventory->GetInventoryByType(FromInventoryType);
	FInventoryStructMaster* ToInventory = TargetInventory->GetInventoryByType(ToInventoryType);

	if (!FromInventory || !ToInventory)
	{
		return;
	}

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
		if (!SourceInventory->CanPlaceItemInSlot(ToSlot.StaticDataID, FromInventoryType, FromSlotIndex))
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
}

void UTSInventoryMasterComponent::Internal_UseItem(int32 SlotIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Internal_UseItem called on client!"));
		return;
	}

	if (!IsValidSlotIndex(EInventoryType::HotKey, SlotIndex))
		return;

	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];

	if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
		return;

	// ========================================
	// 가방 아이템인 경우
	// ========================================
	if (IsItemBagType(Slot.StaticDataID))
	{
		bool bExpanded = ExpandBagInventory(BagSlotIncrement);

		if (bExpanded)
		{
			// 아이템 소비
			Slot.CurrentStackSize -= 1;
			if (Slot.CurrentStackSize <= 0)
			{
				ClearSlot(Slot);
			}

			UE_LOG(LogTemp, Log, TEXT("Bag item used: Expanded bag by %d slots (Total: %d/%d)"),
			       BagSlotIncrement, BagInventory.InventorySlotContainer.Num(), MaxBagSlotCount);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot use bag: Already at max capacity (%d)"), MaxBagSlotCount);
		}

		return;
	}

	// ========================================
	// 일반 소비 아이템인 경우
	// ========================================

	// TODO: 아이템 효과 적용 (HP 회복 등)

	// 아이템 소비
	Slot.CurrentStackSize -= 1;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot(Slot);
	}

	UE_LOG(LogTemp, Log, TEXT("Item used: ID=%d"), Slot.StaticDataID);
}

// ========================================
// 아이템 추가/제거
// ========================================

bool UTSInventoryMasterComponent::AddItem(int32 StaticDataID, int32 DynamicDataID, int32 Quantity)
{
	if (!GetOwner()->HasAuthority() || StaticDataID == INDEX_NONE || Quantity <= 0)
	{
		return false;
	}

	FItemData ItemInfo;
	if (!GetItemData(StaticDataID, ItemInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("AddItem failed: Item %d not found"), StaticDataID);
		return false;
	}

	int32 RemainingQuantity = Quantity;

	// 빈 슬롯 캐싱용
	TArray<int32> EmptyHotkeySlots;
	TArray<int32> EmptyBagSlots;

	// ========================================
	// 1단계: 핫키 스택 가능한 슬롯
	// ========================================
	if (ItemInfo.IsStackable())
	{
		for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[i];

			if (Slot.StaticDataID == StaticDataID && Slot.CurrentStackSize < ItemInfo.MaxStack)
			{
				int32 CanAdd = FMath::Min(RemainingQuantity, ItemInfo.MaxStack - Slot.CurrentStackSize);
				Slot.CurrentStackSize += CanAdd;
				RemainingQuantity -= CanAdd;

				if (RemainingQuantity <= 0)
					return true;
			}
			else if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
			{
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

	// ========================================
	// 2단계: 가방 스택 가능한 슬롯
	// ========================================
	if (ItemInfo.IsStackable() && BagInventory.InventorySlotContainer.Num() > 0)
	{
		for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];

			if (Slot.StaticDataID == StaticDataID && Slot.CurrentStackSize < ItemInfo.MaxStack)
			{
				int32 CanAdd = FMath::Min(RemainingQuantity, ItemInfo.MaxStack - Slot.CurrentStackSize);
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
	else if (!ItemInfo.IsStackable() && BagInventory.InventorySlotContainer.Num() > 0)
	{
		// 스택 불가하면 빈 슬롯만 수집
		for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
		{
			const FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];
			if (Slot.StaticDataID == INDEX_NONE || Slot.CurrentStackSize <= 0)
			{
				EmptyBagSlots.Add(i);
			}
		}
	}

	// ========================================
	// 3단계: 핫키 빈 슬롯
	// ========================================
	for (int32 SlotIndex : EmptyHotkeySlots)
	{
		if (RemainingQuantity <= 0) break;

		FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];
		Slot.StaticDataID = StaticDataID;
		Slot.DynamicDataID = DynamicDataID;
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(RemainingQuantity, ItemInfo.MaxStack) : 1;
		Slot.CurrentStackSize = AddAmount;
		RemainingQuantity -= AddAmount;
	}

	// ========================================
	// 4단계: 가방 빈 슬롯
	// ========================================
	for (int32 SlotIndex : EmptyBagSlots)
	{
		if (RemainingQuantity <= 0) break;

		FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[SlotIndex];
		Slot.StaticDataID = StaticDataID;
		Slot.DynamicDataID = DynamicDataID;
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(RemainingQuantity, ItemInfo.MaxStack) : 1;
		Slot.CurrentStackSize = AddAmount;
		RemainingQuantity -= AddAmount;
	}

	if (RemainingQuantity > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddItem partial: All slots full! Added %d, Failed %d"),
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
		ClearSlot(Slot);
		return true;
	}

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
	if (!GetItemData(StaticDataID, ItemInfo))
	{
		return false;
	}

	// TODO: 방어구 아이템 타입 검증, ItemData에 Enum추가되면 수정 예정
	// if (InventoryType == EInventoryType::Equipment)
	// {
	// 	const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	// 	if (!Inventory)
	// 		return false;
	//
	// 	ESlotType TargetSlotType = Inventory->InventorySlotContainer[SlotIndex].SlotType;
	//
	// 	아이템 카테고리 방어구인지, 슬롯 타입과 같은지 확인
	// }

	return true;
}

// ========================================
// 가방 시스템
// ========================================

bool UTSInventoryMasterComponent::ExpandBagInventory(int32 AdditionalSlots)
{
	if (!GetOwner()->HasAuthority())
		return false;

	int32 CurrentSlotCount = BagInventory.InventorySlotContainer.Num();
	int32 NewSlotCount = CurrentSlotCount + AdditionalSlots;

	if (NewSlotCount > MaxBagSlotCount)
	{
		NewSlotCount = MaxBagSlotCount;

		if (CurrentSlotCount >= MaxBagSlotCount)
		{
			return false;
		}
	}

	int32 OldSize = CurrentSlotCount;
	BagInventory.InventorySlotContainer.SetNum(NewSlotCount);

	for (int32 i = OldSize; i < NewSlotCount; ++i)
	{
		BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
	}

	OnBagSizeChanged.Broadcast(NewSlotCount);

	UE_LOG(LogTemp, Log, TEXT("Bag expanded: %d -> %d (Max: %d)"),
	       OldSize, NewSlotCount, MaxBagSlotCount);

	return true;
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

	UnequipCurrentItem();

	// TODO: GameplayAbility 연동 시 ASC 코드 추가

	UE_LOG(LogTemp, Log, TEXT("Equipped: ID=%d (Slot %d)"),
	       ActiveSlot.StaticDataID, ActiveHotkeyIndex);
}

void UTSInventoryMasterComponent::UnequipCurrentItem()
{
	if (!HasItemEquipped())
		return;

	// TODO: GameplayAbility 연동 시 ASC 코드 추가

	FSlotStructMaster ActiveSlot = GetActiveHotkeySlot();
	UE_LOG(LogTemp, Log, TEXT("Unequipped: ID=%d"), ActiveSlot.StaticDataID);
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
	if (!GetItemData(FromSlot.StaticDataID, ItemInfo))
	{
		return false;
	}

	if (!ItemInfo.IsStackable())
		return false;

	int32 MaxStack = ItemInfo.MaxStack;

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

UItemDataSubsystem* UTSInventoryMasterComponent::GetItemDataSubsystem() const
{
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		return GI->GetSubsystem<UItemDataSubsystem>();
	}
	return nullptr;
}

bool UTSInventoryMasterComponent::GetItemData(int32 StaticDataID, FItemData& OutData) const
{
	if (StaticDataID == INDEX_NONE)
	{
		return false;
	}

	if (!CachedIDS)
	{
		CachedIDS = GetItemDataSubsystem();
	}

	if (CachedIDS && CachedIDS->GetItemDataSafe(StaticDataID, OutData))
	{
		return true;
	}
	return false;
}

bool UTSInventoryMasterComponent::IsItemBagType(int32 StaticDataID) const
{
	// 멤버변수 BagItemID와 비교
	return StaticDataID == BagItemID;
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
