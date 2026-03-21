// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSInventoryMasterComponent.h"

#include "Net/UnrealNetwork.h"

#include "A_FOR_INGAME/SECTION_PLAYER/Controller/TSPlayerController.h"

#include "A_FOR_COMMON/Library/Item/TSInventoryHelperLibrary.h"
#include "A_FOR_COMMON/Library/Item/TSItemHelperLibrary.h"
#include "A_FOR_COMMON/Library/Item/TSItemUseHelperLibrary.h"
#include "A_FOR_COMMON/Library/System/TSDecayLibrary.h"
#include "A_FOR_COMMON/Library/System/TSSystemGetterLibrary.h"

#include "A_FOR_INGAME/SECTION_ITEM/Item/TSEquippedItem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/ItemData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Runtime/ItemInstance.h"

#include "A_FOR_COMMON/Tag/HelpHeader/MasterTagHelpHeader.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/RefactoringFloder/TSRFEquipArmor.h"


//```````````````````````
	// 게터, 델리게이트, Rep 섹션
	//.......................

//======================================================================================================================
#pragma region REP_API


	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━

void UTSInventoryMasterComponent::OnRep_ActiveHotkeyIndex()
{
	HandleActiveHotkeyIndexChanged_internal();
}

void UTSInventoryMasterComponent::OnRep_HotkeyInventory()
{
	HandleInventoryChanged_internal();
}

void UTSInventoryMasterComponent::OnRep_EquipmentInventory()
{
	HandleInventoryChanged_internal();
}

void UTSInventoryMasterComponent::OnRep_BagInventory()
{
	HandleInventoryChanged_internal();
}

void UTSInventoryMasterComponent::OnRep_CurrentEquippedItem()
{
	UTSItemUseHelperLibrary::HandleCurrentEquippedItemChanged_Lib(this);
}


#pragma endregion
//======================================================================================================================




	//```````````````````````
	// 사이클 섹션
	//.......................

//======================================================================================================================
#pragma region 라이프_사이클


	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

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
	DOREPLIFETIME(UTSInventoryMasterComponent, EquippedItemScale); // 소모품 회복약 사이즈 조정 관련
}

void UTSInventoryMasterComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeInventory_internal();
}

void UTSInventoryMasterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UTSItemUseHelperLibrary::ClearConsumableAbilityResources_Lib(this);
	
	Super::EndPlay(EndPlayReason);
}


#pragma endregion
//======================================================================================================================




	//```````````````````````
	// API 섹션
	//.......................

//======================================================================================================================	
#pragma region UI_관련
	

	//━━━━━━━━━━━━━━━━━━━━
	// UI 관련
	//━━━━━━━━━━━━━━━━━━━━

void UTSInventoryMasterComponent::ClientRPC_NotifyItemAdded_internal_Implementation(int32 ItemID, int32 Quantity)
{
	OnItemAdded.Broadcast(ItemID, Quantity);
}


#pragma endregion
//======================================================================================================================	
#pragma region 아이템_관련_API
	

	//━━━━━━━━━━━━━━━━━━━━
	// 아이템 관련 API
	//━━━━━━━━━━━━━━━━━━━━
	
	//--------------------
	// 아이템 사용 
	//--------------------

void UTSInventoryMasterComponent::ServerUseItem_Implementation(int32 SlotIndex)
{
	UTSItemUseHelperLibrary::UseItem_Lib(this, SlotIndex);
}

	//--------------------
	// 아이템 소비
	//--------------------

