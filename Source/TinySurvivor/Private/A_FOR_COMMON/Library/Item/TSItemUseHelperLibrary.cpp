// All CopyRight From YulRyongGameStudio //


#include "A_FOR_COMMON/Library/Item/TSItemUseHelperLibrary.h"
#include "A_FOR_COMMON/Library/GAS/TSASCLibrary.h"
#include "A_FOR_COMMON/Library/Item/TSInventoryHelperLibrary.h"
#include "A_FOR_COMMON/Library/Item/TSItemHelperLibrary.h"
#include "A_FOR_COMMON/Tag/AbilityGameplayTags.h"
#include "A_FOR_COMMON/Tag/ItemGameplayTags.h"
#include "A_FOR_INGAME/SECTION_GAS/AttributeSet/TSAttributeSet.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSInventoryMasterComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/RefactoringFloder/TSInventoryGASControlComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/RefactoringFloder/TSRFEquipArmor.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/TSEquippedItem.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Character/TSCharacter.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Controller/TSPlayerController.h"

	//--------------------
	// 아이템 사용 
	//--------------------

void UTSItemUseHelperLibrary::UseItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 InSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	// 서버 권한 체크: 서버에서만 실행
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	// 유효한 핫 키인지 체크
	if (!UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(InInventoryMasterComponent, EInventoryType::HotKey, InSlotIndex)) return;

	// 핫키 슬롯 가져오기
	FSlotStructMaster& Slot = InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer[InSlotIndex];

	// 핫키 슬롯 검증
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 정보 가져오기
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, Slot.ItemData.StaticDataID, ItemInfo)) return;
	
	//------------------
	// 가방 아이템인 경우
	//------------------
	if (Slot.ItemData.StaticDataID == InInventoryMasterComponent->BagItemID)
	{
		int32 AddSize = 4;
		
		ActionWithBagItem_Lib(InInventoryMasterComponent, Slot, AddSize);
		InInventoryMasterComponent->HandleInventoryChanged_internal();
		return;
	}
	
	//------------------
	// 소모품 아이템인 경우
	//------------------
	if (ItemInfo.Category == EItemCategory::CONSUMABLE)
	{
		ActionWithConsumableItem_Lib(InInventoryMasterComponent, Slot, InSlotIndex);
		InInventoryMasterComponent->HandleInventoryChanged_internal();
		return;
	}

	//------------------
	// 방어구인 경우 ( Ability를 사용 안함. )
	//------------------
	if (ItemInfo.Category == EItemCategory::ARMOR)
	{
		ActionWithArmorItem_Lib(InInventoryMasterComponent, Slot, InSlotIndex);
		InInventoryMasterComponent->HandleInventoryChanged_internal();
		return;
		
	}
}

void UTSItemUseHelperLibrary::ActionWithBagItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, FSlotStructMaster& InTargetSlot, int32& InAdditionalSlots)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	
	// 가방 확장
	bool bExpanded = UTSInventoryHelperLibrary::ExpandBagInventory_Lib(InInventoryMasterComponent, InAdditionalSlots);
	if (!bExpanded) return;
	
	// 아이템 소비
	InTargetSlot.CurrentStackSize -= 1;

	// 아이템이 0개 남은 경우 슬롯 초기화
	if (InTargetSlot.CurrentStackSize <= 0) InInventoryMasterComponent->ClearSlot_internal(InTargetSlot);
}

