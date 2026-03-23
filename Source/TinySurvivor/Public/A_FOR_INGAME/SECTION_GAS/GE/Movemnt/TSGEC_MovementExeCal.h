// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGE_ExeCalBase.h"
#include "TSGEC_MovementExeCal.generated.h"


/**
 * 모디파이어 및 실행(Modifiers and Executions): 모디파이어(Modifiers)는 게임플레이 이펙트가 어트리뷰트와 상호작용하는 방식을 결정합니다.
 * 여기에는 '방어력 5퍼센트 상승'과 같은 어트리뷰트 자체에 대한 수학적 인터랙션은 물론, 이펙트 실행에 필수인 게임플레이 태그도 포함됩니다.
 * 게임플레이 이펙트에 모디파이어가 지원하는 것 이상의 무언가가 필요할 때는 실행(Executions)이 도움이 될 수 있습니다. 
 * 실행은 UGameplayEffectExecutionCalculation 을 사용하여 실행 시 게임플레이 이펙트가 가질 커스텀 비헤이비어를 정의합니다.
 * 모디파이어로는 충분히 커버할 수 없는 복잡한 방정식을 정의할 때 특히 유용합니다.
 */
UCLASS()
class TINYSURVIVOR_API UTSGEC_MovementExeCal : public UTSGE_ExeCalBase
{
	GENERATED_BODY()
	
public:
	UTSGEC_MovementExeCal();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
protected:
	
	// 어빌리티 활성화 시 바꿀 속도 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Movement")
	float WantChangeMoveSpeed = 500.0f;

};
