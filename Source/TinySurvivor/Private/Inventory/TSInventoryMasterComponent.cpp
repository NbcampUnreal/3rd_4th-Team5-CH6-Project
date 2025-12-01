// TSInventoryMasterComponent.cpp

#include "Inventory/TSInventoryMasterComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Character/TSCharacter.h"
#include "Item/TSEquippedItem.h"
#include "Item/System/ItemDataSubsystem.h"
#include "Item/Data/ItemData.h"
#include "Item/Runtime/DecayManager.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Item/System/WorldItemPoolSubsystem.h"

// 로그 카테고리 정의 (이 파일 내에서만 사용)
DEFINE_LOG_CATEGORY_STATIC(LogInventoryComp, Log, All);

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
	DOREPLIFETIME(UTSInventoryMasterComponent, CurrentEquippedItem);
	DOREPLIFETIME(UTSInventoryMasterComponent, EquippedArmors);
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
	
	// WeaponStatEffectClass가 설정되지 않은 경우 출력
	// 이 클래스가 없으면 무기 장착 시 캐릭터 스탯을 적용할 수 없음
	if (!WeaponStatEffectClass)
	{
		UE_LOG(LogInventoryComp, Warning, TEXT("WeaponStatEffectClass가 설정되지 않았습니다! 무기 스탯이 적용되지 않습니다."));
	}

	if (GetOwner()->HasAuthority())
	{
		// ■ ItemConsumed
		//[S]=====================================================================================
		// Event.Item.Consumed 태그 리스닝
		UAbilitySystemComponent* ASC = GetASC();
		if (ASC)
		{
			//FGameplayTag ConsumedTag = FGameplayTag::RequestGameplayTag(FName("Event.Item.Consumed"));
			FGameplayTag ConsumedTag = ItemTags::TAG_Event_Item_Consumed;
			
			ASC->GenericGameplayEventCallbacks.FindOrAdd(ConsumedTag).AddUObject(
				this, &UTSInventoryMasterComponent::OnItemConsumedEvent);
			
			UE_LOG(LogInventoryComp, Log, TEXT("Registered Event.Item.Consumed listener"));
		}
		//[E]=====================================================================================
		
		// 부패도 매니저 OnDecayTick 바인딩
		UDecayManager* DecayMgr = GetWorld()->GetSubsystem<UDecayManager>();
		if (DecayMgr)
		{
			DecayMgr->OnDecayTick.AddDynamic(this, &UTSInventoryMasterComponent::OnDecayTick);
			CachedDecayedItemID = DecayMgr->GetDecayItemID();
		}

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
		EquippedArmors.SetNum(EquipmentSlotTypes.Num());
		int32 idx = 0;
		for (const auto& Pair : EquipmentSlotTypes)
		{
			EquipmentInventory.InventorySlotContainer[idx].SlotType = Pair.Key;
			EquippedArmors[idx].SlotType = Pair.Value;
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

		// 초기화 브로드캐스트
		HandleInventoryChanged();
		OnInventoryInitialized.Broadcast();
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
	UE_LOG(LogTemp, Log, TEXT("Dropping item: ID=%d x%d at %s. Owner: %s"), Slot.ItemData.StaticDataID, DropQuantity,
	       *GetOwner()->GetActorLocation().ToString(), *GetOwner()->GetName());

	UWorldItemPoolSubsystem* IPS = GetWorld()->GetSubsystem<UWorldItemPoolSubsystem>();
	if (IPS)
	{
		FSlotStructMaster TempSlot = Slot;
		TempSlot.CurrentStackSize = DropQuantity;
		IPS->DropItem(TempSlot, GetOwner()->GetTransform(), GetOwner()->GetActorLocation());
	}

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

	// 활성화 슬롯 아이템 변경 플래그
	bool bAddedToActiveSlot = false;
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
			bAddedToActiveSlot = true;
			// SlotType 백업
			ESlotType FromSlotType = FromSlot.SlotType;
			ESlotType ToSlotType = ToSlot.SlotType;

			// 아이템 데이터 교환
			FSlotStructMaster Temp = FromSlot;
			FromSlot = ToSlot;
			ToSlot = Temp;

			// SlotType 복원
			FromSlot.SlotType = FromSlotType;
			ToSlot.SlotType = ToSlotType;
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

	// 활성화 슬롯 변경 시 브로드캐스트
	if (bAddedToActiveSlot)
	{
		HandleActiveHotkeyIndexChanged();
	}
	// 방어구이면 장착 해제
	if (FromInventoryType == EInventoryType::Equipment)
	{
		UnequipArmor(FromSlotIndex);
	}
	// 소스 인벤토리의 델리게이트 브로드캐스트
	if (SourceInventory)
	{
		SourceInventory->HandleInventoryChanged();
	}
	// 타겟 인벤토리의 델리게이트 브로드캐스트
	if (TargetInventory && TargetInventory != SourceInventory)
	{
		TargetInventory->HandleInventoryChanged();
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
	
	// ■ ItemConsumed
	//[S]=====================================================================================
	// 원본[S]-------------------------------------------------------
	// if (ItemInfo.AbilityBP)
	// {
	// 	FGameplayAbilitySpec Spec(ItemInfo.AbilityBP, 1, 0);
	// 	FGameplayAbilitySpecHandle Handle = ASC->GiveAbilityAndActivateOnce(Spec);
	// 	// 어빌리티 활성화 시 진행
	// 	if (Handle.IsValid())
	// 	{
	// 		// 아이템 소비
	// 		if (ItemInfo.Category != EItemCategory::ARMOR)
	// 		{
	// 			// 방어구가 아닌 경우: 소비
	// 			Slot.CurrentStackSize -= 1;
	// 			if (Slot.CurrentStackSize <= 0)
	// 			{
	// 				ClearSlot(Slot);
	// 			}
	// 			UE_LOG(LogTemp, Log, TEXT("Item used: ID=%d"), Slot.ItemData.StaticDataID);
	// 		}
	// 		else
	// 		{
	// 			// 방어구인 경우: 장착
	// 			UnequipCurrentItem();
	// 			int32 TargetSlotIndex = FindEquipmentSlot(ItemInfo.ArmorData.EquipSlot);
	// 			if (TargetSlotIndex == -1)
	// 			{
	// 				return;
	// 			}
	// 			Internal_TransferItem(this, this, EInventoryType::HotKey, SlotIndex,
	// 			                      EInventoryType::Equipment, TargetSlotIndex, true);
	// 			EquipArmor(ItemInfo, TargetSlotIndex);
	// 			UE_LOG(LogTemp, Log, TEXT("Item equipped: ID=%d"), Slot.ItemData.StaticDataID);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("Failed to activate ability for item: ID=%d"), Slot.ItemData.StaticDataID);
	// 	}
	// }
	// HandleInventoryChanged();
	//
	// // 활성화 슬롯 상태 변경 시 브로드캐스트
	// if (Slot.CurrentStackSize == 0)
	// {
	// 	HandleActiveHotkeyIndexChanged();
	// }
	// 원본[E]-------------------------------------------------------
	
	if (ItemInfo.AbilityBP)
	{
		// ItemInfo.AbilityBP가 존재할 경우, 해당 Ability를 런타임으로 ASC(AbilitySystemComponent)에 추가
		FGameplayAbilitySpec Spec(ItemInfo.AbilityBP, 1, 0);
		FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(Spec);
		if (SpecHandle.IsValid())
		{
			if (ItemInfo.Category == EItemCategory::CONSUMABLE)
			{
				/*
					여기서 아이템 소비하지 않음 (GameplayEvent 수신 후 처리)
					아이템 Ability를 Add한 직후 곧바로 이벤트 트리거 시도하면 실패해서, 다음 틱에서 트리거하기 위한 구조
					
					원래는 GiveAbility 후 즉시 TriggerAbilityFromGameplayEvent하려 했으나,
					다음 틱(0.01초 뒤)에 TriggerAbilityFromGameplayEvent를 호출.
					
					GiveAbility → TriggerAbilityFromGameplayEvent를 같은 틱에서 호출하면,
					
					- 문제 1:
					ASC가 Ability를 아직 InternalList에 제대로 등록하지 않아 실패할 수 있음.
					부여 직후 즉시 Trigger하면 Activation 실패 가능
					
					- 문제 2:
					AbilityActorInfo 갱신 시점 문제
					AbilityActorInfo가 업데이트되기 전에 Trigger하면,
					SpecHandle이 유효하지 않거나 Ability가 검색되지 않는 이슈 발생.
					
					따라서, 0.01초 지연 = 한 frame 이후 처리로 이 문제를 회피하려는 의도.
				*/
				// EventData를 전달하며 활성화
				// if (!ASC->TriggerAbilityFromGameplayEvent(
				// 	SpecHandle,
				// 	ASC->AbilityActorInfo.Get(),
				// 	ItemTags::TAG_Ability_Item_Consume,
				// 	&EventData,
				// 	*ASC))
				// {
				// 	UE_LOG(LogTemp, Warning, TEXT("Failed to trigger ability for ItemID: %d"), Slot.ItemData.StaticDataID);
				// }
			
				// 다음 틱에서 트리거
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [ASC, SpecHandle, SlotIndex]()
				{
					// EventData로 슬롯 인덱스 전달
					FGameplayEventData EventData;
					EventData.EventTag = ItemTags::TAG_Ability_Item_Consume;
					EventData.EventMagnitude = static_cast<float>(SlotIndex);
					EventData.Instigator = ASC->GetOwner();
					EventData.Target = ASC->GetOwner();
				
					ASC->TriggerAbilityFromGameplayEvent(
						SpecHandle, ASC->AbilityActorInfo.Get(),
						ItemTags::TAG_Ability_Item_Consume, &EventData, *ASC);
				}, 0.01f, false);
			}
			else if (ItemInfo.Category == EItemCategory::ARMOR)
			{
				// 방어구인 경우: 장착, 기존 코드 유지
				UnequipCurrentItem();
				int32 TargetSlotIndex = FindEquipmentSlot(ItemInfo.ArmorData.EquipSlot);
				if (TargetSlotIndex == -1)
				{
					return;
				}
				Internal_TransferItem(this, this, EInventoryType::HotKey, SlotIndex,
									  EInventoryType::Equipment, TargetSlotIndex, true);
				EquipArmor(ItemInfo, TargetSlotIndex);
				UE_LOG(LogTemp, Log, TEXT("Item equipped: ID=%d"), Slot.ItemData.StaticDataID);
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to activate ability for item: ID=%d"), Slot.ItemData.StaticDataID);
		}
	}

	
	HandleInventoryChanged();
	//[E]=====================================================================================
}
#pragma endregion

#pragma region Add/Remove Item
bool UTSInventoryMasterComponent::AddItem(const FItemInstance& ItemData, int32 Quantity, int32& OutRemainingQuantity)
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

	OutRemainingQuantity = Quantity;

	// 빈 슬롯 캐싱용
	TArray<int32> EmptyHotkeySlots;
	TArray<int32> EmptyBagSlots;

	// 인벤토리 변경 플래그
	bool bInventoryChanged = false;
	// 활성화 슬롯 아이템 변경 플래그
	bool bAddedToActiveSlot = false;
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
				int32 CanAdd = FMath::Min(OutRemainingQuantity, ItemInfo.MaxStack - Slot.CurrentStackSize);
				// 부패 만료 시각 업데이트
				Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
					                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, CanAdd,
					                                             ItemInfo.ConsumableData.DecayRate)
					                      : 0;
				Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled()
					                           ? UpdateDecayPercent(Slot.ExpirationTime,
					                                                ItemInfo.ConsumableData.DecayRate)
					                           : 0.f;
				Slot.CurrentStackSize += CanAdd;
				OutRemainingQuantity -= CanAdd;
				bInventoryChanged = true;
				if (i == ActiveHotkeyIndex)
				{
					bAddedToActiveSlot = true;
				}
				if (OutRemainingQuantity <= 0)
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
	if (OutRemainingQuantity > 0 && ItemInfo.IsStackable() && BagInventory.InventorySlotContainer.Num() > 0)
	{
		for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];

			if (Slot.ItemData.StaticDataID == ItemData.StaticDataID && Slot.CurrentStackSize < ItemInfo.MaxStack)
			{
				int32 CanAdd = FMath::Min(OutRemainingQuantity, ItemInfo.MaxStack - Slot.CurrentStackSize);
				// 부패 만료 시각 업데이트
				Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
					                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, CanAdd,
					                                             ItemInfo.ConsumableData.DecayRate)
					                      : 0;
				Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled()
					                           ? UpdateDecayPercent(Slot.ExpirationTime,
					                                                ItemInfo.ConsumableData.DecayRate)
					                           : 0.f;
				Slot.CurrentStackSize += CanAdd;
				OutRemainingQuantity -= CanAdd;
				bInventoryChanged = true;

				if (OutRemainingQuantity <= 0)
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
		if (OutRemainingQuantity <= 0)
		{
			break;
		}

		bInventoryChanged = true;
		if (SlotIndex == ActiveHotkeyIndex)
		{
			bAddedToActiveSlot = true;
		}
		FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];
		Slot.ItemData = ItemData;
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(OutRemainingQuantity, ItemInfo.MaxStack) : 1;
		// 부패 만료 시각 업데이트
		Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
			                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, AddAmount,
			                                             ItemInfo.ConsumableData.DecayRate)
			                      : 0;
		Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled()
			                           ? UpdateDecayPercent(Slot.ExpirationTime,
			                                                ItemInfo.ConsumableData.DecayRate)
			                           : 0.f;
		Slot.CurrentStackSize = AddAmount;
		OutRemainingQuantity -= AddAmount;
	}

	// ========================================
	// 4단계: 가방 빈 슬롯
	// ========================================
	for (int32 SlotIndex : EmptyBagSlots)
	{
		if (OutRemainingQuantity <= 0)
		{
			break;
		}

		bInventoryChanged = true;

		FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[SlotIndex];
		Slot.ItemData = ItemData;
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(OutRemainingQuantity, ItemInfo.MaxStack) : 1;
		// 부패 만료 시각 업데이트
		Slot.ExpirationTime = ItemInfo.IsDecayEnabled()
			                      ? UpdateExpirationTime(Slot.ExpirationTime, Slot.CurrentStackSize, AddAmount,
			                                             ItemInfo.ConsumableData.DecayRate)
			                      : 0;
		Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled()
			                           ? UpdateDecayPercent(Slot.ExpirationTime,
			                                                ItemInfo.ConsumableData.DecayRate)
			                           : 0.f;
		Slot.CurrentStackSize = AddAmount;
		OutRemainingQuantity -= AddAmount;
	}

	if (OutRemainingQuantity > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddItem partial: All slots full! Added %d, Failed %d"),
		       Quantity - OutRemainingQuantity, OutRemainingQuantity);
	}

	if (bInventoryChanged)
	{
		HandleInventoryChanged();
	}
	if (bAddedToActiveSlot)
	{
		HandleActiveHotkeyIndexChanged();
	}

	return OutRemainingQuantity == 0;
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
		if (ItemInfo.Category != EItemCategory::ARMOR)
		{
			return false;
		}
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
	
	// 서버에서만 실행
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	UnequipCurrentItem();
	
	FItemData ItemInfo;
	if (!GetItemData(ActiveSlot.ItemData.StaticDataID, ItemInfo))
	{
		return;
	}
	
	// 아이템 ID 캐싱 (UnequipCurrentItem에서 사용)
	CachedEquippedItemID = ActiveSlot.ItemData.StaticDataID;
		
	// 무기 스탯 적용 추가
	if (ItemInfo.Category == EItemCategory::WEAPON)
	{
		ApplyWeaponStats(ItemInfo);
	}
	
	// 아이템 액터 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	CurrentEquippedItem = GetWorld()->SpawnActor<ATSEquippedItem>(
		ATSEquippedItem::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams);
	
	if (!CurrentEquippedItem)
	{
		UE_LOG(LogInventoryComp, Error, TEXT("장착 아이템 액터 생성 실패"));
		RemoveWeaponStats(); // 스탯도 다시 제거
		CachedEquippedItemID = 0;
		return;
	}
	
	// 메시 설정
	if (UStaticMesh* LoadedMesh = ItemInfo.WorldMesh.LoadSynchronous())
	{
		CurrentEquippedItem->SetMesh(LoadedMesh);
	}
	else
	{
		UE_LOG(LogInventoryComp, Warning, TEXT("아이템 메시를 찾을 수 없습니다. ID=%d"), ActiveSlot.ItemData.StaticDataID);
	}
	
	ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetOwner());
	if (TSCharacter)
	{
		CurrentEquippedItem->AttachToComponent(
			TSCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("Ws_r"));
		TSCharacter->SetAnimType(ItemInfo.AnimType);
	}
	
	UE_LOG(LogInventoryComp, Log, TEXT("Equipped: ID=%d (Slot %d, Category=%s)"),
		ActiveSlot.ItemData.StaticDataID, 
		ActiveHotkeyIndex,
		*UEnum::GetValueAsString(ItemInfo.Category));
}

