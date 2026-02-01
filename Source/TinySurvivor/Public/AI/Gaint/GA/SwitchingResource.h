#pragma once

#include "CoreMinimal.h"
#include "TSGiantAbilityMaster.h"
#include "SwitchingResource.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TINYSURVIVOR_API USwitchingResource : public UTSGiantAbilityMaster
{
	GENERATED_BODY()
	
public:
	USwitchingResource();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
};
