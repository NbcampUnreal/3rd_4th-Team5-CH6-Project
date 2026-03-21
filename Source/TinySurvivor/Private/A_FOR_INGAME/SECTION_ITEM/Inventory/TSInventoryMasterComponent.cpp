// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSInventoryMasterComponent.h"

#include "Net/UnrealNetwork.h"

#include "A_FOR_INGAME/SECTION_GAS//AttributeSet/TSAttributeSet.h"

#include "A_FOR_INGAME/SECTION_PLAYER/Character/TSCharacter.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Controller/TSPlayerController.h"

#include "A_FOR_COMMON/Library/GAS/TSASCLibrary.h"
#include "A_FOR_COMMON/Library/Item/TSItemHelperLibrary.h"
#include "A_FOR_COMMON/Library/System/TSDecayLibrary.h"
#include "A_FOR_COMMON/Library/System/TSSystemGetterLibrary.h"
#include "A_FOR_COMMON/Library/System/TSTimeLibrary.h"

#include "A_FOR_INGAME/SECTION_ITEM/Item/TSEquippedItem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/ItemData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Runtime/ItemInstance.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/RefactoringFloder/RFEquipArmor.h"

#include "A_FOR_COMMON/Tag/HelpHeader/MasterTagHelpHeader.h"




	
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
	HandleCurrentEquippedItemChanged_internal();
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
	ClearConsumableAbilityResources_internal();
	
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
	UseItem_internal(SlotIndex);
}

void UTSInventoryMasterComponent::UseItem_internal(int32 SlotIndex)
{
	// 서버 권한 체크: 서버에서만 실행
	if (!GetOwner()->HasAuthority()) return;

	// 유효한 핫 키인지 체크
	if (!IsValidSlotIndex_internal(EInventoryType::HotKey, SlotIndex)) return;

	// 핫키 슬롯 가져오기
	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];

	// 핫키 슬롯 검증
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 정보 가져오기
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this, Slot.ItemData.StaticDataID, ItemInfo)) return;
	
	//------------------
	// 가방 아이템인 경우
	//------------------
	if (Slot.ItemData.StaticDataID == BagItemID)
	{
		ActionWithBagItem_internal(Slot);
		HandleInventoryChanged_internal();
		return;
	}
	
	//------------------
	// 소모품 아이템인 경우
	//------------------
	if (ItemInfo.Category == EItemCategory::CONSUMABLE)
	{
		ActionWithConsumableItem_internal(Slot, SlotIndex);
		HandleInventoryChanged_internal();
		return;
	}

	//------------------
	// 방어구인 경우 ( Ability를 사용 안함. )
	//------------------
	if (ItemInfo.Category == EItemCategory::ARMOR)
	{
		ActionWithArmorItem_internal(Slot, SlotIndex);
		HandleInventoryChanged_internal();
		return;
		
	}
}

void UTSInventoryMasterComponent::ActionWithBagItem_internal(FSlotStructMaster& InTargetSlot)
{
	// 가방 확장
	bool bExpanded = ExpandBagInventory_internal(BagSlotIncrement);
	if (!bExpanded) return;
	
	// 아이템 소비
	InTargetSlot.CurrentStackSize -= 1;

	// 아이템이 0개 남은 경우 슬롯 초기화
	if (InTargetSlot.CurrentStackSize <= 0) ClearSlot_internal(InTargetSlot);
}

void UTSInventoryMasterComponent::ActionWithConsumableItem_internal(FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex)
{
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC)) return;
	
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this, InTargetSlot.ItemData.StaticDataID, ItemInfo)) return;
	
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
	ClearConsumableAbilityResources_internal();
	
	
	//=======================================================================
	// 3. 몽타주 재생 (Multicast)
	//=======================================================================
	bool bMontageStarted = false;
	if (!ItemInfo.ConsumableData.ConsumptionMontage.IsValid()) return;
	
	UAnimMontage* Montage = ItemInfo.ConsumableData.ConsumptionMontage.LoadSynchronous();
	if (!IsValid(Montage)) return;
	
	ATSCharacter* Character = Cast<ATSCharacter>(GetOwner());
	if (!IsValid(Character)) return;
	
	// 서버 시작 시간 기록
	float ServerStartTime = GetWorld()->GetTimeSeconds();

	// 서버 시간을 함께 전달
	Character->Multicast_PlayConsumeMontage(Montage, 1.0f, ServerStartTime);
	bMontageStarted = true;

	// 몽타주 재생 실패 시 중단
	if (ItemInfo.ConsumableData.ConsumptionMontage.IsValid() && !bMontageStarted) return;
	
	
	//=======================================================================
	// 4. 어빌리티 발동
	//=======================================================================
	GrantAndScheduleConsumableAbility_internal(ItemInfo, InTargetSlotIndex, ASC);
}