void UTSItemUseHelperLibrary::ActionWithConsumableItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
	if (!IsValid(ASC)) return;
	
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, InTargetSlot.ItemData.StaticDataID, ItemInfo)) return;
	
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
	
	// Cancel 태그 중 하나라도 있으면 소비 불가
	if (ASC->HasAnyMatchingGameplayTags(CancelTags)) return; 
	
	
	//=======================================================================
	// 2. 기존 리소스 정리
	//=======================================================================
	ClearConsumableAbilityResources_Lib(InInventoryMasterComponent);
	
	
	//=======================================================================
	// 3. 몽타주 재생 (Multicast)
	//=======================================================================
	bool bMontageStarted = false;
	if (!ItemInfo.ConsumableData.ConsumptionMontage.IsValid()) return;
	
	UAnimMontage* Montage = ItemInfo.ConsumableData.ConsumptionMontage.LoadSynchronous();
	if (!IsValid(Montage)) return;
	
	ATSCharacter* Character = Cast<ATSCharacter>(InInventoryMasterComponent->GetOwner());
	if (!IsValid(Character)) return;
	
	// 서버 시작 시간 기록
	float ServerStartTime = InInventoryMasterComponent->GetWorld()->GetTimeSeconds();

	// 서버 시간을 함께 전달
	Character->Multicast_PlayConsumeMontage(Montage, 1.0f, ServerStartTime);
	bMontageStarted = true;

	// 몽타주 재생 실패 시 중단
	if (ItemInfo.ConsumableData.ConsumptionMontage.IsValid() && !bMontageStarted) return;
	
	
	//=======================================================================
	// 4. 어빌리티 발동
	//=======================================================================
	GrantAndScheduleConsumableAbility_Lib(InInventoryMasterComponent, ItemInfo, InTargetSlotIndex, ASC);
}

void UTSItemUseHelperLibrary::ActionWithArmorItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	// 아이템 사용 어빌리티 활성화
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
	if (!IsValid(ASC)) return;
	
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, InTargetSlot.ItemData.StaticDataID, ItemInfo)) return;
	
	// 방어구 장착 로직 (기존 방식 유지)
	UnequipCurrentItem_Lib(InInventoryMasterComponent);
		
	int32 TargetSlotIndex = InInventoryMasterComponent->FindEquipmentSlot_internal(ItemInfo.ArmorData.EquipSlot);
	if (TargetSlotIndex == -1) return;

	// PlayerController 가져오기
	ATSPlayerController* PC = Cast<ATSPlayerController>(Cast<APawn>(InInventoryMasterComponent->GetOwner())->GetController());
	if (!IsValid(PC)) return;

	// Internal_TransferItem()에서 자동으로 EquipArmor() 호출됨
	InInventoryMasterComponent->TransferItem(InInventoryMasterComponent, InInventoryMasterComponent, EInventoryType::HotKey, InTargetSlotIndex,EInventoryType::Equipment, TargetSlotIndex, true, PC);
}


	//--------------------
	// 아이템 소비
	//--------------------

void UTSItemUseHelperLibrary::OnItemConsumedEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* Payload)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	if (!Payload) return;

	// 권한 체크 강화: 서버에서만 실행
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	// 추가 체크: 로컬 Role이 Authority인지 확인
	if (InInventoryMasterComponent->GetOwner()->GetLocalRole() != ROLE_Authority) return;

	int32 SlotIndex = static_cast<int32>(Payload->EventMagnitude);

	if (!UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(InInventoryMasterComponent, EInventoryType::HotKey, SlotIndex)) return;

	FSlotStructMaster& Slot = InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer[SlotIndex];

	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 소비
	Slot.CurrentStackSize -= 1;
	if (Slot.CurrentStackSize <= 0)
	{
		InInventoryMasterComponent->ClearSlot_internal(Slot);
	}

	InInventoryMasterComponent->HandleInventoryChanged_internal();

	// 활성화 슬롯 상태 변경 시 브로드캐스트
	if (Slot.CurrentStackSize == 0)
	{
		InInventoryMasterComponent->HandleActiveHotkeyIndexChanged_internal();
	}
}

