#include "GAS/GA/GA_Crouch.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h" 
#include "Character/TSCharacter.h"
#include "GameFramework/PlayerController.h" 

UGA_Crouch::UGA_Crouch()
{
	SpeedHandle.Invalidate();
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	return true;
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ATSCharacter* Character = Cast<ATSCharacter>(ActorInfo->AvatarActor.Get());
	
	if ( !ASC || !Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (Character->GetCharacterMovement()->IsFalling())
	{
		UAbilityTask_WaitMovementModeChange* WaitLandTask = UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(this,EMovementMode::MOVE_Walking);

		if (WaitLandTask)
		{ 
			WaitLandTask->OnChange.AddDynamic(this, &UGA_Crouch::OnLanded);
			WaitLandTask->ReadyForActivation();
			return; 
		}
	}
	OnLanded(EMovementMode::MOVE_Walking);
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ATSCharacter* Character = Cast<ATSCharacter>(ActorInfo->AvatarActor.Get());
	
	if (Character)
	{
		Character->bIsCrouching = false;
	}

	if (ASC && SpeedHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(SpeedHandle); 
		SpeedHandle.Invalidate();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Crouch::OnLanded(EMovementMode NewMovementMode)
{
	if (!IsActive())
	{
		return;
	}
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	if (Character && ASC)
	{
		Character->bIsCrouching = true;
		
		if (CrouchSpeedEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CrouchSpeedEffectClass, GetAbilityLevel(), ContextHandle);
          
			if (SpecHandle.IsValid())
			{
				ASC->RemoveActiveGameplayEffect(SpeedHandle);
				SpeedHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}
