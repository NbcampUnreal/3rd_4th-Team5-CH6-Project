// Copyright 2023 X-Games. All Rights Reserved.


#include "AsyncInitializePool.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ActorPool.h"


UAsyncInitializePool* UAsyncInitializePool::AsyncInitializePool(UObject* WorldContextObject, AActorPool* InActorPool, TSubclassOf<APoolableActorBase> InActorClass, int32 InPoolSize, EPoolStrategy InStrategy, int32 InBatchSize, float InBatchInterval)
{
	UAsyncInitializePool* Node = NewObject<UAsyncInitializePool>();
	Node->WorldContextObject = WorldContextObject;
	Node->ActorPool = InActorPool;
	Node->ActorClass = InActorClass;
	Node->PoolSize = InPoolSize;
	Node->Strategy = InStrategy;
	Node->BatchSize = InBatchSize;
	Node->BatchInterval = InBatchInterval;
	return Node;
}

void UAsyncInitializePool::Activate()
{
	if (!ActorPool)
	{
		UE_LOG(LogTemp, Error, TEXT("ActorPool is null."));
		OnComplete.Broadcast();
		return;
	}

	ActorPool->StartInitializePoolAsync(ActorClass, PoolSize, Strategy, BatchSize, BatchInterval);

	if (WorldContextObject)
	{
		WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAsyncInitializePool::OnPoolInitialized);
	}
}

void UAsyncInitializePool::OnPoolInitialized()
{
	if (!ActorPool)
	{
		OnComplete.Broadcast();
		return;
	}

	if (ActorPool->IsInitializationComplete())
	{
		OnComplete.Broadcast();
	}
	else
	{
		if (WorldContextObject)
		{
			WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAsyncInitializePool::OnPoolInitialized);
		}
	}
}