void UTSInventoryMasterComponent::UnequipCurrentItem()
{
	/*
		슬롯 변경 시 (예: 3번 → 1번) ActiveHotkeyIndex가 먼저 갱신되므로
		GetActiveHotkeySlot()으로 조회하면 이미 새 슬롯(1번)의 정보가 반환됨.
		따라서 해제되는 아이템(3번)의 ID를 올바르게 로그에 남기기 위해
		EquipActiveHotkeyItem()에서 장착 시점에 CachedEquippedItemID에 ID를 저장하고,
		UnequipCurrentItem()에서 캐싱된 값을 사용.
	*/
	
	// 서버에서 아이템 장착 해제
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	// 1. 먼저 무기 스탯 제거 (CurrentEquippedItem이 nullptr이 되기 전에)
	RemoveWeaponStats();
	
#if WITH_EDITOR
	// 2. 로그 출력 (캐싱된 ID 사용)
	if (CurrentEquippedItem && CachedEquippedItemID != 0)
	{
		UE_LOG(LogInventoryComp, Log, TEXT("Unequipped item: ID=%d"), CachedEquippedItemID);
	}
#endif
	
	// 3. 아이템 제거
	if (CurrentEquippedItem)
	{
		CurrentEquippedItem->Destroy();
		CurrentEquippedItem = nullptr;
	}
	
	// 4. 캐싱 초기화 (제일 마지막에)
	CachedEquippedItemID = 0;
	
	// 5. 애니메이션 타입 초기화
	ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetOwner());
	if (TSCharacter)
	{
		TSCharacter->SetAnimType(EItemAnimType::NONE);
	}
}