void UTSInventoryMasterComponent::ActionWithArmorItem_internal(FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex)
{
	// 아이템 사용 어빌리티 활성화
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC)) return;
	
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this, InTargetSlot.ItemData.StaticDataID, ItemInfo)) return;
	
	// 방어구 장착 로직 (기존 방식 유지)
	UnequipCurrentItem_internal();
		
	int32 TargetSlotIndex = FindEquipmentSlot_internal(ItemInfo.ArmorData.EquipSlot);
	if (TargetSlotIndex == -1) return;

	// PlayerController 가져오기
	ATSPlayerController* PC = Cast<ATSPlayerController>(Cast<APawn>(GetOwner())->GetController());
	if (!IsValid(PC)) return;

	// Internal_TransferItem()에서 자동으로 EquipArmor() 호출됨
	TransferItem(this, this, EInventoryType::HotKey, InTargetSlotIndex,EInventoryType::Equipment, TargetSlotIndex, true, PC);
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
	if (GetCurrentBagSlotCount() > 0)
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

	//--------------------
	// 아이템 장착
	//--------------------

bool UTSInventoryMasterComponent::HasItemEquipped_internal() const
{
	FSlotStructMaster ActiveSlot = GetActiveHotkeySlot_internal();
	return ActiveSlot.ItemData.StaticDataID != 0 && ActiveSlot.CurrentStackSize > 0;
}

void UTSInventoryMasterComponent::EquipActiveHotkeyItem_internal()
{
	if (ActiveHotkeyIndex < 0 || ActiveHotkeyIndex >= HotkeyInventory.InventorySlotContainer.Num())
	{
		UnequipCurrentItem_internal();
		return;
	}

	const FSlotStructMaster& ActiveSlot = HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];

	if (ActiveSlot.ItemData.StaticDataID == 0 || ActiveSlot.CurrentStackSize <= 0)
	{
		UnequipCurrentItem_internal();
		return;
	}

	// 서버에서만 실행
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	UnequipCurrentItem_internal();

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this, ActiveSlot.ItemData.StaticDataID, ItemInfo)) return;

	// 아이템 ID 캐싱 (UnequipCurrentItem에서 사용)
	CachedEquippedItemID = ActiveSlot.ItemData.StaticDataID;

	// 무기와 도구 모두 동일 GE 사용
	if (ItemInfo.Category == EItemCategory::WEAPON || ItemInfo.Category == EItemCategory::TOOL)
	{
		ApplyWeaponStats_internal(ItemInfo);
	}
	if (ItemInfo.Category == EItemCategory::TOOL)
	{
		ApplyToolTags_internal(ItemInfo); // 채취 태그
	}

	// 아이템 액터 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentEquippedItem = GetWorld()->SpawnActor<ATSEquippedItem>(ATSEquippedItem::StaticClass(),FVector::ZeroVector,FRotator::ZeroRotator,SpawnParams);

	if (!CurrentEquippedItem)
	{
		RemoveWeaponStats_internal(); // 스탯도 다시 제거
		CachedEquippedItemID = 0;
		return;
	}

	// 메시 설정
	if (UStaticMesh* LoadedMesh = ItemInfo.WorldMesh.LoadSynchronous())
	{
		CurrentEquippedItem->SetMesh(LoadedMesh);
	}

	ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetOwner());
	if (!IsValid(TSCharacter)) return;
	
	if (ItemInfo.Category == EItemCategory::TOOL)
	{
		/*
				도구 전용 처리:
				- 캐릭터 파트 요청에 따라 도구만 왼손(Ws_l) 소켓에 장착.
			*/
		CurrentEquippedItem->AttachToComponent(TSCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("Ws_l"));

		// 도구는 기본 스케일
		EquippedItemScale = FVector(1.0f);
	}
	else
	{
		CurrentEquippedItem->AttachToComponent(TSCharacter->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("Ws_r"));

		/*
				회복약 전용 처리:
				- 소모품 회복약만 스케일을 0.3 축소. (회복약 에셋이 큰 문제로)
			*/
	
		// 스케일 조정 대상
		const int32 HealPotionID = 304;

		// 회복약만 스케일 축소
		if (ItemInfo.ItemID == HealPotionID)
		{
			// 스케일 값 저장 (리플리케이션됨)
			EquippedItemScale = FVector(0.5f);

			// 메시 컴포넌트에 직접 적용
			if (CurrentEquippedItem->MeshComp)
			{
				CurrentEquippedItem->MeshComp->SetRelativeScale3D(EquippedItemScale);
			}
		}
		else
		{
			EquippedItemScale = FVector(1.0f);}
		}

	TSCharacter->SetAnimType(ItemInfo.AnimType);
	
}

