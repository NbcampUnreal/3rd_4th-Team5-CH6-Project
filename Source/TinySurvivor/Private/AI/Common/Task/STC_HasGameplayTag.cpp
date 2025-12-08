// STC_HasGameplayTag.cpp

#include "AI/Chaser/Task/STC_HasGameplayTag.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

bool FSTC_HasGameplayTag::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& Data = Context.GetInstanceData<FInstanceDataType>(*this);
    
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
		return false;
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!ASC || !Data.TagToCheck.IsValid()) 
		return false;

	bool bHasTag = ASC->HasMatchingGameplayTag(Data.TagToCheck);
	return Data.bInvert ? !bHasTag : bHasTag;
}