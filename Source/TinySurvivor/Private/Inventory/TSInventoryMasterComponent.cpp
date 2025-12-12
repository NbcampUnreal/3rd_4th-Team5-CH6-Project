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
#include "GameplayTags/AbilityGameplayTags.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Item/System/WorldItemPoolSubsystem.h"

// 로그 카테고리 정의 (이 파일 내에서만 사용)
DEFINE_LOG_CATEGORY_STATIC(LogInventoryComp, Log, All);

#pragma region UTSInventoryMasterComponent
UTSInventoryMasterComponent::UTSInventoryMasterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}
#pragma endregion

#pragma region GetLifetimeReplicatedProps
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
#pragma endregion

#pragma region BeginPlay
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
		UE_LOG(LogInventoryComp, Warning,
			TEXT("WeaponStatEffectClass가 설정되지 않았습니다! 무기 스탯이 적용되지 않습니다."));
	}
	
	// 이벤트 리스너 등록 시도 (ASC가 준비될 때까지 재시도)
	TryRegisterEventListeners();
	
	if (GetOwner()->HasAuthority())
	{
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
#pragma endregion

#pragma region EndPlay
void UTSInventoryMasterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearConsumableAbilityResources();
	Super::EndPlay(EndPlayReason);
}
#pragma endregion

#pragma region ReplicationCallback
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

void UTSInventoryMasterComponent::ClientNotifyItemAdded_Implementation(int32 ItemID, int32 Quantity)
{
	OnItemAdded.Broadcast(ItemID, Quantity);
}
#pragma endregion

#pragma region ServerRPC
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

	// 액터 앞에 드랍
	FTransform DropTransform = FTransform(
		GetOwner()->GetActorTransform().GetRotation(),
		GetOwner()->GetActorTransform().TransformPosition(FVector(100,0,0)));
	int32 DropQuantity = (Quantity <= 0) ? Slot.CurrentStackSize : FMath::Min(Quantity, Slot.CurrentStackSize);
	UE_LOG(LogInventoryComp, Log, TEXT("Dropping item: ID=%d x%d, Durability=%d at %s. Owner: %s"), 
		Slot.ItemData.StaticDataID,
		DropQuantity,
		Slot.ItemData.CurrentDurability, // 내구도 로그 추가
		*GetOwner()->GetActorLocation().ToString(),
		*GetOwner()->GetName());

	UWorldItemPoolSubsystem* IPS = GetWorld()->GetSubsystem<UWorldItemPoolSubsystem>();
	if (IPS)
	{
		FSlotStructMaster TempSlot = Slot;
		TempSlot.CurrentStackSize = DropQuantity;
		
		// TempSlot.ItemData에 내구도 포함됨
		// WorldItemActor가 이 데이터를 보존해야 함
		IPS->DropItem(TempSlot, DropTransform, GetOwner()->GetActorLocation());
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
	
	// 슬롯 변경 시 예약된 소모품 Ability Trigger 취소
	ClearConsumableAbilityResources();
	
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

#pragma region Internal_TransferItem
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
	// // 방어구이면 장착 해제
	// if (FromInventoryType == EInventoryType::Equipment)
	// {
	// 	UnequipArmor(FromSlotIndex);
	// }
	
	/*
		신규: 방어구 처리 내용 보완
	*/
	//========================================
	// 방어구 장착 해제 로직
	//========================================
	// 방어구 탈착 여부 플래그
	bool bArmorUnequipped = false;
	
	// 방어구라면 장착 해제
	if (FromInventoryType == EInventoryType::Equipment)
	{
		UnequipArmor(FromSlotIndex);
		bArmorUnequipped = true;
	}
	
	//========================================
	// 방어구 장착 로직
	//========================================
	bool bArmorEquipped = false;
	
	// Equipment 슬롯으로 아이템이 이동된 경우 장착 처리
	if (ToInventoryType == EInventoryType::Equipment && ToSlot.ItemData.StaticDataID != 0)
	{
		FItemData ItemInfo;
		if (TargetInventory->GetItemData(ToSlot.ItemData.StaticDataID, ItemInfo))
		{
			// 방어구인 경우에만 장착
			if (ItemInfo.Category == EItemCategory::ARMOR)
			{
				TargetInventory->EquipArmor(ItemInfo, ToSlotIndex);
				bArmorEquipped = true;
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogInventoryComp, Log,
					TEXT("드래그 앤 드롭으로 방어구 장착: ItemID=%d, SlotIndex=%d"),
					ItemInfo.ItemID, ToSlotIndex);
#endif
			}
		}
	}
	
	// 활성화 슬롯 변경 시 또는 방어구 탈착 시 브로드캐스트
	if (bAddedToActiveSlot || bArmorUnequipped || bArmorEquipped)
	{
		HandleActiveHotkeyIndexChanged();
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
#pragma endregion

#pragma region Internal_UseItem
void UTSInventoryMasterComponent::Internal_UseItem(int32 SlotIndex)
{
	// 서버 권한 체크: 서버에서만 실행
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

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogTemp, Log, TEXT("Bag item used: Expanded bag by %d slots (Total: %d/%d)"),
				BagSlotIncrement, BagInventory.InventorySlotContainer.Num(), MaxBagSlotCount);
#endif
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot use bag: Already at max capacity (%d)"), MaxBagSlotCount);
		}

		return;
	}

	//========================================
	// 아이템 - 소모품 / 방어구
	//========================================
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
	
	//============================================================
	// 소모품
	//============================================================
	if (ItemInfo.Category == EItemCategory::CONSUMABLE)
	{
		//=======================================================================
		// 1. Cancel 태그 체크 (몽타주 재생 전)
		//=======================================================================
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(AbilityTags::TAG_State_Move_WASD);
		CancelTags.AddTag(AbilityTags::TAG_State_Move_Sprint);
		CancelTags.AddTag(AbilityTags::TAG_State_Move_Roll);
		CancelTags.AddTag(AbilityTags::TAG_State_Move_Jump);
		CancelTags.AddTag(AbilityTags::TAG_State_Move_Climb);
		CancelTags.AddTag(AbilityTags::TAG_State_Status_Attack);
		CancelTags.AddTag(AbilityTags::TAG_State_Status_Downed);
		CancelTags.AddTag(AbilityTags::TAG_State_Status_Dead);
		CancelTags.AddTag(AbilityTags::TAG_State_Status_Rescuing);
		CancelTags.AddTag(AbilityTags::TAG_State_Status_PickUpItem);
		//CancelTags.AddTag(AbilityTags::TAG_State_Move_Crouch);
		// TODO: 피격 태그 추가
		// CancelTags.AddTag(AbilityTags::TAG_State_Combat_Hit); // 예상
		
		// Cancel 태그 중 하나라도 있으면 소비 불가
		if (ASC->HasAnyMatchingGameplayTags(CancelTags))
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogInventoryComp, Warning,
				TEXT("소모품 사용 불가: Cancel 태그 감지됨 (ItemID=%d)"),
				ItemInfo.ItemID);