#pragma endregion
#pragma region Helper - Search/Consumer Item
int32 UTSInventoryMasterComponent::GetItemCount(int32 StaticDataID) const
{
	int32 ResultCount = 0;
	// 핫키 인벤토리 탐색
	for (const FSlotStructMaster& Slot : HotkeyInventory.InventorySlotContainer)
	{
		if (Slot.ItemData.StaticDataID == StaticDataID)
		{
			ResultCount += Slot.CurrentStackSize;
		}
	}
	// 가방 인벤토리 탐색
	if (GetCurrentBagSlotCount() == 0)
	{
		return ResultCount;
	}
	for (const FSlotStructMaster& Slot : BagInventory.InventorySlotContainer)
	{
		if (Slot.ItemData.StaticDataID == StaticDataID)
		{
			ResultCount += Slot.CurrentStackSize;
		}
	}
	return ResultCount;
}

void UTSInventoryMasterComponent::ConsumeItem(int32 StaticDataID, int32 Quantity)
{
	// 핫키 인벤토리 탐색
	for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
	{
		FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[i];
		if (Slot.ItemData.StaticDataID == StaticDataID)
		{
			int32 ToRemove = FMath::Min(Quantity, Slot.CurrentStackSize);
			RemoveItem(EInventoryType::HotKey, i, ToRemove);
			Quantity -= ToRemove;
			if (ActiveHotkeyIndex == i)
			{
				HandleActiveHotkeyIndexChanged();
			}
			if (Quantity <= 0)
			{
				break;
			}
		}
	}
	// 가방 인벤토리 탐색
	if (GetCurrentBagSlotCount() > 0)
	{
		for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];
			if (Slot.ItemData.StaticDataID == StaticDataID)
			{
				int32 ToRemove = FMath::Min(Quantity, Slot.CurrentStackSize);
				RemoveItem(EInventoryType::BackPack, i, ToRemove);
				Quantity -= ToRemove;
				if (Quantity <= 0)
				{
					break;
				}
			}
		}
	}
	HandleInventoryChanged();
}
#pragma endregion
#pragma region Helper - Decay
void UTSInventoryMasterComponent::OnDecayTick()
{
	ConvertToDecayedItem(EInventoryType::HotKey);
	ConvertToDecayedItem(EInventoryType::BackPack);
	HandleInventoryChanged();
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
				Slot.CurrentDecayPercent = UpdateDecayPercent(Slot.ExpirationTime, ItemInfo.ConsumableData.DecayRate);
				continue;
			}
			if (CachedDecayedItemInfo.ItemID != CachedDecayedItemID)
			{
				if (!GetItemData(CachedDecayedItemID, CachedDecayedItemInfo))
				{
					continue;
				}
			}
			Slot.ItemData.StaticDataID = CachedDecayedItemID;
			Slot.ExpirationTime = 0;
			Slot.bCanStack = CachedDecayedItemInfo.IsStackable();
			Slot.MaxStackSize = CachedDecayedItemInfo.MaxStack;
			UE_LOG(LogTemp, Log, TEXT("Decayed: ID=%d"), Slot.ItemData.StaticDataID);
		}
	}
}
#pragma endregion
#pragma region Helper - Slot