void UTSInventoryMasterComponent::ConsumeItem(int32 StaticDataID, int32 Quantity)
{
	// 핫키 인벤토리 탐색
	for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
	{
		FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[i];
		if (Slot.ItemData.StaticDataID == StaticDataID)
		{
			int32 ToRemove = FMath::Min(Quantity, Slot.CurrentStackSize);
			
			RemoveItem_internal(EInventoryType::HotKey, i, ToRemove);
			
			Quantity -= ToRemove;
			
			if (ActiveHotkeyIndex == i) HandleActiveHotkeyIndexChanged_internal();
			
			if (Quantity <= 0) break;
		}
	}
	
	// 가방 인벤토리 탐색
	if (BagInventory.InventorySlotContainer.Num() > 0)
	{
		for (int32 i = 0; i < BagInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[i];
			if (Slot.ItemData.StaticDataID == StaticDataID)
			{
				int32 ToRemove = FMath::Min(Quantity, Slot.CurrentStackSize);
				
				RemoveItem_internal(EInventoryType::BackPack, i, ToRemove);
				
				Quantity -= ToRemove;
				
				if (Quantity <= 0) break;
			}
		}
	}
	
	HandleInventoryChanged_internal();
}

#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_관련_API


	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 관련 API
	//━━━━━━━━━━━━━━━━━━━━
	
	//--------------------
	// 초기화
	//--------------------

void UTSInventoryMasterComponent::InitializeInventory_internal()
{
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return;
	
	// 핫키 인벤토리 초기화
	{
		HotkeyInventory.InventoryType = EInventoryType::HotKey;
		HotkeyInventory.InventorySlotContainer.SetNum(HotkeySlotCount);
		for (int32 i = 0; i < HotkeySlotCount; ++i)
		{
			HotkeyInventory.InventorySlotContainer[i].SlotAccessType = SlotAccessType;
			HotkeyInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
		}
	}
	
	// 장비 인벤토리 초기화
	{
		EquipmentInventory.InventoryType = EInventoryType::Equipment;
		EquipmentInventory.InventorySlotContainer.SetNum(EquipmentSlotTypes.Num());
		EquippedArmors.SetNum(EquipmentSlotTypes.Num());
	
		int32 idx = 0;
		for (const auto& Pair : EquipmentSlotTypes)
		{
			EquipmentInventory.InventorySlotContainer[idx].SlotAccessType = SlotAccessType;
			EquipmentInventory.InventorySlotContainer[idx].SlotType = Pair.Key;
			EquippedArmors[idx].SlotType = Pair.Value;
			++idx;
		}
	}

	// 가방 인벤토리 초기화
	{
		BagInventory.InventoryType = EInventoryType::BackPack;
		BagInventory.InventorySlotContainer.SetNum(InitialBagSlotCount);
			
		for (int32 i = 0; i < InitialBagSlotCount; ++i)
		{
			BagInventory.InventorySlotContainer[i].SlotAccessType = SlotAccessType;
			BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
		}
	}
	
	// 초기화 브로드캐스트
	HandleInventoryChanged_internal();
	OnInventoryInitialized.Broadcast();
}

	//--------------------
	// 아이템 습득
	//--------------------

