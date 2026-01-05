#pragma once

#include "CoreMinimal.h"
#include "BaseAbility/TSGameplayAbilityBase.h"
#include "GA_Crouch.generated.h"

class UGameplayEffect;

UCLASS()
class TINYSURVIVOR_API UGA_Crouch : public UTSGameplayAbilityBase
{
	GENERATED_BODY()
	
public:
	UGA_Crouch();
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch")
	TSubclassOf<UGameplayEffect> CrouchSpeedEffectClass;
private:
	FActiveGameplayEffectHandle SpeedHandle;
	
	UFUNCTION()
	void OnLanded(EMovementMode NewMovementMode);
};