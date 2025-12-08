// TSAISpawner.cpp

#include "AI/Common/Spawn/TSAISpawner.h"
#include "AI/Common/Spawn/MonsterSpawnSubsystem.h"
#include "AI/Common/Spawn/AMonsterRegionVolume.h"
#include "Kismet/KismetMathLibrary.h"

ATSAISpawner::ATSAISpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
	NetUpdateFrequency = 1.0f;
	
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = SpawnVolume;
	
	// 스포너 자체 충돌은 끄되, 볼륨 갑지를 위해 Query는 킴
	SpawnVolume->SetSimulatePhysics(false);
	SpawnVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ATSAISpawner::BeginPlay()
{
	// [자동 감지 로직] 내 위치에 RegionVolume이 있는지 검사
	TArray<AActor*> OverlappingActors;
	SpawnVolume->GetOverlappingActors(OverlappingActors, AAMonsterRegionVolume::StaticClass());
	
	for (AActor* Actor : OverlappingActors)
	{
		if (AAMonsterRegionVolume* RegionVolume = Cast<AAMonsterRegionVolume>(Actor))
		{
			// 봄륨을 찾으면 그 키를 내 키로 덮어씌움
			this->RegionKey = RegionVolume->Regionkey;
			break;
		}
	}
	
	Super::BeginPlay();
	
	// 서브시스템에 등록
	if (HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			if (auto* SpawnSys = GetWorld()->GetSubsystem<UMonsterSpawnSubsystem>())
			{
				SpawnSys->RegisterSpawner(this);
			}
		}
	}
}

FVector ATSAISpawner::GetRandomPointInVolume() const
{
	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnVolume->GetComponentLocation(), SpawnVolume->GetScaledBoxExtent());
}
