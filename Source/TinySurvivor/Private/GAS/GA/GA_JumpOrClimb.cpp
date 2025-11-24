#include "GAS/GA/GA_JumpOrClimb.h"

#include "Character/TSCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "GAS/AttributeSet/TSAttributeSet.h"

UGA_JumpOrClimb::UGA_JumpOrClimb()
{
	CostHandle.Invalidate();
}

void UGA_JumpOrClimb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if ( ClimbableActor())
	{
		StartClimb(); // 스태미나 소모 O
		ASC->AddLooseGameplayTag(AbilityTags::TAG_State_Move_Climb);
	} else
	{
		StartJump(); // 스태미나 소모 X
		ASC->AddLooseGameplayTag(AbilityTags::TAG_State_Move_Jump);
	}
}

void UGA_JumpOrClimb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(ClimbableCheckTimerHandle); //타이머 클리어
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	const bool bWasClimbing = CostHandle.IsValid();
	
	if (bWasClimbing && Character->GetCharacterMovement()->MovementMode == MOVE_Flying) //클라이밍 종료 
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking); //중력 적용 되어 뚝 떨어짐 
	}
	
	Character->CurrentWallNormal = FVector::ZeroVector;
	Character->StopJumping();
	
	
	if (StaminaDelegateHandle.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute()).Remove(StaminaDelegateHandle);
		StaminaDelegateHandle.Reset();
	}
	
	if (bWasClimbing)
	{
		if (CostHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(CostHandle);
			CostHandle.Invalidate();
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
		
		ASC->RemoveLooseGameplayTag(AbilityTags::TAG_State_Move_Climb);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(AbilityTags::TAG_State_Move_Jump);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_JumpOrClimb::ClimbableActor()
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	const FVector Start = Character->GetActorLocation();
	const FVector Forward = Character->GetActorForwardVector();
	const FVector End = Start + ( Forward * TraceDistance );
	
	FHitResult Hit;
	TArray<AActor*> Ignored;
	Ignored.Add(Character);
	
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(this ,Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::ForDuration, Hit,true);
	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->ActorHasTag(ClimbableTagName))
		{
			Character->CurrentWallNormal = Hit.ImpactNormal;
			return true;
		}
	}
	
	return false;
}

void UGA_JumpOrClimb::StartJump()
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	if (Character->CanJump())
	{
		Character->Jump();
	}
}

void UGA_JumpOrClimb::StartClimb()
{
	//State.Climb 태그 부여
	UE_LOG(LogTemp,Error,TEXT("Climbable 태그가진 액터 발견 -> Climb 시작 "));
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	// Stamina
	StaminaDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute()).AddUObject(this, &UGA_JumpOrClimb::OnAttributeChanged);

	if (!CostHandle.IsValid() && ClimbCostEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ClimbCostEffectClass, GetAbilityLevel(), ContextHandle);

		if (SpecHandle.IsValid())
		{
			CostHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	// 물리 모드 변경 (Flying : 중력 무시해서 벽타도록 )
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	Character->GetCharacterMovement()->StopMovementImmediately(); //안미끄러지게 
	
	// 벽 상태 감시 타이머 시작 (0.05마다 )
	GetWorld()->GetTimerManager().SetTimer(ClimbableCheckTimerHandle, this, &UGA_JumpOrClimb::CheckClimbingState, 0.05f, true);
}

void UGA_JumpOrClimb::CheckClimbingState()
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	const FVector Start = Character->GetActorLocation();
	const FVector Forward = Character->GetActorForwardVector();
	const FVector End = Start + ( Forward * TraceDistance );
	
	FHitResult Hit;
	TArray<AActor*> Ignored;
	Ignored.Add(Character);
	
	//배에서 쏘는 트레이스 (기존 벽 감지)
	bool bBodyHit = UKismetSystemLibrary::SphereTraceSingle(this ,Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::ForDuration, Hit,true);
	if (!bBodyHit) //배에서 탐지 -> 벽이 없다? 그러면 허공이니까 낙하
	{
		UE_LOG(LogTemp,Warning, TEXT("벽이 끝남 -> 낙하 "));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
		return;
	}
	// 눈높이에서 쏘는 트레이스 (난간? 감지) 
	const FVector EyeStart = Start + FVector(0, 0, EyeHeight);
	const FVector EyeEnd = EyeStart + (Forward * TraceDistance);
	FHitResult EyeHit;
	bool bEyeHit = UKismetSystemLibrary::SphereTraceSingle(this ,EyeStart, EyeEnd, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::ForDuration, EyeHit,true);
	
	// 배는 닿았는데 눈은 안닿았다? -> 난간 끝에 도달했구나!
	
	if (!bEyeHit)
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		//끝부분에서 약간 점프해서 올라가기
		FVector VaultVel = (FVector::UpVector * 400.f) + (Character->GetActorForwardVector() * 200.f);
		Character->LaunchCharacter(VaultVel, true, true);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
	}

	Character->CurrentWallNormal = Hit.ImpactNormal;
}

void UGA_JumpOrClimb::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f) // Stamina <= 0 && Thirst <= 0 이면 
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
