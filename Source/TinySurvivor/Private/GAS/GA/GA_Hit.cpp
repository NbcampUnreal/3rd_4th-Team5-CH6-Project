// GA_Hit.cpp

#include "GAS/GA/GA_Hit.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/AbilityGameplayTags.h"

UGA_Hit::UGA_Hit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UGA_Hit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 1. Payload 유효성 검사
    if (!TriggerEventData)
    {
		UE_LOG(LogTemp, Warning, TEXT("GA_Hit: Damage Fail1"));
    	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // 2. Payload에서 데이터 꺼내기 (Notify에서 포장한 것들)
    AActor* AttackerActor = const_cast<AActor*>(TriggerEventData->Instigator.Get()); // 때린 놈
    float IncomingDamage = TriggerEventData->EventMagnitude; // 데미지

    UAbilitySystemComponent* TargetASC = ActorInfo->AbilitySystemComponent.Get();
    if (!TargetASC || !DamageEffect) 
    {
    	UE_LOG(LogTemp, Warning, TEXT("GA_Hit: Damage Fail2"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

	// 3. 데미지 이펙트 적용
    FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
    ContextHandle.AddInstigator(AttackerActor, AttackerActor); 
    FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);

    if (SpecHandle.IsValid())
    {
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        UE_LOG(LogTemp, Warning, TEXT("GA_Hit: Applied %.1f Damage from %s"), IncomingDamage, *GetNameSafe(AttackerActor));
    }
    else
    {
	    UE_LOG(LogTemp, Warning, TEXT("GA_Hit: Damage Fail3"));
    }
	
	//=======================================================================
	// 방어구 피격 이벤트 전송 (방어구 내구도 감소용)
	//=======================================================================
	FGameplayEventData ArmorHitEventData;
	ArmorHitEventData.Instigator = AttackerActor;
	ArmorHitEventData.Target = ActorInfo->OwnerActor.Get();
	ArmorHitEventData.EventMagnitude = IncomingDamage;
	TargetASC->HandleGameplayEvent(AbilityTags::TAG_Event_Armor_Hit,&ArmorHitEventData);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogTemp, Log, TEXT("GA_Hit: 방어구 피격 이벤트 전송 (Damage=%.1f)"), IncomingDamage);
#endif

    // -----------------------------------------------------------------------
    // 피격 몽타주 재생
    // -----------------------------------------------------------------------
    // UAbilityTask_PlayMontageAndWait* Task = ...
    
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
