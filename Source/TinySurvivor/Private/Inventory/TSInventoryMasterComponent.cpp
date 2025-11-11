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
		// TODO : 부패도 매니저와 OnDecayTick 바인딩
		
		// 핫키 인벤토리 초기화
		HotkeyInventory.InventoryType = EInventoryType::HotKey;
		HotkeyInventory.InventorySlotContainer.SetNum(HotkeySlotCount);
		for (int32 i = 0; i < HotkeySlotCount; ++i)
		{
			HotkeyInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
		}

		// 장비 인벤토리 초기화
		EquipmentInventory.InventoryType = EInventoryType::Equipment;
		EquipmentInventory.InventorySlotContainer.SetNum(EquipmentSlotTypes.Num());
		int32 idx = 0;
		for (const auto& Pair : EquipmentSlotTypes)
		{
			EquipmentInventory.InventorySlotContainer[idx].SlotType = Pair.Key;
			++idx;
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
			       HotkeySlotCount, EquipmentSlotTypes.Num(), InitialBagSlotCount);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Inventory initialized: Hotkey=%d, Equipment=%d, Bag=Disabled (0 slots)"),
			       HotkeySlotCount, EquipmentSlotTypes.Num());
		}
	}
}

#pragma region Replication Callback
void UTSInventoryMasterComponent::OnRep_HotkeyInventory()
{
	HandleInventoryChanged();
}

void UTSInventoryMasterComponent::OnRep_EquipmentInventory()
{
	HandleInventoryChanged();
}

void UTSInventoryMasterComponent::OnRep_BagInventory()
{
	HandleInventoryChanged();
}

void UTSInventoryMasterComponent::OnRep_ActiveHotkeyIndex()
{
	HandleActiveHotkeyIndexChanged();
}
#pragma endregion

#pragma region Server RPC
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
	if (!SourceInventory || !TargetInventory)
	{
		return false;
	}
	return SourceInventory->IsValidSlotIndex(FromInventoryType, FromSlotIndex) &&
		TargetInventory->IsValidSlotIndex(ToInventoryType, ToSlotIndex);
}

void UTSInventoryMasterComponent::ServerDropItemToWorld_Implementation(
	EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	if (!IsValidSlotIndex(InventoryType, SlotIndex))
	{
		return;
	}

	FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
	{
		return;
	}

	FSlotStructMaster& Slot = Inventory->InventorySlotContainer[SlotIndex];

	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
	{
		return;
	}

	int32 DropQuantity = (Quantity <= 0) ? Slot.CurrentStackSize : FMath::Min(Quantity, Slot.CurrentStackSize);

	// TODO: 스폰 매니저에 아이템 액터 스폰 요청
	UE_LOG(LogTemp, Log, TEXT("Dropping item: ID=%d x%d"), Slot.ItemData.StaticDataID, DropQuantity);

	Slot.CurrentStackSize -= DropQuantity;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot(Slot);
	}

	HandleInventoryChanged();
}

bool UTSInventoryMasterComponent::ServerDropItemToWorld_Validate(
	EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	return IsValidSlotIndex(InventoryType, SlotIndex);
}

void UTSInventoryMasterComponent::ServerActivateHotkeySlot_Implementation(int32 SlotIndex)
{
	if (SlotIndex < -1 || SlotIndex >= HotkeyInventory.InventorySlotContainer.Num())
	{
		return;
	}

	ActiveHotkeyIndex = SlotIndex;

	HandleActiveHotkeyIndexChanged();
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
#pragma endregion

#pragma region Internal function
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
	if (FromSlot.ItemData.StaticDataID != 0)
	{
		if (!TargetInventory->CanPlaceItemInSlot(FromSlot.ItemData.StaticDataID, ToInventoryType, ToSlotIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot place item in target inventory"));
			return;
		}
	}

	if (ToSlot.ItemData.StaticDataID != 0)
	{
		if (!SourceInventory->CanPlaceItemInSlot(ToSlot.ItemData.StaticDataID, FromInventoryType, FromSlotIndex))
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
			if (FromSlot.ItemData.StaticDataID != 0 && ToSlot.ItemData.StaticDataID == 0)
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

	// 소스 인벤토리의 델리게이트 브로드캐스트
	if (SourceInventory)
	{
		SourceInventory->HandleInventoryChanged();
		SourceInventory->GetOwner()->ForceNetUpdate();
	}
	// 타겟 인벤토리의 델리게이트 브로드캐스트
	if (TargetInventory && TargetInventory != SourceInventory)
	{
		TargetInventory->HandleInventoryChanged();
		TargetInventory->GetOwner()->ForceNetUpdate();
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
	{
		return;
	}

	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];

	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
	{
		return;
	}

	// ========================================
	// 가방 아이템인 경우
	// ========================================
	if (IsItemBagType(Slot.ItemData.StaticDataID))
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

			HandleInventoryChanged();

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
	// 아이템 사용 어빌리티 활성화
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UseItem failed: No ASC found"));
		return;
	}
	FItemData ItemInfo;
	if (!GetItemData(Slot.ItemData.StaticDataID, ItemInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("UseItem failed: Invalid ItemData for ID=%d"), Slot.ItemData.StaticDataID);
		return;
	}
	if (ItemInfo.AbilityBP)
	{
		FGameplayAbilitySpec Spec(ItemInfo.AbilityBP, 1, 0);
		ASC->GiveAbilityAndActivateOnce(Spec);
	}
	// 아이템 소비
	Slot.CurrentStackSize -= 1;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot(Slot);
	}

	UE_LOG(LogTemp, Log, TEXT("Item used: ID=%d"), Slot.ItemData.StaticDataID);

	HandleInventoryChanged();
	GetOwner()->ForceNetUpdate();
}
#pragma endregion

