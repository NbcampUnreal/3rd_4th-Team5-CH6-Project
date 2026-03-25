// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TSGA_TEST_ReceiveGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_TEST_ReceiveGameplayEvent : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UTSGA_TEST_ReceiveGameplayEvent();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
};
