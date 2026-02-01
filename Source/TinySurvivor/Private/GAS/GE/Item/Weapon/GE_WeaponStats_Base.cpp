// GE_WeaponStats_Base.cpp
#include "GAS/GE/Item/Weapon/GE_WeaponStats_Base.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "GameplayTags/ItemGameplayTags.h"

UGE_WeaponStats_Base::UGE_WeaponStats_Base()
{
	//=======================================================================
	// Duration 설정
	//=======================================================================
	// Infinite: 장착 중 계속 유지, 해제 시 자동 제거
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	//=======================================================================
	// Period 설정 (주기적 효과)
	//=======================================================================
	// 0: 주기적 효과 없음 (즉시 적용만)
	Period = 0.0f;
	
	//=======================================================================
	// Stacking 설정
	//=======================================================================
	// 동일한 무기를 중복 장착할 수 없으므로 스택 불필요
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackLimitCount = 1;
	
	// 같은 GE를 다시 적용하면 기존 Duration을 새 Duration으로 갱신
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	
	// 같은 GE를 다시 적용하면 주기 타이머가 초기화
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	
	// 스택 중 하나가 만료되면 모든 스택을 제거
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;
	
	//=======================================================================
	// Modifiers 설정 (C++에서 직접 추가)
	//=======================================================================
	
	// Modifier 1: DamageBonus (Add)
	{
		FGameplayModifierInfo DamageModifier;
		DamageModifier.Attribute = UTSAttributeSet::GetDamageBonusAttribute();
		DamageModifier.ModifierOp = EGameplayModOp::Additive;
		
		FSetByCallerFloat DamageMagnitude;
		DamageMagnitude.DataTag = ItemTags::TAG_Data_AttackDamage;
		DamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(DamageMagnitude);
		
		Modifiers.Add(DamageModifier);
	}
	
	// Modifier 2: AttackSpeedBonus (Multiply)
	{
		FGameplayModifierInfo SpeedModifier;
		SpeedModifier.Attribute = UTSAttributeSet::GetAttackSpeedBonusAttribute();
		SpeedModifier.ModifierOp = EGameplayModOp::Multiplicitive;
		
		FSetByCallerFloat SpeedMagnitude;
		SpeedMagnitude.DataTag = ItemTags::TAG_Data_AttackSpeed;
		SpeedModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SpeedMagnitude);
		
		Modifiers.Add(SpeedModifier);
	}
	
	// Modifier 3: AttackRangeBonus (Multiply)
	{
		FGameplayModifierInfo RangeModifier;
		RangeModifier.Attribute = UTSAttributeSet::GetAttackRangeBonusAttribute();
		RangeModifier.ModifierOp = EGameplayModOp::Multiplicitive;
		
		FSetByCallerFloat RangeMagnitude;
		RangeMagnitude.DataTag = ItemTags::TAG_Data_AttackRange;
		RangeModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(RangeMagnitude);
		
		Modifiers.Add(RangeModifier);
	}
}