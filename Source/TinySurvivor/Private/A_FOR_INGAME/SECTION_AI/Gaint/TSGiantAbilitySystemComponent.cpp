// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_AI/Gaint/TSGiantAbilitySystemComponent.h"

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