#pragma region Add/Remove Item
bool UTSInventoryMasterComponent::AddItem(FItemInstance& ItemData, int32 Quantity)
{
	if (!GetOwner()->HasAuthority() || ItemData.StaticDataID == 0 || Quantity <= 0)
	{
		return false;
	}

	FItemData ItemInfo;
	if (!GetItemData(ItemData.StaticDataID, ItemInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("AddItem failed: Item %d not found"), ItemData.StaticDataID);
		return false;
	}

	int32 RemainingQuantity = Quantity;

	// 빈 슬롯 캐싱용
	TArray<int32> EmptyHotkeySlots;
	TArray<int32> EmptyBagSlots;

	// 인벤토리 변경 플래그
	bool bInventoryChanged = false;

	// ========================================
	// 1단계: 핫키 스택 가능한 슬롯
	// ========================================
	if (ItemInfo.IsStackable())
	{
		for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[i];

			if (Slot.ItemData.StaticDataID == ItemData.StaticDataID && Slot.CurrentStackSize < ItemInfo.MaxStack)
			{
				int32 CanAdd = FMath::Min(RemainingQuantity, ItemInfo.MaxStack - Slot.CurrentStackSize);
				// 부패 만료 시각 업데이트
				Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
					                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, CanAdd,
					                                             ItemInfo.ConsumableData.DecayRate)
					                      : 0;
				Slot.CurrentStackSize += CanAdd;
				RemainingQuantity -= CanAdd;
				bInventoryChanged = true;

				if (RemainingQuantity <= 0)
				{
					break;
				}
			}
			else if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
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
			if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
			{
				EmptyHotkeySlots.Add(i);
			}
		}
	}

	// ========================================
	// 2단계: 가방 스택 가능한 슬롯
	// ========================================
	if (RemainingQuantity > 0 && ItemInfo.IsStackable() && BagInventory.InventorySlotContainer.Num() > 0)
	{
		for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];

			if (Slot.ItemData.StaticDataID == ItemData.StaticDataID && Slot.CurrentStackSize < ItemInfo.MaxStack)
			{
				int32 CanAdd = FMath::Min(RemainingQuantity, ItemInfo.MaxStack - Slot.CurrentStackSize);
				// 부패 만료 시각 업데이트
				Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
					                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, CanAdd,
					                                             ItemInfo.ConsumableData.DecayRate)
					                      : 0;
				Slot.CurrentStackSize += CanAdd;
				RemainingQuantity -= CanAdd;
				bInventoryChanged = true;

				if (RemainingQuantity <= 0)
				{
					break;
				}
			}
			else if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
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
			if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
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
		if (RemainingQuantity <= 0)
		{
			break;
		}

		bInventoryChanged = true;

		FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];
		Slot.ItemData = ItemData;
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(RemainingQuantity, ItemInfo.MaxStack) : 1;
		// 부패 만료 시각 업데이트
		Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
			                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, AddAmount,
			                                             ItemInfo.ConsumableData.DecayRate)
			                      : 0;
		Slot.CurrentStackSize = AddAmount;
		RemainingQuantity -= AddAmount;
	}

	// ========================================
	// 4단계: 가방 빈 슬롯
	// ========================================
	for (int32 SlotIndex : EmptyBagSlots)
	{
		if (RemainingQuantity <= 0)
		{
			break;
		}

		bInventoryChanged = true;

		FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[SlotIndex];
		Slot.ItemData = ItemData;
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(RemainingQuantity, ItemInfo.MaxStack) : 1;
		// 부패 만료 시각 업데이트
		Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
			                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, AddAmount,
			                                             ItemInfo.ConsumableData.DecayRate)
			                      : 0;
		Slot.CurrentStackSize = AddAmount;
		RemainingQuantity -= AddAmount;
	}

	if (RemainingQuantity > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddItem partial: All slots full! Added %d, Failed %d"),
		       Quantity - RemainingQuantity, RemainingQuantity);
	}

	if (bInventoryChanged)
	{
		HandleInventoryChanged();
		GetOwner()->ForceNetUpdate();
	}

	return RemainingQuantity == 0;
}

