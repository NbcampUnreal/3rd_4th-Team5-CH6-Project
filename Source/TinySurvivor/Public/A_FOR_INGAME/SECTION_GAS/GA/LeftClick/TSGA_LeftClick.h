// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseAbility.h"
#include "TSGA_LeftClick.generated.h"

/**
 * 왼쪽 마우스 총괄 어빌리티 
 * 이 어빌리티는 네트워크에 관여하지 않는다. 이 어빌리티는 순수하게 왼쪽 마우스를 클릭한 현재 플레이어의 상태를 파악해서 적절하게 어빌리티를 선택하도록 하는 역할을 맡는다.
 * = 작동 상세 설명 = 
 * 1. 현재 플레이어가 어떤 상태인지 판단 -> 현재 활성화되어 있는 핫키가 있는가? -> 현재 활성화된 핫키에 아이템이 있는가? -> 현재 왼쪽 마우스 클릭을 하면 실행해야 하는 어빌리티가 있는가?
 * 2. 1번에 따라 만약 실행하는 어빌리티가 있다면 그걸 실행하고 마침. 하지만 만약에 아이템이 없거나 실행하는 어빌리티가 없다면 다음과 같은 프로세스로 넘어감.
 * 3. 현재 보고 있는 타겟이 무엇인가? -> 현재 보고 있는 타겟에 따라 기본으로 설정된 어빌리티 실행.
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_LeftClick : public UTSGA_BaseAbility
{
	GENERATED_BODY()
	
public:
	UTSGA_LeftClick();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
};