#endif
			return; // 조기 종료
		}
		
		//=======================================================================
		// 2. 기존 리소스 정리
		//=======================================================================
		ClearConsumableAbilityResources();
		
		//=======================================================================
		// 3. 몽타주 재생 (Multicast)
		//=======================================================================
		bool bMontageStarted = false;
		
		if (ItemInfo.ConsumableData.ConsumptionMontage.IsValid())
		{
			UAnimMontage* Montage = ItemInfo.ConsumableData.ConsumptionMontage.LoadSynchronous();
			if (Montage)
			{
				ATSCharacter* Character = Cast<ATSCharacter>(GetOwner());
				if (Character)
				{
					// 서버 시작 시간 기록
					float ServerStartTime = GetWorld()->GetTimeSeconds();
					
					// 서버 시간을 함께 전달
					Character->Multicast_PlayConsumeMontage(Montage, 1.0f, ServerStartTime);
					bMontageStarted = true;
					
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
					UE_LOG(LogInventoryComp, Log,
						TEXT("[Server] 소모품 몽타주 Multicast 호출: %s (StartTime=%.2f)"),
						*Montage->GetName(), ServerStartTime);
#endif
				}
			}
		}
		
		// 몽타주 재생 실패 시 중단
		if (ItemInfo.ConsumableData.ConsumptionMontage.IsValid() && !bMontageStarted)
		{
			UE_LOG(LogTemp, Error,
				TEXT("몽타주 재생 실패로 아이템 사용 취소: ItemID=%d"),
				Slot.ItemData.StaticDataID);
			return;
		}
		
		//=======================================================================
		// 4. 어빌리티 발동
		//=======================================================================
		GrantAndScheduleConsumableAbility(ItemInfo, SlotIndex, ASC);
		
		// 여기서 return (아이템 소비는 어빌리티에서 처리)
		return;
	}
	
	//============================================================
	// 방어구
	// 방어구는 Ability를 사용 안함.
	//============================================================
	if (ItemInfo.Category == EItemCategory::ARMOR)
	{
		// 방어구 장착 로직 (기존 방식 유지)
		UnequipCurrentItem();
		int32 TargetSlotIndex = FindEquipmentSlot(ItemInfo.ArmorData.EquipSlot);
		if (TargetSlotIndex == -1)
		{
			return;
		}
		Internal_TransferItem(this, this, EInventoryType::HotKey, SlotIndex,
							  EInventoryType::Equipment, TargetSlotIndex, true);
		EquipArmor(ItemInfo, TargetSlotIndex);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("Item equipped: ID=%d"), Slot.ItemData.StaticDataID);
#endif
	}
	
	HandleInventoryChanged();
	//[E]=====================================================================================
}
#pragma endregion

#pragma region AddItem
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
	
	/*
		내구도 초기화 (한 번만)
		
		이미 내구도가 설정된 아이템은 그대로 유지
		(월드에서 드랍한 아이템을 다시 주웠을 때)
	*/
	FItemInstance ProcessedItemData = ItemData; // 복사본 생성
	
	if (ProcessedItemData.NeedsDurabilityInit() && ItemInfo.HasDurability())
	{
		ProcessedItemData.CurrentDurability = ItemInfo.GetMaxDurability();
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("AddItem: 내구도 초기화 - ItemID=%d, Durability=%d"),
			ItemInfo.ItemID, ProcessedItemData.CurrentDurability);
#endif
	}
	
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
		//Slot.ItemData = ItemData;
		Slot.ItemData = ProcessedItemData; // 내구도 포함
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
		//Slot.ItemData = ItemData; // 원본 (내구도 초기화 안됨)
		Slot.ItemData = ProcessedItemData;
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
		// 아이템 습득 HUD 표시용 이벤트 브로드캐스트
		ClientNotifyItemAdded(ItemData.StaticDataID, Quantity - OutRemainingQuantity);
	}
	if (bAddedToActiveSlot)
	{
		HandleActiveHotkeyIndexChanged();
	}
	
	return OutRemainingQuantity == 0;
}
#pragma endregion

#pragma region RemoveItem
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

#pragma region HotkeyEquipItem
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
		
	// 무기와 도구 모두 동일 GE 사용
	if (ItemInfo.Category == EItemCategory::WEAPON
		|| ItemInfo.Category == EItemCategory::TOOL)
	{
		ApplyWeaponStats(ItemInfo);
	}
	if (ItemInfo.Category == EItemCategory::TOOL)
	{
		ApplyToolTags(ItemInfo); // 채취 태그
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
		UE_LOG(LogInventoryComp, Warning, TEXT("아이템 메시를 찾을 수 없습니다. ID=%d"),
			ActiveSlot.ItemData.StaticDataID);
	}
	
	ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetOwner());
	if (TSCharacter)
	{
		/*
			도구 전용 처리:
			- 캐릭터 파트 요청에 따라 도구만 왼손(Ws_l) 소켓에 장착.
			- 기획 파트 요청에 따라 도구만 스케일을 1.75로 확대. (도구 에셋이 작게 보여서)
			- 단, 현재 횃불(예: JunkTorch)은 에셋 크기가 이미 커서 스케일 확대 제외.
			  추후 횃불 에셋이 다른 도구와 동일한 크기로 조정되면 스케일 1.75 확대 적용 예정.
		*/
		if (ItemInfo.Category == EItemCategory::TOOL)
		{
			CurrentEquippedItem->AttachToComponent(
				TSCharacter->GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				TEXT("Ws_l"));
			
			// 스케일 조정 제외 대상(예: 횃불)
			const int32 JunkTorchToolID = 202;
			
			// 특정 도구 제외하고 스케일 확대
			if (ItemInfo.ItemID != JunkTorchToolID)
			{
				CurrentEquippedItem->SetActorScale3D(FVector(1.75f));
			}
		}
		else
		{
			CurrentEquippedItem->AttachToComponent(
				TSCharacter->GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				TEXT("Ws_r"));
		}
		
		TSCharacter->SetAnimType(ItemInfo.AnimType);
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("Equipped: ID=%d (Slot %d, Category=%s)"),
		ActiveSlot.ItemData.StaticDataID,
		ActiveHotkeyIndex,
		*UEnum::GetValueAsString(ItemInfo.Category));
#endif
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
	
	// 1. 무기 스탯 제거 (CurrentEquippedItem이 nullptr이 되기 전에)
	RemoveWeaponStats();
	
	// 2. 도구 태그 제거
	RemoveToolTags();

#if WITH_EDITOR
	// 로그 출력 (캐싱된 ID 사용)
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

#pragma region SearchItem
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

