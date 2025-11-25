#include "AI/Gaint/GA/TSGiantJustScream.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AI/Gaint/GA/TSGiantStateTreeSendTag.h"

UTSGiantJustScream::UTSGiantJustScream()
{
}

void UTSGiantJustScream::ActivateAbility(
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

	Task->ReadyForActivation();
}

void UTSGiantJustScream::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UTSGiantJustScream::OnMontageCompleted()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_JUSTSCREAM_ENDTAG);
	K2_EndAbility();
}

void UTSGiantJustScream::OnMontageCancelled()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_JUSTSCREAM_ENDTAG);
	K2_EndAbility();
}

void UTSGiantJustScream::OnMontageInterrupted()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_JUSTSCREAM_ENDTAG);
	K2_EndAbility();
}

void UTSGiantJustScream::OnMontageBlendOut()
{
	SendFinishMontageEventToStateTree(GiantStateTreeSendTag::GIANTSTATETREESENDTAG_GA_JUSTSCREAM_ENDTAG);
	K2_EndAbility();
}
