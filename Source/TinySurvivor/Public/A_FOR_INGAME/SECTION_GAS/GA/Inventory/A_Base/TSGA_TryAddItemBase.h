// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseAbility.h"
#include "TSGA_TryAddItemBase.generated.h"

/**
 * try add 로 아이템을 먹는 GA들 공통 로직 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_TryAddItemBase : public UTSGA_BaseAbility
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	UTSGA_TryAddItemBase();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
#pragma endregion
//======================================================================================================================	
#pragma region 내부 동작 API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 몽타주
	//━━━━━━━━━━━━━━━━━━━━
protected:
	
	// 유효성 체크 
	bool CheckValidAbilityAndTarget(const FGameplayEventData* InTriggerEventData);
	
#pragma endregion
//======================================================================================================================	
#pragma region 몽타주
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 몽타주
	//━━━━━━━━━━━━━━━━━━━━
protected:
	
	// 몽타주 
	
	// 가상 함수 (자식들이 정의)
	
#pragma endregion
//======================================================================================================================	
#pragma region 내부_동작_데이터 
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 내부 동작 데이터
	//━━━━━━━━━━━━━━━━━━━━
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TS | Target")
	TObjectPtr<AActor> TargetActor = nullptr;
	
#pragma endregion
//======================================================================================================================	
};
