// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StatDataObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TINYSURVIVOR_API UStatDataObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Stat")
	FText StatName;

	UPROPERTY(BlueprintReadWrite, Category = "Stat")
	FText StatValue;
	
	UPROPERTY(BlueprintReadWrite, Category = "Stat")
	FText StatUnit;
};