bool UTSInventoryMasterComponent::AddItem(const FItemInstance& ItemData, int32 Quantity, int32& OutRemainingQuantity)
{
	if (!GetOwner()->HasAuthority() || ItemData.StaticDataID == 0 || Quantity <= 0) return false;

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(this, ItemData.StaticDataID, ItemInfo)) return false;

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
	}

	// ========================================
	// 1단계: 핫키 스택 가능한 슬롯
	// ========================================
	if (ItemInfo.IsStackable())
	{
		for (int32 i = 0; i < HotkeyInventory.InventorySlotContainer.Num(); ++i)
		{
			FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[i];

			if (Slot.ItemData.StaticDataID == ItemData.StaticDataID && Slot.CurrentStackSize < Slot.MaxStackSize)
			{
				int32 CanAdd = FMath::Min(OutRemainingQuantity, Slot.MaxStackSize - Slot.CurrentStackSize);
				
				// 부패 만료 시각 업데이트
				Slot.ExpirationTime = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateExpirationTime(this, Slot.ExpirationTime, Slot.CurrentStackSize, CanAdd, ItemInfo.ConsumableData.DecayRate) : 0;
				Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateDecayPercent(this, Slot.ExpirationTime,ItemInfo.ConsumableData.DecayRate) : 0.f;
				
				Slot.CurrentStackSize += CanAdd;
				OutRemainingQuantity -= CanAdd;
				bInventoryChanged = true;
				
				if (i == ActiveHotkeyIndex) bAddedToActiveSlot = true;
				if (OutRemainingQuantity <= 0) break;
			}
			else if (Slot.ItemData.StaticDataID == 0)
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

			if (Slot.ItemData.StaticDataID == ItemData.StaticDataID && Slot.CurrentStackSize < Slot.MaxStackSize)
			{
				int32 CanAdd = FMath::Min(OutRemainingQuantity, Slot.MaxStackSize - Slot.CurrentStackSize);
				
				// 부패 만료 시각 업데이트
				Slot.ExpirationTime = ItemInfo.IsDecayEnabled()? UTSDecayLibrary::CalculateExpirationTime(this, Slot.ExpirationTime, Slot.CurrentStackSize, CanAdd, ItemInfo.ConsumableData.DecayRate) : 0;
				Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateDecayPercent(this,Slot.ExpirationTime, ItemInfo.ConsumableData.DecayRate) : 0.f;
				
				Slot.CurrentStackSize += CanAdd;
				OutRemainingQuantity -= CanAdd;
				bInventoryChanged = true;

				if (OutRemainingQuantity <= 0) break;
			}
			else if (Slot.ItemData.StaticDataID == 0)
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
		if (OutRemainingQuantity <= 0) break;

		bInventoryChanged = true;
		if (SlotIndex == ActiveHotkeyIndex) bAddedToActiveSlot = true;
		
		FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];
		
		Slot.ItemData = ProcessedItemData; // 내구도 포함
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(OutRemainingQuantity, ItemInfo.MaxStack) : 1;

		// 부패 만료 시각 업데이트
		Slot.ExpirationTime = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateExpirationTime(this, Slot.ExpirationTime, Slot.CurrentStackSize, AddAmount, ItemInfo.ConsumableData.DecayRate) : 0;
		Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateDecayPercent(this, Slot.ExpirationTime, ItemInfo.ConsumableData.DecayRate) : 0.f;
		
		Slot.CurrentStackSize = AddAmount;
		OutRemainingQuantity -= AddAmount;
	}

	// ========================================
	// 4단계: 가방 빈 슬롯
	// ========================================
	for (int32 SlotIndex : EmptyBagSlots)
	{
		if (OutRemainingQuantity <= 0) break;

		bInventoryChanged = true;

		FSlotStructMaster& Slot = BagInventory.InventorySlotContainer[SlotIndex];
		Slot.ItemData = ProcessedItemData;
		Slot.bCanStack = ItemInfo.IsStackable();
		Slot.MaxStackSize = ItemInfo.MaxStack;

		int32 AddAmount = ItemInfo.IsStackable() ? FMath::Min(OutRemainingQuantity, ItemInfo.MaxStack) : 1;
		
		// 부패 만료 시각 업데이트
		Slot.ExpirationTime = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateExpirationTime(this, Slot.ExpirationTime, Slot.CurrentStackSize, AddAmount, ItemInfo.ConsumableData.DecayRate) : 0;
		Slot.CurrentDecayPercent = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateDecayPercent(this, Slot.ExpirationTime, ItemInfo.ConsumableData.DecayRate) : 0.f;
		
		Slot.CurrentStackSize = AddAmount;
		OutRemainingQuantity -= AddAmount;
	}

	if (bInventoryChanged)
	{
		HandleInventoryChanged_internal();
		
		// 아이템 습득 HUD 표시용 이벤트 브로드캐스트
		ClientRPC_NotifyItemAdded_internal_Implementation(ItemData.StaticDataID, Quantity - OutRemainingQuantity);
	}

	if (bAddedToActiveSlot)
	{
		HandleActiveHotkeyIndexChanged_internal();
	}

	return OutRemainingQuantity == 0;
}

