// GA_MonsterAttack.cpp

#include "AI/Common/GA/GA_MonsterAttack.h"
#include "Abilities//Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/AbilityGameplayTags.h"


UGA_MonsterAttack::UGA_MonsterAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(MonsterTags::TAG_Ability_Attack);
}

void UGA_MonsterAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 몽타주 생성
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	Task->OnCompleted.AddDynamic(this, &UGA_MonsterAttack::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_MonsterAttack::OnMontageCompleted);
	Task->ReadyForActivation();
	
	// 히트 타이밍 대기
	FGameplayTag HitTag = MonsterTags::TAG_Event_Montage_Hit;
	UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitTag);
	WaitTask->EventReceived.AddDynamic(this, &UGA_MonsterAttack::OnEventReceived);
	WaitTask->ReadyForActivation();
}

void UGA_MonsterAttack::OnEventReceived(FGameplayEventData EventData)
{
	// 공격 판정
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
		return;
	
	FVector Center = Avatar->GetActorLocation() + (Avatar->GetActorForwardVector() * 100.0f);
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);
	
	bool bHit = GetWorld()->OverlapMultiByChannel(Overlaps, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AttackRadius), Params);
	
	if (bHit)
	{
		for (const FOverlapResult& Result : Overlaps)
		{
			AActor* Target = Result.GetActor();
			if (!Target)
				return;
			
			// 플레이어 태그 확인
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
			if (TargetASC)
			{
				if (TargetASC->HasMatchingGameplayTag(MonsterTags::TAG_Character_Type_Player))
				{
					// 데미지 적용
					FGameplayEffectContextHandle Context = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
					TargetASC->ApplyGameplayEffectToSelf(DamageEffectClass.GetDefaultObject(), 1.0f, Context);
				}
			}
		}
	}
}

void UGA_MonsterAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}