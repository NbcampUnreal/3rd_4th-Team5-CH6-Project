// Copyright 2023 X-Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "PoolableActorBase.h"
#include "AsyncInitializePool.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitializationComplete);

UCLASS()
class ACTORPOOLMANAGER_API UAsyncInitializePool : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

		
public:

	/** Delegate that is called when the asynchronous pool initialization is complete. */
	UPROPERTY(BlueprintAssignable)
	FOnInitializationComplete OnComplete;
	
	/**
	 * Asynchronously initializes the actor pool.
	 *
	 * @param InActorClass The class of actors to be pooled.
	 * @param InPoolSize The total number of actors to create in the pool.
	 * @param InStrategy The strategy to use for pooling (e.g., CreateNew, RecycleFirst).��
	 * @param InBatchSize The number of actors to create in each batch.
	 * @param InBatchInterval The interval (in seconds) between consecutive batches.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Actor Pool")
	static UAsyncInitializePool* AsyncInitializePool(UObject* WorldContextObject, AActorPool* ActorPool, TSubclassOf<APoolableActorBase> InActorClass, int32 InPoolSize, EPoolStrategy InStrategy, int32 InBatchSize, float InBatchInterval);

	virtual void Activate() override;

private:
	UPROPERTY()
	UObject* WorldContextObject;

	UPROPERTY()
	AActorPool* ActorPool;

	TSubclassOf<APoolableActorBase> ActorClass;
	int32 PoolSize;
	EPoolStrategy Strategy;
	int32 BatchSize;
	float BatchInterval;

	void OnPoolInitialized();
};
