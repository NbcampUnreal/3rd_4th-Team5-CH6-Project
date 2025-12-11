// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Monster/MonsterGAS/GA_Mon_Dead.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Mon_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!K2_CommitAbility() || !IsValid(AbilityMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Mon_Dead::ActivateAbility() AbilityMontage is invalid"));
		K2_EndAbility();
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
		Character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,                   // OwningAbility
			NAME_None,              // TaskInstanceName
			AbilityMontage,      // 재생할 몽타주
			1.0f,                   // 재생 속도
			NAME_None,              // StartSectionName
			false,                   // StopWhenAbilityEnds
			1.0f                    // RootMotionTranslationScale
		);
	
	if (!IsValid(Task))
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Mon_Dead::ActivateAbility() Task is invalid"));
		K2_EndAbility();
		return;
	}
	
	Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
	Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageBlendOut);
	
	Task->ReadyForActivation();
}

void UGA_Mon_Dead::OnMontageCompleted()
{
	K2_EndAbility();
}

void UGA_Mon_Dead::OnMontageCancelled()
{
	K2_EndAbility();
}

void UGA_Mon_Dead::OnMontageInterrupted()
{
	K2_EndAbility();
}

void UGA_Mon_Dead::OnMontageBlendOut()
{
	K2_EndAbility();
}