void UTSInventoryMasterComponent::ClearSlot(FSlotStructMaster& Slot)
{
	ESlotType PreservedSlotType = Slot.SlotType; // 백업
	Slot = FSlotStructMaster(); // 전체 초기화
	Slot.SlotType = PreservedSlotType;
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
	Target.CurrentDecayPercent = Source.CurrentDecayPercent;
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
			ToSlot.CurrentDecayPercent = ItemInfo.IsDecayEnabled()
				                             ? UpdateDecayPercent(ToSlot.ExpirationTime,
				                                                  ItemInfo.ConsumableData.DecayRate)
				                             : 0.f;
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
			ToSlot.CurrentDecayPercent = ItemInfo.IsDecayEnabled()
				                             ? UpdateDecayPercent(ToSlot.ExpirationTime,
				                                                  ItemInfo.ConsumableData.DecayRate)
				                             : 0.f;
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
                                                         int NewItemStack, float DecayRate) const
{
	double NewItemExpirationTime = GetWorld()->GetTimeSeconds() + DecayRate;
	return (CurrentExpirationTime * CurrentStack + NewItemExpirationTime * NewItemStack) / (CurrentStack +
		NewItemStack);
}

float UTSInventoryMasterComponent::UpdateDecayPercent(double CurrentExpirationTime, float DecayRate) const
{
	double CurrentTime = GetWorld()->GetTimeSeconds();
	return DecayRate > 0 ? (float)((CurrentExpirationTime - CurrentTime) / DecayRate) : 0.f;
}
#pragma endregion

