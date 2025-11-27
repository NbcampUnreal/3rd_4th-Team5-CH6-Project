// GE_Antidote_RemoveTag.cpp
#include "GAS/GE/Item/Consumable/GE_Antidote_RemoveTag.h"
#include "GAS/GE/Item/Consumable/GEC_RemovePoisonTag.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogAntidoteRemoveTag, Log, All);

UGE_Antidote_RemoveTag::UGE_Antidote_RemoveTag()
{
	// 로그: GE_Antidote 생성자 호출 여부 확인
	UE_LOG(LogAntidoteRemoveTag, Log, TEXT("[Antidote] GE_Antidote_RemoveTag 생성자 호출"));
	
	//=======================================================================
	// Duration 설정
	//=======================================================================
	// Execution 설정을 위해 Instant로 설정
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
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
	
	//=======================================================================
	// Execution Calculation 설정
	//=======================================================================
	FGameplayEffectExecutionDefinition ExecutionDef;
	ExecutionDef.CalculationClass = UGEC_RemovePoisonTag::StaticClass();
	
	// 로그: ExecutionDef 설정이 제대로 이루어졌는지 확인
	UE_LOG(LogAntidoteRemoveTag, Log,
		TEXT("[Antidote] AntidoteRemoveTag ExecutionDef 설정: %s"),
		*ExecutionDef.CalculationClass->GetName());
	
	Executions.Add(ExecutionDef);
	
	//=======================================================================
	// 참고: Duration과 Modifier등의 설정은 블루프린트에서 설정
	//=======================================================================
	// C++에서는 정책만 설정하고, 실제 값은 블루프린트에서 아이템별로 설정
}
