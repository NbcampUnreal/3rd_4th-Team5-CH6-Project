// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseAbility.h"
#include "TSGA_CharacterLeftClick.generated.h"

/**
 * 왼쪽 마우스 총괄 어빌리티 
 * 이 어빌리티는 네트워크에 관여하지 않는다. 이 어빌리티는 순수하게 왼쪽 마우스를 클릭한 현재 플레이어의 상태를 파악해서 적절하게 어빌리티를 선택하도록 하는 역할을 맡는다.
 * 상세 설명 : cpp 하단 주석 참조
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_CharacterLeftClick : public UTSGA_BaseAbility
{
	GENERATED_BODY()
	
public:
	UTSGA_CharacterLeftClick();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
};
