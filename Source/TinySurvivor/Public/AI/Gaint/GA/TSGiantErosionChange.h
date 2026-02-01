#pragma once

#include "CoreMinimal.h"
#include "TSGiantAbilityMaster.h"
#include "TSGiantErosionChange.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TINYSURVIVOR_API UTSGiantErosionChange : public UTSGiantAbilityMaster
{
	GENERATED_BODY()
	
	public:
	UTSGiantErosionChange();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
	// 델리게이트로 받을 함수들 오버이드
	virtual void OnMontageCompleted() override;

	virtual void OnMontageCancelled() override;

	virtual void OnMontageInterrupted() override;

	virtual void OnMontageBlendOut() override;
	
	// 침식도 증가 수치
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ability|Montage")
	float ErosionChangeValue = 10.0f;
};
