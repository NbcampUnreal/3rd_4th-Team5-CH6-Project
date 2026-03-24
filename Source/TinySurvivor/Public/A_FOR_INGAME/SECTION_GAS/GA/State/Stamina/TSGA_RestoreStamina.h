// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_GAS/GA/State/A_Base/TSGA_RestoreAttributeBase.h"
#include "TSGA_RestoreStamina.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_RestoreStamina : public UTSGA_RestoreAttributeBase
{
	GENERATED_BODY()
	
public:
	UTSGA_RestoreStamina();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
