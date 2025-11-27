// GE_BuffAttribute_Base.cpp
#include "GAS/GE/Item/Consumable/GE_BuffAttribute_Base.h"

UGE_BuffAttribute_Base::UGE_BuffAttribute_Base()
{
	//=======================================================================
	// Duration 설정
	//=======================================================================
	// 버프 효과이므로 HasDuration 정책 사용
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	
	// Duration은 블루프린트에서 Scalable Float로 설정
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(0.0f));
	
	//=======================================================================
	// Period 설정 (주기적 효과)
	//=======================================================================
	// 기본값: 0 (지속 효과만 적용, 주기 효과 없음)
	// 필요시 블루프린트에서 설정 가능
	Period = 0.0f;
	bExecutePeriodicEffectOnApplication = false;
	
	//=======================================================================
	// Stacking 설정
	//=======================================================================
	// 동일한 버프가 중첩되지 않도록 설정
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackLimitCount = 1;
	
	// 스택 만료 정책: Duration 갱신
	StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration;
	
	//=======================================================================
	// Modifiers 설정
	//=======================================================================
	// Modifier는 블루프린트에서 수동으로 추가해야 함
	// 이유: ItemID 기반 자동 매핑 불가 (Execution과 달리 정적 설정 필요)
	
	// 블루프린트 설정 예시:
	// Modifiers[0]:
	//	- Attribute: Temperature
	//	- ModifierOp: Add
	//	- ModifierMagnitude: Scalable Float (10.0 또는 -10.0)
	
	//=======================================================================
	// 참고: Duration과 Modifier는 블루프린트에서 설정
	//=======================================================================
	// C++에서는 정책만 설정하고, 실제 값은 블루프린트에서 아이템별로 설정
}