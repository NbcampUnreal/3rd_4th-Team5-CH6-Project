#include "GAS/GA/GA_Emote.h"
#include "Character/TSCharacter.h"
#include "EmoteSystem/TSEmoteTypes.h"
#include "GameplayTags/AbilityGameplayTags.h"

UGA_Emote::UGA_Emote()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Emote::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!TriggerEventData)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	float PayloadValue = TriggerEventData->EventMagnitude;
	ETSEmoteType EmoteMontageType = static_cast<ETSEmoteType>((int32)PayloadValue);
	switch (EmoteMontageType)
	{
	case ETSEmoteType::NONE:
		FirstMontage = nullptr;
		break;
		
	case ETSEmoteType::HELLO:
		FirstMontage = HelloMontage;
		break;
		
	case ETSEmoteType::DANCE1:
		FirstMontage = Dance1Montage;
		break;
		
	case ETSEmoteType::DANCE2:
		FirstMontage = Dance2Montage;
		break;
		
	case ETSEmoteType::DANCE3:
		FirstMontage = Dance3Montage;
		break;
	}
	if (FirstMontage)
	{
		PlayMontage();
	} else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
}