#pragma region Decay
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
	
	// 활성화된 슬롯 변경 여부 추적
	bool bActiveSlotChanged = false;
	
	/*
		현재 슬롯 인덱스 추적
		범위 기반 for 루프는 인덱스 정보를 제공하지 않으므로,
		ActiveHotkeyIndex와 비교하기 위해 수동으로 추적
	*/
	int32 SlotIndex = 0;
	
	for (FSlotStructMaster& Slot : Inventory->InventorySlotContainer)
	{
		// 빈 슬롯은 건너뛰기
		if (Slot.ItemData.StaticDataID == 0)
		{
			/*
				인덱스가 엉망이 되기 때문에 빼먹으면 안 됨!
				만약 `++SlotIndex`를 빼먹으면:
				슬롯 0: 비어있음 → continue (SlotIndex는 여전히 0)
				슬롯 1: 사과    → SlotIndex = 0으로 처리됨!! 문제발생!! (실제로는 1번인데!)
			*/
			++SlotIndex; // continue 전 인덱스 증가 필수
			continue;
		}
		
		// 아이템 정보 조회 실패 시 건너뛰기
		FItemData ItemInfo;
		if (!GetItemData(Slot.ItemData.StaticDataID, ItemInfo))
		{
			++SlotIndex; // continue 전 인덱스 증가 필수
			continue;
		}
		
		// 부패 가능한 아이템만 처리
		if (ItemInfo.IsDecayEnabled() && Slot.ExpirationTime > 0)
		{
			// 아직 만료되지 않았으면 부패도만 업데이트
			if (CurrentTime < Slot.ExpirationTime)
			{
				Slot.CurrentDecayPercent = UpdateDecayPercent(Slot.ExpirationTime, ItemInfo.ConsumableData.DecayRate);
				++SlotIndex; // continue 전 인덱스 증가 필수
				continue;
			}
			
			//=======================================================================
			// 부패 시간 만료: 부패물로 전환
			//=======================================================================
			
			// 부패물 정보 캐싱
			if (CachedDecayedItemInfo.ItemID != CachedDecayedItemID)
			{
				if (!GetItemData(CachedDecayedItemID, CachedDecayedItemInfo))
				{
					++SlotIndex; // continue 전 인덱스 증가 필수
					continue;
				}
			}
			
			// 슬롯 데이터를 부패물로 변경
			Slot.ItemData.StaticDataID = CachedDecayedItemID;
			Slot.ExpirationTime = 0;
			Slot.bCanStack = CachedDecayedItemInfo.IsStackable();
			Slot.MaxStackSize = CachedDecayedItemInfo.MaxStack;
			
			// 현재 손에 들고 있는 슬롯이 부패물로 전환된 경우 플래그 설정
			if (InventoryType == EInventoryType::HotKey && SlotIndex == ActiveHotkeyIndex)
			{
				bActiveSlotChanged = true;
			}
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogInventoryComp, Log, TEXT("Decayed: ID=%d (Slot=%d, IsActive=%s)"),
				Slot.ItemData.StaticDataID,
				SlotIndex,
				(InventoryType == EInventoryType::HotKey && SlotIndex == ActiveHotkeyIndex) ? TEXT("Yes") : TEXT("No"));
#endif
		}
		
		++SlotIndex; // 다음 슬롯으로
	}
	
	//=======================================================================
	// 활성화된 슬롯이 부패물로 전환된 경우 메시 재장착
	//=======================================================================
	if (bActiveSlotChanged)
	{
		HandleActiveHotkeyIndexChanged(); // 기존 메시 제거 -> 새 메시 장착
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Log,
			TEXT("활성화된 슬롯(#%d)의 아이템이 부패물로 전환되어 메시를 업데이트했습니다."),
			ActiveHotkeyIndex);
#endif
	}
}
#pragma endregion

#pragma region Slot
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

#pragma region Inventory
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

#pragma region ItemInfo
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

#pragma region FindEquipmentSlot
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
#pragma endregion

#pragma region EquipArmor
void UTSInventoryMasterComponent::EquipArmor(const FItemData& ItemInfo, int32 ArmorSlotIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	EEquipSlot ArmorSlot = ItemInfo.ArmorData.EquipSlot;
	
	// 기존 방어구 해제
	UnequipArmor(ArmorSlotIndex);
	
	// 메시 생성 및 장착
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
				EquippedArmor->LeftLegMeshComp->AttachToComponent(
					TSCharacter->GetMesh(),
					FAttachmentTransformRules::SnapToTargetIncludingScale,
					TEXT("LeftLegSocket"));
			}
			if (EquippedArmor->RightLegMeshComp)
			{
				EquippedArmor->RightLegMeshComp->AttachToComponent(
					TSCharacter->GetMesh(),
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
			EquippedArmor->AttachToComponent(
				TSCharacter->GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				SocketName);
		}
	}
	
	/*
		이하 로직은 신규 내용
	*/
	
	// 액터 저장
	EquippedArmors[ArmorSlotIndex].EquippedArmor = EquippedArmor;
	
	//=======================================================================
	// GE 적용
	//=======================================================================
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		UE_LOG(LogInventoryComp, Error, TEXT("ASC를 찾을 수 없습니다!"));
		return;
	}
	
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	//=======================================================================
	// 적용 전 스탯 로그
	//=======================================================================
	const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
	if (AttrSet)
	{
		UE_LOG(LogInventoryComp, Display, TEXT("========== 방어구 장착 (ItemID=%d) =========="), ItemInfo.ItemID);
		UE_LOG(LogInventoryComp, Display, TEXT(">> 적용 전 스탯"));
		UE_LOG(LogInventoryComp, Display, TEXT("MaxHealth: %.1f"), AttrSet->GetMaxHealth());
		UE_LOG(LogInventoryComp, Display, TEXT("DamageReflection: %.2f"),
			AttrSet->GetBaseDamageReflection() + AttrSet->GetDamageReflectionBonus());
		UE_LOG(LogInventoryComp, Display, TEXT("DamageReduction: %.2f"),
			AttrSet->GetBaseDamageReduction() + AttrSet->GetDamageReductionBonus());
		UE_LOG(LogInventoryComp, Display, TEXT("MoveSpeed: %.1f"), AttrSet->GetMoveSpeed());
		UE_LOG(LogInventoryComp, Display, TEXT("MaxMoveSpeed: %.1f"), AttrSet->GetMaxMoveSpeed());
	}
#endif
	
	// 1. 공통 스탯 (HealthBonus)
	if (ArmorCommonStatEffectClass)
	{
		FGameplayEffectSpecHandle CommonSpec = ASC->MakeOutgoingSpec(
			ArmorCommonStatEffectClass, 1, ContextHandle);
		
		CommonSpec.Data->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag("Data.HealthBonus"),
			ItemInfo.ArmorData.HealthBonus);
		
		FActiveGameplayEffectHandle CommonHandle =
			ASC->ApplyGameplayEffectSpecToSelf(*CommonSpec.Data);
		
		// 핸들 저장
		EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle = CommonHandle;
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Log, TEXT("방어구 공통 스탯 적용: HealthBonus=%.1f"),
			ItemInfo.ArmorData.HealthBonus);