void UTSInventoryMasterComponent::UnequipCurrentItem_internal()
{
	/*
		슬롯 변경 시 (예: 3번 → 1번) ActiveHotkeyIndex가 먼저 갱신되므로
		GetActiveHotkeySlot()으로 조회하면 이미 새 슬롯(1번)의 정보가 반환됨.
		따라서 해제되는 아이템(3번)의 ID를 올바르게 로그에 남기기 위해
		EquipActiveHotkeyItem()에서 장착 시점에 CachedEquippedItemID에 ID를 저장하고,
		UnequipCurrentItem()에서 캐싱된 값을 사용.
	*/

	// 서버에서 아이템 장착 해제
	if (!GetOwner()->HasAuthority()) return;

	// 1. 무기 스탯 제거 (CurrentEquippedItem이 nullptr이 되기 전에)
	RemoveWeaponStats_internal();

	// 2. 도구 태그 제거
	RemoveToolTags_internal();

	// 3. 아이템 제거
	if (CurrentEquippedItem)
	{
		CurrentEquippedItem->Destroy();
		CurrentEquippedItem = nullptr;
	}

	// 4. 스케일 초기화
	EquippedItemScale = FVector(1.0f);

	// 5. 캐싱 초기화
	CachedEquippedItemID = 0;

	// 6. 애니메이션 타입 초기화
	ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetOwner());
	if (!IsValid(TSCharacter)) return;
	TSCharacter->SetAnimType(EItemAnimType::NONE);
}

void UTSInventoryMasterComponent::HandleCurrentEquippedItemChanged_internal()
{
	// 클라이언트에서 CurrentEquippedItem이 리플리케이션되면 호출됨
	if (CurrentEquippedItem && CurrentEquippedItem->MeshComp && EquippedItemScale != FVector(1.0f))
	{
		CurrentEquippedItem->MeshComp->SetRelativeScale3D(EquippedItemScale);
	}
}

	//--------------------
	// 방어구
	//--------------------

void UTSInventoryMasterComponent::EquipArmor_internal(const FItemData& ItemInfo, int32 ArmorSlotIndex)
{
    if (!GetOwner()->HasAuthority()) return;
    
	EEquipSlot ArmorSlot = ItemInfo.ArmorData.EquipSlot;
   
    // 기존 방어구 해제
    UnequipArmor_internal(ArmorSlotIndex);
   
    // 메시 생성 및 장착
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
   
    ATSEquippedItem* EquippedArmor = GetWorld()->SpawnActor<ATSEquippedItem>(ATSEquippedItem::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
   
    ATSCharacter* TSCharacter = Cast<ATSCharacter>(GetOwner());
    if (!TSCharacter) return;
   
    if (UStaticMesh* LoadedMesh = ItemInfo.WorldMesh.LoadSynchronous())
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
    EquippedArmors[ArmorSlotIndex].EquippedArmor = EquippedArmor;
   
    //=======================================================================
    // GE 적용
    //=======================================================================

    UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
    if (!IsValid(ASC)) return;
   
    FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
    ContextHandle.AddSourceObject(this);
   
    //=======================================================================
    // 적용 전 스탯 로그
    //=======================================================================
   
    // 1. 공통 스탯 (HealthBonus)
    if (ArmorCommonStatEffectClass)
    {
    	FGameplayEffectSpecHandle CommonSpec = ASC->MakeOutgoingSpec(ArmorCommonStatEffectClass, 1, ContextHandle);
   
    	CommonSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.HealthBonus"),ItemInfo.ArmorData.HealthBonus);
   
    	FActiveGameplayEffectHandle CommonHandle =ASC->ApplyGameplayEffectSpecToSelf(*CommonSpec.Data);
   
    	// 핸들 저장
    	EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle = CommonHandle;
    }
   
    // 2. 이펙트 스탯 (EffectTag별)
    if (ArmorEffectStatEffectClass && ItemInfo.EffectTag_Armor.IsValid())
    {
    	FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(ArmorEffectStatEffectClass, 1, ContextHandle);
   
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
    		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReflection, ItemInfo.EffectValue);
    	}
    	else if (ItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_DAMAGE_REDUCTION))
    	{
    		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_DamageReduction, ItemInfo.EffectValue);
    	}
    	else if (ItemInfo.EffectTag_Armor.MatchesTag(AbilityTags::TAG_State_Modifier_MOVE_SPEED))
    	{
    		EffectSpec.Data->SetSetByCallerMagnitude(ItemTags::TAG_Data_Armor_MoveSpeed, ItemInfo.EffectValue);
    	}
   
    	FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
   
    	// 핸들 저장
    	EquippedArmors[ArmorSlotIndex].ArmorEffectHandle = EffectHandle;
    }
}

