#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "TimerManager.h"
#include "GA_Sprint.generated.h"

class UGameplayEffect;
struct FOnAttributeChangeData;

UCLASS()
class TINYSURVIVOR_API UGA_Sprint : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_Sprint();
	//시프트 누르면 호출 -> 스프린트 시작
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//시프트 떼거나 스태미나 0 돼서 끝날때 호출
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// 스태미나 0 이하면 활성화 불가능 
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> SprintEffectClass; //속도 1000, 0.1초당 스태미나 -1
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> NonSprintEffectClass; //속도 600
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> RecoverStaminaEffectClass; //Nonsprint일때 스태미나 0.05초당 +1
	
	FActiveGameplayEffectHandle SprintEffectHandle;
	FActiveGameplayEffectHandle NonSprintEffectHandle;
	
};