#endif
	}
	
	// 2. 이펙트 스탯 (EffectTag별)
	if (ArmorEffectStatEffectClass && ItemInfo.EffectTag_Armor.IsValid())
	{
		FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(
			ArmorEffectStatEffectClass, 1, ContextHandle);
		
		// =======================================
		// 모든 Modifier 값 초기화 (0으로)
		// =======================================
		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReflection, 0.0f);
		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReduction, 0.0f);
		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_MoveSpeed, 0.0f);
		
		// =======================================
		// EffectTag에 따라 실제 값 설정
		// =======================================
		if (ItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_DAMAGE_REFLECT))
		{
			EffectSpec.Data->SetSetByCallerMagnitude(
				ItemTags::TAG_Data_Armor_DamageReflection, ItemInfo.EffectValue);
		}
		else if (ItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_DAMAGE_REDUCTION))
		{
			EffectSpec.Data->SetSetByCallerMagnitude(
				ItemTags::TAG_Data_Armor_DamageReduction, ItemInfo.EffectValue);
		}
		else if (ItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_MOVE_SPEED))
		{
			EffectSpec.Data->SetSetByCallerMagnitude(
				ItemTags::TAG_Data_Armor_MoveSpeed, ItemInfo.EffectValue);
		}
		
		FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
		
		// 핸들 저장
		EquippedArmors[ArmorSlotIndex].ArmorEffectHandle = EffectHandle;
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Log,
			TEXT("방어구 이펙트 스탭 적용: ItemID=%d, EffectTag=%s, EffectValue=%.1f"),
			ItemInfo.ItemID, *ItemInfo.EffectTag_Armor.ToString(), ItemInfo.EffectValue);
#endif
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	//=======================================================================
	// 적용 후 스탯 로그
	//=======================================================================
	if (AttrSet)
	{
		UE_LOG(LogInventoryComp, Display, TEXT(">> 적용 후 스탯"));
		UE_LOG(LogInventoryComp, Display, TEXT("MaxHealth: %.1f"), AttrSet->GetMaxHealth());
		UE_LOG(LogInventoryComp, Display, TEXT("DamageReflection: %.2f"),
			AttrSet->GetBaseDamageReflection() + AttrSet->GetDamageReflectionBonus());
		UE_LOG(LogInventoryComp, Display, TEXT("DamageReduction: %.2f"),
			AttrSet->GetBaseDamageReduction() + AttrSet->GetDamageReductionBonus());
		UE_LOG(LogInventoryComp, Display, TEXT("MoveSpeed: %.1f"), AttrSet->GetMoveSpeed());
		UE_LOG(LogInventoryComp, Display, TEXT("MaxMoveSpeed: %.1f"), AttrSet->GetMaxMoveSpeed());
		UE_LOG(LogInventoryComp, Display, TEXT("==================================="));
	}
#endif
}
#pragma endregion

#pragma region UnequipArmor
void UTSInventoryMasterComponent::UnequipArmor(int32 ArmorSlotIndex)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		UE_LOG(LogInventoryComp, Error,
			TEXT("방어구 탈착 실패: ASC가 존재하지 않습니다! (SlotIndex=%d)"),
			ArmorSlotIndex);
		return;
	}
	
	//=======================================================================
	// 제거 전 스탯 로그
	//=======================================================================
	{
		const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
		if (!AttrSet)
		{
			UE_LOG(LogInventoryComp, Error,
				TEXT("방어구 탈착: AttributeSet(UTSAttributeSet)을 찾을 수 없습니다! (SlotIndex=%d)"),
				ArmorSlotIndex);
		}
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		else
		{
			UE_LOG(LogInventoryComp, Display, TEXT("========== 방어구 탈착 =========="));
			UE_LOG(LogInventoryComp, Display, TEXT(">> 탈착 전 스탯"));
			UE_LOG(LogInventoryComp, Display, TEXT("MaxHealth: %.1f"), AttrSet->GetMaxHealth());
			UE_LOG(LogInventoryComp, Display, TEXT("DamageReflection: %.2f"),
				AttrSet->GetBaseDamageReflection() + AttrSet->GetDamageReflectionBonus());
			UE_LOG(LogInventoryComp, Display, TEXT("DamageReduction: %.2f"),
				AttrSet->GetBaseDamageReduction() + AttrSet->GetDamageReductionBonus());
			UE_LOG(LogInventoryComp, Display, TEXT("MoveSpeed: %.1f"), AttrSet->GetMoveSpeed());
			UE_LOG(LogInventoryComp, Display, TEXT("MaxMoveSpeed: %.1f"), AttrSet->GetMaxMoveSpeed());
		}
#endif
	}
	
	//=======================================================================
	// 1. EffectStats GE 제거
	//=======================================================================
	if (EquippedArmors[ArmorSlotIndex].ArmorEffectHandle.IsValid())
	{
		bool bRemoved = ASC->RemoveActiveGameplayEffect(EquippedArmors[ArmorSlotIndex].ArmorEffectHandle);
		if (bRemoved)
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogInventoryComp, Log, TEXT("방어구 이펙트 스탯 제거 성공: SlotIndex=%d"), ArmorSlotIndex);
#endif
		}
		else
		{
			UE_LOG(LogInventoryComp, Warning,
				TEXT("방어구 이펙트 스탯 제거 실패: RemoveActiveGameplayEffect 반환값 FALSE (SlotIndex=%d, HandleIsValid=true)"),
				ArmorSlotIndex);
		}
		
		EquippedArmors[ArmorSlotIndex].ArmorEffectHandle.Invalidate();
	}
	else
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Warning,
			TEXT("방어구 이펙트 스탯 제거 생략: EffectHandle이 이미 무효 상태입니다. (SlotIndex=%d)"),
			ArmorSlotIndex);
#endif
	}
	
	//=======================================================================
	// 2. CommonStats GE 제거
	//=======================================================================
	if (EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle.IsValid())
	{
		bool bRemoved = ASC->RemoveActiveGameplayEffect(EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle);
		if (bRemoved)
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogInventoryComp, Log, TEXT("방어구 공통 스탯 제거 성공: SlotIndex=%d"), ArmorSlotIndex);
#endif
		}
		else
		{
			UE_LOG(LogInventoryComp, Warning,
				TEXT("방어구 공통 스탯 제거 실패: RemoveActiveGameplayEffect 반환값 FALSE (SlotIndex=%d, HandleIsValid=true)"),
				ArmorSlotIndex);
		}
		
		EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle.Invalidate();
	}
	else
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Warning,
			TEXT("방어구 공통 스탯 제거 생략: ArmorCommonEffectHandle이 이미 무효 상태입니다. (SlotIndex=%d)"),
			ArmorSlotIndex);
