// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Monster/MonsterGAS/GA_MonBase.h"
#include "AI/Monster/Base/MonsterAICInterface.h"

UGA_MonBase::UGA_MonBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_MonBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_MonBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MonBase::OnMontageBlendOut()
{
}

void UGA_MonBase::OnMontageInterrupted()
{
}

void UGA_MonBase::OnMontageCancelled()
{
}

void UGA_MonBase::OnMontageCompleted()
{
}

void UGA_MonBase::SendFinishMontageEventToStateTree(const FGameplayTag& InSendTag)
{
	if (!IsValid(GiantStateTreeComponent())) return;
	
	if (MontageEndTag == FGameplayTag::EmptyTag)
	{
		GiantStateTreeComponent()->SendStateTreeEvent(InSendTag);
	}
	else
	{
		GiantStateTreeComponent()->SendStateTreeEvent(MontageEndTag);
	}
}

UStateTreeAIComponent* UGA_MonBase::GiantStateTreeComponent()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!IsValid(AvatarActor)) return nullptr;
	
	APawn* CastingPawn = Cast<APawn>(AvatarActor);
	if (!IsValid(CastingPawn)) return nullptr;

	AController* CastingAIC = CastingPawn->GetController();
	if (!IsValid(CastingAIC)) return nullptr;
	
	IMonsterAICInterface* MonsterInterface = Cast<IMonsterAICInterface>(CastingAIC);
	if (!MonsterInterface) return nullptr;
	
	return MonsterInterface->GetStateTreeAIComponent();
}
