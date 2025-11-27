#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_Roll.generated.h"

class UGameplayEffect;

UCLASS()
class TINYSURVIVOR_API UGA_Roll : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_Roll();
	
	// Stamina >= 20 && Thirst > 0 이어야 발동
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	// 활성화 (GE 적용) + 몽타주 재생
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// 종료 (GE 제거 후 Delay GE 적용)
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> RollForwardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> RollBackwardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> RollLeftMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> RollRightMontage;
	
	// Stamina 관련 GE 들 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TSubclassOf<UGameplayEffect> RollCostEffectClass; // 바로 -20 스태미나
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TSubclassOf<UGameplayEffect> StaminaDelayEffectClass; //EndAbility 후 1초 딜레이
	
	// 롤 상태 관련 태그 
	UPROPERTY(EditDefaultsOnly, Category = "Roll|Tags")
	FGameplayTag RollMoveTag; // 몽타주 재생동안 state.move.roll 태그 활성화

	UPROPERTY(EditDefaultsOnly, Category = "Roll|Tags")
	FGameplayTag StaminaBlockTag; // 몽타주 재생동안 stamina 회복 차단
	
	//방향 결정 함수
	UAnimMontage* DetermineRollMontage() const;
	
	// 몽타주 종료시 처리 
	UFUNCTION()
	void OnRollMontageFinished();
};
