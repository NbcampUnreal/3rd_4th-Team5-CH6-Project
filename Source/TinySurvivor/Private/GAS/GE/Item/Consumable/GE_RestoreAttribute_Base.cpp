// GE_RestoreAttribute_Base.cpp
#include "GAS/GE/Item/Consumable/GE_RestoreAttribute_Base.h"
#include "GAS/GE/Item/Consumable/GEC_RestoreAttribute.h"

UGE_RestoreAttribute_Base::UGE_RestoreAttribute_Base()
{
	//=======================================================================
	// Duration 설정
	//=======================================================================
	// 기본값은 Instant로 설정
	// GA에서 EffectDuration > 0이면 별도 GE로 처리해야 함.
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	//=======================================================================
	// Period 설정 (주기적 효과)
	//=======================================================================
	// 필요시 블루프린트에서 설정
	Period = 0.0f;
	
	//=======================================================================
	// Stacking 설정
	//=======================================================================
	// 동일한 효과가 중첩되지 않도록 설정
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackLimitCount = 1;
	
	//=======================================================================
	// Execution Calculation 설정
	//=======================================================================
	// Custom Execution을 사용하여 ItemID 기반 Attribute 매핑
	FGameplayEffectExecutionDefinition ExecutionDef;
	ExecutionDef.CalculationClass = UGEC_RestoreAttribute::StaticClass();
	
	Executions.Add(ExecutionDef);
	
	//=======================================================================
	// 참고: SetByCaller 설정은 블루프린트에서 수동 추가 필요
	//=======================================================================
	// Data.ItemID 태그를 사용하여 아이템 ID 수신
	// 블루프린트 Details 패널 → Duration → Set by Caller Magnitudes 추가
}