#pragma region Helper - EquipArmor
int32 UTSInventoryMasterComponent::FindEquipmentSlot(EEquipSlot ArmorSlot) const
{
	for (int32 i = 0; i < EquipmentInventory.InventorySlotContainer.Num(); ++i)
	{
		if (*EquipmentSlotTypes.Find(EquipmentInventory.InventorySlotContainer[i].SlotType) == ArmorSlot)
		{
			return i;
		}
	}
	return -1;
}

void UTSInventoryMasterComponent::EquipArmor(const FItemData& ItemInfo, int32 ArmorSlotIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	EEquipSlot ArmorSlot = ItemInfo.ArmorData.EquipSlot;

	UnequipArmor(ArmorSlotIndex);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATSEquippedItem* EquippedArmor = GetWorld()->SpawnActor<ATSEquippedItem>(
		ATSEquippedItem::StaticClass(), FVector::ZeroVector,
		FRotator::ZeroRotator, SpawnParams);

	ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetOwner());
	if (!TSCharacter)
	{
		return;
	}
	if (UStaticMesh* LoadedMesh = ItemInfo.WorldMesh.LoadSynchronous())
	{
		if (ArmorSlot == EEquipSlot::LEG)
		{
			EquippedArmor->SetLegMesh(LoadedMesh);
			if (EquippedArmor->LeftLegMeshComp)
			{
				EquippedArmor->LeftLegMeshComp->AttachToComponent(TSCharacter->GetMesh(),
				                                                  FAttachmentTransformRules::SnapToTargetIncludingScale,
				                                                  TEXT("LeftLegSocket"));
			}
			if (EquippedArmor->RightLegMeshComp)
			{
				EquippedArmor->RightLegMeshComp->AttachToComponent(TSCharacter->GetMesh(),
				                                                   FAttachmentTransformRules::SnapToTargetIncludingScale,
				                                                   TEXT("RightLegSocket"));
			}
		}
		else
		{
			FName SocketName;
			if (ArmorSlot == EEquipSlot::HEAD)
			{
				SocketName = TEXT("HeadSocket");
			}
			else
			{
				SocketName = TEXT("TorsoSocket");
			}
			EquippedArmor->SetMesh(LoadedMesh);
			EquippedArmor->AttachToComponent(TSCharacter->GetMesh(),
			                                 FAttachmentTransformRules::SnapToTargetIncludingScale,
			                                 SocketName);
		}
	}
	EquippedArmors[ArmorSlotIndex].EquippedArmor = EquippedArmor;
}