bool UTSInventoryMasterComponent::TryStackSlots_internal(FSlotStructMaster& FromSlot,FSlotStructMaster& ToSlot,bool bIsFullStack)
{
	if (FromSlot.ItemData.StaticDataID == 0 || ToSlot.ItemData.StaticDataID == 0 || FromSlot.ItemData.StaticDataID != ToSlot.ItemData.StaticDataID)
	{
		return false;
	}

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(this, FromSlot.ItemData.StaticDataID, ItemInfo)) return false;

	if (!ItemInfo.IsStackable()) return false;

	int32 MaxStack = ToSlot.MaxStackSize;

	if (bIsFullStack)
	{
		int32 CanAdd = FMath::Min(FromSlot.CurrentStackSize, MaxStack - ToSlot.CurrentStackSize);

		if (CanAdd > 0)
		{
			// 부패 만료 시각 업데이트
			ToSlot.ExpirationTime = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateExpirationTime(this, ToSlot.ExpirationTime, ToSlot.CurrentStackSize, CanAdd,ItemInfo.ConsumableData.DecayRate) : 0;
			ToSlot.CurrentDecayPercent = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateDecayPercent(this, ToSlot.ExpirationTime,ItemInfo.ConsumableData.DecayRate): 0.f;
			ToSlot.CurrentStackSize += CanAdd;
			FromSlot.CurrentStackSize -= CanAdd;

			if (FromSlot.CurrentStackSize <= 0) ClearSlot_internal(FromSlot);
			return true;
		}
	}
	else
	{
		if (ToSlot.CurrentStackSize < MaxStack)
		{
			// 부패 만료 시각 업데이트
			ToSlot.ExpirationTime = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateExpirationTime(this, ToSlot.ExpirationTime, ToSlot.CurrentStackSize, 1, ItemInfo.ConsumableData.DecayRate) : 0;
			ToSlot.CurrentDecayPercent = ItemInfo.IsDecayEnabled() ? UTSDecayLibrary::CalculateDecayPercent(this, ToSlot.ExpirationTime, ItemInfo.ConsumableData.DecayRate) : 0.f;
			ToSlot.CurrentStackSize += 1;
			FromSlot.CurrentStackSize -= 1;

			if (FromSlot.CurrentStackSize <= 0) ClearSlot_internal(FromSlot);
			return true;
		}
	}

	return false;
}

int32 UTSInventoryMasterComponent::FindEmptySlot_internal(EInventoryType InventoryType)
{
	const FInventoryStructMaster* Inventory = UTSInventoryHelperLibrary::GetInventoryByType_Lib(this,InventoryType);
	if (!Inventory) return -1;

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

int32 UTSInventoryMasterComponent::FindEquipmentSlot_internal(EEquipSlot ArmorSlot) const
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

	//--------------------
	// 아이템 드랍
	//--------------------

void UTSInventoryMasterComponent::ServerDropItemToWorld_Implementation(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	if (!UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(this,InventoryType, SlotIndex)) return;

	FInventoryStructMaster* Inventory = UTSInventoryHelperLibrary::GetInventoryByType_Lib(this, InventoryType);
	if (!Inventory) return;

	FSlotStructMaster& Slot = Inventory->InventorySlotContainer[SlotIndex];

	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	//===============================================================
	//드롭되는 슬롯이 현재 활성화된 핫키 슬롯인지 미리 확인
	//===============================================================
	bool bIsActiveHotkeySlot = (InventoryType == EInventoryType::HotKey && SlotIndex == ActiveHotkeyIndex);

	// 액터 앞에 드랍
	FTransform DropTransform = FTransform(GetOwner()->GetActorTransform().GetRotation(),GetOwner()->GetActorTransform().TransformPosition(FVector(100, 0, 0)));
	
	int32 DropQuantity = (Quantity <= 0) ? Slot.CurrentStackSize : FMath::Min(Quantity, Slot.CurrentStackSize);
	

	UWorldItemPoolSubsystem* IPS = GetWorld()->GetSubsystem<UWorldItemPoolSubsystem>();
	if (IPS)
	{
		FSlotStructMaster TempSlot = Slot;
		TempSlot.CurrentStackSize = DropQuantity;

		// TempSlot.ItemData에 내구도 포함됨 : WorldItemActor가 이 데이터를 보존해야 함
		IPS->DropItem(TempSlot, DropTransform, GetOwner()->GetActorLocation());
	}

	Slot.CurrentStackSize -= DropQuantity;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot_internal(Slot);
	}
	
	HandleInventoryChanged_internal();

	//===============================================================
	// 활성화된 슬롯이 비워졌으면 메시 제거
	//===============================================================
	if (bIsActiveHotkeySlot && Slot.CurrentStackSize <= 0)
	{
		// 이 함수가 UnequipCurrentItem()을 호출하여 메시 제거
		HandleActiveHotkeyIndexChanged_internal();
	}
	
}

	//--------------------
	// 아이템 제거
	//--------------------