void UTSInventoryMasterComponent::UnequipArmor_internal(int32 ArmorSlotIndex)
    {
    	if (!GetOwner()->HasAuthority()) return;
    
		UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
		if (!IsValid(ASC)) return;
    
    	//=======================================================================
    	// 1. EffectStats GE 제거
    	//=======================================================================
    	if (EquippedArmors[ArmorSlotIndex].ArmorEffectHandle.IsValid())
    	{
    		ASC->RemoveActiveGameplayEffect(EquippedArmors[ArmorSlotIndex].ArmorEffectHandle);
    		EquippedArmors[ArmorSlotIndex].ArmorEffectHandle.Invalidate();
    	}
    
    	//=======================================================================
    	// 2. CommonStats GE 제거
    	//=======================================================================
    	if (EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle.IsValid())
    	{
    		ASC->RemoveActiveGameplayEffect(EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle);
    		EquippedArmors[ArmorSlotIndex].ArmorCommonEffectHandle.Invalidate();
    	}
    
    	//=======================================================================
    	// 3. 메시 제거
    	//=======================================================================
    
    	AActor* ArmorMeshActor = EquippedArmors[ArmorSlotIndex].EquippedArmor;
    
    	if (IsValid(ArmorMeshActor))
    	{
    		// nullptr + Pending Kill 체크 포함
    		if (ArmorMeshActor->IsActorBeingDestroyed())
    		{
    			EquippedArmors[ArmorSlotIndex].EquippedArmor = nullptr;
    			return;
    		}
    
    		// Destroy 시도
    		ArmorMeshActor->Destroy();
    		EquippedArmors[ArmorSlotIndex].EquippedArmor = nullptr;
    	}
    
    	// Health 클램핑
    	UTSAttributeSet* AttrSet = const_cast<UTSAttributeSet*>(ASC->GetSet<UTSAttributeSet>());
    	if (AttrSet && AttrSet->GetHealth() > AttrSet->GetMaxHealth())
    	{
    		AttrSet->SetHealth(AttrSet->GetMaxHealth());
    	}
    }
	
void UTSInventoryMasterComponent::OnArmorHitEvent_internal(const FGameplayEventData* Payload)
{
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!Payload || !GetOwner()->HasAuthority()) return;

	// 현재 장착된 모든 방어구의 내구도 감소

	for (int32 i = 0; i < EquippedArmors.Num(); ++i)
	{
		// 장착된 방어구가 없으면 스킵
		if (!EquippedArmors[i].EquippedArmor) continue;

		// 해당 슬롯의 장비 인벤토리 확인
		if (!IsValidSlotIndex_internal(EInventoryType::Equipment, i)) continue;

		FSlotStructMaster& Slot = EquipmentInventory.InventorySlotContainer[i];

		// 슬롯이 비어있으면 스킵
		if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) continue;

		// 아이템 정보 조회
		FItemData ItemInfo;
		if (!UTSItemHelperLibrary::GetItemData(this, Slot.ItemData.StaticDataID, ItemInfo)) continue;

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
			UnequipArmor_internal(i);

			// 슬롯 초기화
			ClearSlot_internal(Slot);
		}
	}

	// UI 업데이트
	HandleInventoryChanged_internal();
}

	//--------------------
	// 무기 
	//--------------------

void UTSInventoryMasterComponent::ApplyWeaponStats_internal(const FItemData& ItemInfo)
{
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC)) return;

	if (!WeaponStatEffectClass) return;

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
	EffectContext.AddSourceObject(this);

	// GameplayEffect Spec 생성
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(WeaponStatEffectClass, 1, EffectContext);

	if (!SpecHandle.IsValid()) return;
	
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
}

