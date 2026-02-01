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
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> SprintCostEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> SprintSpeedEffectClass; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> StaminaDelayEffectClass;
	
private:
	FActiveGameplayEffectHandle CostHandle;
	FActiveGameplayEffectHandle SpeedHandle;
	FTimerHandle MovementCheckTimerHandle;
	
	//Delegate Handle
	FDelegateHandle StaminaDelegateHandle;
	FDelegateHandle ThirstDelegateHandle;
	
	void OnAttributeChanged(const FOnAttributeChangeData& Data);
	void SprintCheck();
};
