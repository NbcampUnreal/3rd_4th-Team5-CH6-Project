// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Gaint/GA/TSGiantErosionChange.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AI/Gaint/GA/TSGiantStateTreeSendTag.h"
#include "System/Erosion/TSErosionSubsystem.h"

UTSGiantErosionChange::UTSGiantErosionChange()
{
}

void UTSGiantErosionChange::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
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
		0.3f,                   // 재생 속도
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

	if (HasAuthority(&ActivationInfo))
	{
		UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
		if (IsValid(ErosionSubSystem))
		{
			ErosionSubSystem->AddOrSubtractErosion(ErosionChangeValue);
		}
	}
	
	Task->ReadyForActivation();
}

void UTSGiantErosionChange::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTSGiantErosionChange::OnMontageCompleted()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_EROSIONCHANGE_ENDTAG);
	K2_EndAbility();
}

void UTSGiantErosionChange::OnMontageCancelled()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_EROSIONCHANGE_ENDTAG);
	K2_EndAbility();
}

void UTSGiantErosionChange::OnMontageInterrupted()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_EROSIONCHANGE_ENDTAG);
	K2_EndAbility();
}

void UTSGiantErosionChange::OnMontageBlendOut()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_EROSIONCHANGE_ENDTAG);
	K2_EndAbility();
}
