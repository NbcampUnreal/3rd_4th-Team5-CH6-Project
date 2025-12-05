// STT_ActivateAbilityByTag.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "STT_ActivateAbilityByTag.generated.h"

USTRUCT()
struct FSTT_ActivateAbilityByTagInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag AbilityTag;
};

// State Tree에서 GAS 어빌리티를 태그로 실행하는 태스크 
USTRUCT(meta = (DisplayName = "Activate GAS Ability"))
struct TINYSURVIVOR_API FSTT_ActivateAbilityByTag : public FStateTreeTaskBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FSTT_ActivateAbilityByTagInstanceData;
	
	FSTT_ActivateAbilityByTag() = default;
	
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
