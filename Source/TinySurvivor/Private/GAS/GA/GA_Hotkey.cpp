// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/GA_Hotkey.h"

#include "AbilitySystemComponent.h"

UGA_Hotkey::UGA_Hotkey()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_Hotkey::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (!ActorInfo)
	{
		return;
	}

	if (ActorInfo->IsNetAuthority() && !Spec.IsActive())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (ASC)
		{
			ASC->TryActivateAbility(Spec.Handle);
		}
	}
}