void UTSInventoryMasterComponent::UnequipArmor(int32 ArmorSlotIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (EquippedArmors[ArmorSlotIndex].EquippedArmor)
	{
		EquippedArmors[ArmorSlotIndex].EquippedArmor->Destroy();
		EquippedArmors[ArmorSlotIndex].EquippedArmor = nullptr;
	}
}

void UTSInventoryMasterComponent::RemoveArmorStats(int32 ArmorSlotIndex)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return;
	}
	FSlotStructMaster& Slot = EquipmentInventory.InventorySlotContainer[ArmorSlotIndex];
	FItemData ItemInfo;
	if (!GetItemData(Slot.ItemData.StaticDataID, ItemInfo))
	{
		return;
	}
	if (ItemInfo.EffectTag.IsValid())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(ItemInfo.EffectTag);

		ASC->RemoveActiveEffectsWithTags(TagContainer);
	}
}
#pragma endregion

#pragma region Helper - EquipWeapon
void UTSInventoryMasterComponent::ApplyWeaponStats(const FItemData& ItemInfo)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("무기 스탯 적용 실패: ASC가 존재하지 않습니다."));
		return;
	}
	
	if (!WeaponStatEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponStatEffectClass가 설정되지 않았습니다!"));
		return;
	}
	
	//=======================================================================
	// 적용 전 캐릭터 기본 스탯 조회
	//=======================================================================
	const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
	if (!AttrSet)
	{
		UE_LOG(LogTemp, Error, TEXT("AttributeSet을 찾을 수 없습니다!"));
		return;
	}
	
	// 기존 Base 값 (캐릭터 기본)
	float CurrentBaseDamage = AttrSet->GetBaseDamage();
	float CurrentBaseAttackSpeed = AttrSet->GetBaseAttackSpeed();
	float CurrentBaseAttackRange = AttrSet->GetBaseAttackRange();
	
	// 기존 Bonus 값 (적용 전)
	float CurrentDamageBonus = AttrSet->GetDamageBonus();
	float CurrentAttackSpeedBonus = AttrSet->GetAttackSpeedBonus();
	float CurrentAttackRangeBonus = AttrSet->GetAttackRangeBonus();
	
