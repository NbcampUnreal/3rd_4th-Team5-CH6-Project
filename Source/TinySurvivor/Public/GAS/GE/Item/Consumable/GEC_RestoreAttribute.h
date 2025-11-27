// GEC_RestoreAttribute.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEC_RestoreAttribute.generated.h"

/*
	소모품 효과 적용을 위한 Custom Execution Calculation
	
	역할:
	1. SetByCaller로 전달된 ItemID 수신
	2. ItemDataSubsystem에서 아이템 정보 조회
	3. EffectTag에 따라 적용할 Attribute 자동 결정
	4. EffectValue만큼 Attribute 변경
	
	EffectTag → Attribute 매핑:
	- HUNGER_RESTORE    → Hunger 증가
	- THIRST_RESTORE    → Thirst 증가
	- HP_HEAL           → Health 증가
	- SANITY_RESTORE    → Sanity 증가
	- TEMP_ADJUST       → Temperature 증가
	- HP_DAMAGE         → Health 감소
	- POISON_HEAL       → (독 상태이상 제거는 별도 처리)
	- CURE_DEBUFF       → (디버프 제거는 별도 처리)
	
	네트워크:
	- Execution은 서버에서만 실행
	- Attribute 변경은 자동으로 복제됨
*/
UCLASS()
class TINYSURVIVOR_API UGEC_RestoreAttribute : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
#pragma region Constructor
public:
	UGEC_RestoreAttribute();
#pragma endregion
	
	//========================================
	// Execution Override
	//========================================
#pragma region Execute_Implementation
public:
	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
#pragma endregion
	
#pragma region GetAttributeFromEffectTag
private:
	//========================================
	// Helper Functions
	//========================================
	
	/*
		EffectTag에 해당하는 Attribute 가져오기
		@param EffectTag 아이템의 효과 태그
		@param OutAttribute 출력될 Attribute
		@return 매핑 성공 여부
	*/
	bool GetAttributeFromEffectTag(
		const FGameplayTag& EffectTag,
		FGameplayAttribute& OutAttribute) const;
#pragma endregion
	
#pragma region IsNegativeEffect
private:
	/*
		EffectValue가 감소 효과인지 확인
		@param EffectTag 효과 태그
		@return HP_DAMAGE 등 감소 효과면 true
	*/
	bool IsNegativeEffect(const FGameplayTag& EffectTag) const;
#pragma endregion
};
