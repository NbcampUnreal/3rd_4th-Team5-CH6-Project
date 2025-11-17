#include "BaseAbility/TSGameplayAbilityBase.h"

UTSGameplayAbilityBase::UTSGameplayAbilityBase()
{
	
}

void UTSGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle,ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void UTSGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

//몽타주
void UTSGameplayAbilityBase::OnMontageStarted()
{
	
}

void UTSGameplayAbilityBase::OnMontageBlendOut()
{
	
}

void UTSGameplayAbilityBase::OnMontageInterrupted()
{
	
}

void UTSGameplayAbilityBase::OnMontageCancelled()
{
	
}

void UTSGameplayAbilityBase::OnMontageCompleted()
{
}