void UTSItemUseHelperLibrary::ClearConsumableAbilityResources_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	UTSInventoryGASControlComponent* GasControlComponent = InInventoryMasterComponent->GetOwner()->FindComponentByClass<UTSInventoryGASControlComponent>();
	if (!IsValid(GasControlComponent)) return;
	
	// Timer 정리
	if (GasControlComponent->ConsumableAbilityTriggerTimer.IsValid())
	{
		if (UWorld* World = InInventoryMasterComponent->GetWorld())
		{
			World->GetTimerManager().ClearTimer(GasControlComponent->ConsumableAbilityTriggerTimer);
		}
		GasControlComponent->ConsumableAbilityTriggerTimer.Invalidate();
	}

	// Ability Spec 제거
	if (GasControlComponent->ActiveConsumableAbilityHandle.IsValid())
	{
		UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
		if (IsValid(ASC))
		{
			FGameplayAbilitySpec* ExistingSpec = ASC->FindAbilitySpecFromHandle(GasControlComponent->ActiveConsumableAbilityHandle);

			if (ExistingSpec)
			{
				if (ExistingSpec->IsActive())
				{
					ASC->CancelAbilityHandle(GasControlComponent->ActiveConsumableAbilityHandle);
				}
				
				ASC->ClearAbility(GasControlComponent->ActiveConsumableAbilityHandle);
			}
		}
		GasControlComponent->ActiveConsumableAbilityHandle = FGameplayAbilitySpecHandle();
	}
	
}

bool UTSItemUseHelperLibrary::GrantAndScheduleConsumableAbility_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo, int32 InSlotIndex,UAbilitySystemComponent* InASC)
{
	if (!IsValid(InInventoryMasterComponent)) return false;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return false;
	
	UTSInventoryGASControlComponent* GasControlComponent = InInventoryMasterComponent->GetOwner()->FindComponentByClass<UTSInventoryGASControlComponent>();
	if (!IsValid(GasControlComponent)) return false;
	
	if (!InItemInfo.AbilityBP) return false;

	FGameplayAbilitySpec Spec(InItemInfo.AbilityBP, 1, 0);
	FGameplayAbilitySpecHandle SpecHandle = InASC->GiveAbility(Spec);

	if (!SpecHandle.IsValid()) return false;

	// 새로운 SpecHandle 저장
	GasControlComponent->ActiveConsumableAbilityHandle = SpecHandle;

	// EventData 전달
	FGameplayEventData EventData;
	EventData.EventTag = ItemTags::TAG_Ability_Item_Consume;
	EventData.EventMagnitude = static_cast<float>(InSlotIndex);
	EventData.Instigator = InASC->GetOwner();
	EventData.Target = InASC->GetOwner();

	// TWeakObjectPtr로 안전하게 캡처
	TWeakObjectPtr<UAbilitySystemComponent> WeakASC(InASC);
	TWeakObjectPtr<UTSInventoryGASControlComponent> WeakGasControlComponent(GasControlComponent);
	
	// 다음 틱에서 트리거, Timer Handle을 멤버 변수로 관리
	InInventoryMasterComponent->GetWorld()->GetTimerManager().SetTimer
	(
		GasControlComponent->ConsumableAbilityTriggerTimer,[WeakASC, WeakGasControlComponent, SpecHandle, EventData]()
		{
			// 유효성 체크
			if (!WeakASC.IsValid() && !IsValid(WeakASC.Get())) return;
			if (!WeakGasControlComponent.IsValid() && !IsValid(WeakGasControlComponent.Get())) return;
			
			UAbilitySystemComponent* ASC = WeakASC.Get();
			UTSInventoryGASControlComponent* GasControlComponent = WeakGasControlComponent.Get();
			
			// SpecHandle 재검증
			if (GasControlComponent->ActiveConsumableAbilityHandle == SpecHandle&& GasControlComponent->ActiveConsumableAbilityHandle.IsValid())
			{
				ASC->TriggerAbilityFromGameplayEvent(SpecHandle, ASC->AbilityActorInfo.Get(), ItemTags::TAG_Ability_Item_Consume, &EventData, *ASC);
			}
		},
		
		0.01f, false
	);

	return true;
	
}

	//--------------------
	// 아이템 장착
	//--------------------

