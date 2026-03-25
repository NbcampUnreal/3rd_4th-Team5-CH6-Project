// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseAbility.h"
#include "TSGA_CharacterLeftClick_Default.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_CharacterLeftClick_Default : public UTSGA_BaseAbility
{
	GENERATED_BODY()

public:
	UTSGA_CharacterLeftClick_Default();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