#if WITH_EDITOR
	auto PrintStatLine = [](const FString& Name, float Base, float Bonus, float Total, const FString& Extra = TEXT(""))
	{
		UE_LOG(LogInventoryComp, Display, TEXT("%-15s | Base: %6.1f | Bonus: %6.2f | Total: %6.2f %s"),
			*Name, Base, Bonus, Total, *Extra);
	};
	
	UE_LOG(LogInventoryComp, Display, TEXT("========== 무기 스탯 적용 =========="));
	UE_LOG(LogInventoryComp, Display, TEXT(">> 적용 전 스탯"));
	PrintStatLine(TEXT("Damage"), CurrentBaseDamage, CurrentDamageBonus, CurrentBaseDamage + CurrentDamageBonus);
	PrintStatLine(TEXT("AttackSpeed"), CurrentBaseAttackSpeed, CurrentAttackSpeedBonus, CurrentBaseAttackSpeed * CurrentAttackSpeedBonus);
	PrintStatLine(TEXT("AttackRange"), CurrentBaseAttackRange, CurrentAttackRangeBonus, CurrentBaseAttackRange * CurrentAttackRangeBonus);
	
	UE_LOG(LogInventoryComp, Display, TEXT(">> 무기 스탯"));
	UE_LOG(LogInventoryComp, Display, TEXT("Damage: %.1f, AttackSpeed: %.1f, AttackRange: %.1f"),
		ItemInfo.WeaponData.DamageValue, ItemInfo.WeaponData.AttackSpeed, ItemInfo.WeaponData.AttackRange);
#endif
	
	//=======================================================================
	// GameplayEffect 적용
	//=======================================================================
	// GameplayEffect Context 생성
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	
	// GameplayEffect Spec 생성
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		WeaponStatEffectClass, 1, EffectContext);
	
	if (SpecHandle.IsValid())
	{
		// SetByCaller로 무기 스탯 전달
		SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackDamage, ItemInfo.WeaponData.DamageValue);
		SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackSpeed, ItemInfo.WeaponData.AttackSpeed);
		SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackRange, ItemInfo.WeaponData.AttackRange);
		
		// Effect 적용 및 핸들 저장
		CurrentWeaponEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		if (CurrentWeaponEffectHandle.IsValid())
		{
#if WITH_EDITOR
			//=======================================================================
			// 적용 후 스탯 조회
			//=======================================================================
			float NewDamageBonus = AttrSet->GetDamageBonus();
			float NewAttackSpeedBonus = AttrSet->GetAttackSpeedBonus();
			float NewAttackRangeBonus = AttrSet->GetAttackRangeBonus();
			
			UE_LOG(LogInventoryComp, Display, TEXT(">> 적용 후 스탯"));
			PrintStatLine(TEXT("Damage"), CurrentBaseDamage, NewDamageBonus, CurrentBaseDamage + NewDamageBonus,
				FString::Printf(TEXT("(+%.2f)"), NewDamageBonus - CurrentDamageBonus));
			PrintStatLine(TEXT("AttackSpeed"), CurrentBaseAttackSpeed, NewAttackSpeedBonus, CurrentBaseAttackSpeed * NewAttackSpeedBonus,
				FString::Printf(TEXT("(×%.2f)"), NewAttackSpeedBonus / CurrentAttackSpeedBonus));
			PrintStatLine(TEXT("AttackRange"), CurrentBaseAttackRange, NewAttackRangeBonus, CurrentBaseAttackRange * NewAttackRangeBonus,
				FString::Printf(TEXT("(×%.2f)"), NewAttackRangeBonus / CurrentAttackRangeBonus));
			
			UE_LOG(LogInventoryComp, Display, TEXT("==================================="));
			UE_LOG(LogInventoryComp, Log, TEXT("무기 스탯 적용 성공!"));
#endif
		}
		else
		{
			UE_LOG(LogInventoryComp, Error, TEXT("무기 스탯 적용 실패!"));
		}
	}
}

void UTSInventoryMasterComponent:: RemoveWeaponStats()
{
	// Authority 체크
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	// 핸들이 유효하지 않으면 조기 종료
	if (!CurrentWeaponEffectHandle.IsValid())
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		UE_LOG(LogInventoryComp, Error, TEXT("무기 스탯 제거 실패: ASC가 존재하지 않습니다."));
		CurrentWeaponEffectHandle.Invalidate();
		return;
	}
	
	const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
	if (!AttrSet)
	{
		UE_LOG(LogInventoryComp, Error, TEXT("무기 스탯 제거 실패: AttributeSet이 존재하지 않습니다."));
		CurrentWeaponEffectHandle.Invalidate();
		return;
	}
	
	//=======================================================================
	// 제거 전 스탯 조회
	//=======================================================================
	
	// 현재 Base 값 (캐릭터 기본)
	float CurrentBaseDamage = AttrSet->GetBaseDamage();
	float CurrentBaseAttackSpeed = AttrSet->GetBaseAttackSpeed();
	float CurrentBaseAttackRange = AttrSet->GetBaseAttackRange();
	
	// 현재 Bonus 값 (현재 적용된 모든 GE의 Bonus 총합 — 무기 GE 포함)
	float CurrentDamageBonus = AttrSet->GetDamageBonus();
	float CurrentAttackSpeedBonus = AttrSet->GetAttackSpeedBonus();
	float CurrentAttackRangeBonus = AttrSet->GetAttackRangeBonus();
	
