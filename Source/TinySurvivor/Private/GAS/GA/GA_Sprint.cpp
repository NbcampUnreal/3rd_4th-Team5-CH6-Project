#include "GAS/GA/GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/AttributeSet/TSAttributeSet.h"

UGA_Sprint::UGA_Sprint()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Move.Sprint")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Move.Sprint")));
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!IsActive() || !ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (RecoverStaminaEffectClass) //기존 스태미나 회복 GE 제거
	{
		ASC->RemoveActiveGameplayEffectBySourceEffect(RecoverStaminaEffectClass, ASC); 
	}
	if (NonSprintEffectHandle.IsValid())  // 기존 Nonsprint GE 제거
	{
		ASC->RemoveActiveGameplayEffect(NonSprintEffectHandle);
		NonSprintEffectHandle.Invalidate();
	}
	//현재 스태미나 <= 0 이면 -> 실행 못하도록 
	const FGameplayAttribute StaminaAttribute = UTSAttributeSet::GetStaminaAttribute();
	const float CurrentStamina = ASC->GetNumericAttribute(StaminaAttribute);
	
	if (CurrentStamina <= 0.0f) //현재 스태미나 0 이하로 떨어지면 못뜀->바로종료
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// 스태미나 변할때마다 받아오는 델리게이트 -> 0 이하로 떨어지면 자동 종료
	ASC->GetGameplayAttributeValueChangeDelegate(StaminaAttribute).Clear();
	ASC->GetGameplayAttributeValueChangeDelegate(StaminaAttribute)
		.AddLambda([this](const FOnAttributeChangeData& Data)
		{
			if (Data.NewValue <= 0.0f && IsActive()) // 스태미나 변화 감지 : 0 이하로 떨어지면 바로 end 
			{
				EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
			}
		});
	
	//스프린트 GE 적용
	if (SprintEffectClass) 
	{
		SprintEffectHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, SprintEffectClass.GetDefaultObject(), 1.0f);
	}
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		//스태미나 변경 델리게이트 클리어로 정리
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute()).Clear();
		if (SprintEffectHandle.IsValid()) //스프린트 GE 제거
		{
			ASC->RemoveActiveGameplayEffect(SprintEffectHandle);
			SprintEffectHandle.Invalidate();
		}
		if (NonSprintEffectClass) //항상 Nonsprint GE 적용 -> 속도 600
		{
			if (NonSprintEffectHandle.IsValid()) //기존이면 제거 후 재적용
			{
				ASC->RemoveActiveGameplayEffect(NonSprintEffectHandle);
				NonSprintEffectHandle.Invalidate();
			}
			NonSprintEffectHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo,NonSprintEffectClass.GetDefaultObject(), 1.0f);
		}
		
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

bool UGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) return false;
	// 현재 스테미나 가져와서 0 이하면 sprint 못함
	const float CurrentStamina = ASC->GetNumericAttribute(UTSAttributeSet::GetStaminaAttribute());
	return CurrentStamina > 0.f;
}