// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Monster/MonsterGAS/GA_Mon_Hit.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AI/Monster/Base/MonsterCharacterInterface.h"
#include "AbilitySystemComponent.h"
#include "AI/Monster/MonsterGAS/TSMonsterAS.h"
#include "GameplayTags/AbilityGameplayTags.h"

UGA_Mon_Hit::UGA_Mon_Hit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_Mon_Hit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilitySystemComponent* MonsterASC = ActorInfo->AbilitySystemComponent.Get();
	if (MonsterASC)
	{
		if (TriggerEventData)
		{
			if (DamageEffect)
			{
				if (TriggerEventData->EventMagnitude > 0.f)
				{
					UE_LOG(LogTemp, Log, TEXT("캐릭터 무기 공격력 : %f"), 
						TriggerEventData->EventMagnitude);
					FGameplayEffectContextHandle ContextHandle = MonsterASC->MakeEffectContext();
					ContextHandle.AddInstigator(ActorInfo->AvatarActor.Get(), ActorInfo->AvatarActor.Get()); 
					FGameplayEffectSpecHandle SpecHandle = MonsterASC->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);
					if (SpecHandle.IsValid())
					{
						SpecHandle.Data.Get()->SetSetByCallerMagnitude(AbilityTags::TAG_Enemy_Damage, -TriggerEventData->EventMagnitude);
						MonsterASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
	}
	
	if (!K2_CommitAbility() || !IsValid(AbilityMontage))
	{
		K2_EndAbility();
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("몬스터한테 데미지 적용 후 시각 처리 지시 수신1"))
	
#pragma region 속도_처리
	IMonsterCharacterInterface* MonsterInterface = Cast<IMonsterCharacterInterface>(ActorInfo->AvatarActor.Get());
	if (!MonsterInterface) 
	{
		UE_LOG(LogTemp, Warning, TEXT("몬스터한테 데미지 적용 후 시각 처리 지시 수신2	"))
		K2_EndAbility();
		return;
	}
	MonsterInterface->StopWalk();
	CachingMonster = ActorInfo->AvatarActor.Get();
#pragma endregion
	
	UAbilitySystemComponent* ThisMonsterASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ThisMonsterASC)
	{
		K2_EndAbility();
		return;
	}
	
	float CurrentHP = ThisMonsterASC->GetNumericAttribute(UTSMonsterAS::GetHealthAttribute());
	UE_LOG(LogTemp, Warning, TEXT("몬스터한테 데미지 적용 :%f"), CurrentHP);
	
	if (CurrentHP <= 0.f)
	{
#pragma region 죽어야하는_경우
		SendFinishMontageEventToStateTree(MonsterDeadTag);
		K2_EndAbility();
#pragma endregion
	}
	else
	{
#pragma region 시각_처리 (죽음 아닐 경우)
		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
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
	
		UE_LOG(LogTemp, Warning, TEXT("몬스터한테 데미지 적용 후 시각 처리 지시 수신"))
	
		Task->ReadyForActivation();
#pragma endregion
	}
}

void UGA_Mon_Hit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	
	IMonsterCharacterInterface* MonsterInterface = Cast<IMonsterCharacterInterface>(CachingMonster);
	if (MonsterInterface)
	{
		MonsterInterface->RegainSpeed();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Mon_Hit::OnMontageCompleted()
{
	SendFinishMontageEventToStateTree(MonsterHitNoticeTag);
	K2_EndAbility();
}

void UGA_Mon_Hit::OnMontageCancelled()
{
	SendReceiveHitToStateTree(MonsterHitNoticeTag);
	K2_EndAbility();
}

void UGA_Mon_Hit::OnMontageInterrupted()
{
	SendReceiveHitToStateTree(MonsterHitNoticeTag);
	K2_EndAbility();
}

void UGA_Mon_Hit::OnMontageBlendOut()
{
	SendReceiveHitToStateTree(MonsterHitNoticeTag);
	K2_EndAbility();
}
