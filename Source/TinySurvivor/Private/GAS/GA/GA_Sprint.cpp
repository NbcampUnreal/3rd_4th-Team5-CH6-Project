#include "GAS/GA/GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "GameFramework/Character.h" 

UGA_Sprint::UGA_Sprint()
{
	CostHandle.Invalidate();
	SpeedHandle.Invalidate();
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
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
	if (SprintSpeedEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(Handle, ActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SprintSpeedEffectClass, GetAbilityLevel(), ContextHandle);
		if (SpecHandle.IsValid())
		{
			SpeedHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(MovementCheckTimerHandle, this, &UGA_Sprint::SprintCheck, 0.1f, true);  
	SprintCheck();
	
	StaminaDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute()).AddUObject(this, &UGA_Sprint::OnAttributeChanged);
	ThirstDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetThirstAttribute()).AddUObject(this, &UGA_Sprint::OnAttributeChanged);
}

void UGA_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(MovementCheckTimerHandle);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
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
		if (CostHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(CostHandle);
			CostHandle.Invalidate();
		}
	}
	
}