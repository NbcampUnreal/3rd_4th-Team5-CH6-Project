#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TSGameplayAbilityBase.generated.h"

UCLASS()
class TINYSURVIVOR_API UTSGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
public: 
	UTSGameplayAbilityBase();
	
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
	
	//GA 에서 재생할 기본 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "Montage")
	TObjectPtr<UAnimMontage> FirstMontage;
	
	// 몽타주 재생할 때 호출 하는 함수
	UFUNCTION(BlueprintCallable, Category = "Montage")
	void PlayMontage();
	
	// 몽타주가 정상적으로 재생을 시작했을 때 호출
	UFUNCTION()
	virtual void OnMontageStarted();
	// 몽타주가 재생 끝나고 블렌드 아웃이 시작될 때 호출
	UFUNCTION()
	virtual void OnMontageBlendOut();
	// 중간에 끊겼을 때
	UFUNCTION()
	virtual void OnMontageInterrupted();
	// 어빌리티 자체가 cancel 되었을 때 호출
	UFUNCTION()
	virtual void OnMontageCancelled();
	// 몽타주가 끝까지 재생을 다 마쳤을 때 호출
	UFUNCTION()
	virtual void OnMontageCompleted();
};