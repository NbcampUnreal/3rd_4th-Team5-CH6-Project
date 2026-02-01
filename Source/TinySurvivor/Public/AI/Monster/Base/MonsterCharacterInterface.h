// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Monster/TSMonsterTable.h"
#include "UObject/Interface.h"
#include "MonsterCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMonsterCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API IMonsterCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetSpeedIncrease() = 0;
	virtual void ResetSpeed() = 0;
	virtual void StopWalk() = 0;
	virtual void RegainSpeed() = 0;
	virtual void SetDropRootItems(FTSMonsterTable& MonsterTable) = 0;
	virtual void RequestSpawnDropRooItems() = 0;
	virtual void MakeTimeToDead() = 0;
};
