#include "GAS/GA/GA_Emote.h"
#include "Character/TSCharacter.h"
#include "Components/AudioComponent.h"
#include "EmoteSystem/TSEmoteTypes.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "Kismet/GameplayStatics.h"

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
		EmoteSound = nullptr;
		break;
		
	case ETSEmoteType::DANCE1:
		FirstMontage = Dance1Montage;
		EmoteSound = Dance1Sound;
		break;
		
	case ETSEmoteType::DANCE2:
		FirstMontage = Dance2Montage;
		EmoteSound = Dance2Sound;
		break;
		
	case ETSEmoteType::DANCE3:
		FirstMontage = Dance3Montage;
		EmoteSound = Dance3Sound;
		break;
	}
	if (FirstMontage)
	{
		PlayMontage(); // 애니메이션 몽타주 재생
		
		if (EmoteSound) // 각 모션 별 사운드 재생
		{
			ActiveEmoteSound = UGameplayStatics::SpawnSoundAttached(
				EmoteSound,
				GetAvatarActorFromActorInfo()->GetRootComponent()
			);
		}
	} else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
}

void UGA_Emote::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 어빌리티가 종료되거나 취소될 때 사운드 중지
	if (ActiveEmoteSound && ActiveEmoteSound->IsPlaying())
	{
		ActiveEmoteSound->Stop();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}