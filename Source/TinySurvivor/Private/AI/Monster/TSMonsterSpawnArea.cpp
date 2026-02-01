// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Monster/TSMonsterSpawnArea.h"
#include "AI/Monster/TSMonsterSpawnSystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

ATSMonsterSpawnArea::ATSMonsterSpawnArea()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(RootComp);
	
	SpawnArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpawnArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
}

void ATSMonsterSpawnArea::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnArea->OnComponentBeginOverlap.AddDynamic(this, &ATSMonsterSpawnArea::DoSpawnWhenDetectedPlayer);
	}
}

void ATSMonsterSpawnArea::DoSpawnWhenDetectedPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!PlayerPawn) return;
	if (!PlayerPawn->IsPlayerControlled()) return;
	
	const FVector BoxOrigin = SpawnArea->GetComponentLocation();
	const FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	
	float InnerMargin = 100.f;  
	FVector AdjustedExtent = BoxExtent - FVector(InnerMargin);
	float MinDistanceFromPlayer = 300.f;
	FVector FinalSpawnLocation = BoxOrigin;
	int32 MaxTry = 10;
	int32 TryCount = 0;

	while (TryCount < MaxTry)
	{
		FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(BoxOrigin, AdjustedExtent);
		float DistToPlayer = FVector::Dist(RandomPoint, PlayerPawn->GetActorLocation());

		if (DistToPlayer >= MinDistanceFromPlayer)
		{
			FinalSpawnLocation = RandomPoint;
			break;
		}
		TryCount++;
	}
	
	if (TryCount >= MaxTry)
	{
		FinalSpawnLocation = BoxOrigin;
	}
	
	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.SetLocation(FinalSpawnLocation);

	UTSMonsterSpawnSystem* SpawnSystem = UTSMonsterSpawnSystem::Get(GetWorld());
	if (!IsValid(SpawnSystem)) return;
	SpawnSystem->RequestMonsterSpawn(SpawnTransform, SpawnMonsterTag, OtherActor);
}