#endif
	}
	
	//=======================================================================
	// 제거 후 스탯 로그
	//=======================================================================
	{
		const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
		if (!AttrSet)
		{
			UE_LOG(LogInventoryComp, Error,
				TEXT("방어구 탈착 후 AttributeSet 조회 실패: UTSAttributeSet이 없습니다! (SlotIndex=%d)"),
				ArmorSlotIndex);
		}
		else
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogInventoryComp, Display, TEXT(">> 탈착 후 스탯"));
			UE_LOG(LogInventoryComp, Display, TEXT("MaxHealth: %.1f"), AttrSet->GetMaxHealth());
			UE_LOG(LogInventoryComp, Display, TEXT("DamageReflection: %.2f"),
				AttrSet->GetBaseDamageReflection() + AttrSet->GetDamageReflectionBonus());
			UE_LOG(LogInventoryComp, Display, TEXT("DamageReduction: %.2f"),
				AttrSet->GetBaseDamageReduction() + AttrSet->GetDamageReductionBonus());
			UE_LOG(LogInventoryComp, Display, TEXT("MoveSpeed: %.1f"), AttrSet->GetMoveSpeed());
			UE_LOG(LogInventoryComp, Display, TEXT("MaxMoveSpeed: %.1f"), AttrSet->GetMaxMoveSpeed());
			UE_LOG(LogInventoryComp, Display, TEXT("==================================="));
#endif
		}
	}
	
	//=======================================================================
	// 3. 메시 제거
	//=======================================================================
	// if (EquippedArmors[ArmorSlotIndex].EquippedArmor)
	// {
	// 	EquippedArmors[ArmorSlotIndex].EquippedArmor->Destroy();
	// 	EquippedArmors[ArmorSlotIndex].EquippedArmor = nullptr;
	// 	
	// 	UE_LOG(LogInventoryComp, Log, TEXT("방어구 메시 제거: SlotIndex=%d"), ArmorSlotIndex);
	// }
	
	AActor* ArmorMeshActor = EquippedArmors[ArmorSlotIndex].EquippedArmor;
	
	if (IsValid(ArmorMeshActor))
	{// nullptr + Pending Kill 체크 포함
		if (ArmorMeshActor->IsActorBeingDestroyed())
		{
			UE_LOG(LogInventoryComp, Warning,
				TEXT("방어구 메시 제거 생략: 이미 Destroy 진행 중입니다. (SlotIndex=%d, Actor=%s)"),
				ArmorSlotIndex, *ArmorMeshActor->GetName());
			
			EquippedArmors[ArmorSlotIndex].EquippedArmor = nullptr;
			return;
		}
		
		// Destroy 시도
		const bool bDestroyed = ArmorMeshActor->Destroy();
		if (bDestroyed)
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogInventoryComp, Log,
				TEXT("방어구 메시 제거 성공: SlotIndex=%d, Actor=%s"),
				ArmorSlotIndex, *ArmorMeshActor->GetName());
#endif
		}
		else
		{
			UE_LOG(LogInventoryComp, Error,
				TEXT("방어구 메시 제거 실패: Destroy() 반환값 FALSE! (SlotIndex=%d, Actor=%s)"),
				ArmorSlotIndex, *ArmorMeshActor->GetName());
		}
		
		EquippedArmors[ArmorSlotIndex].EquippedArmor = nullptr;
	}
	else
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Warning,
			TEXT("방어구 메시 제거 생략: EquippedArmor 포인터가 NULL 또는 유효하지 않음 (SlotIndex=%d)"),
			ArmorSlotIndex);
#endif
	}
	
	// Health 클램핑
	UTSAttributeSet* AttrSet = const_cast<UTSAttributeSet*>(ASC->GetSet<UTSAttributeSet>());
	if (AttrSet && AttrSet->GetHealth() > AttrSet->GetMaxHealth())
	{
		AttrSet->SetHealth(AttrSet->GetMaxHealth());
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Warning,
			TEXT("방어구 탈착 후 Health 클램핑: %.1f → %.1f"),
			AttrSet->GetHealth(), AttrSet->GetMaxHealth());
#endif
	}
}
#pragma endregion

#pragma region OnArmorHitEvent
void UTSInventoryMasterComponent::OnArmorHitEvent(const FGameplayEventData* Payload)
{
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!Payload || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	// 현재 장착된 모든 방어구의 내구도 감소
	
	for (int32 i = 0; i < EquippedArmors.Num(); ++i)
	{
		// 장착된 방어구가 없으면 스킵
		if (!EquippedArmors[i].EquippedArmor)
		{
			continue;
		}
		
		// 해당 슬롯의 장비 인벤토리 확인
		if (!IsValidSlotIndex(EInventoryType::Equipment, i))
		{
			continue;
		}
		
		FSlotStructMaster& Slot = EquipmentInventory.InventorySlotContainer[i];
		
		// 슬롯이 비어있으면 스킵
		if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
		{
			continue;
		}
		
		// 아이템 정보 조회
		FItemData ItemInfo;
		if (!GetItemData(Slot.ItemData.StaticDataID, ItemInfo))
		{
			continue;
		}
		
		// 방어구가 아니면 스킵 (안전 체크)
		if (ItemInfo.Category != EItemCategory::ARMOR)
		{
			continue;
		}
		
		// 방어구 피격으로 인한 내구도 감소
		int32 DurabilityLoss = ItemInfo.ArmorData.DurabilityLossAmount;
		Slot.ItemData.CurrentDurability -= DurabilityLoss;
		Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Log, TEXT("방어구 내구도 감소: 아이템ID=%d, 슬롯=%d, 감소량=%d, 남은 내구도=%d/%d"),
			Slot.ItemData.StaticDataID,
			i,
			DurabilityLoss,
			Slot.ItemData.CurrentDurability,
			ItemInfo.ArmorData.MaxDurability);
#endif
		
		// 내구도가 0 이하가 되면 방어구 파괴
		if (Slot.ItemData.CurrentDurability <= 0)
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogInventoryComp, Warning, TEXT("방어구 파괴: 아이템ID=%d, 슬롯=%d"), Slot.ItemData.StaticDataID, i);
#endif
			// 방어구 탈착 (스탯 제거, 메시 제거)
			UnequipArmor(i);
			
			// 슬롯 초기화
			ClearSlot(Slot);
		}
	}
	
	// UI 업데이트
	HandleInventoryChanged();
}
#pragma endregion

