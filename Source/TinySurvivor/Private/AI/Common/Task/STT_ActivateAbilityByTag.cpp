// STT_ActivateAbilityByTag.cpp

#include "AI/Chaser/Task/STT_ActivateAbilityByTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_ActivateAbilityByTag::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// Instance Data 접근
	const FInstanceDataType& Data = Context.GetInstanceData<FInstanceDataType>(*this);

	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	
	if (!OwnerActor)
	{
		return EStateTreeRunStatus::Failed;	
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);

	if (ASC && Data.AbilityTag.IsValid())
	{
		if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(Data.AbilityTag)))
		{
			return EStateTreeRunStatus::Running;
		}
	}

	return EStateTreeRunStatus::Failed;
}
