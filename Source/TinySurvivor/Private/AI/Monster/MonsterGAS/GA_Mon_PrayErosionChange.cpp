// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Monster/MonsterGAS/GA_Mon_PrayErosionChange.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_Mon_PrayErosionChange::UGA_Mon_PrayErosionChange()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_Mon_PrayErosionChange::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	
	if (!K2_CommitAbility() || !IsValid(AbilityMontage))
	{
		K2_EndAbility();
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* Task =
	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,                   // OwningAbility
		NAME_None,              // TaskInstanceName
		AbilityMontage,      // 재생할 몽타주
		1.0f,                   // 재생 속도
		NAME_None,              // StartSectionName
		true,                   // StopWhenAbilityEnds
		1.0f                    // RootMotionTranslationScale
	);
	
	if (!IsValid(Task))
	{
		K2_EndAbility();
		return;
	}
	
	Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
	Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageBlendOut);
	
	SendErosionChangeToErosionSystem();
	
	Task->ReadyForActivation();
}

void UGA_Mon_PrayErosionChange::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Mon_PrayErosionChange::OnMontageCompleted()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}

void UGA_Mon_PrayErosionChange::OnMontageCancelled()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}

void UGA_Mon_PrayErosionChange::OnMontageInterrupted()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}

void UGA_Mon_PrayErosionChange::OnMontageBlendOut()
{
	SendFinishMontageEventToStateTree(MontageEndTag);
	K2_EndAbility();
}
