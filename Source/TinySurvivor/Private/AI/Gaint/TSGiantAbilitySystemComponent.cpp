#include "AI/Gaint/TSGiantAbilitySystemComponent.h"

UTSGiantAbilitySystemComponent::UTSGiantAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTSGiantAbilitySystemComponent::SetGiantAbilityList()
{
	for (auto& GiantAbilityClass : GiantAbilityClassList)
	{
		K2_GiveAbility(GiantAbilityClass);
	}
}
