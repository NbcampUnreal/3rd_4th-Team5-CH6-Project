// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MonsterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMonsterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API IMonsterInterface
{
	GENERATED_BODY()

public:
	
	virtual void SetSpawningPoint(FVector SpawnPoint) = 0;
	virtual FVector GetSpawningPoint() = 0;
	virtual void StartLogic() = 0;
};
