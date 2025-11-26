#include "GAS/GA/GA_Roll.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/TSCharacter.h"
#include "GAS/AttributeSet/TSAttributeSet.h"

UGA_Roll::UGA_Roll()
{
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
	//roll, block 태그 알림
	if (RollMoveTag.IsValid())
	{
		ASC->AddLooseGameplayTag(RollMoveTag);
	}
	if (StaminaBlockTag.IsValid())
	{
		ASC->AddLooseGameplayTag(StaminaBlockTag);
	}
	
	/*
	 * GE_RollCost 적용
	 */
	if (RollCostEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(Handle, ActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(RollCostEffectClass, GetAbilityLevel(), ContextHandle);
		
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	//몽타주 선택
	UAnimMontage* Selected = DetermineRollMontage();
	//그 몽타주 재생 
	this->FirstMontage = Selected;
	
	//끝났을때 알려줄 태스크
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
	// wasd 입력과 카메라 방향을 합쳐서 4방향 롤 중에 하나 선택
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{ 
		// 실패시 그냥 앞구르기
		return RollForwardMontage;
	}
	const FVector Velocity = Character->GetVelocity();
	
	if (Velocity.SizeSquared() <= KINDA_SMALL_NUMBER)
	{
		return RollForwardMontage;
	}
	// 카메라 방향 제어 -> 컨트롤러 가져오기
	AController* Controller = Character->GetController();
	if (!Controller)
	{
		return RollForwardMontage; //없으면 그냥 정면 돌기
	}
	const FRotator ControlYaw(0, Controller->GetControlRotation().Yaw, 0); //카메라의 yaw값만 쓰기
	const FVector Forward = ControlYaw.Vector(); // 카메라 기준 앞벡터
	const FVector Right = FRotationMatrix(ControlYaw).GetUnitAxis(EAxis::Y); // 카메라 기준 오른쪽 벡터
	
	const FVector Dir = Velocity.GetSafeNormal();
	
	float ForwardDot = FVector::DotProduct(Dir, Forward);  // 월드벡터 앞 뒤 중 어디에 가까운지 +면 앞
	float RightDot = FVector::DotProduct(Dir, Right);//월드 벡터 좌 우 중 어디에 가까운지 +면 우
	
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		return ForwardDot > 0 ? RollForwardMontage : RollBackwardMontage; // W쪽이 더 가까우면 앞으로 돌기
	}
	else
	{
		return RightDot > 0 ? RollRightMontage : RollLeftMontage; // D 쪽이 가까우면 오른쪽 돌기
	}
}

void UGA_Roll::OnRollMontageFinished()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	// 태그 없애기
	if (RollMoveTag.IsValid())
	{
		ASC->RemoveLooseGameplayTag(RollMoveTag);
	}
	if (StaminaBlockTag.IsValid())
	{
		ASC->RemoveLooseGameplayTag(StaminaBlockTag);
	}
	
	// 롤 끝나고 1초 동안 딜레이
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