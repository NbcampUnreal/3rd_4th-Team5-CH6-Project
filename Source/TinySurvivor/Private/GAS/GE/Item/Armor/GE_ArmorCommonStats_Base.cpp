// GE_ArmorCommonStats_Base.cpp
#include "GAS/GE/Item/Armor/GE_ArmorCommonStats_Base.h"
#include "GAS/AttributeSet/TSAttributeSet.h"

/*
	■ 이 파일의 역할:
	- 모든 방어구의 공통 스탯(HealthBonus)을 적용하는 GE 구현
	- 생성자에서 Modifier를 정의하여 블루프린트에서 사용 가능하도록 함
	
	■ 적용 프로세스:
	[EquipArmor() 호출 시]
	1. 이 GE 클래스로 Spec 생성
	2. SetByCaller로 HealthBonus 값 설정
	3. ASC에 적용 → MaxHealth 증가
	4. CommonEffectHandle에 핸들 저장
	
	[UnequipArmor() 호출 시]
	1. CommonEffectHandle로 GE 제거
	2. MaxHealth 원상복구
*/

UGE_ArmorCommonStats_Base::UGE_ArmorCommonStats_Base()
{
	//========================================
	// Duration 설정
	//========================================
	// Infinite: 방어구 장착 중 계속 유지
	// UnequipArmor()에서 명시적으로 제거될 때까지 지속
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	//========================================
	// HealthBonus Modifier 추가
	//========================================
	// 방어구의 체력 보너스를 MaxHealth에 가산
	FGameplayModifierInfo HealthModifier;
	
	// 대상 Attribute: MaxHealth
	HealthModifier.Attribute = UTSAttributeSet::GetMaxHealthAttribute();
	
	// 연산 방식: Additive (기존 값에 더하기)
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	
	// SetByCaller로 HealthBonus 값 수신
	// EquipArmor()에서 ItemInfo.ArmorData.HealthBonus 값을 전달
	FSetByCallerFloat HealthMagnitude;
	HealthMagnitude.DataTag = FGameplayTag::RequestGameplayTag("Data.HealthBonus");
	
	// Magnitude 설정
	HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealthMagnitude);
	
	// Modifier 배열에 추가
	Modifiers.Add(HealthModifier);
}