#pragma region ApplyWeaponStats
void UTSInventoryMasterComponent::ApplyWeaponStats(const FItemData& ItemInfo)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("스탯 적용 실패: ASC가 존재하지 않습니다."));
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
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	auto PrintStatLine = [](const FString& Name, float Base, float Bonus, float Total, const FString& Extra = TEXT(""))
	{
		UE_LOG(LogInventoryComp, Display, TEXT("%-15s | Base: %6.1f | Bonus: %6.2f | Total: %6.2f %s"),
			*Name, Base, Bonus, Total, *Extra);
	};
	
	UE_LOG(LogInventoryComp, Display, TEXT("========== 스탯 적용 =========="));
	UE_LOG(LogInventoryComp, Display, TEXT(">> 적용 전 스탯"));
	PrintStatLine(TEXT("Damage"),
		CurrentBaseDamage, CurrentDamageBonus, CurrentBaseDamage + CurrentDamageBonus);
	PrintStatLine(TEXT("AttackSpeed"),
		CurrentBaseAttackSpeed, CurrentAttackSpeedBonus, CurrentBaseAttackSpeed * CurrentAttackSpeedBonus);
	PrintStatLine(TEXT("AttackRange"),
		CurrentBaseAttackRange, CurrentAttackRangeBonus, CurrentBaseAttackRange * CurrentAttackRangeBonus);
	
	UE_LOG(LogInventoryComp, Display, TEXT(">> 무기/도구 스탯"));
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
		// 무기/도구 스탯 추출
		float DamageValue = 0.0f;
		float AttackSpeed = 1.0f;
		float AttackRange = 100.0f;
		
		if (ItemInfo.Category == EItemCategory::WEAPON)
		{
			DamageValue = ItemInfo.WeaponData.DamageValue;
			AttackSpeed = ItemInfo.WeaponData.AttackSpeed;
			AttackRange = ItemInfo.WeaponData.AttackRange;
		}
		else if (ItemInfo.Category == EItemCategory::TOOL)
		{
			DamageValue = ItemInfo.ToolData.DamageValue;
			AttackSpeed = ItemInfo.ToolData.AttackSpeed;
			AttackRange = ItemInfo.ToolData.AttackRange;
		}
		
		// SetByCaller로 스탯 전달
		SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackDamage, DamageValue);
		SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackSpeed, AttackSpeed);
		SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackRange, AttackRange);
		
		// Effect 적용 및 핸들 저장
		CurrentWeaponEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		
		if (CurrentWeaponEffectHandle.IsValid())
		{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			//=======================================================================
			// 적용 후 스탯 조회
			//=======================================================================
			float NewDamageBonus = AttrSet->GetDamageBonus();
			float NewAttackSpeedBonus = AttrSet->GetAttackSpeedBonus();
			float NewAttackRangeBonus = AttrSet->GetAttackRangeBonus();
			
			UE_LOG(LogInventoryComp, Display, TEXT(">> 적용 후 스탯"));
			PrintStatLine(TEXT("Damage"),
				CurrentBaseDamage, NewDamageBonus, CurrentBaseDamage + NewDamageBonus,
				FString::Printf(TEXT("(+%.2f)"), NewDamageBonus - CurrentDamageBonus));
			PrintStatLine(TEXT("AttackSpeed"),
				CurrentBaseAttackSpeed, NewAttackSpeedBonus, CurrentBaseAttackSpeed * NewAttackSpeedBonus,
				FString::Printf(TEXT("(×%.2f)"), NewAttackSpeedBonus / CurrentAttackSpeedBonus));
			PrintStatLine(TEXT("AttackRange"),
				CurrentBaseAttackRange, NewAttackRangeBonus, CurrentBaseAttackRange * NewAttackRangeBonus,
				FString::Printf(TEXT("(×%.2f)"), NewAttackRangeBonus / CurrentAttackRangeBonus));
			
			UE_LOG(LogInventoryComp, Display, TEXT("==================================="));
			UE_LOG(LogInventoryComp, Log, TEXT("무기/도구 스탯 적용 성공!"));
#endif
		}
		else
		{
			UE_LOG(LogInventoryComp, Error, TEXT("무기/도구 스탯 적용 실패!"));
		}
	}
}
#pragma endregion

#pragma region RemoveWeaponStats
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
		UE_LOG(LogInventoryComp, Error, TEXT("스탯 제거 실패: ASC가 존재하지 않습니다."));
		CurrentWeaponEffectHandle.Invalidate();
		return;
	}
	
	const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
	if (!AttrSet)
	{
		UE_LOG(LogInventoryComp, Error, TEXT("스탯 제거 실패: AttributeSet이 존재하지 않습니다."));
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
	
	// 현재 Bonus 값 (현재 적용된 모든 GE의 Bonus 총합 — 무기/도구 GE 포함)
	float CurrentDamageBonus = AttrSet->GetDamageBonus();
	float CurrentAttackSpeedBonus = AttrSet->GetAttackSpeedBonus();
	float CurrentAttackRangeBonus = AttrSet->GetAttackRangeBonus();
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	auto PrintStatLine = [](const FString& Name, float Base, float Bonus, float Total, const FString& Extra = TEXT(""))
	{
		UE_LOG(LogInventoryComp, Display, TEXT("%-15s | Base: %6.1f | Bonus: %6.2f | Total: %6.2f %s"),
			*Name, Base, Bonus, Total, *Extra);
	};
	
	UE_LOG(LogInventoryComp, Display, TEXT("========== 스탯 제거 =========="));
	UE_LOG(LogInventoryComp, Display, TEXT(">> 제거 전 스탯 (장착아이템 스탯 포함)"));
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
		UE_LOG(LogInventoryComp, Warning, TEXT("무기/도구 효과 제거 실패 (이미 제거되었을 수 있음)"));
		return;
	}
	
	//=======================================================================
	// 제거 후 스탯 조회
	//=======================================================================
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	float NewDamageBonus = AttrSet->GetDamageBonus();
	float NewAttackSpeedBonus = AttrSet->GetAttackSpeedBonus();
	float NewAttackRangeBonus = AttrSet->GetAttackRangeBonus();
	
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
	UE_LOG(LogInventoryComp, Log, TEXT("스탯 이펙트 제거 완료"));
#endif
}
#pragma endregion

#pragma region OnWeaponAttackEvent
void UTSInventoryMasterComponent::OnWeaponAttackEvent(const FGameplayEventData* Payload)
{
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!Payload || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	// 현재 활성화된 핫키 슬롯 확인
	if (ActiveHotkeyIndex < 0 || ActiveHotkeyIndex >= HotkeyInventory.InventorySlotContainer.Num())
	{
		return;
	}
	
	// 현재 활성화된 슬롯 가져오기
	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];
	
	// 슬롯에 아이템이 없거나 스택이 0이면 처리하지 않음
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
	{
		return;
	}
	
	// 아이템 정보 조회
	FItemData ItemInfo;
	if (!GetItemData(Slot.ItemData.StaticDataID, ItemInfo))
	{
		return;
	}
	
	// 무기가 아닌 경우 처리하지 않음
	if (ItemInfo.Category != EItemCategory::WEAPON)
	{
		return;
	}
	
	// 무기 사용으로 인한 내구도 감소
	int32 DurabilityLoss = ItemInfo.WeaponData.DurabilityLossAmount;
	Slot.ItemData.CurrentDurability -= DurabilityLoss;
	Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("무기 내구도 감소: 아이템ID=%d, 감소량=%d, 남은 내구도=%d/%d"),
		Slot.ItemData.StaticDataID,
		DurabilityLoss,
		Slot.ItemData.CurrentDurability,
		ItemInfo.WeaponData.MaxDurability);
