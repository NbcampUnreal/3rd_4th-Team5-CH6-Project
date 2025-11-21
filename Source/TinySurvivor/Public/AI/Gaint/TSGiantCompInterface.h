// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TSGiantCompInterface.generated.h"

class UStateTreeAIComponent;
// This class does not need to be modified.
UINTERFACE()
class UTSGiantCompInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TINYSURVIVOR_API ITSGiantCompInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent)
	UStateTreeAIComponent* GetStateTreeAIComponent();
	
};
