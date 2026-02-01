// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GA/Resource/GAT_TimeRemaining.h"

UGAT_TimeRemaining::UGAT_TimeRemaining()
{
	bTickingTask = true;
}

void UGAT_TimeRemaining::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	TimePassed += DeltaTime;
	float Progress = TimePassed / Duration;
	
	OnProgress.Broadcast(Progress);

	if (TimePassed >= Duration)
	{
		CallOnEndTask.Broadcast();
		EndTask();
	}
}

UGAT_TimeRemaining* UGAT_TimeRemaining::StartTimeRemaining(UGameplayAbility* OwningAbility, float InDuration)
{
	UGAT_TimeRemaining* MyObj = NewAbilityTask<UGAT_TimeRemaining>(OwningAbility);
	MyObj->Duration = InDuration;
	return MyObj;
}
