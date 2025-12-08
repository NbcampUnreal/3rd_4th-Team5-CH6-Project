// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GAT_TimeRemaining.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMineProgress, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCallOnEndTask);

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UGAT_TimeRemaining : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UGAT_TimeRemaining();
	virtual void TickTask(float DeltaTime) override;
	static UGAT_TimeRemaining* StartTimeRemaining(UGameplayAbility* OwningAbility, float InDuration);

	FOnMineProgress OnProgress;

	FOnCallOnEndTask CallOnEndTask;
	
	float Duration;
	float TimePassed; 
};
