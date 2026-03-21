// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Struct/TSInventorySlot.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/ItemData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TSItemUseHelperLibrary.generated.h"

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
	void UseItem_Lib(int32 SlotIndex);
	
	// 가방 아이템 사용 시 액션 함수 
	void ActionWithBagItem_Lib(FSlotStructMaster& InTargetSlot,int32& InAdditionalSlots);
	
	// 소모품 사용 시 액션 함수
	void ActionWithConsumableItem_Lib(FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex);

	// 방어구 사용 시 액션 함수
	void ActionWithArmorItem_Lib(FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex);

	//--------------------
	// 아이템 소비
	//--------------------
	
	//--------------------
	// 아이템 장착
	//--------------------

	
	// 핫키 버튼에 따른 아이템 창작
	void EquipActiveHotkeyItem_Lib();

	// 핫키 버튼에 따른 아이템 장착 해제
	void UnequipCurrentItem_Lib();

	// 장착 아이템 변환 발생 시 호출
	void HandleCurrentEquippedItemChanged_Lib();

	//--------------------
	// 방어구
	//--------------------

		
	// 방어구 장착
	void EquipArmor_Lib(const FItemData& ItemInfo, int32 ArmorSlotIndex);
	
	// 방어구 해제
	void UnequipArmor_Lib(int32 ArmorSlotIndex);
	
	// 방어구 피격 이벤트 수신 함수
	void OnArmorHitEvent_Lib(const FGameplayEventData* Payload);
	
	//--------------------
	// 무기 
	//--------------------
	
	
	// 무기 장착
	void ApplyWeaponStats_Lib(const FItemData& ItemInfo);
	
	// 무기 해제
	void RemoveWeaponStats_Lib();
	
	// 무기 사용 이벤트 수신 함수
	void OnWeaponAttackEvent_Lib(const FGameplayEventData* Payload);
	
	//--------------------
	// 도구 관련
	//--------------------
	
protected:
	
	// 도구 장착
	void ApplyToolTags_Lib(const FItemData& ItemInfo);

	// 도구 해제
	void RemoveToolTags_Lib();
	
	// 도구 사용 이벤트 수신 함수
	void OnToolHarvestEvent_Lib(const FGameplayEventData* Payload);
	
};
