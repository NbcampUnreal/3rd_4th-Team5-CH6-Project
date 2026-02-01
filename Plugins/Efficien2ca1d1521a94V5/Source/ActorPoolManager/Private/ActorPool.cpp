// Copyright 2023 X-Games. All Rights Reserved.


#include "ActorPool.h"
#include "Engine/World.h"
#include "PoolableActorBase.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"


AActorPool::AActorPool()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AActorPool::InitializePool(const TSubclassOf<APoolableActorBase> InActorClass, const int32 InPoolSize, const EPoolStrategy InStrategy)
{
	ActorClass = InActorClass;
	PoolSize = InPoolSize;
	Strategy = InStrategy;

	if (GetWorld())
	{
		for (int32 i = 0; i < PoolSize; ++i)
		{
			CreateNewActor(); 
		}
	}
}

void AActorPool::StartInitializePoolAsync(const TSubclassOf<APoolableActorBase> InActorClass, const int32 InPoolSize, const EPoolStrategy InStrategy, const int32 InBatchSize, const float InBatchInterval)
{
	if (!InActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("The provided Actor class is null."));
		return;
	}
	
	if (InActorClass == APoolableActorBase::StaticClass())
	{
		UE_LOG(LogTemp, Error, TEXT("The provided Actor class must be a subclass of APoolableActorBase."));
		return;
	}
	
	ActorClass = InActorClass;
	PoolSize = InPoolSize;
	Strategy = InStrategy;
	CurrentBatchSize = InBatchSize;
	BatchInterval = InBatchInterval;
	bIsInitializing = true;
	bInitializationComplete = false;
	ActorsCreated = 0;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AActorPool::InitializePoolBatch, InBatchInterval, true);
	}
}

bool AActorPool::IsInitializationComplete() const
{
	return bInitializationComplete;
}

int32 AActorPool::GetPoolSize() const
{
	return  ActivatedActors.Num();
}

bool AActorPool::IsActorFromPool(APoolableActorBase* Actor) const
{
	return AllSpawnedActors.Contains(Actor);
}

void AActorPool::InitializePoolBatch()
{
	const int32 BatchCount = FMath::Min(CurrentBatchSize, PoolSize - ActorsCreated);
	for (int32 i = 0; i < BatchCount; ++i)
	{
		if (APoolableActorBase* NewActor = CreateNewActor())
		{
			++ActorsCreated;
		}
	}

	if (ActorsCreated >= PoolSize)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}
		bIsInitializing = false;
		bInitializationComplete = true;
	}
}


APoolableActorBase* AActorPool::AcquireActor(const int32 IntParam, const FString StringParam, const UObject* ObjectParam)
{
	if (ActivatedActors.Num() > 0)
	{
		APoolableActorBase* Actor = ActivatedActors[0];
		ActivatedActors.RemoveAt(0);
		Actor->OnAcquire(IntParam, StringParam, ObjectParam);
		return Actor;
	}
	else
	{
		if (Strategy == EPoolStrategy::CreateNew)
		{
			APoolableActorBase* NewActor = CreateNewActor();
			NewActor->OnAcquire(IntParam, StringParam, ObjectParam);
			return NewActor;
		}
		else if (Strategy == EPoolStrategy::RecycleFirst)
		{
			if (AllSpawnedActors.Num() > 0)
			{
				APoolableActorBase* Actor = AllSpawnedActors[0];
				AllSpawnedActors.RemoveAt(0);
				AllSpawnedActors.Add(Actor);
				Actor->OnAcquire(IntParam, StringParam, ObjectParam);
				return Actor;
			}
		}
	}
	return nullptr;
}

void AActorPool::ReleaseActor(APoolableActorBase* InActor)
{
	if (!InActor)
	{
		UE_LOG(LogTemp, Error, TEXT("ReleaseActor called with a null Actor."));
		return;
	}

	if (!AllSpawnedActors.Contains(InActor))
	{
		UE_LOG(LogTemp, Error, TEXT("The provided Actor is not part of the pool's spawned actors."));
		return;
	}
	
	ActivatedActors.Add(InActor);
	InActor->OnRelease();
}

APoolableActorBase* AActorPool::CreateNewActor()
{
	if (!GetWorld() || !ActorClass)
	{
		return nullptr;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APoolableActorBase* NewActor = GetWorld()->SpawnActor<APoolableActorBase>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (NewActor)
	{
		AllSpawnedActors.Add(NewActor);
		ActivatedActors.Add(NewActor);
		NewActor->SetPoolOwner(this);
		NewActor->OnInitialize();
	}

	return NewActor;
}

void AActorPool::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AActorPool, ActivatedActors);
	DOREPLIFETIME(AActorPool, AllSpawnedActors);
}
