#include "GAS/GA/GA_Crouch.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitMovementModeChange.h" 
#include "GameFramework/PlayerController.h" 

UGA_Crouch::UGA_Crouch()
{
	SpeedHandle.Invalidate();
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
}

bool UGA_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Move.Roll"))))
	{
		return false; // roll 중이면 앉기 불가
	} 
	return true;
}

void UGA_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	
	if ( !ASC || !Character || !IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	// 공중 (점프 중이라면)
	if (Character->GetCharacterMovement()->IsFalling())
	{
		// 착지할때까지 기다림
		UAbilityTask_WaitMovementModeChange* WaitLandTask = UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(this,EMovementMode::MOVE_Walking);

		if (WaitLandTask)
		{ //발이 땅에 닿으면 OnLanded 함수 실행
			WaitLandTask->OnChange.AddDynamic(this, &UGA_Crouch::OnLanded);
			WaitLandTask->ReadyForActivation();
			return; 
		}
	}
	OnLanded(EMovementMode::MOVE_Walking); //땅에 서있으면 즉시 앉기 로직 실행
}

void UGA_Crouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	
	if (Character)
	{
		Character->UnCrouch(); //일어서기
	}

	if (ASC && SpeedHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(SpeedHandle); //속도 감소 GE 제거
		SpeedHandle.Invalidate();
	}
	
	//일어날때 달리기 해야하는 경우 
	if (!bWasCancelled && SprintAbilityClass && Character)
	{
		APlayerController* PC = Cast<APlayerController>(Character->GetController());
       
		// Shift 키가 눌려있는지 확인
		if (PC && PC->IsInputKeyDown(EKeys::LeftShift))
		{
			// 계속  sprint 어빌리티 실행 시도
			ASC->TryActivateAbilityByClass(SprintAbilityClass);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Crouch::OnLanded(EMovementMode NewMovementMode)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (Character && ASC)
	{
		Character->Crouch(); // crouch 호출
		
		if (CrouchSpeedEffectClass) //speed GE 호출 0.75배
		{
			FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CrouchSpeedEffectClass, GetAbilityLevel(), ContextHandle);
          
			if (SpecHandle.IsValid())
			{
				SpeedHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}
