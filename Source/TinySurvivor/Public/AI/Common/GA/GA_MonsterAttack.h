// GA_ChaserAttack.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_MonsterAttack.generated.h"

UCLASS()
class TINYSURVIVOR_API UGA_MonsterAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_MonsterAttack();
	
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
	void OnEventReceived(FGameplayEventData EventData);
	
};
