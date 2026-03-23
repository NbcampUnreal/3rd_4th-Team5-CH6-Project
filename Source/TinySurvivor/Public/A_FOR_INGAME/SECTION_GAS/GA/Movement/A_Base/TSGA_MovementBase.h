// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_GAS/A_Base/TSGA_BaseAbility.h"
#include "TSGA_MovementBase.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSGA_MovementBase : public UTSGA_BaseAbility
{
	GENERATED_BODY()

//======================================================================================================================	
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	UTSGA_MovementBase();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;	
	
#pragma endregion	
//======================================================================================================================
#pragma region 속도

	//━━━━━━━━━━━━━━━━━━━━
	// 속도
	//━━━━━━━━━━━━━━━━━━━━	
protected:
	
	// 어빌리티 비활성화 시 바꿀 속도 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Movement")
	float WantChangeMoveSpeedWhenDeactivate = 500.0f;
	
	// 어빌리티 종료 시 해당 태그가 있으면 속도를 바꾸지 않음 (버그 방지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TS | Movement")
	FGameplayTagContainer WantChangeMoveSpeedWhenDeactivateTag;
	
#pragma endregion	
//======================================================================================================================
	
};
