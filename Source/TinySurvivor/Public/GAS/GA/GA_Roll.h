#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_Roll.generated.h"


UCLASS()
class TINYSURVIVOR_API UGA_Roll : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_Roll();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	//스태미나 즉시 -20 하는 GE 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TSubclassOf<UGameplayEffect> RollEffectClass;
	
	//1초 뒤부터 적용할 스태미나 회복 GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Roll")
	TSubclassOf<UGameplayEffect> RecoverStaminaEffectClass;
};
