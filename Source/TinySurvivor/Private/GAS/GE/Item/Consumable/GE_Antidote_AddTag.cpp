// GE_Antidote_AddTag.cpp
#include "GAS/GE/Item/Consumable/GE_Antidote_AddTag.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogAntidoteAddTag, Log, All);

UGE_Antidote_AddTag::UGE_Antidote_AddTag()
{
	// 로그: GE_Antidote 생성자 호출 여부 확인
	UE_LOG(LogAntidoteAddTag, Log, TEXT("[Antidote] GE_Antidote_AddTag 생성자 호출"));
	
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
	// Execution Calculation 설정
	// HasDuration 상태: Execution적용 불가
	//=======================================================================
	
	//=======================================================================
	// 참고: Duration과 Modifier등의 설정은 블루프린트에서 설정
	//=======================================================================
	// C++에서는 정책만 설정하고, 실제 값은 블루프린트에서 아이템별로 설정
}