void UTSInventoryMasterComponent::RemoveWeaponStats_internal()
{
	// Authority 체크
	if (!GetOwner()->HasAuthority()) return;

	// 핸들이 유효하지 않으면 조기 종료
	if (!CurrentWeaponEffectHandle.IsValid()) return;

	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC))
	{
		CurrentWeaponEffectHandle.Invalidate();
		return;
	}

	const UTSAttributeSet* AttrSet = ASC->GetSet<UTSAttributeSet>();
	if (!IsValid(AttrSet))
	{
		CurrentWeaponEffectHandle.Invalidate();
		return;
	}

	//=======================================================================
	// GameplayEffect 제거
	//=======================================================================
	ASC->RemoveActiveGameplayEffect(CurrentWeaponEffectHandle);
	CurrentWeaponEffectHandle.Invalidate();

}

void UTSInventoryMasterComponent::OnWeaponAttackEvent_internal(const FGameplayEventData* Payload)
{
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!Payload || !GetOwner()->HasAuthority()) return;

	// 현재 활성화된 핫키 슬롯 확인
	if (ActiveHotkeyIndex < 0 || ActiveHotkeyIndex >= HotkeyInventory.InventorySlotContainer.Num()) return;

	// 현재 활성화된 슬롯 가져오기
	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];

	// 슬롯에 아이템이 없거나 스택이 0이면 처리하지 않음
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 정보 조회
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this, Slot.ItemData.StaticDataID, ItemInfo)) return;

	// 무기가 아닌 경우 처리하지 않음
	if (ItemInfo.Category != EItemCategory::WEAPON) return;

	// 무기 사용으로 인한 내구도 감소
	int32 DurabilityLoss = ItemInfo.WeaponData.DurabilityLossAmount;
	Slot.ItemData.CurrentDurability -= DurabilityLoss;
	Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지

	// 내구도가 0 이하가 되면 아이템 파괴
	if (Slot.ItemData.CurrentDurability <= 0)
	{
		ClearSlot_internal(Slot); // 슬롯 초기화
		HandleActiveHotkeyIndexChanged_internal(); // 장착 해제
	}

	// UI 업데이트
	HandleInventoryChanged_internal();
}

	//--------------------
	// 도구 관련
	//--------------------
	
void UTSInventoryMasterComponent::ApplyToolTags_internal(const FItemData& ItemInfo)
{
	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC)) return;

	// 도구의 HarvestTargetTags를 캐릭터 ASC에 추가
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void UTSInventoryMasterComponent::RemoveToolTags_internal()
{
	// Authority 체크
	if (!GetOwner()->HasAuthority()) return;

	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC)) return;

	// 현재 장착된 아이템이 유효한 ID를 갖고 있는지 확인
	if (CachedEquippedItemID == 0) return;

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this, CachedEquippedItemID, ItemInfo)) return;

	// 도구가 아니면 제거할 태그가 없음
	if (ItemInfo.Category != EItemCategory::TOOL) return;

	// 도구의 HarvestTargetTags를 ASC에서 제거
	for (const FGameplayTag& Tag : ItemInfo.ToolData.HarvestTargetTag)
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

