// TSAICharacter.cpp

#include "AI/Common/TSAICharacter.h"
#include "AI/Common/MonsterSpawnSubsystem.h"
#include "AI/Common/TSAIController.h"
#include "Components/CapsuleComponent.h"
#include "System/Erosion/TSErosionSubSystem.h"

ATSAICharacter::ATSAICharacter()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UMonsterAttributeSet>("AttributeSet");
	
	AIControllerClass = ATSAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
}

UAbilitySystemComponent* ATSAICharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void ATSAICharacter::OnDeath(AActor* Killer)
{
	if (!HasAuthority())
		return;
	
	// 처치 시 침식도 감소
	if (UTSErosionSubSystem* ErosionSys = UTSErosionSubSystem::GetErosionSubSystem(this))
	{
		float ErosionReward = AttributeSet->GetErosionReward();
		
		ErosionSys->AddOrSubtractErosion(-ErosionReward);
	}
	
	// State Tree 정지
	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic("Dead");
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

void ATSAICharacter::ResetMonster()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	if (AttributeSet)
	{
		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	}
	
	if (StateTreeComponent)
	{
		StateTreeComponent->StartLogic();
	}
	
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UAIPerceptionComponent* Perception = AIC->GetPerceptionComponent())
		{
			Perception->ForgetAll();
		}
	}
}

void ATSAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
		StateTreeComponent->StartLogic();
}
