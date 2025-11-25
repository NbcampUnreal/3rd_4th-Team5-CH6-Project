#pragma once

#include "CoreMinimal.h"
#include "TSGiantAbilityMaster.h"
#include "TSGiantJustScream.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TINYSURVIVOR_API UTSGiantJustScream : public UTSGiantAbilityMaster
{
	GENERATED_BODY()
	
public:
	UTSGiantJustScream();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	// 델리게이트로 받을 함수들 오버이드
	virtual void OnMontageCompleted() override;

	virtual void OnMontageCancelled() override;

	virtual void OnMontageInterrupted() override;

	virtual void OnMontageBlendOut() override;
};