bool UTSInventoryMasterComponent::RemoveItem(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	if (!GetOwner()->HasAuthority() || !IsValidSlotIndex(InventoryType, SlotIndex))
	{
		return false;
	}

	FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
	{
		return false;
	}

	FSlotStructMaster& Slot = Inventory->InventorySlotContainer[SlotIndex];

	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
	{
		return false;
	}

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

	HandleInventoryChanged();
	GetOwner()->ForceNetUpdate();

	return true;
}
#pragma endregion

#pragma region Slot
FSlotStructMaster UTSInventoryMasterComponent::GetSlot(EInventoryType InventoryType, int32 SlotIndex) const
{
	if (!IsValidSlotIndex(InventoryType, SlotIndex))
	{
		return FSlotStructMaster();
	}

	const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
	{
		return FSlotStructMaster();
	}

	return Inventory->InventorySlotContainer[SlotIndex];
}

bool UTSInventoryMasterComponent::IsSlotEmpty(EInventoryType InventoryType, int32 SlotIndex) const
{
	FSlotStructMaster Slot = GetSlot(InventoryType, SlotIndex);
	return Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0;
}

int32 UTSInventoryMasterComponent::FindEmptySlot(EInventoryType InventoryType) const
{
	const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
	{
		return -1;
	}

	for (int32 i = 0; i < Inventory->InventorySlotContainer.Num(); ++i)
	{
		const FSlotStructMaster& Slot = Inventory->InventorySlotContainer[i];
		if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
		{
			return i;
		}
	}

	return -1;
}
#pragma endregion

#pragma region CheckSlotType

bool UTSInventoryMasterComponent::CanPlaceItemInSlot(
	int32 StaticDataID,
	EInventoryType InventoryType,
	int32 SlotIndex) const
{
	if (StaticDataID == 0 || !IsValidSlotIndex(InventoryType, SlotIndex))
	{
		return false;
	}

	FItemData ItemInfo;
	if (!GetItemData(StaticDataID, ItemInfo))
	{
		return false;
	}

	// 방어구 아이템 타입 검증
	if (InventoryType == EInventoryType::Equipment)
	{
		const FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
		if (!Inventory)
		{
			return false;
		}
		ESlotType TargetSlotType = Inventory->InventorySlotContainer[SlotIndex].SlotType;
		if (EquipmentSlotTypes[TargetSlotType] != ItemInfo.ArmorData.EquipSlot)
		{
			return false;
		}
	}

	return true;
}
#pragma endregion

#pragma region BagInventory

bool UTSInventoryMasterComponent::ExpandBagInventory(int32 AdditionalSlots)
{
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

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
	GetOwner()->ForceNetUpdate();

	UE_LOG(LogTemp, Log, TEXT("Bag expanded: %d -> %d (Max: %d)"),
	       OldSize, NewSlotCount, MaxBagSlotCount);

	return true;
}
#pragma endregion

#pragma region Hotkey / EquipItem

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
	return ActiveSlot.ItemData.StaticDataID != 0 && ActiveSlot.CurrentStackSize > 0;
}

void UTSInventoryMasterComponent::EquipActiveHotkeyItem()
{
	if (ActiveHotkeyIndex < 0 || ActiveHotkeyIndex >= HotkeyInventory.InventorySlotContainer.Num())
	{
		UnequipCurrentItem();
		return;
	}

	const FSlotStructMaster& ActiveSlot = HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];

	if (ActiveSlot.ItemData.StaticDataID == 0 || ActiveSlot.CurrentStackSize <= 0)
	{
		UnequipCurrentItem();
		return;
	}

	UnequipCurrentItem();

	// TODO: GameplayAbility 연동 시 ASC 코드 추가

	UE_LOG(LogTemp, Log, TEXT("Equipped: ID=%d (Slot %d)"),
	       ActiveSlot.ItemData.StaticDataID, ActiveHotkeyIndex);
}

void UTSInventoryMasterComponent::UnequipCurrentItem()
{
	if (!HasItemEquipped())
	{
		return;
	}

	// TODO: GameplayAbility 연동 시 ASC 코드 추가

	FSlotStructMaster ActiveSlot = GetActiveHotkeySlot();
	UE_LOG(LogTemp, Log, TEXT("Unequipped: ID=%d"), ActiveSlot.ItemData.StaticDataID);
}