bool UTSInventoryMasterComponent::RemoveItem_internal(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity)
{
	if (!GetOwner()->HasAuthority() || !UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(this,InventoryType, SlotIndex)) return false;

	FInventoryStructMaster* Inventory = UTSInventoryHelperLibrary::GetInventoryByType_Lib(this,InventoryType);
	if (!Inventory) return false;

	FSlotStructMaster& Slot = Inventory->InventorySlotContainer[SlotIndex];

	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return false;

	if (Quantity <= 0)
	{
		ClearSlot_internal(Slot);
		return true;
	}

	Slot.CurrentStackSize -= Quantity;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot_internal(Slot);
	}

	HandleInventoryChanged_internal();

	return true;
}

void UTSInventoryMasterComponent::ClearSlot_internal(FSlotStructMaster& Slot)
{
	ESlotAccessType PreservedSlotAccessType = Slot.SlotAccessType; // 백업
	ESlotType PreservedSlotType = Slot.SlotType; // 백업
	Slot = FSlotStructMaster(); // 전체 초기화
	Slot.SlotAccessType = PreservedSlotAccessType;
	Slot.SlotType = PreservedSlotType;
}

	//--------------------
	// 사용
	//--------------------

void UTSInventoryMasterComponent::ServerActivateHotkeySlot_Implementation(int32 SlotIndex)
{
	if (SlotIndex < -1 || SlotIndex >= HotkeyInventory.InventorySlotContainer.Num()) return;

	// 슬롯 변경 시 예약된 소모품 Ability Trigger 취소
	UTSItemUseHelperLibrary::ClearConsumableAbilityResources_Lib(this);
	
	ActiveHotkeyIndex = SlotIndex;
	HandleActiveHotkeyIndexChanged_internal();
}

	//--------------------
	// 유틸
	//--------------------

void UTSInventoryMasterComponent::CopySlotData_internal(const FSlotStructMaster& Source, FSlotStructMaster& Target, int32 Quantity)
{
	Target.ItemData = Source.ItemData;
	Target.bCanStack = Source.bCanStack;
	Target.MaxStackSize = Source.MaxStackSize;
	Target.CurrentStackSize = (Quantity < 0) ? Source.CurrentStackSize : Quantity;
	Target.ExpirationTime = Source.ExpirationTime;
	Target.CurrentDecayPercent = Source.CurrentDecayPercent;
}

