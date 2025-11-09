// Copyright 2023 X-Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolableActorBase.generated.h"

class AActorPool;

UCLASS(Blueprintable)
class ACTORPOOLMANAGER_API APoolableActorBase : public AActor
{
	GENERATED_BODY()
	

public:
	APoolableActorBase();

/**  Checks if the actor is currently active. If true, the actor is currently in use. If false, the actor is idle.*/
	UFUNCTION(BlueprintCallable, Category = "Poolable Actor")
	virtual bool IsActive();

	UFUNCTION(BlueprintCallable, Category = "Poolable Actor")
	AActorPool* GetPoolOwner() const;
	
	/** Initialize the actor to its default state when added to the pool */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable Actor")
	void OnInitialize();

	/** Configure the actor when it is acquired from the pool */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable Actor")
	void OnAcquire(const int32& IntParam, const FString& StringParam, const UObject* ObjectParam);

	/** Configure the actor when it is released back to the pool */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Poolable Actor")
	void OnRelease();

	void SetPoolOwner(AActorPool* InPoolOwner);

protected:

	// Initialization configuration

	/** Whether tick is enabled during initialization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Initialization")
	bool bInitializeTickEnabled;

	/** Whether actor is hidden in game during initialization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Initialization")
	bool bInitializeHiddenInGame;

	/** Whether collision is enabled during initialization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Initialization")
	bool bInitializeEnableCollision;

	/** Net dormancy state during initialization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Initialization")
	TEnumAsByte<ENetDormancy> InitializeNetDormancy;

	// Acquire configuration

	/** Whether tick is enabled when the actor is acquired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Acquire")
	bool bAcquireTickEnabled;

	/** Whether actor is hidden in game when acquired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Acquire")
	bool bAcquireHiddenInGame;

	/** Whether collision is enabled when the actor is acquired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Acquire")
	bool bAcquireEnableCollision;

	/** Net dormancy state when the actor is acquired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Acquire")
	TEnumAsByte<ENetDormancy> AcquireNetDormancy;

	// Release configuration
	/** Whether tick is enabled when the actor is released */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Release")
	bool bReleaseTickEnabled;

	/** Whether actor is hidden in game when released */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Release")
	bool bReleaseHiddenInGame;

	/** Whether collision is enabled when the actor is released */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Release")
	bool bReleaseEnableCollision;

	/** Net dormancy state when the actor is released */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Poolable Actor|Release")
	TEnumAsByte<ENetDormancy> ReleaseNetDormancy;
	
private:
	bool bIsActive = false;

	UPROPERTY()
	TWeakObjectPtr<AActorPool> PoolOwner;
	// TSharedPtr<AActorPool> PoolOwner;
};
