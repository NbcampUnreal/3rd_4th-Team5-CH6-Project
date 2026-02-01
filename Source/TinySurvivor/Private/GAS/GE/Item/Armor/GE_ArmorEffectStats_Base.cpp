// GE_ArmorEffectStats_Base.cpp
#include "GAS/GE/Item/Armor/GE_ArmorEffectStats_Base.h"
#include "GAS/AttributeSet/TSAttributeSet.h"

/*
	■ 이 파일의 역할:
	- 방어구의 EffectTag 기반 특수 효과 스탯을 적용하는 GE 구현
	- 하나의 GE로 3가지 효과(피해반사/피해감소/이동속도)를 선택적으로 처리
	
	■ 핵심 메커니즘:
	- 모든 Modifier가 항상 정의되어 있음 (4개)
	- EquipArmor()에서 적용 시:
		1. 모든 DataTag를 0.0f로 초기화
		2. EffectTag에 맞는 값만 설정
		3. 나머지는 0.0f이므로 Attribute에 영향 없음
	
	■ 적용 예시:
	[DAMAGE_REFLECT 방어구 장착 시]
	- DamageReflection: 0.2 (설정됨)
	- DamageReduction: 0.0 (초기화)
	- MoveSpeed: 0.0 (초기화)
	→ DamageReflectionBonus만 +0.2 적용
	
	[MOVE_SPEED 방어구 장착 시]
	- DamageReflection: 0.0 (초기화)
	- DamageReduction: 0.0 (초기화)
	- MoveSpeed: 30.0 (설정됨)
	→ MoveSpeed와 MaxMoveSpeed 모두 +30 적용
*/

UGE_ArmorEffectStats_Base::UGE_ArmorEffectStats_Base()
{
	//========================================
	// Duration 설정
	//========================================
	// Infinite: 방어구 장착 중 계속 유지
	// UnequipArmor()에서 명시적으로 제거될 때까지 지속
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	//========================================
	// 1. DamageReflectionBonus Modifier
	//========================================
	// 피해 반사 비율을 DamageReflectionBonus에 가산
	// EffectTag: State.Modifier.DAMAGE_REFLECT
	{
		FGameplayModifierInfo Modifier;
		
		// 대상 Attribute
		Modifier.Attribute = UTSAttributeSet::GetDamageReflectionBonusAttribute();
		
		// 연산 방식: Additive (기존 값에 더하기)
		Modifier.ModifierOp = EGameplayModOp::Additive;
		
		// SetByCaller 설정
		// EquipArmor()에서 EffectTag에 따라 값 설정
		// 해당 효과가 아니면 0.0f로 초기화됨
		FSetByCallerFloat Magnitude;
		Magnitude.DataTag = FGameplayTag::RequestGameplayTag("Data.Armor.DamageReflection");
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
		
		Modifiers.Add(Modifier);
	}
	
	//========================================
	// 2. DamageReductionBonus Modifier
	//========================================
	// 피해 감소 비율을 DamageReductionBonus에 가산
	// EffectTag: State.Modifier.DAMAGE_REDUCTION
	{
		FGameplayModifierInfo Modifier;
		
		// 대상 Attribute
		Modifier.Attribute = UTSAttributeSet::GetDamageReductionBonusAttribute();
		
		// 연산 방식: Additive
		Modifier.ModifierOp = EGameplayModOp::Additive;
		
		// SetByCaller 설정
		FSetByCallerFloat Magnitude;
		Magnitude.DataTag = FGameplayTag::RequestGameplayTag("Data.Armor.DamageReduction");
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
		
		Modifiers.Add(Modifier);
	}
	
	//========================================
	// 3. MoveSpeed Modifier
	//========================================
	// 이동 속도 증가량을 MoveSpeed에 가산
	// EffectTag: State.Modifier.MOVE_SPEED
	{
		FGameplayModifierInfo Modifier;
		
		// 대상 Attribute
		Modifier.Attribute = UTSAttributeSet::GetMoveSpeedAttribute();
		
		// 연산 방식: Additive
		Modifier.ModifierOp = EGameplayModOp::Additive;
		
		// SetByCaller 설정
		FSetByCallerFloat Magnitude;
		Magnitude.DataTag = FGameplayTag::RequestGameplayTag("Data.Armor.MoveSpeed");
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
		
		Modifiers.Add(Modifier);
	}
	
	//========================================
	// 4. MaxMoveSpeed Modifier
	//========================================
	// 최대 이동 속도 증가량을 MaxMoveSpeed에 가산
	// EffectTag: State.Modifier.MOVE_SPEED
	// MoveSpeed와 함께 적용되어 실제 속도 증가 구현
	{
		FGameplayModifierInfo Modifier;
		
		// 대상 Attribute
		Modifier.Attribute = UTSAttributeSet::GetMaxMoveSpeedAttribute();
		
		// 연산 방식: Additive
		Modifier.ModifierOp = EGameplayModOp::Additive;
		
		// SetByCaller 설정
		// MoveSpeed와 동일한 DataTag 사용 (같은 값 적용)
		FSetByCallerFloat Magnitude;
		Magnitude.DataTag = FGameplayTag::RequestGameplayTag("Data.Armor.MoveSpeed");
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
		
		Modifiers.Add(Modifier);
	}
}