void UTSInventoryMasterComponent::OnToolHarvestEvent_internal(const FGameplayEventData* Payload)
{
	// Payload가 유효하지 않거나 서버 권한이 없는 경우 처리하지 않고 반환
	if (!Payload || !GetOwner()->HasAuthority()) return;

	// 현재 활성화된 핫키 슬롯 확인 : 유효하지 않은 인덱스이면 처리하지 않음
	if (ActiveHotkeyIndex < 0 || ActiveHotkeyIndex >= HotkeyInventory.InventorySlotContainer.Num()) return;

	// 현재 활성화된 슬롯 가져오기
	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];

	// 슬롯에 아이템이 없거나 스택이 0이면 처리하지 않음
	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 정보 조회
	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this, Slot.ItemData.StaticDataID, ItemInfo)) return;

	// 도구가 아닌 경우 처리하지 않음
	if (ItemInfo.Category != EItemCategory::TOOL) return;

	// 도구 사용으로 인한 내구도 감소
	int32 DurabilityLoss = ItemInfo.ToolData.DurabilityLossAmount;
	Slot.ItemData.CurrentDurability -= DurabilityLoss;
	Slot.ItemData.CurrentDurability = FMath::Max(0, Slot.ItemData.CurrentDurability); // 음수 방지

	// 내구도가 0 이하가 되면 아이템 파괴
	if (Slot.ItemData.CurrentDurability <= 0)
	{
		ClearSlot_internal(Slot); // 슬롯 초기화
		HandleActiveHotkeyIndexChanged_internal(); // 장착 해제
	}

	// UI 업데이트
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
	
	// 이벤트 리스너 등록 시도 (ASC가 준비될 때까지 재시도)
	TryRegisterEventListeners_internal();

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
	if (!UTSItemHelperLibrary::GetItemData(this, ItemData.StaticDataID, ItemInfo)) return false;

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
	if (!UTSItemHelperLibrary::GetItemData(this, FromSlot.ItemData.StaticDataID, ItemInfo)) return false;

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
	const FInventoryStructMaster* Inventory = GetInventoryByType_internal(InventoryType);
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
	if (!IsValidSlotIndex_internal(InventoryType, SlotIndex)) return;

	FInventoryStructMaster* Inventory = GetInventoryByType_internal(InventoryType);
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
	if (!GetOwner()->HasAuthority() || !IsValidSlotIndex_internal(InventoryType, SlotIndex)) return false;

	FInventoryStructMaster* Inventory = GetInventoryByType_internal(InventoryType);
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

void UTSInventoryMasterComponent::ServerActivateHotkeySlot_internal_Implementation(int32 SlotIndex)
{
	if (SlotIndex < -1 || SlotIndex >= HotkeyInventory.InventorySlotContainer.Num()) return;

	// 슬롯 변경 시 예약된 소모품 Ability Trigger 취소
	ClearConsumableAbilityResources_internal();

	ActiveHotkeyIndex = SlotIndex;
	HandleActiveHotkeyIndexChanged_internal();
}

	//--------------------
	// 게터
	//--------------------

FSlotStructMaster UTSInventoryMasterComponent::GetSlot(EInventoryType InventoryType, int32 SlotIndex)
{
	FInventoryStructMaster* Inventory = GetInventoryByType_internal(InventoryType);
	if (!Inventory || !Inventory->IsValidSlotIndex(SlotIndex))
	{
		return FSlotStructMaster();
	}
	
	return Inventory->GetSlot(SlotIndex);
}

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
	if (GetCurrentBagSlotCount() == 0) return ResultCount;
	
	for (const FSlotStructMaster& Slot : BagInventory.InventorySlotContainer)
	{
		if (Slot.ItemData.StaticDataID == StaticDataID)
		{
			ResultCount += Slot.CurrentStackSize;
		}
	}
	return ResultCount;
}

FInventoryStructMaster* UTSInventoryMasterComponent::GetInventoryByType_internal(EInventoryType InventoryType)
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

FSlotStructMaster UTSInventoryMasterComponent::GetActiveHotkeySlot_internal() const
{
	if (ActiveHotkeyIndex >= 0 && ActiveHotkeyIndex < HotkeyInventory.InventorySlotContainer.Num())
	{
		return HotkeyInventory.InventorySlotContainer[ActiveHotkeyIndex];
	}
	return FSlotStructMaster();
}


	//--------------------
	// 검증
	//--------------------

bool UTSInventoryMasterComponent::IsValidSlotIndex_internal(EInventoryType InventoryType, int32 SlotIndex)
{
	FInventoryStructMaster* Inventory = GetInventoryByType_internal(InventoryType);
	if (!Inventory) return false;
	
	return Inventory->IsValidSlotIndex(SlotIndex);
}

bool UTSInventoryMasterComponent::IsSlotEmpty_internal(EInventoryType InventoryType, int32 SlotIndex)
{
	FSlotStructMaster Slot = GetSlot(InventoryType, SlotIndex);
	return Slot.IsSlotEmpty();
}

