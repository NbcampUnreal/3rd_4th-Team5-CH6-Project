// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Hit.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UGA_Hit : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Hit();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	FGameplayTag HitTag = FGameplayTag::EmptyTag;

	// 에디터에서 GE_Damage (데미지 이펙트 블루프린트)를 넣을 수 있는 슬롯입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffect;
};
