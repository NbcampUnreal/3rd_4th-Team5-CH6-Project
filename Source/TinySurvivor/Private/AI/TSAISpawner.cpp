// TSAISpawner.cpp

#include "AI/TSAISpawner.h"
#include "AI/Chaser/ChaserCharacter.h"
#include "AI/TSAIController.h"

// Sets default values
ATSAISpawner::ATSAISpawner()
{
	bReplicates = false;
	PrimaryActorTick.bCanEverTick = false;
}

void ATSAISpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		SpawnMonster();
	}
}

void ATSAISpawner::OnMonsterDestroyed(AActor* DestroyedActor)
{
	if (!MonsterClass)
		return;
	
	UWorld* World = GetWorld();
	if (!World)
		return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	// 몬스터 스폰
	AChaserCharacter* NewMonster = World->SpawnActor<AChaserCharacter>(MonsterClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	
	if (NewMonster)
	{
		// 컨트롤러 가져오기
		NewMonster->SpawnDefaultController();
		
		if (ATSAIController* AICon = Cast<ATSAIController>(NewMonster->GetController()))
		{
			// 구역 정보 주입
			AICon->HomeLocation = GetActorLocation();
			AICon->PatrolRadius = PatrolRadius;
		}
		
		// 리스폰 관리를 위해 사망 이벤트 구독
		NewMonster->OnDestroyed.AddDynamic(this, &ATSAISpawner::OnMonsterDestroyed);
	}
}

void ATSAISpawner::SpawnMonster()
{
	// 리스폰 타이머
	if (RespawnTime > 0.0f)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATSAISpawner::SpawnMonster, RespawnTime, false);
	}
}