void UTSItemUseHelperLibrary::EquipActiveHotkeyItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	if (InInventoryMasterComponent->ActiveHotkeyIndex < 0 || InInventoryMasterComponent->ActiveHotkeyIndex >= InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer.Num())
	{
		UnequipCurrentItem_Lib(InInventoryMasterComponent);
		return;
	}

	const FSlotStructMaster& ActiveSlot = InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer[InInventoryMasterComponent->ActiveHotkeyIndex];

	if (ActiveSlot.ItemData.StaticDataID == 0 || ActiveSlot.CurrentStackSize <= 0)
	{
		UnequipCurrentItem_Lib(InInventoryMasterComponent);
		return;
	}

	// 서버에서만 실행
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;
	
	UnequipCurrentItem_Lib(InInventoryMasterComponent);

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, ActiveSlot.ItemData.StaticDataID, ItemInfo)) return;

	// 아이템 ID 캐싱 (UnequipCurrentItem에서 사용)
	InInventoryMasterComponent->CachedEquippedItemID = ActiveSlot.ItemData.StaticDataID;

	// 무기와 도구 모두 동일 GE 사용
	if (ItemInfo.Category == EItemCategory::WEAPON || ItemInfo.Category == EItemCategory::TOOL)
	{
		ApplyWeaponStats_Lib(InInventoryMasterComponent, ItemInfo);
	}
	if (ItemInfo.Category == EItemCategory::TOOL)
	{
		ApplyToolTags_Lib(InInventoryMasterComponent, ItemInfo); // 채취 태그
	}

	// 아이템 액터 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InInventoryMasterComponent->GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	InInventoryMasterComponent->CurrentEquippedItem = InInventoryMasterComponent->GetWorld()->SpawnActor<ATSEquippedItem>(ATSEquippedItem::StaticClass(),FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);

	if (!InInventoryMasterComponent->CurrentEquippedItem)
	{
		RemoveWeaponStats_Lib(InInventoryMasterComponent); // 스탯도 다시 제거
		InInventoryMasterComponent->CachedEquippedItemID = 0;
		return;
	}

	// 메시 설정
	if (UStaticMesh* LoadedMesh = ItemInfo.WorldMesh.LoadSynchronous())
	{
		InInventoryMasterComponent->CurrentEquippedItem->SetMesh(LoadedMesh);
	}

	ATSCharacter* TSCharacter = Cast<ATSCharacter>(InInventoryMasterComponent->GetOwner());
	if (!IsValid(TSCharacter)) return;
	
	if (ItemInfo.Category == EItemCategory::TOOL)
	{
		// 도구 전용 처리: - 캐릭터 파트 요청에 따라 도구만 왼손(Ws_l) 소켓에 장착.
		InInventoryMasterComponent->CurrentEquippedItem->AttachToComponent(TSCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("Ws_l"));

		// 도구는 기본 스케일
		InInventoryMasterComponent->EquippedItemScale = FVector(1.0f);
	}
	else
	{
		InInventoryMasterComponent->CurrentEquippedItem->AttachToComponent(TSCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("Ws_r"));

		//	회복약 전용 처리: - 소모품 회복약만 스케일을 0.3 축소. (회복약 에셋이 큰 문제로)
		const int32 HealPotionID = 304;

		// 회복약만 스케일 축소
		if (ItemInfo.ItemID == HealPotionID)
		{
			// 스케일 값 저장 (리플리케이션됨)
			InInventoryMasterComponent->EquippedItemScale = FVector(0.5f);

			// 메시 컴포넌트에 직접 적용
			if (InInventoryMasterComponent->CurrentEquippedItem->MeshComp)
			{
				InInventoryMasterComponent->CurrentEquippedItem->MeshComp->SetRelativeScale3D(InInventoryMasterComponent->EquippedItemScale);
			}
		}
		else
		{
			InInventoryMasterComponent->EquippedItemScale = FVector(1.0f);}
		}

	TSCharacter->SetAnimType(ItemInfo.AnimType);
	
}

