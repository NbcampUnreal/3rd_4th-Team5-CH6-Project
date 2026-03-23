// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseAbility.h"
#include "TSGA_TryInteractByNormal.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_TryInteractByNormal : public UTSGA_BaseAbility
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	UTSGA_TryInteractByNormal();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

#pragma endregion
//======================================================================================================================	

};
