// STT_ActivateGASAbility.cpp

#include "AI/Common/Task/STT_ActivateGASAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_ActivateGASAbility::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	
	if (!OwnerActor || !Data.AbilityTag.IsValid())
	{
		return EStateTreeRunStatus::Failed;
	}
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!ASC)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(Data.AbilityTag)))
	{
		return EStateTreeRunStatus::Succeeded;
	}
	
	return EStateTreeRunStatus::Failed;
}