void UTSItemUseHelperLibrary::UnequipCurrentItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	/*
		슬롯 변경 시 (예: 3번 → 1번) ActiveHotkeyIndex가 먼저 갱신되므로
		GetActiveHotkeySlot()으로 조회하면 이미 새 슬롯(1번)의 정보가 반환됨.
		따라서 해제되는 아이템(3번)의 ID를 올바르게 로그에 남기기 위해
		EquipActiveHotkeyItem()에서 장착 시점에 CachedEquippedItemID에 ID를 저장하고,
		UnequipCurrentItem()에서 캐싱된 값을 사용.
	*/

	// 서버에서 아이템 장착 해제
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	// 1. 무기 스탯 제거 (CurrentEquippedItem이 nullptr이 되기 전에)
	RemoveWeaponStats_Lib(InInventoryMasterComponent);

	// 2. 도구 태그 제거
	RemoveToolTags_Lib(InInventoryMasterComponent);

	// 3. 아이템 제거
	if (InInventoryMasterComponent->CurrentEquippedItem)
	{
		InInventoryMasterComponent->CurrentEquippedItem->Destroy();
		InInventoryMasterComponent->CurrentEquippedItem = nullptr;
	}

	// 4. 스케일 초기화
	InInventoryMasterComponent->EquippedItemScale = FVector(1.0f);

	// 5. 캐싱 초기화
	InInventoryMasterComponent->CachedEquippedItemID = 0;

	// 6. 애니메이션 타입 초기화
	ATSCharacter* TSCharacter = Cast<ATSCharacter>(InInventoryMasterComponent->GetOwner());
	if (!IsValid(TSCharacter)) return;
	TSCharacter->SetAnimType(EItemAnimType::NONE);
}

	//--------------------
	// 방어구
	//--------------------

void UTSItemUseHelperLibrary::HandleCurrentEquippedItemChanged_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	
	// 클라이언트에서 CurrentEquippedItem이 리플리케이션되면 호출됨
	if (InInventoryMasterComponent->CurrentEquippedItem && InInventoryMasterComponent->CurrentEquippedItem->MeshComp && InInventoryMasterComponent->EquippedItemScale != FVector(1.0f))
	{
		InInventoryMasterComponent->CurrentEquippedItem->MeshComp->SetRelativeScale3D(InInventoryMasterComponent->EquippedItemScale);
	}
	
}

void UTSItemUseHelperLibrary::EquipArmor_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo, int32 InArmorSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;
    
	EEquipSlot ArmorSlot = InItemInfo.ArmorData.EquipSlot;
   
    // 기존 방어구 해제
   UnequipArmor_Lib(InInventoryMasterComponent, InArmorSlotIndex);
   
    // 메시 생성 및 장착
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = InInventoryMasterComponent->GetOwner();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
   
    ATSEquippedItem* EquippedArmor = InInventoryMasterComponent->GetWorld()->SpawnActor<ATSEquippedItem>(ATSEquippedItem::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
   
    ATSCharacter* TSCharacter = Cast<ATSCharacter>(InInventoryMasterComponent->GetOwner());
    if (!TSCharacter) return;
   
    if (UStaticMesh* LoadedMesh = InItemInfo.WorldMesh.LoadSynchronous())
    {
    	if (ArmorSlot == EEquipSlot::LEG)
    	{
    		EquippedArmor->SetLegMesh(LoadedMesh);
    		
    		if (EquippedArmor->LeftLegMeshComp) EquippedArmor->LeftLegMeshComp->AttachToComponent(TSCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("LeftLegSocket"));
    		if (EquippedArmor->RightLegMeshComp) EquippedArmor->RightLegMeshComp->AttachToComponent(TSCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("RightLegSocket"));
    	}
    	else
    	{
    		FName SocketName;
    		if (ArmorSlot == EEquipSlot::HEAD) SocketName = TEXT("HeadSocket");
    		else SocketName = TEXT("TorsoSocket");
   
    		EquippedArmor->SetMesh(LoadedMesh);
    		EquippedArmor->AttachToComponent(TSCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
    	}
    }
   
    // 액터 저장
    InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].EquippedArmor = EquippedArmor;
   
    //=======================================================================
    // GE 적용
    //=======================================================================

    UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
    if (!IsValid(ASC)) return;
   
    FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
    ContextHandle.AddSourceObject(InInventoryMasterComponent);
   
    //=======================================================================
    // 적용 전 스탯 로그
    //=======================================================================
   
    // 1. 공통 스탯 (HealthBonus)
    if (InInventoryMasterComponent->ArmorCommonStatEffectClass)
    {
    	FGameplayEffectSpecHandle CommonSpec = ASC->MakeOutgoingSpec(InInventoryMasterComponent->ArmorCommonStatEffectClass, 1, ContextHandle);
   
    	CommonSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.HealthBonus"),InItemInfo.ArmorData.HealthBonus);
   
    	FActiveGameplayEffectHandle CommonHandle =ASC->ApplyGameplayEffectSpecToSelf(*CommonSpec.Data);
   
    	// 핸들 저장
    	InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorCommonEffectHandle = CommonHandle;
    }
   
    // 2. 이펙트 스탯 (EffectTag별)
    if (InInventoryMasterComponent->ArmorEffectStatEffectClass && InItemInfo.EffectTag_Armor.IsValid())
    {
    	FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(InInventoryMasterComponent->ArmorEffectStatEffectClass, 1, ContextHandle);
   
    	// =======================================
    	// 모든 Modifier 값 초기화 (0으로)
    	// =======================================
    	EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReflection, 0.0f);
    	EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReduction, 0.0f);
    	EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_MoveSpeed, 0.0f);
   
    	// =======================================
    	// EffectTag에 따라 실제 값 설정
    	// =======================================
    	if (InItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_DAMAGE_REFLECT))
    	{
    		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReflection, InItemInfo.EffectValue);
    	}
    	else if (InItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_DAMAGE_REDUCTION))
    	{
    		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReduction, InItemInfo.EffectValue);
    	}
    	else if (InItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_MOVE_SPEED))
    	{
    		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_MoveSpeed, InItemInfo.EffectValue);
    	}
   
    	FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
   
    	// 핸들 저장
    	InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorEffectHandle = EffectHandle;
    }
}