#endif
	
	// 내구도가 0 이하가 되면 아이템 파괴
	if (Slot.ItemData.CurrentDurability <= 0)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Warning, TEXT("무기 파괴: 아이템ID=%d"), Slot.ItemData.StaticDataID);
#endif
		ClearSlot(Slot); // 슬롯 초기화
		HandleActiveHotkeyIndexChanged(); // 장착 해제
	}
	
	// UI 업데이트
	HandleInventoryChanged();
}
#pragma endregion

#pragma region ApplyToolTags
void UTSInventoryMasterComponent::ApplyToolTags(const FItemData& ItemInfo)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return;
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("도구 채취 태그 추가 전: ID=%d"), ItemInfo.ItemID);
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			UE_LOG(LogInventoryComp, Log, TEXT("  이미 존재: %s"), *Tag.ToString());
		}
		else
		{
			UE_LOG(LogInventoryComp, Log, TEXT("  아직 없음: %s"), *Tag.ToString());
		}
	}
#endif
	
	// 도구의 HarvestTargetTags를 캐릭터 ASC에 추가
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("도구 채취 태그 추가 후: ID=%d"), ItemInfo.ItemID);
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			UE_LOG(LogInventoryComp, Log, TEXT("  추가 성공: %s 존재"), *Tag.ToString());
		}
		else
		{
			UE_LOG(LogInventoryComp, Log, TEXT("  추가 실패: %s 없음"), *Tag.ToString());
		}
	}
#endif
}
#pragma endregion

#pragma region RemoveToolTags
void UTSInventoryMasterComponent::RemoveToolTags()
{
	// Authority 체크
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return;
	}
	
	// 현재 장착된 아이템이 유효한 ID를 갖고 있는지 확인
	if (CachedEquippedItemID == 0)
	{
		return;
	}
	
	FItemData ItemInfo;
	if (!GetItemData(CachedEquippedItemID, ItemInfo))
	{
		return;
	}
	
	// 도구가 아니면 제거할 태그가 없음
	if (ItemInfo.Category != EItemCategory::TOOL)
	{
		return;
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("도구 채취 태그 제거 전: ID=%d"), CachedEquippedItemID);
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		UE_LOG(LogInventoryComp, Log, TEXT("  제거 대상 태그: %s"), *Tag.ToString());
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			UE_LOG(LogInventoryComp, Log, TEXT("    현재 ASC에 존재"));
		}
		else
		{
			UE_LOG(LogInventoryComp, Log, TEXT("    현재 ASC에 없음"));
		}
	}
#endif
	
	// 도구의 HarvestTargetTags를 ASC에서 제거
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("도구 채취 태그 제거 후: ID=%d"), CachedEquippedItemID);
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			UE_LOG(LogInventoryComp, Log, TEXT("  제거 실패: %s 존재"), *Tag.ToString());
		}
		else
		{
			UE_LOG(LogInventoryComp, Log, TEXT("  제거 성공: %s 없음"), *Tag.ToString());
		}
	}
#endif
}
#pragma endregion

#pragma region OnToolHarvestEvent
void UTSInventoryMasterComponent::OnToolHarvestEvent(const FGameplayEventData* Payload)
{
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!Payload || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	// 현재 활성화된 핫키 슬롯 확인
	// 유효하지 않은 인덱스이면 처리하지 않음
	if (ActiveHotkeyIndex < 0 || ActiveHotkeyIndex >= HotkeyInventory.InventorySlotContainer.Num())
	{
		return;
	}
	
	// 현재 활성화된 슬롯 가져오기
	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];
	
	// 슬롯에 아이템이 없거나 스택이 0이면 처리하지 않음
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0)
	{
		return;
	}
	
	// 아이템 정보 조회
	FItemData ItemInfo;
	if (!GetItemData(Slot.ItemData.StaticDataID, ItemInfo))
	{
		return;
	}
	
	// 도구가 아닌 경우 처리하지 않음
	if (ItemInfo.Category != EItemCategory::TOOL)
	{
		return;
	}
	
	// 도구 사용으로 인한 내구도 감소
	int32 DurabilityLoss = ItemInfo.ToolData.DurabilityLossAmount;
	Slot.ItemData.CurrentDurability -= DurabilityLoss;
	Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("도구 내구도 감소: 아이템ID=%d, 감소량=%d, 남은 내구도=%d/%d"),
		Slot.ItemData.StaticDataID,
		DurabilityLoss,
		Slot.ItemData.CurrentDurability,
		ItemInfo.ToolData.MaxDurability);
#endif
	
	// 내구도가 0 이하가 되면 아이템 파괴
	if (Slot.ItemData.CurrentDurability <= 0)
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogInventoryComp, Warning, TEXT("도구 파괴: 아이템ID=%d"), Slot.ItemData.StaticDataID);
#endif
		ClearSlot(Slot); // 슬롯 초기화
		HandleActiveHotkeyIndexChanged(); // 장착 해제
	}
	
	// UI 업데이트
	HandleInventoryChanged();
}
#pragma endregion

#pragma region GetASC
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

#pragma region HandleInventoryChanged
void UTSInventoryMasterComponent::HandleInventoryChanged()
{
	OnInventoryUpdated.Broadcast(HotkeyInventory, EquipmentInventory, BagInventory);
}
#pragma endregion

#pragma region HandleActiveHotkeyIndexChanged
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
void UTSInventoryMasterComponent::OnItemConsumedEvent(const FGameplayEventData* Payload)
{
	if (!Payload)
	{
		UE_LOG(LogInventoryComp, Error, TEXT("OnItemConsumedEvent: Payload is NULL!"));
		return;
	}
	
	// 권한 체크 강화: 서버에서만 실행
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogInventoryComp, Warning, TEXT("OnItemConsumedEvent: 클라이언트에서 호출됨 - 무시"));
		return;
	}
	
	// 추가 체크: 로컬 Role이 Authority인지 확인
	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogInventoryComp, Warning, TEXT("OnItemConsumedEvent: LocalRole이 Authority가 아님 - 무시"));
		return;
	}
	
	int32 SlotIndex = static_cast<int32>(Payload->EventMagnitude);
	
	if (!IsValidSlotIndex(EInventoryType::HotKey, SlotIndex))
	{
		UE_LOG(LogInventoryComp, Error, TEXT("OnItemConsumedEvent: 잘못된 슬롯 인덱스: %d"), SlotIndex);
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
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("아이템 소비됨: SlotIndex=%d, ItemID=%d"),
		SlotIndex, Slot.ItemData.StaticDataID);
#endif
}
#pragma endregion

