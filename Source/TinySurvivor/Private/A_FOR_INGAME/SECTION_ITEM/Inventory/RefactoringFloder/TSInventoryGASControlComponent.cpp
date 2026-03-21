// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/RefactoringFloder/TSInventoryGASControlComponent.h"
#include "A_FOR_COMMON/Library/GAS/TSASCLibrary.h"
#include "A_FOR_COMMON/Tag/AbilityGameplayTags.h"
#include "A_FOR_COMMON/Tag/ItemGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "A_FOR_COMMON/Library/Item/TSItemUseHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSInventoryMasterComponent.h"


//======================================================================================================================
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

UTSInventoryGASControlComponent::UTSInventoryGASControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTSInventoryGASControlComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

#pragma endregion
//======================================================================================================================
#pragma region GAS_관련_API

	
	//━━━━━━━━━━━━━━━━━━━━
	// GAS 관련 API
	//━━━━━━━━━━━━━━━━━━━━

void UTSInventoryGASControlComponent::TryRegisterEventListeners_internal()
{
	// 이미 등록되었으면 리턴
	if (bEventListenersRegistered) return;

	UAbilitySystemComponent* ASC = UTSASCLibrary::GetASCFromComp(GetOwner());
	if (!IsValid(ASC))
	{
		// 0.1초 후 재시도
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(ASCCheckTimerHandle,this,&UTSInventoryGASControlComponent::TryRegisterEventListeners_internal,0.1f,false);
		}
		
		return;
	}

	/*
		리스너는 모든 머신에서 등록하지만,
		실제 처리는 내부에서 서버 권한 체크
	*/

	// 소모품 사용 이벤트 리스닝
	FGameplayTag ConsumedTag = ItemTags::TAG_Event_Item_Consumed;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ConsumedTag).AddUObject(this, &UTSInventoryGASControlComponent::OnItemConsumedEvent_internal);

	// 도구 채취 이벤트 리스닝
	FGameplayTag HarvestTag = ItemTags::TAG_Event_Item_Tool_Harvest;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(HarvestTag).AddUObject(this, &UTSInventoryGASControlComponent::OnToolHarvestEvent_internal);

	// 무기 공격 이벤트 추가
	FGameplayTag WeaponAttackTag = ItemTags::TAG_Event_Item_Weapon_Attack;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(WeaponAttackTag).AddUObject(this, &UTSInventoryGASControlComponent::OnWeaponAttackEvent_internal); 

	// 방어구 피격 이벤트 리스닝
	FGameplayTag ArmorHitTag = AbilityTags::TAG_Event_Armor_Hit;
	ASC->GenericGameplayEventCallbacks.FindOrAdd(ArmorHitTag).AddUObject(this, &UTSInventoryGASControlComponent::OnArmorHitEvent_internal);

	// 등록 완료 플래그
	bEventListenersRegistered = true;

	// 타이머 정리
	if (ASCCheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ASCCheckTimerHandle);
	}
}

void UTSInventoryGASControlComponent::OnItemConsumedEvent_internal(const FGameplayEventData* GameplayEventData)
{
	if (!IsValid(InventoryMasterComp)) return;
	UTSItemUseHelperLibrary::OnItemConsumedEvent_Lib(InventoryMasterComp, GameplayEventData);
}

void UTSInventoryGASControlComponent::OnToolHarvestEvent_internal(const FGameplayEventData* GameplayEventData)
{
	if (!IsValid(InventoryMasterComp)) return;
	UTSItemUseHelperLibrary::OnToolHarvestEvent_Lib(InventoryMasterComp, GameplayEventData);
}

void UTSInventoryGASControlComponent::OnWeaponAttackEvent_internal(const FGameplayEventData* GameplayEventData)
{
	if (!IsValid(InventoryMasterComp)) return;
	UTSItemUseHelperLibrary::OnWeaponAttackEvent_Lib(InventoryMasterComp, GameplayEventData);
}

void UTSInventoryGASControlComponent::OnArmorHitEvent_internal(const FGameplayEventData* GameplayEventData)
{
	if (!IsValid(InventoryMasterComp)) return;
	UTSItemUseHelperLibrary::OnArmorHitEvent_Lib(InventoryMasterComp, GameplayEventData);
}


#pragma endregion	
//======================================================================================================================