void UTSItemUseHelperLibrary::UnequipArmor_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 InArmorSlotIndex)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;
    
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
	if (!IsValid(ASC)) return;
    
	//=======================================================================
	// 1. EffectStats GE 제거
	//=======================================================================
	if (InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorEffectHandle);
		InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorEffectHandle.Invalidate();
	}
    
	//=======================================================================
	// 2. CommonStats GE 제거
	//=======================================================================
	if (InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorCommonEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorCommonEffectHandle);
		InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].ArmorCommonEffectHandle.Invalidate();
	}
    
	//=======================================================================
	// 3. 메시 제거
	//=======================================================================
    
	AActor* ArmorMeshActor = InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].EquippedArmor;
    
	if (IsValid(ArmorMeshActor))
	{
		// nullptr + Pending Kill 체크 포함
		if (ArmorMeshActor->IsActorBeingDestroyed())
		{
			InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].EquippedArmor = nullptr;
			return;
		}
    
		// Destroy 시도
		ArmorMeshActor->Destroy();
		InInventoryMasterComponent->EquippedArmors[InArmorSlotIndex].EquippedArmor = nullptr;
	}
    
	// Health 클램핑
	UTSAttributeSet* AttrSet = const_cast<UTSAttributeSet*>(ASC->GetSet<UTSAttributeSet>());
	if (AttrSet && AttrSet->GetHealth() > AttrSet->GetMaxHealth())
	{
		AttrSet->SetHealth(AttrSet->GetMaxHealth());
	}
}

