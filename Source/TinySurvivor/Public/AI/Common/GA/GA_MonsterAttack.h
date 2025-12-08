// GA_ChaserAttack.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ChaserAttack.generated.h"

UCLASS()
class TINYSURVIVOR_API UGA_ChaserAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ChaserAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRadius = 150.0f;
	
	// 몽타주 재생 및 이벤트 대기
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
	
};
