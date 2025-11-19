#include "GAS/GA/GA_Roll.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/AttributeSet/TSAttributeSet.h"

UGA_Roll::UGA_Roll()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Move.Roll")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Move.Roll")));
}

void UGA_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsActive() || !ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (RecoverStaminaEffectClass)
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(RecoverStaminaEffectClass, ASC);
	}
	
	if (RollEffectClass)
	{
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, RollEffectClass.GetDefaultObject(), 1.0f);
	}
	// if(FirstMontage) PlayMontage();
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Roll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		// 스프린트 끝나고 1초 뒤 GE_RecoverStamina 적용
		if (RecoverStaminaEffectClass)
		{
			if (UWorld* World = GetWorld())
			{
				FTimerHandle TempHandle;
				TWeakObjectPtr<UAbilitySystemComponent> ASCWeak = ASC;
				TSubclassOf<UGameplayEffect> RecoverClass = RecoverStaminaEffectClass;

				World->GetTimerManager().SetTimer(
					TempHandle,
					[ASCWeak, RecoverClass]()
					{
						if (!ASCWeak.IsValid() || !RecoverClass)
						{
							return;
						}
						UAbilitySystemComponent* LocalASC = ASCWeak.Get();
						FGameplayEffectContextHandle Ctx = LocalASC->MakeEffectContext();
						Ctx.AddSourceObject(LocalASC->GetOwner());

						const UGameplayEffect* EffectCDO = RecoverClass.GetDefaultObject();
						LocalASC->ApplyGameplayEffectToSelf(EffectCDO, 1.0f, Ctx);
					},
					1.0f, // 1초 딜레이 
					false); //반복인가 아닌가 Loop
			}
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_Roll::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) return false;
	// 현재 스태미나 가져와서 0 이하면 sprint 못함
	const float CurrentStamina = ASC->GetNumericAttribute(UTSAttributeSet::GetStaminaAttribute());
	return CurrentStamina >= 20.f;
}

