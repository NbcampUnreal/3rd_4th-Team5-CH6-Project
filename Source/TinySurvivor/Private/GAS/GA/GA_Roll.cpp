#include "GAS/GA/GA_Roll.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/TSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/TSAttributeSet.h"

UGA_Roll::UGA_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}
bool UGA_Roll::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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
	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character && Character->GetCharacterMovement()->IsFalling())
	{
		return false;
	}
	const float CurrentStamina = ASC->GetNumericAttribute(UTSAttributeSet::GetStaminaAttribute());
	const float CurrentThirst = ASC->GetNumericAttribute(UTSAttributeSet::GetThirstAttribute());
	
	return CurrentStamina >= 20.f && CurrentThirst > 0.f;
}

void UGA_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if ( !ASC || !IsActive() )
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (RollCostEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(Handle, ActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(RollCostEffectClass, GetAbilityLevel(), ContextHandle);
		
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	UAnimMontage* Selected = DetermineRollMontage();
	this->FirstMontage = Selected;
	
	UAbilityTask_PlayMontageAndWait* Task =UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Selected);
	if (Task)
	{
		Task->OnCompleted.AddDynamic(this, &UGA_Roll::OnRollMontageFinished);
		Task->OnBlendOut.AddDynamic(this, &UGA_Roll::OnRollMontageFinished);
		Task->OnInterrupted.AddDynamic(this, &UGA_Roll::OnRollMontageFinished);
		Task->OnCancelled.AddDynamic(this, &UGA_Roll::OnRollMontageFinished);
		Task->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UGA_Roll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UAnimMontage* UGA_Roll::DetermineRollMontage() const
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{ 
		return RollForwardMontage;
	}
	const FVector Velocity = Character->GetVelocity();
	
	if (Velocity.SizeSquared() <= KINDA_SMALL_NUMBER)
	{
		return RollForwardMontage;
	}
	AController* Controller = Character->GetController();
	if (!Controller)
	{
		return RollForwardMontage;
	}
	const FRotator ControlYaw(0, Controller->GetControlRotation().Yaw, 0); 
	const FVector Forward = ControlYaw.Vector();
	const FVector Right = FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::Y);
	
	const FVector Dir = Velocity.GetSafeNormal();
	
	float ForwardDot = FVector::DotProduct(Dir, Forward);
	float RightDot = FVector::DotProduct(Dir, Right);
	
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		return ForwardDot > 0 ? RollForwardMontage : RollBackwardMontage;
	}
	else
	{
		return RightDot > 0 ? RollRightMontage : RollLeftMontage;
	}
}

void UGA_Roll::OnRollMontageFinished()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (StaminaDelayEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaDelayEffectClass, GetAbilityLevel(), ContextHandle);
			
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}