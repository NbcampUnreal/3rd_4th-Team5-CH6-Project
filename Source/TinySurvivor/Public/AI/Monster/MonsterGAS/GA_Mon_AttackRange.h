// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA_MonBase.h"
#include "GA_Mon_AttackRange.generated.h"

class AMon_ProjectileBase;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UGA_Mon_AttackRange : public UGA_MonBase
{
	GENERATED_BODY()

public:
	UGA_Mon_AttackRange();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void OnReceived(FGameplayEventData Payload);

	// 델리게이트로 받을 함수들 오버이드
	virtual void OnMontageCompleted() override;

	virtual void OnMontageCancelled() override;

	virtual void OnMontageInterrupted() override;

	virtual void OnMontageBlendOut() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<AMon_ProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	FGameplayTag ReceiveShootProjectileTag;

	FVector ProjectileSpawnLocation;
	FRotator ProjectileSpawnRotation;
	
};