void UTSItemUseHelperLibrary::OnArmorHitEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* InPayload)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!InPayload || !InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	// 현재 장착된 모든 방어구의 내구도 감소

	for (int32 i = 0; i < InInventoryMasterComponent->EquippedArmors.Num(); ++i)
	{
		// 장착된 방어구가 없으면 스킵
		if (!InInventoryMasterComponent->EquippedArmors[i].EquippedArmor) continue;

		// 해당 슬롯의 장비 인벤토리 확인
		if (!UTSInventoryHelperLibrary::IsValidSlotIndex_Lib(InInventoryMasterComponent, EInventoryType::Equipment, i)) continue;

		FSlotStructMaster& Slot = InInventoryMasterComponent->EquipmentInventory.InventorySlotContainer[i];

		// 슬롯이 비어있으면 스킵
		if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) continue;

		// 아이템 정보 조회
		FItemData ItemInfo;
		if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, Slot.ItemData.StaticDataID, ItemInfo)) continue;

		// 방어구가 아니면 스킵 (안전 체크)
		if (ItemInfo.Category != EItemCategory::ARMOR) continue;

		// 방어구 피격으로 인한 내구도 감소
		int32 DurabilityLoss = ItemInfo.ArmorData.DurabilityLossAmount;
		Slot.ItemData.CurrentDurability -= DurabilityLoss;
		Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지

		// 내구도가 0 이하가 되면 방어구 파괴
		if (Slot.ItemData.CurrentDurability <= 0)
		{
			// 방어구 탈착 (스탯 제거, 메시 제거)
			
			UnequipArmor_Lib(InInventoryMasterComponent,i);

			// 슬롯 초기화
			InInventoryMasterComponent->ClearSlot_internal(Slot);
		}
	}

	// UI 업데이트
	InInventoryMasterComponent->HandleInventoryChanged_internal();
}

	//--------------------
	// 무기 
	//--------------------

void UTSItemUseHelperLibrary::ApplyWeaponStats_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
	if (!IsValid(ASC)) return;

	if (!InInventoryMasterComponent->WeaponStatEffectClass) return;

	//=======================================================================
	// 적용 전 캐릭터 기본 스탯 조회
	//=======================================================================
	const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
	if (!IsValid(AttrSet)) return;

	//=======================================================================
	// GameplayEffect 적용
	//=======================================================================

	// GameplayEffect Context 생성
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(InInventoryMasterComponent);

	// GameplayEffect Spec 생성
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(InInventoryMasterComponent->WeaponStatEffectClass, 1, EffectContext);

	if (!SpecHandle.IsValid()) return;
	
	// 무기/도구 스탯 추출
	float DamageValue = 0.0f;
	float AttackSpeed = 1.0f;
	float AttackRange = 100.0f;

	if (InItemInfo.Category == EItemCategory::WEAPON)
	{
		DamageValue = InItemInfo.WeaponData.DamageValue;
		AttackSpeed = InItemInfo.WeaponData.AttackSpeed;
		AttackRange = InItemInfo.WeaponData.AttackRange;
	}
	else if (InItemInfo.Category == EItemCategory::TOOL)
	{
		DamageValue = InItemInfo.ToolData.DamageValue;
		AttackSpeed = InItemInfo.ToolData.AttackSpeed;
		AttackRange = InItemInfo.ToolData.AttackRange;
	}

	// SetByCaller로 스탯 전달
	SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackDamage, DamageValue);
	SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackSpeed, AttackSpeed);
	SpecHandle.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_AttackRange, AttackRange);

	// Effect 적용 및 핸들 저장
	InInventoryMasterComponent->CurrentWeaponEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
}

void UTSItemUseHelperLibrary::RemoveWeaponStats_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	// Authority 체크
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	// 핸들이 유효하지 않으면 조기 종료
	if (!InInventoryMasterComponent->CurrentWeaponEffectHandle.IsValid()) return;

	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
	if (!IsValid(ASC))
	{
		InInventoryMasterComponent->CurrentWeaponEffectHandle.Invalidate();
		return;
	}

	const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
	if (!IsValid(AttrSet))
	{
		InInventoryMasterComponent->CurrentWeaponEffectHandle.Invalidate();
		return;
	}

	//=======================================================================
	// GameplayEffect 제거
	//=======================================================================
	ASC->RemoveActiveGameplayEffect(InInventoryMasterComponent->CurrentWeaponEffectHandle);
	InInventoryMasterComponent->CurrentWeaponEffectHandle.Invalidate();
	
}

