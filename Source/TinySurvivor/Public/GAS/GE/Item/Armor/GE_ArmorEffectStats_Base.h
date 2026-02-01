// GE_ArmorEffectStats_Base.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_ArmorEffectStats_Base.generated.h"

/*
	방어구의 EffectTag 기반 '특수 효과 스탯' 적용을 위한 베이스 GameplayEffect
	
	이 GE는 방어구가 가진 EffectTag(D-5-1 정의)를 기반으로,
	각 효과의 수치(EffectValue)를 SetByCaller로 전달받아 Attributes에 반영.
	
	■ 처리 대상 EffectTag (D-5-1. 방어구 효과 종류)
		==========================================================================
		1. DAMAGE_REFLECT (State.Modifier.DAMAGE_REFLECT)
			- 설명: 받은 피해의 일정 비율을 공격자에게 반사
			- EffectValue: 반사 비율 (0.0 ~ 1.0)
			- 적용 Attribute: DamageReflectionBonus
			- DataTag: "Data.Armor.DamageReflection"
			
		2. DAMAGE_REDUCTION (State.Modifier.DAMAGE_REDUCTION)
			- 설명: 받는 피해를 일정 비율만큼 감소
			- EffectValue: 감소 비율 (0.0 ~ 1.0)
			- 적용 Attribute: DamageReductionBonus
			- DataTag: "Data.Armor.DamageReduction"
			
		3. MOVE_SPEED (State.Modifier.MOVE_SPEED)
			- 설명: 이동 속도 증가
			- EffectValue: 증가 속도 값 (예: +30)
			- 적용 Attribute: MoveSpeed, MaxMoveSpeed
			- DataTag: "Data.Armor.MoveSpeed"
		==========================================================================
	
	■ 사용 방법:
		==========================================================================
		1. 블루프린트에서 이 클래스를 상속받아 BP_GE_ArmorEffectStats 생성
		2. UTSInventoryMasterComponent의 ArmorEffectStatEffectClass에 BP 할당
		3. EquipArmor() 함수에서 자동으로 적용됨:
			// UTSInventoryMasterComponent::EquipArmor()
			FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(
			   ArmorEffectStatEffectClass, 1, ContextHandle);
			
			// 모든 값 0으로 초기화
			EffectSpec.Data->SetSetByCallerMagnitude(TAG_Data_Armor_DamageReflection, 0.0f);
			EffectSpec.Data->SetSetByCallerMagnitude(TAG_Data_Armor_DamageReduction, 0.0f);
			EffectSpec.Data->SetSetByCallerMagnitude(TAG_Data_Armor_MoveSpeed, 0.0f);
			
			// EffectTag에 따라 해당 값만 설정
			if (ItemInfo.EffectTag_Armor.MatchesTag(TAG_State_Modifier_DAMAGE_REFLECT))
			   EffectSpec.Data->SetSetByCallerMagnitude(TAG_Data_Armor_DamageReflection, ItemInfo.EffectValue);
			
			// 적용 및 핸들 저장
			FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
			EquippedArmors[ArmorSlotIndex].EffectHandle = Handle;
		==========================================================================
	
	■ 동작 흐름:
	[장착 시]
	1. EquipArmor() 호출
	2. GE Spec 생성
	3. 모든 Modifier를 0.0f로 초기화
	4. EffectTag_Armor에 따라 해당 DataTag에만 EffectValue 설정
	5. ASC에 적용 → 해당 Attribute만 변경
	6. EffectHandle 저장
	
	[탈착 시]
	1. UnequipArmor() 호출
	2. EffectHandle로 GE 제거
	3. 적용된 Attribute 원상복구
	
	■ SetByCaller Tags:
	- "Data.Armor.DamageReflection": 피해 반사 비율
	- "Data.Armor.DamageReduction": 피해 감소 비율
	- "Data.Armor.MoveSpeed": 이동속도 증가량
	
	■ 중요 구현 사항:
	- cpp에서 모든 Modifier가 정의되어 있음 (4개)
	- EquipArmor()에서 적용 시 반드시 모든 값을 0.0f로 초기화한 후, 해당 EffectTag에 맞는 값만 설정
	- 이를 통해 하나의 GE로 3가지 효과를 선택적으로 처리
	
	■ Duration:
	- Infinite (장착 중 계속 유지)
	- 장비 교체 시 기존 GE 제거 후 새 GE 적용
	
	■ 네트워크:
	- 서버에서만 적용 (GetOwner()->HasAuthority() 체크)
	- Attribute 변화는 GAS에 의해 자동 복제
	
	■ 특징:
	- 방어구의 "특수 효과(EffectTag)"만 처리하는 전용 GE
	- ArmorCommonStats와 별개로 동작하며 동시에 적용 가능
	- 전체 방어구 시스템의 EffectTag 기반 확장성을 보장
	
	■ 연관 시스템:
	- UTSInventoryMasterComponent::EquipArmor()
	- UTSInventoryMasterComponent::UnequipArmor()
	- FEquippedArmor::EffectHandle
	- FItemData.EffectTag_Armor, EffectValue
	- UTSAttributeSet::DamageReflectionBonus, DamageReductionBonus, MoveSpeed, MaxMoveSpeed
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API UGE_ArmorEffectStats_Base : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UGE_ArmorEffectStats_Base();
};