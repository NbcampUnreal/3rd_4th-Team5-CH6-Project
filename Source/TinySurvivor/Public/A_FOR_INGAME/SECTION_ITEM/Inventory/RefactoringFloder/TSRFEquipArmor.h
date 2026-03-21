// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/ItemData.h"
#include "TSRFEquipArmor.generated.h"

class ATSEquippedItem;

// ========================================
// 장착한 방어구 구조체
// ========================================
USTRUCT(BlueprintType)
struct FEquippedArmor
{
	GENERATED_BODY()

	UPROPERTY()
	EEquipSlot SlotType = EEquipSlot::HEAD;

	UPROPERTY()
	TObjectPtr<ATSEquippedItem> EquippedArmor = nullptr;
	
	/*
		현재 장착된 방어구의 스탯 이펙트 핸들
	*/
	FActiveGameplayEffectHandle ArmorCommonEffectHandle;	// HealthBonus용
	FActiveGameplayEffectHandle ArmorEffectHandle;			// EffectTag용
	
};
