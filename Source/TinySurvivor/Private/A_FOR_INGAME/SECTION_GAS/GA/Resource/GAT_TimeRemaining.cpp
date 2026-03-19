// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GA/Resource/GAT_TimeRemaining.h"

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
