#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_Sprint.generated.h"

class UGameplayEffect;

UCLASS()
class TINYSURVIVOR_API UGA_Sprint : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_Sprint();
	
	// Stamina > 0 && Thrist > 0 이어야 발동
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	// 활성화 (GE 적용)
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// 종료 (GE 제거 후 Delay GE 적용)
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> SprintCostEffectClass; //0.1초당 스태미나 -1
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> SprintSpeedEffectClass; //스피드 + 400 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> StaminaDelayEffectClass; //EndAbility 후 1초 딜레이
	
private:
	FActiveGameplayEffectHandle CostHandle;
	FActiveGameplayEffectHandle SpeedHandle;
	
	//Delegate Handle
	FDelegateHandle StaminaDelegateHandle;
	FDelegateHandle ThirstDelegateHandle;
	
	void OnAttributeChanged(const FOnAttributeChangeData& Data);
	
};
