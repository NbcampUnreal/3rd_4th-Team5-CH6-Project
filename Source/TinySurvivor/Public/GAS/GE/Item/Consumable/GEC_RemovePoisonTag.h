// GEC_RemovePoisonTag.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEC_RemovePoisonTag.generated.h"

/*
	독 상태 태그를 제거하는 커스텀 GameplayEffect 실행 클래스
	
	이 클래스는 GameplayEffect에서 사용되며,
	독 상태(Poison) 태그를 가진 대상에게 해당 태그를 제거하는 로직을 실행합.
	예를 들어, 해독 아이템이 사용될 때 이 클래스가 실행되어 독 상태를 제거.
*/
UCLASS()
class TINYSURVIVOR_API UGEC_RemovePoisonTag : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UGEC_RemovePoisonTag();
	
	/*
		커스텀 실행 로직을 구현하는 함수
		
		이 함수는 GameplayEffect가 적용될 때 호출되며,
		대상의 AbilitySystemComponent(ASC)에서 독 상태 태그를 확인하고,
		해당 태그가 있으면 이를 제거.
		
		@param ExecutionParams - 커스텀 실행에 필요한 파라미터들
		@param OutExecutionOutput - 실행 결과를 담을 구조체 (효과를 처리 후 수정)
	*/
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