bool UTSInventoryMasterComponent::CanPlaceItemInSlot_internal(int32 StaticDataID, EInventoryType InventoryType, int32 SlotIndex, bool IsTarget)
{
	if (StaticDataID == 0 || !IsValidSlotIndex_internal(InventoryType, SlotIndex)) return false;

	// 슬롯 접근 타입 확인
	if (IsTarget && GetSlot(InventoryType, SlotIndex).SlotAccessType == ESlotAccessType::ReadOnly) return false;

	FItemData ItemInfo;
	if (!UTSItemHelperLibrary::GetItemData(this,StaticDataID, ItemInfo)) return false;

	// 방어구 아이템 타입 검증
	if (InventoryType == EInventoryType::Equipment)
	{
		if (ItemInfo.Category != EItemCategory::ARMOR) return false;
		
		const FInventoryStructMaster* Inventory = GetInventoryByType_internal(InventoryType);
		if (!Inventory) return false;
		
		ESlotType TargetSlotType = Inventory->InventorySlotContainer[SlotIndex].SlotType;

		if (EquipmentSlotTypes[TargetSlotType] != ItemInfo.ArmorData.EquipSlot) return false;
	}

	return true;
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

	if (!SourceInventory->IsValidSlotIndex_internal(FromInventoryType, FromSlotIndex))
	{
		RequestingPlayer->ClientNotifyTransferResult(false);
		RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_Failed));
		return;
	}

	if (!TargetInventory->IsValidSlotIndex_internal(ToInventoryType, ToSlotIndex))
	{
		RequestingPlayer->ClientNotifyTransferResult(false);
		RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_Failed));
		return;
	}

	FInventoryStructMaster* FromInventory = SourceInventory->GetInventoryByType_internal(FromInventoryType);
	FInventoryStructMaster* ToInventory = TargetInventory->GetInventoryByType_internal(ToInventoryType);

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
		if (!TargetInventory->CanPlaceItemInSlot_internal(FromSlot.ItemData.StaticDataID, ToInventoryType, ToSlotIndex, true))
		{
			RequestingPlayer->ClientNotifyTransferResult(false);
			RequestingPlayer->ClientShowNotificationOnHUD(UTSSystemGetterLibrary::GetGameplayTagDisplaySubsystem(this)->GetDisplayName_KR(NotificationTags::TAG_Notification_Inventory_CannotPlace));
			return;
		}
	}

	if (ToSlot.ItemData.StaticDataID != 0)
	{
		if (!SourceInventory->CanPlaceItemInSlot_internal(ToSlot.ItemData.StaticDataID, FromInventoryType, FromSlotIndex, false))
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
				if (UTSItemHelperLibrary::GetItemData(SourceInventory, FromSlot.ItemData.StaticDataID, ItemInfo))
				{
					FromSlot.MaxStackSize = ItemInfo.MaxStack;
				}
			}
			// ToSlot 재설정
			if (ToSlot.ItemData.StaticDataID != 0)
			{
				FItemData ItemInfo;
				if (UTSItemHelperLibrary::GetItemData(TargetInventory, ToSlot.ItemData.StaticDataID, ItemInfo))
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
		UnequipArmor_internal(FromSlotIndex);
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
		if (UTSItemHelperLibrary::GetItemData(TargetInventory, ToSlot.ItemData.StaticDataID, ItemInfo))
		{
			// 방어구인 경우에만 장착
			if (ItemInfo.Category == EItemCategory::ARMOR)
			{
				TargetInventory->EquipArmor_internal(ItemInfo, ToSlotIndex);
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
		EquipActiveHotkeyItem_internal();
	}
	else
	{
		UnequipCurrentItem_internal();
	}
}


#pragma endregion
//======================================================================================================================
#pragma region 가방_시스템_API
	

	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 관련 API
	//━━━━━━━━━━━━━━━━━━━━

bool UTSInventoryMasterComponent::ExpandBagInventory_internal(int32 AdditionalSlots)
{
	if (!GetOwner()->HasAuthority()) return false;

	int32 CurrentSlotCount = BagInventory.InventorySlotContainer.Num();
	int32 NewSlotCount = CurrentSlotCount + AdditionalSlots;

	if (NewSlotCount > MaxBagSlotCount)
	{
		NewSlotCount = MaxBagSlotCount;

		if (CurrentSlotCount >= MaxBagSlotCount) return false;
	}

	int32 OldSize = CurrentSlotCount;
	BagInventory.InventorySlotContainer.SetNum(NewSlotCount);

	for (int32 i = OldSize; i < NewSlotCount; ++i)
	{
		BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
	}

	OnBagSizeChanged.Broadcast(NewSlotCount);

	return true;
}


#pragma endregion
//======================================================================================================================
#pragma region GAS_관련_API
    	

	//━━━━━━━━━━━━━━━━━━━━
	// GAS 관련 API
	//━━━━━━━━━━━━━━━━━━━━


void UTSInventoryMasterComponent::OnItemConsumedEvent_internal(const FGameplayEventData* Payload)
{
	if (!Payload) return;

	// 권한 체크 강화: 서버에서만 실행
	if (!GetOwner()->HasAuthority()) return;

	// 추가 체크: 로컬 Role이 Authority인지 확인
	if (GetOwner()->GetLocalRole() != ROLE_Authority) return;

	int32 SlotIndex = static_cast<int32>(Payload->EventMagnitude);

	if (!IsValidSlotIndex_internal(EInventoryType::HotKey, SlotIndex)) return;

	FSlotStructMaster& Slot = HotkeyInventory.InventorySlotContainer[SlotIndex];

	if (Slot.ItemData.StaticDataID == 0 || Slot.CurrentStackSize <= 0) return;

	// 아이템 소비
	Slot.CurrentStackSize -= 1;
	if (Slot.CurrentStackSize <= 0)
	{
		ClearSlot_internal(Slot);
	}

	HandleInventoryChanged_internal();

	// 활성화 슬롯 상태 변경 시 브로드캐스트
	if (Slot.CurrentStackSize == 0)
	{
		HandleActiveHotkeyIndexChanged_internal();
	}
}

void UTSInventoryMasterComponent::ClearConsumableAbilityResources_internal()
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
		UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
		if (IsValid(ASC))
		{
			FGameplayAbilitySpec* ExistingSpec = ASC->FindAbilitySpecFromHandle(ActiveConsumableAbilityHandle);

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

bool UTSInventoryMasterComponent::GrantAndScheduleConsumableAbility_internal(const FItemData& ItemInfo, int32 SlotIndex, UAbilitySystemComponent* ASC)
{
	if (!ItemInfo.AbilityBP) return false;

	FGameplayAbilitySpec Spec(ItemInfo.AbilityBP, 1, 0);
	FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(Spec);

	if (!SpecHandle.IsValid()) return false;

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
	GetWorld()->GetTimerManager().SetTimer
	(
		ConsumableAbilityTriggerTimer,[WeakThis, WeakASC, SpecHandle, EventData]()
		{
			// 컴포넌트 유효성 체크
			if (!WeakThis.IsValid()) return;

			// ASC 유효성 체크
			if (!WeakASC.IsValid()) return;

			UTSInventoryMasterComponent* This = WeakThis.Get();
			UAbilitySystemComponent* ASC = WeakASC.Get();

			// SpecHandle 재검증
			if (This->ActiveConsumableAbilityHandle == SpecHandle&& This->ActiveConsumableAbilityHandle.IsValid())
			{
				ASC->TriggerAbilityFromGameplayEvent(SpecHandle, ASC->AbilityActorInfo.Get(), ItemTags::TAG_Ability_Item_Consume, &EventData, *ASC);
			}
		},
		
		0.01f, false
	);

	return true;
}

void UTSInventoryMasterComponent::TryRegisterEventListeners_internal()
{
	// 이미 등록되었으면 리턴
	if (bEventListenersRegistered) return;

	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC))
	{
		// 0.1초 후 재시도
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(ASCCheckTimerHandle,this,&UTSInventoryMasterComponent::TryRegisterEventListeners_internal,0.1f,false);
		}
		
		return;
	}

	/*
		리스너는 모든 머신에서 등록하지만,
		실제 처리는 내부에서 서버 권한 체크
	*/

	// 소모품 사용 이벤트 리스닝
	FGameplayTag ConsumedTag = ItemTags::TAG_Event_Item_Consumed;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ConsumedTag).AddUObject(this, &UTSInventoryMasterComponent::OnItemConsumedEvent_internal);

	// 도구 채취 이벤트 리스닝
	FGameplayTag HarvestTag = ItemTags::TAG_Event_Item_Tool_Harvest;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(HarvestTag).AddUObject(this, &UTSInventoryMasterComponent::OnToolHarvestEvent_internal);

	// 무기 공격 이벤트 추가
	FGameplayTag WeaponAttackTag = ItemTags::TAG_Event_Item_Weapon_Attack;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(WeaponAttackTag).AddUObject(this, &UTSInventoryMasterComponent::OnWeaponAttackEvent_internal); 

	// 방어구 피격 이벤트 리스닝
	FGameplayTag ArmorHitTag = AbilityTags::TAG_Event_Armor_Hit;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ArmorHitTag).AddUObject(this, &UTSInventoryMasterComponent::OnArmorHitEvent_internal);

	// 등록 완료 플래그
	bEventListenersRegistered = true;

	// 타이머 정리
	if (ASCCheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ASCCheckTimerHandle);
	}
}



#pragma endregion
//======================================================================================================================


