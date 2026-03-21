// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Struct/TSInventorySlot.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/ItemData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSItemUseHelperLibrary.generated.h"

class UTSInventoryMasterComponent;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSItemUseHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	//━━━━━━━━━━━━━━━━━━━━
	// 아이템 관련 API
	//━━━━━━━━━━━━━━━━━━━━

	
	//--------------------
	// 아이템 사용 
	//--------------------
	
	
	// 아이템 사용 처리 내부 API
	static void UseItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 InSlotIndex);
	
	// 가방 아이템 사용 시 액션 함수 
	static void ActionWithBagItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, FSlotStructMaster& InTargetSlot,int32& InAdditionalSlots);
	
	// 소모품 사용 시 액션 함수
	static void ActionWithConsumableItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex);

	// 방어구 사용 시 액션 함수
	static void ActionWithArmorItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex);

	//--------------------
	// 아이템 소비
	//--------------------
	
	static void OnItemConsumedEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* Payload);
	
	static void ClearConsumableAbilityResources_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent);
	
	/*
			소모품 Ability를 부여하고 Trigger 예약
			@param ItemInfo 아이템 정보
			@param SlotIndex 슬롯 인덱스
			@param ASC AbilitySystemComponent
			@return 성공 여부
	*/
	static bool GrantAndScheduleConsumableAbility_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo, int32 InSlotIndex, UAbilitySystemComponent* InASC);
	
	//--------------------
	// 아이템 장착
	//--------------------

	
	// 핫키 버튼에 따른 아이템 창작
	static void EquipActiveHotkeyItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent);

	// 핫키 버튼에 따른 아이템 장착 해제
	static void UnequipCurrentItem_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent);

	// 장착 아이템 변환 발생 시 호출
	static void HandleCurrentEquippedItemChanged_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent);

	//--------------------
	// 방어구
	//--------------------

		
	// 방어구 장착
	static void EquipArmor_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo, int32 InArmorSlotIndex);
	
	// 방어구 해제
	static void UnequipArmor_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, int32 InArmorSlotIndex);
	
	// 방어구 피격 이벤트 수신 함수
	static void OnArmorHitEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* InPayload);
	
	//--------------------
	// 무기 
	//--------------------
	
	// 무기 장착
	static void ApplyWeaponStats_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo);
	
	// 무기 해제
	static void RemoveWeaponStats_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent);
	
	// 무기 사용 이벤트 수신 함수
	static void OnWeaponAttackEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* InPayload);
	
	//--------------------
	// 도구 관련
	//--------------------
	
	// 도구 장착
	static void ApplyToolTags_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FItemData& InItemInfo);

	// 도구 해제
	static void RemoveToolTags_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent);
	
	// 도구 사용 이벤트 수신 함수
	static void OnToolHarvestEvent_Lib(UTSInventoryMasterComponent* InInventoryMasterComponent, const FGameplayEventData* InPayload);
	
};
