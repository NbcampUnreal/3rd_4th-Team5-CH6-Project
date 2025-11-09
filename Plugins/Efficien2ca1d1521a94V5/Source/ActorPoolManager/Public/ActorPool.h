// Copyright 2023 X-Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorPool.generated.h"

class APoolableActorBase;
/**
 * Enum that defines strategies for actor pooling.
 */
UENUM(BlueprintType)
enum class EPoolStrategy : uint8
{
	/** Always create a new actor if none are available in the pool. */
	CreateNew,
	/** Recycle the first available actor from the pool. */
	RecycleFirst
};


UCLASS(BlueprintType)
class ACTORPOOLMANAGER_API AActorPool : public AActor
{
	GENERATED_BODY()
	

public:
	// Sets default values for this actor's properties
	AActorPool();

	/**
	  * Initializes the actor pool with the specified parameters.
	  *
	  * @param InActorClass    The class of actors to be pooled.
	  * @param InPoolSize      The total number of actors to create in the pool.
	  * @param InStrategy     The number of actors to create in each batch.
	  */
	UFUNCTION(BlueprintCallable, Category = "Actor Pool")
	void InitializePool(TSubclassOf<APoolableActorBase> InActorClass, int32 InPoolSize, EPoolStrategy InStrategy);

	/**
	 * Asynchronously initializes the actor pool with the specified parameters.
	 *
	 * @param InActorClass    The class of actors to be pooled.
	 * @param InPoolSize      The total number of actors to create in the pool.
	 * @param InStrategy     The number of actors to create in each batch.
	 * @param InBatchSize     The number of actors to create in each batch.
	 * @param InBatchInterval The interval (in seconds) between consecutive batches.
	 */
	void StartInitializePoolAsync(TSubclassOf<APoolableActorBase> InActorClass, int32 InPoolSize, EPoolStrategy InStrategy, int32 InBatchSize, float InBatchInterval);

	/**
	* Acquires an actor from the pool.
	*
	* @return A pointer to the acquired actor, or nullptr if no actor is available.
	*/
	UFUNCTION(BlueprintCallable, Category = "Actor Pool")
	APoolableActorBase* AcquireActor(const int32 IntParam = -1, const FString StringParam = TEXT(""), const UObject* ObjectParam = nullptr);

	/**
    * Releases an actor back to the pool.
    *
    * @param InActor The actor to be released.
    */
	UFUNCTION(BlueprintCallable, Category = "Actor Pool")
	void ReleaseActor(APoolableActorBase* InActor);

	/**
	* Checks if the pool initialization is complete.
	*
	* @return True if the initialization is complete, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Actor Pool")
	bool IsInitializationComplete() const;

	/**
	* Gets the current size of the actor pool.
	*
	* @return The number of actors in the pool.
	*/
	UFUNCTION(BlueprintCallable, Category = "Actor Pool")
	int32 GetPoolSize() const;

	/**
    * Checks if the given actor is from the pool.
    *
    * @param Actor The actor to check.
    * @return True if the actor is from the pool, false otherwise.
    */
	UFUNCTION(BlueprintCallable, Category = "Actor Pool")
	bool IsActorFromPool(APoolableActorBase* Actor) const;

	UPROPERTY(Replicated)
	TArray<APoolableActorBase*> AllSpawnedActors;

private:
	int32 CurrentBatchSize;
	int32 ActorsCreated;
	bool bIsInitializing = false;
	bool bInitializationComplete = false;
	float BatchInterval = 0.1f;

	FTimerHandle TimerHandle;

	void InitializePoolBatch();

	UPROPERTY(Replicated)
	TArray<APoolableActorBase*> ActivatedActors;

	UPROPERTY()
	TSubclassOf<APoolableActorBase> ActorClass;

	UPROPERTY()
	int32 PoolSize = 0;

	UPROPERTY()
	EPoolStrategy Strategy = EPoolStrategy::CreateNew;

	APoolableActorBase* CreateNewActor();
};
