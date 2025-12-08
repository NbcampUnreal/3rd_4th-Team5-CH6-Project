// TSAICharacter.cpp

#include "AI/Common/TSAICharacter.h"
#include "AI/Common/Spawn/MonsterSpawnSubsystem.h"
#include "AI/Common/TSAIController.h"
#include "Components/CapsuleComponent.h"
#include "System/Erosion/TSErosionSubSystem.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"

ATSAICharacter::ATSAICharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	ASC->AddLooseGameplayTag(MonsterTags::TAG_Character_Type_Monster);
	
	AttributeSet = CreateDefaultSubobject<UMonsterAttributeSet>(TEXT("AttributeSet"));
	
	AIControllerClass = ATSAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* ATSAICharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void ATSAICharacter::OnDeath(AActor* Killer)
{
	if (!HasAuthority())
		return;
	
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	
	// 처치 시 침식도 감소
	if (UTSErosionSubSystem* ErosionSys = UTSErosionSubSystem::GetErosionSubSystem(this))
	{
		float ErosionReward = AttributeSet->GetErosionReward();
		
		ErosionSys->AddOrSubtractErosion(-ErosionReward);
	}
	
	// State Tree 정지
	if (ATSAIController* AICon = Cast<ATSAIController>(GetController()))
	{
		if (UStateTreeAIComponent* STComp = AICon->GetStateTreeComponent())
		{
			STComp->StopLogic("Dead");
		}
	}
	
	// 스폰 매니저에게 반납 요청
	if (UWorld* World = GetWorld())
	{
		if (auto* SpawnSys = World->GetSubsystem<UMonsterSpawnSubsystem>())
		{
			// 사망 모션 등을 위해 3초 뒤 반납
			FTimerHandle TimerHandle;
			
			World->GetTimerManager().SetTimer(TimerHandle, [this, SpawnSys]()
			{
				SpawnSys->ReturnMonsterInPool(this);
			}, 3.0f, false);
		}
	}
	
	// 콜리전 끄기
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATSAICharacter::OnDamaged(float DamageAmount, const FGameplayTagContainer& DamageTags, AActor* Attacker)
{
	if (AttributeSet->GetHealth() <= 0.0f)
		return;
	
	// 방향에 따른 몽타주 선택
	UAnimMontage* SelectedMontage = GetHitMontageByDirection(Attacker);
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	
	if (!AnimInst || !SelectedMontage)
		return;
	
	UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
	
	if (CurrentMontage)
	{
		// 이미 피격 모션 중이라면 무시
		if (CurrentMontage == HitReactMontageFront ||
			CurrentMontage == HitReactMontageBack ||
			CurrentMontage == HitReactMontageLeft ||
			CurrentMontage == HitReactMontageRight)
		{
			return;
		}
	}
	
	// 몽타주 실행
	PlayAnimMontage(SelectedMontage);
	
	if (ATSAIController* AICon = Cast<ATSAIController>(GetController()))
	{
		if (UStateTreeAIComponent* STComp = AICon->GetStateTreeComponent())
		{
			STComp->SendStateTreeEvent(FStateTreeEvent(MonsterTags::TAG_State_Hit));
		}
	}
}

void ATSAICharacter::ResetMonster()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		AnimInst->Montage_Stop(0.0f);
	}
	
	if (AttributeSet)
	{
		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	}
	
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(MonsterTags::TAG_State_Dead);
	}
	
	if (ATSAIController* AIC = Cast<ATSAIController>(GetController()))
	{
		if (UAIPerceptionComponent* Perception = AIC->GetPerceptionComponent())
		{
			Perception->ForgetAll();
		}
		
		if (UStateTreeAIComponent* STComp = AIC->GetStateTreeComponent())
		{
			STComp->StartLogic();
		}
	}
}

UAnimMontage* ATSAICharacter::GetHitMontageByDirection(AActor* Attacker)
{
	if (!Attacker)
		return HitReactMontageFront;
	
	FVector MyLoc = GetActorLocation();
	FVector TargetLoc = Attacker->GetActorLocation();
	FVector Direction = (TargetLoc - MyLoc).GetSafeNormal();
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
	FRotator MyRot = GetActorRotation();
	
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRot, MyRot);
	
	float Yaw = DeltaRot.Yaw;
	
	if (Yaw >= -45.0f && Yaw <= 45.0f)
		return HitReactMontageFront;
	else if (Yaw > 45.0f && Yaw <= 135.0f)
		return HitReactMontageRight;
	else if (Yaw >= -135.0f && Yaw < -45.0f)
		return HitReactMontageLeft;
	else
		return HitReactMontageBack;
}
