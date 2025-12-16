#include "GAS/GA/GA_JumpOrClimb.h"

#include "Character/TSCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Components/CapsuleComponent.h"

UGA_JumpOrClimb::UGA_JumpOrClimb()
{
	CostHandle.Invalidate();
}

void UGA_JumpOrClimb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FHitResult HitResult;
	if ( ClimbableActor(HitResult))
	{
		StartClimb(HitResult); // 스태미나 소모 O
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
	GetWorld()->GetTimerManager().ClearTimer(MantleFinishTimerHandle);
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	const bool bWasClimbing = CostHandle.IsValid();
	
	Character->GetCharacterMovement()->MaxFlySpeed = 600.0f;
	Character->GetCharacterMovement()->BrakingDecelerationFlying = 0.0f;
	
	if (bWasClimbing && Character->GetCharacterMovement()->MovementMode == MOVE_Flying) //클라이밍 종료 
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking); //중력 적용 되어 뚝 떨어짐 
	}
	
	Character->CurrentWallNormal = FVector::ZeroVector;
	Character->StopJumping();
	
	if (bWasClimbing && Character->HasAuthority())
	{
		Character->bIsClimbState = false;
	}
	
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

bool UGA_JumpOrClimb::ClimbableActor(FHitResult& OutHit)
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	const FVector Start = Character->GetActorLocation();
	const FVector Forward = Character->GetActorForwardVector();
	const FVector End = Start + ( Forward * TraceDistance );
	
	TArray<AActor*> Ignored;
	Ignored.Add(Character);
	
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(this ,Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::None, OutHit,true);
	if (bHit)
	{
		AActor* HitActor = OutHit.GetActor();
		
		// 벽 기울기 검사 (바닥이나 천장이면 false)
		//if (FMath::Abs(OutHit.ImpactNormal.Z) > 0.2f) // 벽 기울기 검사 (경사나 바닥)
		//{
		//	UE_LOG(LogTemp,Warning,TEXT("벽이 너무 기울어져 있음"));
		//	return false;
		//}
		
		// 정면 판정 (캐릭터가 벽을 정면으로 보고 있는가?)
		float FacingDot = FVector::DotProduct(Forward, -OutHit.ImpactNormal);
		if (FacingDot < 0.6f )
		{
			UE_LOG(LogTemp,Warning,TEXT("벽을 정면으로 보고있지않음"));
			return false;
		}
		// 거리 제한 (벽이 너무 멀면 false)
		float DistanceToWall = (OutHit.ImpactPoint - Start).Size();
		if (DistanceToWall > 80.f)
		{
			UE_LOG(LogTemp,Warning,TEXT("벽이 너무 멀리있음"));
			return false;
		}
		if (HitActor && HitActor->ActorHasTag(ClimbableTagName))
		{
			Character->CurrentWallNormal = OutHit.ImpactNormal;
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

void UGA_JumpOrClimb::StartClimb(const FHitResult& TargetHit)
{
	//State.Climb 태그 부여
	UE_LOG(LogTemp,Error,TEXT("Climbable 태그가진 액터 발견 -> Climb 시작 "));
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (Character->HasAuthority())
	{
		float CapsuleRadius = 0.f;
		if (Character->GetCapsuleComponent())
		{
			CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		}
		// 클라이밍 시작 시 1. 벽에 찰싹 붙기
		// 달라붙을 벽 위치 계산 : 벽 표면에서 캡슐 반지름만큼 떨어트리기 (좀 더 달라붙게 -1 함)
		FVector SnapLocation = TargetHit.ImpactPoint + (TargetHit.ImpactNormal * (CapsuleRadius - 1.0f));
		SnapLocation.Z = Character->GetActorLocation().Z; //z 값은 유지
		FHitResult SweepHit;
		Character->SetActorLocation(SnapLocation, true, &SweepHit, ETeleportType::TeleportPhysics);
		Character->bIsClimbState = true;
	}
	Character->CurrentWallNormal = TargetHit.ImpactNormal;
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

	Character->GetCharacterMovement()->MaxFlySpeed = 200.0f;
	Character->GetCharacterMovement()->BrakingDecelerationFlying = 2000.0f;
	Character->GetCharacterMovement()->StopMovementImmediately();
	// 벽 상태 감시 타이머 시작 (0.05마다 )
	GetWorld()->GetTimerManager().SetTimer(ClimbableCheckTimerHandle, this, &UGA_JumpOrClimb::CheckClimbingState, 0.05f, true);
}

void UGA_JumpOrClimb::CheckClimbingState()
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	const FVector Start = Character->GetActorLocation() - FVector(0, 0, 40.f); // 무릎 쪽으로 내리기
	const FVector Forward = Character->GetActorForwardVector(); 
	const FVector End = Start + ( Forward * TraceDistance );
	
	FHitResult Hit;
	TArray<AActor*> Ignored;
	Ignored.Add(Character);
	
	//배에서 쏘는 트레이스 (기존 벽 감지)
	bool bBodyHit = UKismetSystemLibrary::SphereTraceSingle(this ,Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::None, Hit,true);
	if (!bBodyHit) //배에서 탐지 -> 벽이 없다? 그러면 허공이니까 낙하
	{
		UE_LOG(LogTemp,Warning, TEXT("벽이 끝남 -> 낙하 "));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
		return;
	}
	
	FRotator ControlRot = Character->GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	const FVector CharacterForward = ControlRot.Vector();
	const float FacingDot = FVector::DotProduct(CharacterForward, -Hit.ImpactNormal);
	if (FacingDot <0.7f) //숫자 내릴수록 판정 널널
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
		return;
	}
	
	// 눈높이에서 쏘는 트레이스 (난간? 감지) 
	const FVector EyeStart = Character->GetActorLocation() + FVector(0, 0, EyeHeight);
	const FVector EyeEnd = EyeStart + (Forward * TraceDistance);
	FHitResult EyeHit;
	bool bEyeHit = UKismetSystemLibrary::SphereTraceSingle(this ,EyeStart, EyeEnd, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::None, EyeHit,true);
	
	// 배는 닿았는데 눈은 안닿았다? -> 난간 끝에 도달했구나!
	
	if (!bEyeHit)
	{
		GetWorld()->GetTimerManager().ClearTimer(ClimbableCheckTimerHandle); //기존 벽 체크 타이머 중단
		Character->GetCharacterMovement()->StopMovementImmediately();
		Character->GetCharacterMovement()->MaxFlySpeed = 0.0f;
		
		float AnimMontageDuration = 0.0f;
		if (MantleMontage)
		{
			AnimMontageDuration = Character->PlayAnimMontage(MantleMontage);
		}
		if (AnimMontageDuration <= 0.0f)
		{
			AnimMontageDuration = 1.0f;
		}
		GetWorld()->GetTimerManager().SetTimer(MantleFinishTimerHandle,[this]()
			{
			ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
			if (!Character) return;
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			//위로, 안쪽으로 살짝 위치 조정
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
			}, AnimMontageDuration, false );
		return;
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