#if WITH_EDITOR
	auto PrintStatLine = [](const FString& Name, float Base, float Bonus, float Total, const FString& Extra = TEXT(""))
	{
		UE_LOG(LogInventoryComp, Display, TEXT("%-15s | Base: %6.1f | Bonus: %6.2f | Total: %6.2f %s"),
			*Name, Base, Bonus, Total, *Extra);
	};
	
	UE_LOG(LogInventoryComp, Display, TEXT("========== 무기 스탯 제거 =========="));
	UE_LOG(LogInventoryComp, Display, TEXT(">> 제거 전 스탯 (무기 포함)"));
	PrintStatLine(TEXT("Damage"), CurrentBaseDamage, CurrentDamageBonus,
		CurrentBaseDamage + CurrentDamageBonus);
	PrintStatLine(TEXT("AttackSpeed"), CurrentBaseAttackSpeed, CurrentAttackSpeedBonus,
		CurrentBaseAttackSpeed * CurrentAttackSpeedBonus);
	PrintStatLine(TEXT("AttackRange"), CurrentBaseAttackRange, CurrentAttackRangeBonus,
		CurrentBaseAttackRange * CurrentAttackRangeBonus);
#endif
	
	//=======================================================================
	// GameplayEffect 제거
	//=======================================================================
	bool bRemoved = ASC->RemoveActiveGameplayEffect(CurrentWeaponEffectHandle);
	CurrentWeaponEffectHandle.Invalidate();
	
	if (!bRemoved)
	{
		UE_LOG(LogInventoryComp, Warning, TEXT("무기 효과 제거 실패 (이미 제거되었을 수 있음)"));
		return;
	}
	
	//=======================================================================
	// 제거 후 스탯 조회
	//=======================================================================
	float NewDamageBonus = AttrSet->GetDamageBonus();
	float NewAttackSpeedBonus = AttrSet->GetAttackSpeedBonus();
	float NewAttackRangeBonus = AttrSet->GetAttackRangeBonus();
			
#if WITH_EDITOR
	UE_LOG(LogInventoryComp, Display, TEXT(">> 제거 후 스탯"));
	PrintStatLine(TEXT("Damage"), CurrentBaseDamage, NewDamageBonus,
		CurrentBaseDamage + NewDamageBonus,
		FString::Printf(TEXT("(-%.2f)"),
			CurrentDamageBonus - NewDamageBonus));
	PrintStatLine(TEXT("AttackSpeed"), CurrentBaseAttackSpeed, NewAttackSpeedBonus,
		CurrentBaseAttackSpeed * NewAttackSpeedBonus,
		FString::Printf(TEXT("(÷%.2f)"),
			CurrentAttackSpeedBonus > 0 ? CurrentAttackSpeedBonus / FMath::Max(NewAttackSpeedBonus, 0.01f) : 1.0f));
	PrintStatLine(TEXT("AttackRange"), CurrentBaseAttackRange, NewAttackRangeBonus,
		CurrentBaseAttackRange * NewAttackRangeBonus,
		FString::Printf(TEXT("(÷%.2f)"),
			CurrentAttackRangeBonus > 0 ? CurrentAttackRangeBonus / FMath::Max(NewAttackRangeBonus, 0.01f) : 1.0f));
	
	UE_LOG(LogInventoryComp, Display, TEXT("==================================="));
	UE_LOG(LogInventoryComp, Log, TEXT("무기 스탯 이펙트 제거 완료"));
#endif
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

#pragma region OnItemConsumedEvent
// ■ ItemConsumed
//[S]=====================================================================================
void UTSInventoryMasterComponent::OnItemConsumedEvent(const FGameplayEventData* Payload)
{
	if (!Payload)
	{
		return;
	}
	
	int32 SlotIndex = static_cast<int32>(Payload->EventMagnitude);
	
	if (!IsValidSlotIndex(EInventoryType::HotKey, SlotIndex))
	{
		UE_LOG(LogInventoryComp, Error, TEXT("Invalid SlotIndex in OnItemConsumedEvent: %d"), SlotIndex);
		return;
	}
	
	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];
	
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
	{
		return;
	}
	
	// 아이템 소비
	Slot.CurrentStackSize -= 1;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot(Slot);
	}
	
	HandleInventoryChanged();
	
	// 활성화 슬롯 상태 변경 시 브로드캐스트
	if (Slot.CurrentStackSize == 0)
	{
		HandleActiveHotkeyIndexChanged();
	}
	
	UE_LOG(LogInventoryComp, Log, TEXT("Item consumed: SlotIndex=%d, ItemID=%d"), 
		SlotIndex, Slot.ItemData.StaticDataID);
}
//[E]=====================================================================================
#pragma endregion