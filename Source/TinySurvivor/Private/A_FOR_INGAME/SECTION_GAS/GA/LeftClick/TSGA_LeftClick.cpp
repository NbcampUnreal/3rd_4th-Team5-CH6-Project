// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GA/LeftClick/TSGA_LeftClick.h"
#include "GameFramework/Character.h"

UTSGA_LeftClick::UTSGA_LeftClick()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UTSGA_LeftClick::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* AbilityOwner = CastChecked<ACharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(AbilityOwner))
	{
		K2_EndAbility();
		return;
	}
	
	// 현재 플레이어의 상태 체크할 목록 
	// 1. 현재 활성화되어 있는 핫키가 있는가?
	
	
	
	
}