void UTSInventoryMasterComponent::TransferItem(UTSInventoryMasterComponent* SourceInventory, UTSInventoryMasterComponent* TargetInventory, EInventoryType FromInventoryType, int32 FromSlotIndex, EInventoryType ToInventoryType, int32 ToSlotIndex, bool bIsFullStack, ATSPlayerController* RequestingPlayer)
{
	if (!GetOwner()->HasAuthority())
	{
		RequestingPlayer->ClientNotifyTransferResult(false);
		return;
	}
	
	if (!SourceInventory || !TargetInventory)
	{
		RequestingPlayer->ClientNotifyTransferResult(false);
		RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_Failed));
		return;
	}

	if (!UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(SourceInventory,FromInventoryType, FromSlotIndex))
	{
		RequestingPlayer->ClientNotifyTransferResult(false);
		RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_Failed));
		return;
	}

	if (!UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(TargetInventory, ToInventoryType, ToSlotIndex))
	{
		RequestingPlayer->ClientNotifyTransferResult(false);
		RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_Failed));
		return;
	}

	FInventoryStructMaster* FromInventory = UTSInventoryHelperLibrary::GetInventoryByType_Lib(SourceInventory, FromInventoryType);
	FInventoryStructMaster* ToInventory = UTSInventoryHelperLibrary::GetInventoryByType_Lib(TargetInventory, ToInventoryType);

	if (!FromInventory || !ToInventory)
	{
		RequestingPlayer->ClientNotifyTransferResult(false);
		RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_Failed));
		return;
	}

	FSlotStructMaster& FromSlot = FromInventory->InventorySlotContainer[FromSlotIndex];
	FSlotStructMaster& ToSlot = ToInventory->InventorySlotContainer[ToSlotIndex];

	// 타입 검증
	if (FromSlot.ItemData.StaticDataID != 0)
	{
		if (!UTSInventoryHelperLibrary::CanPlaceItemInSlot_Lib(TargetInventory, FromSlot.ItemData.StaticDataID, ToInventoryType, ToSlotIndex, true))
		{
			RequestingPlayer->ClientNotifyTransferResult(false);
			RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_CannotPlace));
			return;
		}
	}

	if (ToSlot.ItemData.StaticDataID != 0)
	{
		if (!UTSInventoryHelperLibrary::CanPlaceItemInSlot_Lib(SourceInventory, ToSlot.ItemData.StaticDataID, FromInventoryType, FromSlotIndex, false))
		{
			RequestingPlayer->ClientNotifyTransferResult(false);
			RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_CannotPlace));
			return;
		}
	}

	// 활성화 슬롯 아이템 변경 플래그
	bool bAddedToActiveSlot = false;

	// 스택 처리 시도
	if (TryStackSlots_internal(FromSlot, ToSlot, bIsFullStack))
	{
		// 스택 완료
		if (ToInventoryType == EInventoryType::HotKey && ToSlotIndex == TargetInventory->ActiveHotkeyIndex)
		{
			bAddedToActiveSlot = true;
		}
	}
	else
	{
		// 일반 교환
		if (bIsFullStack)
		{
			// FromSlot이 읽기 전용인 경우 빈 슬롯이 아니면 스왑 불가능
			if (ToSlot.CurrentStackSize > 0 && FromSlot.SlotAccessType == ESlotAccessType::ReadOnly)
			{
				RequestingPlayer->ClientNotifyTransferResult(false);
				RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_CannotPlace));
				return;
			}
			
			// ToSlot이 활성화된 핫키 슬롯인지 체크
			if (ToInventoryType == EInventoryType::HotKey && ToSlotIndex == TargetInventory->ActiveHotkeyIndex)
			{
				bAddedToActiveSlot = true;
			}

			// 또는 FromSlot이 활성화된 핫키 슬롯인지 체크 (스왑 시)
			if (FromInventoryType == EInventoryType::HotKey && FromSlotIndex == SourceInventory->ActiveHotkeyIndex)
			{
				bAddedToActiveSlot = true;
			}
			
			// SlotAccessType 백업
			ESlotAccessType FromSlotAccessType = FromSlot.SlotAccessType;
			ESlotAccessType ToSlotAccessType = ToSlot.SlotAccessType;

			// SlotType 백업
			ESlotType FromSlotType = FromSlot.SlotType;
			ESlotType ToSlotType = ToSlot.SlotType;

			// 아이템 데이터 교환
			FSlotStructMaster Temp = FromSlot;
			FromSlot = ToSlot;
			ToSlot = Temp;

			// SlotAccessType 복원
			FromSlot.SlotAccessType = FromSlotAccessType;
			ToSlot.SlotAccessType = ToSlotAccessType;

			// SlotType 복원
			FromSlot.SlotType = FromSlotType;
			ToSlot.SlotType = ToSlotType;

			// MaxStackSize 재설정(연료슬롯의 최대 스택 제한때문에 다시 설정해줘야 함)
			// FromSlot 재설정
			if (FromSlot.ItemData.StaticDataID != 0)
			{
				FItemData ItemInfo;
				if (UTSItemHelperLibrary::GetItemData_Lib(SourceInventory, FromSlot.ItemData.StaticDataID, ItemInfo))
				{
					FromSlot.MaxStackSize = ItemInfo.MaxStack;
				}
			}
			// ToSlot 재설정
			if (ToSlot.ItemData.StaticDataID != 0)
			{
				FItemData ItemInfo;
				if (UTSItemHelperLibrary::GetItemData_Lib(TargetInventory, ToSlot.ItemData.StaticDataID, ItemInfo))
				{
					ToSlot.MaxStackSize = ItemInfo.MaxStack;
				}
			}
		}
		else
		{
			if (FromSlot.ItemData.StaticDataID != 0 && ToSlot.ItemData.StaticDataID == 0)
			{
				CopySlotData_internal(FromSlot, ToSlot, 1);
				FromSlot.CurrentStackSize -= 1;

				if (FromSlot.CurrentStackSize <= 0)
				{
					ClearSlot_internal(FromSlot);
				}
				
				if (ToInventoryType == EInventoryType::HotKey && ToSlotIndex == TargetInventory->ActiveHotkeyIndex)
				{
					bAddedToActiveSlot = true;
				}
			}
		}
	}

	//========================================
	// 방어구 장착 해제 로직
	//========================================

	// 방어구 탈착 여부 플래그
	bool bArmorUnequipped = false;

	// 방어구라면 장착 해제
	if (FromInventoryType == EInventoryType::Equipment)
	{
		UTSItemUseHelperLibrary::UnequipArmor_Lib(this,FromSlotIndex);
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
		if (UTSItemHelperLibrary::GetItemData_Lib(TargetInventory, ToSlot.ItemData.StaticDataID, ItemInfo))
		{
			// 방어구인 경우에만 장착
			if (ItemInfo.Category == EItemCategory::ARMOR)
			{
				UTSItemUseHelperLibrary::EquipArmor_Lib(TargetInventory,ItemInfo, ToSlotIndex);
				bArmorEquipped = true;
			}
		}
	}

	// 활성화 슬롯 변경 시 또는 방어구 탈착 시 브로드캐스트
	if (bAddedToActiveSlot || bArmorUnequipped || bArmorEquipped)
	{
		SourceInventory->HandleActiveHotkeyIndexChanged_internal();
		TargetInventory->HandleActiveHotkeyIndexChanged_internal();
	}

	// 소스 인벤토리의 델리게이트 브로드캐스트
	if (SourceInventory)
	{
		SourceInventory->HandleInventoryChanged_internal();
	}

	// 타겟 인벤토리의 델리게이트 브로드캐스트
	if (TargetInventory && TargetInventory != SourceInventory)
	{
		TargetInventory->HandleInventoryChanged_internal();
	}
	
	RequestingPlayer->ClientNotifyTransferResult(true);
}

	//--------------------
	// 후 처리 알림 (for UI)
	//--------------------

void UTSInventoryMasterComponent::HandleInventoryChanged_internal()
{
	OnInventoryUpdated.Broadcast(HotkeyInventory, EquipmentInventory, BagInventory);
}

void UTSInventoryMasterComponent::HandleActiveHotkeyIndexChanged_internal()
{
	if (ActiveHotkeyIndex >= 0 && ActiveHotkeyIndex < HotkeyInventory.InventorySlotContainer.Num())
	{
		OnHotkeyActivated.Broadcast(ActiveHotkeyIndex, HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex]);
		UTSItemUseHelperLibrary::EquipActiveHotkeyItem_Lib(this);
	}
	else
	{
		UTSItemUseHelperLibrary::UnequipCurrentItem_Lib(this);
	}
}


#pragma endregion
//======================================================================================================================


