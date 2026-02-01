// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"
#include "UObject/Interface.h"
#include "MonsterAICInterface.generated.h"

class UStateTreeAIComponent;

// This class does not need to be modified.
UINTERFACE()
class UMonsterAICInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API IMonsterAICInterface
{
	GENERATED_BODY()

public:
	
	virtual void SetSpawningPoint(FVector SpawnPoint) = 0;
	virtual FVector GetSpawningPoint() = 0;
	virtual void StartLogic() = 0;
	virtual void SetInstigator(AActor* InInstigator) = 0;
	virtual AActor* GetCauseInstigator() = 0;
	virtual UStateTreeAIComponent* GetStateTreeAIComponent() = 0;
	
};