#pragma endregion
void UTSInventoryMasterComponent::OnDecayTick()
{
	ConvertToDecayedItem(EInventoryType::HotKey);
	ConvertToDecayedItem(EInventoryType::BackPack);
	HandleInventoryChanged();
	GetOwner()->ForceNetUpdate();
}

void UTSInventoryMasterComponent::ConvertToDecayedItem(EInventoryType InventoryType)
{
	FInventoryStructMaster* Inventory = GetInventoryByType(InventoryType);
	if (!Inventory)
	{
		return;
	}
	float CurrentTime = GetWorld()->GetTimeSeconds();
	for (FSlotStructMaster& Slot : Inventory->InventorySlotContainer)
	{
		if (Slot.ItemData.StaticDataID == 0)
		{
			continue;
		}
		FItemData ItemInfo;
		if (!GetItemData(Slot.ItemData.StaticDataID, ItemInfo))
		{
			continue;
		}
		if (ItemInfo.IsDecayEnabled() && Slot.ExpirationTime > 0)
		{
			if (CurrentTime < Slot.ExpirationTime)
			{
				continue;
			}
			if (CachedDecayedItemInfo.ItemID != DecayedItemID)
			{
				if (!GetItemData(DecayedItemID, CachedDecayedItemInfo))
				{
					continue;
				}
			}
			Slot.ItemData.StaticDataID = DecayedItemID;
			Slot.ExpirationTime = 0;
			Slot.bCanStack = CachedDecayedItemInfo.IsStackable();
			Slot.MaxStackSize = CachedDecayedItemInfo.MaxStack;
			UE_LOG(LogTemp, Log, TEXT("Decayed: ID=%d"), Slot.ItemData.StaticDataID);
		}
	}
}
#pragma region Helper - Slot

void UTSInventoryMasterComponent::ClearSlot(FSlotStructMaster& Slot)
{
	Slot.ItemData = FItemInstance();
	Slot.CurrentStackSize = 0;
}

void UTSInventoryMasterComponent::CopySlotData(
	const FSlotStructMaster& Source,
	FSlotStructMaster& Target,
	int32 Quantity)
{
	Target.ItemData = Source.ItemData;
	Target.bCanStack = Source.bCanStack;
	Target.MaxStackSize = Source.MaxStackSize;
	Target.CurrentStackSize = (Quantity < 0) ? Source.CurrentStackSize : Quantity;
	Target.ExpirationTime = Source.ExpirationTime;
}

bool UTSInventoryMasterComponent::TryStackSlots(
	FSlotStructMaster& FromSlot,
	FSlotStructMaster& ToSlot,
	bool bIsFullStack)
{
	if (FromSlot.ItemData.StaticDataID == 0 || ToSlot.ItemData.StaticDataID == 0 ||
		FromSlot.ItemData.StaticDataID != ToSlot.ItemData.StaticDataID)
	{
		return false;
	}

	FItemData ItemInfo;
	if (!GetItemData(FromSlot.ItemData.StaticDataID, ItemInfo))
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
			// 부패 만료 시각 업데이트
			ToSlot.ExpirationTime = ItemInfo.IsDecayEnabled()
				                        ? UpdateExpirationTime(ToSlot.ExpirationTime, ToSlot.CurrentStackSize, CanAdd,
				                                               ItemInfo.ConsumableData.DecayRate)
				                        : 0;
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
			// 부패 만료 시각 업데이트
			ToSlot.ExpirationTime = ItemInfo.IsDecayEnabled()
				                        ? UpdateExpirationTime(ToSlot.ExpirationTime, ToSlot.CurrentStackSize, 1,
				                                               ItemInfo.ConsumableData.DecayRate)
				                        : 0;
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
#pragma endregion

#pragma region Helper - Inventory

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
#pragma endregion

#pragma region Helper - ItemInfo
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
	if (StaticDataID == 0)
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

double UTSInventoryMasterComponent::UpdateExpirationTime(double CurrentExpirationTime, int CurrentStack,
                                                         int NewItemStack, float DecayRate)
{
	double NewItemExpirationTime = GetWorld()->GetTimeSeconds() + DecayRate;
	return (CurrentExpirationTime * CurrentStack + NewItemExpirationTime * NewItemStack) / (CurrentStack +
		NewItemStack);
}
#pragma endregion

#pragma region Helper - ASC
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
#pragma endregion

#pragma region Helper - Delegate
void UTSInventoryMasterComponent::HandleInventoryChanged()
{
	OnInventoryUpdated.Broadcast(HotkeyInventory, EquipmentInventory, BagInventory);
}

void UTSInventoryMasterComponent::HandleActiveHotkeyIndexChanged()
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
#pragma endregion