#pragma region TryRegisterEventListeners
void UTSInventoryMasterComponent::TryRegisterEventListeners()
{
	// 이미 등록되었으면 리턴
	if (bEventListenersRegistered)
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = GetASC();
	
	if (!ASC)
	{
		ENetRole LocalRole = GetOwner()->GetLocalRole();
		UE_LOG(LogInventoryComp, Warning,
			TEXT("ASC가 아직 준비되지 않음 (LocalRole=%s), 재시도 중..."),
			*UEnum::GetValueAsString(LocalRole));
		
		// 0.1초 후 재시도
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				ASCCheckTimerHandle,
				this,
				&UTSInventoryMasterComponent::TryRegisterEventListeners,
				0.1f,
				false
			);
		}
		return;
	}
	
	// ASC를 찾았으므로 이벤트 리스너 등록
	ENetRole LocalRole = GetOwner()->GetLocalRole();
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("ASC 준비 완료! 이벤트 리스너를 등록합니다. (LocalRole=%s)"),
		*UEnum::GetValueAsString(LocalRole));
#endif
	
	/*
		리스너는 모든 머신에서 등록하지만,
		실제 처리는 내부에서 서버 권한 체크
	*/
	
	/*
		소모품 사용 이벤트 리스닝
	*/
	//FGameplayTag ConsumedTag = FGameplayTag::RequestGameplayTag(FName("Event.Item.Consumed"));
	FGameplayTag ConsumedTag = ItemTags::TAG_Event_Item_Consumed;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ConsumedTag)
		.AddUObject(this, &UTSInventoryMasterComponent::OnItemConsumedEvent);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Warning, TEXT("소모품 사용 이벤트 리스너 등록 완료 (LocalRole=%s)"),
		*UEnum::GetValueAsString(LocalRole));
#endif
	
	/*
		도구 채취 이벤트 리스닝
	*/
	FGameplayTag HarvestTag = ItemTags::TAG_Event_Item_Tool_Harvest;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(HarvestTag)
		.AddUObject(this, &UTSInventoryMasterComponent::OnToolHarvestEvent);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("도구 채취 이벤트 리스너 등록 완료"));
#endif
	
	/*
		무기 공격 이벤트 추가
	*/
	FGameplayTag WeaponAttackTag = ItemTags::TAG_Event_Item_Weapon_Attack;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(WeaponAttackTag)
		.AddUObject(this, &UTSInventoryMasterComponent::OnWeaponAttackEvent);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("무기 공격 이벤트 리스너 등록 완료"));
#endif
	
	/*
		방어구 피격 이벤트 리스닝
	*/
	FGameplayTag ArmorHitTag = AbilityTags::TAG_Event_Armor_Hit;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ArmorHitTag)
		.AddUObject(this, &UTSInventoryMasterComponent::OnArmorHitEvent);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogInventoryComp, Log, TEXT("방어구 피격 이벤트 리스너 등록 완료"));
#endif
	
	// 등록 완료 플래그
	bEventListenersRegistered = true;
	
	// 타이머 정리
	if (ASCCheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ASCCheckTimerHandle);
	}
}
#pragma endregion

#pragma region ClearConsumableAbilityResources
void UTSInventoryMasterComponent::ClearConsumableAbilityResources()
{
	// Timer 정리
	if (ConsumableAbilityTriggerTimer.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ConsumableAbilityTriggerTimer);
		}
		ConsumableAbilityTriggerTimer.Invalidate();
	}
	
	// Ability Spec 제거
	if (ActiveConsumableAbilityHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = GetASC())
		{
			FGameplayAbilitySpec* ExistingSpec = ASC->FindAbilitySpecFromHandle(
				ActiveConsumableAbilityHandle);
			
			if (ExistingSpec)
			{
				if (ExistingSpec->IsActive())
				{
					ASC->CancelAbilityHandle(ActiveConsumableAbilityHandle);
				}
				ASC->ClearAbility(ActiveConsumableAbilityHandle);
			}
		}
		ActiveConsumableAbilityHandle = FGameplayAbilitySpecHandle();
	}
}
#pragma endregion

#pragma region GrantAndScheduleConsumableAbility
bool UTSInventoryMasterComponent::GrantAndScheduleConsumableAbility(
	const FItemData& ItemInfo,
	int32 SlotIndex,
	UAbilitySystemComponent* ASC)
{
	if (!ItemInfo.AbilityBP)
	{
		return false;
	}
	
	FGameplayAbilitySpec Spec(ItemInfo.AbilityBP, 1, 0);
	FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(Spec);
	
	if (!SpecHandle.IsValid())
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogTemp, Log, TEXT("아이템 어빌리티 활성화 실패: ID=%d"), ItemInfo.ItemID);
#endif 
		return false;
	}
	
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
	
	// 새로운 SpecHandle 저장
	ActiveConsumableAbilityHandle = SpecHandle;
	
	// EventData 전달
	FGameplayEventData EventData;
	EventData.EventTag = ItemTags::TAG_Ability_Item_Consume;
	EventData.EventMagnitude = static_cast<float>(SlotIndex);
	EventData.Instigator = ASC->GetOwner();
	EventData.Target = ASC->GetOwner();
	
	// TWeakObjectPtr로 안전하게 캡처
	TWeakObjectPtr<UTSInventoryMasterComponent> WeakThis(this);
	TWeakObjectPtr<UAbilitySystemComponent> WeakASC(ASC);
	
	// 다음 틱에서 트리거, Timer Handle을 멤버 변수로 관리
	GetWorld()->GetTimerManager().SetTimer(
		ConsumableAbilityTriggerTimer,
		[WeakThis, WeakASC, SpecHandle, EventData]()
		{
			// 컴포넌트 유효성 체크
			if (!WeakThis.IsValid())
			{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogInventoryComp, Warning, TEXT("Ability Trigger 취소: 컴포넌트가 파괴됨"));
#endif 
				return;
			}
			
			// ASC 유효성 체크
			if (!WeakASC.IsValid())
			{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogInventoryComp, Warning, TEXT("Ability Trigger 취소: ASC가 파괴됨"));
#endif
				return;
			}
			
			UTSInventoryMasterComponent* This = WeakThis.Get();
			UAbilitySystemComponent* ASC = WeakASC.Get();
			
			// SpecHandle 재검증
			if (This->ActiveConsumableAbilityHandle == SpecHandle
				&& This->ActiveConsumableAbilityHandle.IsValid())
			{
				ASC->TriggerAbilityFromGameplayEvent(
					SpecHandle, ASC->AbilityActorInfo.Get(),
					ItemTags::TAG_Ability_Item_Consume, &EventData, *ASC);
				
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogInventoryComp, Log, TEXT("Ability Trigger 실행"));
#endif
			}
			else
			{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
				UE_LOG(LogInventoryComp, Warning,
					TEXT("Ability Trigger 취소됨 (이미 새로운 Ability로 대체됨)"));
#endif
			}
		},0.01f, false);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("[Server] 소모품 어빌리티 발동: ID=%d"), ItemInfo.ItemID);
#endif
	
	return true;
}
#pragma endregion