// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_MonBase.generated.h"

class UStateTreeAIComponent;
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UGA_MonBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public: 
	UGA_MonBase();
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Montage")
	TObjectPtr<UAnimMontage> AbilityMontage;
	
	UFUNCTION()
	virtual void OnMontageBlendOut();
	
	UFUNCTION()
	virtual void OnMontageInterrupted();
	
	UFUNCTION()
	virtual void OnMontageCancelled();
	
	UFUNCTION()
	virtual void OnMontageCompleted();
	
	// 몽타주 재생 이후 호출 보낼 이벤트 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Montage", meta = (Categories = "Monster.Behavior"))
	FGameplayTag MontageEndTag = FGameplayTag::EmptyTag;
	
	// 죽었을 때 보내는 이벤트 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Montage", meta = (Categories = "Monster.Behavior"))
	FGameplayTag MonsterDeadTag = FGameplayTag::EmptyTag;
	
	// 스테이트 트리
	UFUNCTION(BlueprintCallable)
	UStateTreeAIComponent* GiantStateTreeComponent();
	
	UFUNCTION(BlueprintCallable)
	void SendFinishMontageEventToStateTree(const FGameplayTag& InSendTag);
};