void UTSItemUseHelperLibrary::OnWeaponAttackEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* InPayload)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!InPayload || !InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	// 현재 활성화된 핫키 슬롯 확인
	if (InInventoryMasterComponent->ActiveHotkeyIndex < 0 || InInventoryMasterComponent->ActiveHotkeyIndex >= InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer.Num()) return;

	// 현재 활성화된 슬롯 가져오기
	FSlotStructMaster& Slot = InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer[InInventoryMasterComponent->ActiveHotkeyIndex];

	// 슬롯에 아이템이 없거나 스택이 0이면 처리하지 않음
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 정보 조회
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, Slot.ItemData.StaticDataID, ItemInfo)) return;

	// 무기가 아닌 경우 처리하지 않음
	if (ItemInfo.Category != EItemCategory::WEAPON) return;

	// 무기 사용으로 인한 내구도 감소
	int32 DurabilityLoss = ItemInfo.WeaponData.DurabilityLossAmount;
	Slot.ItemData.CurrentDurability -= DurabilityLoss;
	Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지

	// 내구도가 0 이하가 되면 아이템 파괴
	if (Slot.ItemData.CurrentDurability <= 0)
	{
		InInventoryMasterComponent->ClearSlot_internal(Slot); // 슬롯 초기화
		InInventoryMasterComponent->HandleActiveHotkeyIndexChanged_internal(); // 장착 해제
	}

	// UI 업데이트
	InInventoryMasterComponent->HandleInventoryChanged_internal();
	
}

	//--------------------
	// 도구 관련
	//--------------------

void UTSItemUseHelperLibrary::ApplyToolTags_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
	if (!IsValid(ASC)) return;

	// 도구의 HarvestTargetTags를 캐릭터 ASC에 추가
	for (const FGameplayTag& Tag : InItemInfo.ToolData.HarvestTargetTag)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
	
}

void UTSItemUseHelperLibrary::RemoveToolTags_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	// Authority 체크
	if (!InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(InInventoryMasterComponent->GetOwner());
	if (!IsValid(ASC)) return;

	// 현재 장착된 아이템이 유효한 ID를 갖고 있는지 확인
	if (InInventoryMasterComponent->CachedEquippedItemID == 0) return;

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, InInventoryMasterComponent->CachedEquippedItemID, ItemInfo)) return;

	// 도구가 아니면 제거할 태그가 없음
	if (ItemInfo.Category != EItemCategory::TOOL) return;

	// 도구의 HarvestTargetTags를 ASC에서 제거
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

void UTSItemUseHelperLibrary::OnToolHarvestEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* InPayload)
{
	if (!IsValid(InInventoryMasterComponent)) return;
	if (!IsValid(InInventoryMasterComponent->GetOwner())) return;
	
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!InPayload || !InInventoryMasterComponent->GetOwner()->HasAuthority()) return;

	// 현재 활성화된 핫키 슬롯 확인 : 유효하지 않은 인덱스이면 처리하지 않음
	if (InInventoryMasterComponent->ActiveHotkeyIndex < 0 || InInventoryMasterComponent->ActiveHotkeyIndex >= InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer.Num()) return;

	// 현재 활성화된 슬롯 가져오기
	FSlotStructMaster& Slot = InInventoryMasterComponent->HotkeyInventory.InventorySlotContainer[InInventoryMasterComponent->ActiveHotkeyIndex];

	// 슬롯에 아이템이 없거나 스택이 0이면 처리하지 않음
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 정보 조회
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData_Lib(InInventoryMasterComponent, Slot.ItemData.StaticDataID, ItemInfo)) return;

	// 도구가 아닌 경우 처리하지 않음
	if (ItemInfo.Category != EItemCategory::TOOL) return;

	// 도구 사용으로 인한 내구도 감소
	int32 DurabilityLoss = ItemInfo.ToolData.DurabilityLossAmount;
	Slot.ItemData.CurrentDurability -= DurabilityLoss;
	Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지

	// 내구도가 0 이하가 되면 아이템 파괴
	if (Slot.ItemData.CurrentDurability <= 0)
	{
		InInventoryMasterComponent->ClearSlot_internal(Slot); // 슬롯 초기화
		InInventoryMasterComponent->HandleActiveHotkeyIndexChanged_internal(); // 장착 해제
	}

	// UI 업데이트
	InInventoryMasterComponent->HandleInventoryChanged_internal();
}
