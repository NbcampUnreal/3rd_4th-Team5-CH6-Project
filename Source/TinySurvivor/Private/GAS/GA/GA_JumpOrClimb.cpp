#include "GAS/GA/GA_JumpOrClimb.h"

#include "Character/TSCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Components/CapsuleComponent.h"

UGA_JumpOrClimb::UGA_JumpOrClimb()
{
	CostHandle.Invalidate();
	ClimbStateTagHandle.Invalidate();
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_JumpOrClimb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	FHitResult HitResult;
	if ( ClimbableActor(HitResult))
	{
		StartClimb(HitResult);
	} 
	else
	{
		StartJump();
		EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	}
}

void UGA_JumpOrClimb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(ClimbableCheckTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(MantleFinishTimerHandle);
	
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	const bool bWasClimbing = CostHandle.IsValid() || ClimbStateTagHandle.IsValid();
	
	if (bWasClimbing)
	{
		Character->GetCharacterMovement()->MaxFlySpeed = 600.0f;
		Character->GetCharacterMovement()->BrakingDecelerationFlying = 0.0f;
		if (Character->GetCharacterMovement()->MovementMode == MOVE_Flying) 
		{
			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		Character->CurrentWallNormal = FVector::ZeroVector;
		Character->StopJumping();
		if (Character->HasAuthority())
		{
			Character->bIsClimbState = false;
		}
		
		if (ASC)
		{
			if (CostHandle.IsValid())
			{
				ASC->RemoveActiveGameplayEffect(CostHandle);
				CostHandle.Invalidate();
			}
			if (ClimbStateTagHandle.IsValid())
			{
				ASC->RemoveActiveGameplayEffect(ClimbStateTagHandle);
				ClimbStateTagHandle.Invalidate();
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
	}
	
	if (ASC && StaminaDelegateHandle.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute()).Remove(StaminaDelegateHandle);
		StaminaDelegateHandle.Reset();
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
		
		float FacingDot = FVector::DotProduct(Forward, -OutHit.ImpactNormal);
		if (FacingDot < 0.6f )
		{
			return false;
		}
		float DistanceToWall = (OutHit.ImpactPoint - Start).Size();
		if (DistanceToWall > 120.f)
		{
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
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (Character->CanJump())
	{
		Character->Jump();
		if (JumpStateTagEffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(JumpStateTagEffectClass, 1, ContextHandle);
         
			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void UGA_JumpOrClimb::StartClimb(const FHitResult& TargetHit)
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (Character->HasAuthority())
	{
		float CapsuleRadius = 0.f;
		if (Character->GetCapsuleComponent())
		{
			CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
		}
		FVector SnapLocation = TargetHit.ImpactPoint + (TargetHit.ImpactNormal * (CapsuleRadius - 1.0f));
		SnapLocation.Z = Character->GetActorLocation().Z;
		FHitResult SweepHit;
		Character->SetActorLocation(SnapLocation, true, &SweepHit, ETeleportType::TeleportPhysics);
		Character->bIsClimbState = true;
	}
	Character->CurrentWallNormal = TargetHit.ImpactNormal;
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
	if (!ClimbStateTagHandle.IsValid() && ClimbStateTagEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ClimbStateTagEffectClass, GetAbilityLevel(), ContextHandle);

		if (SpecHandle.IsValid())
		{
			ClimbStateTagHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	Character->GetCharacterMovement()->MaxFlySpeed = 200.0f;
	Character->GetCharacterMovement()->BrakingDecelerationFlying = 2000.0f;
	Character->GetCharacterMovement()->StopMovementImmediately();
	GetWorld()->GetTimerManager().SetTimer(ClimbableCheckTimerHandle, this, &UGA_JumpOrClimb::CheckClimbingState, 0.05f, true);
}

void UGA_JumpOrClimb::CheckClimbingState()
{
	ATSCharacter* Character = Cast<ATSCharacter>(GetAvatarActorFromActorInfo());
	const FVector Start = Character->GetActorLocation() - FVector(0, 0, 40.f); 
	const FVector Forward = Character->GetActorForwardVector(); 
	const FVector End = Start + ( Forward * TraceDistance );
	
	FHitResult Hit;
	TArray<AActor*> Ignored;
	Ignored.Add(Character);
	
	bool bBodyHit = UKismetSystemLibrary::SphereTraceSingle(this ,Start, End, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::None, Hit,true);
	if (!bBodyHit)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
		return;
	}
	
	FRotator ControlRot = Character->GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	const FVector CharacterForward = ControlRot.Vector();
	const float FacingDot = FVector::DotProduct(CharacterForward, -Hit.ImpactNormal);
	if (FacingDot <0.7f)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
		return;
	}
	
	const FVector EyeStart = Character->GetActorLocation() + FVector(0, 0, EyeHeight);
	const FVector EyeEnd = EyeStart + (Forward * TraceDistance);
	FHitResult EyeHit;
	bool bEyeHit = UKismetSystemLibrary::SphereTraceSingle(this ,EyeStart, EyeEnd, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, Ignored, EDrawDebugTrace::None, EyeHit,true);
	if (!bEyeHit)
	{
		GetWorld()->GetTimerManager().ClearTimer(ClimbableCheckTimerHandle);
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
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false );
			}, AnimMontageDuration, false );
		return;
	}
	Character->CurrentWallNormal = Hit.ImpactNormal;
}

void UGA_JumpOrClimb::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
