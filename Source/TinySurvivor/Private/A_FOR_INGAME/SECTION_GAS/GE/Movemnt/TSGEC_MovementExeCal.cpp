// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GE/Movemnt/TSGEC_MovementExeCal.h"
#include "A_FOR_INGAME/SECTION_GAS/Comp/TSAttributeSet.h"

UTSGEC_MovementExeCal::UTSGEC_MovementExeCal()
{
	// 정석: 생성자에서 캡처할 어트리뷰트들을 등록합니다.
	RelevantAttributesToCapture.Add(MoveStatics().TotalWeightDef);
	RelevantAttributesToCapture.Add(MoveStatics().MovementSpeedDef);
}

void UTSGEC_MovementExeCal::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	// 1. 필요한 기본 데이터 가져오기
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 2. 캡처된 어트리뷰트 값 추출하기
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float CurrentWeight = 0.f;

	// AttemptCalculateCapturedAttributeMagnitude를 사용하여 안전하게 값을 가져옵니다.
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MoveStatics().TotalWeightDef, EvaluationParameters, CurrentWeight);

	// 3. 커스텀 로직 계산
	float SpeedBonus = 200.f; 
    
	// 예: 무게가 30 이상이면 보너스 효과 50% 감소
	if (CurrentWeight > 30.f) SpeedBonus *= 0.5f;

	// 4. 결과 출력 (Modifier 추가)
	// MoveStatics().MovementSpeedDef는 '캡처 정의'이고, 
	// 그 안에 있는 .AttributeToCapture가 실제 '어트리뷰트'입니다.
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(MoveStatics().MovementSpeedDef.AttributeToCapture, EGameplayModOp::Additive, SpeedBonus));
}
