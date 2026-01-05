#include "GAS/GA/GA_Emote.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/TSCharacter.h"
#include "Components/AudioComponent.h"
#include "EmoteSystem/TSEmoteTypes.h"
#include "Kismet/GameplayStatics.h"

UGA_Emote::UGA_Emote()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
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
		// PlayMontage(); // 애니메이션 몽타주 재생
		
		if (EmoteSound&& IsLocallyControlled()) // 각 모션 별 개인에게만 사운드 재생 
		{
			ActiveEmoteSound = UGameplayStatics::SpawnSoundAttached(
				EmoteSound,
				GetAvatarActorFromActorInfo()->GetRootComponent()
			);
		}
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		   this, 
		   NAME_None, 
		   FirstMontage, 
		   1.0f, 
		   NAME_None, 
		   false,
		   1.0f
	   );
		if (MontageTask)
		{
			// 정상 종료, 중단(Interrupted), 취소(Cancelled) 모두 종료 처리로 연결
			MontageTask->OnCompleted.AddDynamic(this, &UGA_Emote::OnMontageEnded);
			MontageTask->OnInterrupted.AddDynamic(this, &UGA_Emote::OnMontageEnded);
			MontageTask->OnCancelled.AddDynamic(this, &UGA_Emote::OnMontageEnded);
           
			MontageTask->ReadyForActivation();
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
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
void UGA_Emote::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}