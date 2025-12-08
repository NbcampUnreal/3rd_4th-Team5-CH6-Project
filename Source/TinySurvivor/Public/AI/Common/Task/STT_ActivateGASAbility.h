// STT_ActivateGASAbility.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "STT_ActivateGASAbility.generated.h"

USTRUCT()
struct FSTT_ActivateGASAbilityInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FGameplayTag AbilityTag;
};

USTRUCT(meta = (DisplayName = "Activate GAS Ability", Category = "GAS"))
struct TINYSURVIVOR_API FSTT_ActivateGASAbility : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FSTT_ActivateGASAbilityInstanceData;
	
	FSTT_ActivateGASAbility() = default;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
