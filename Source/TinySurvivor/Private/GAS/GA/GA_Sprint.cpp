#include "GAS/GA/GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "GameFramework/Character.h" 

UGA_Sprint::UGA_Sprint()
{
	CostHandle.Invalidate();
	SpeedHandle.Invalidate();
}

bool UGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}
	
	const float CurrentStamina = ASC->GetNumericAttribute(UTSAttributeSet::GetStaminaAttribute());
	const float CurrentThirst = ASC->GetNumericAttribute(UTSAttributeSet::GetThirstAttribute());
	
	return CurrentStamina > 0.f && CurrentThirst > 0.f;
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if ( !ASC || !IsActive() )
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	/*
	 * 1. GE_SprintCost 
	 * 2. GE_SprintSpeed 
	 * 3. Stamina <=0 && Thirst <= 0이면 EndAbilty 호출
	 */
	FGameplayEffectContextHandle ContextHandle = MakeEffectContext(Handle, ActorInfo);
	
	/*if (SprintCostEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SprintCostEffectClass, GetAbilityLevel(), ContextHandle);
		if (SpecHandle.IsValid())
		{
			CostHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}*/
	if (SprintSpeedEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SprintSpeedEffectClass, GetAbilityLevel(), ContextHandle);
		if (SpecHandle.IsValid())
		{
			SpeedHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(MovementCheckTimerHandle, this, &UGA_Sprint::SprintCheck, 0.1f, true); // 함수 최초 1회 즉시 호출 
	SprintCheck();
	
	// Stamina
	StaminaDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute()).AddUObject(this, &UGA_Sprint::OnAttributeChanged);

	// Thirst
	ThirstDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetThirstAttribute()).AddUObject(this, &UGA_Sprint::OnAttributeChanged);
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// GE 자동 제지
	// GE StaminaDelay 적용
	// 속도 체크 타이머 정리
	GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		// Delegate 해제
		if (StaminaDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute()).Remove(StaminaDelegateHandle);
			StaminaDelegateHandle.Reset();
		}

		if (ThirstDelegateHandle.IsValid())
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetThirstAttribute()).Remove(ThirstDelegateHandle);
			ThirstDelegateHandle.Reset();
		}

		// GE 제거
		if (CostHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(CostHandle);
			CostHandle.Invalidate();
		}

		if (SpeedHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(SpeedHandle);
			SpeedHandle.Invalidate();
		}

		// GE_StaminaDelay 적용
		if (StaminaDelayEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = MakeEffectContext(Handle, ActorInfo);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaDelayEffectClass, GetAbilityLevel(), ContextHandle);
			
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Sprint::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!IsActive())
	{
		return;
	}

	// 0 이하로 떨어지면 종료 (Stamina<=0 && Thirst<=0)
	if (Data.NewValue <= 0.f)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
void UGA_Sprint::SprintCheck()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	const float CurrentSpeed = Character->GetVelocity().Size2D();
	const bool bIsSprint = CurrentSpeed > 600.0f; 
	
	if (bIsSprint)
	{
		// 아직 코스트 GE가 없다면 적용
		if (!CostHandle.IsValid() && SprintCostEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
			FGameplayEffectSpecHandle SpecHandle =
				ASC->MakeOutgoingSpec(SprintCostEffectClass, GetAbilityLevel(), ContextHandle);

			if (SpecHandle.IsValid())
			{
				CostHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	else
	{
		// 속도가 떨어졌으면 코스트 GE 제거
		if (CostHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(CostHandle);
			CostHandle.Invalidate();
		}
	}
	
}