// STC_HasGameplayTag.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "GameplayTagContainer.h"
#include "STC_HasGameplayTag.generated.h"

USTRUCT()
struct FSTC_HasGameplayTagInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Parameter")
	FGameplayTag TagToCheck;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvert = false;
};

// GAS 태그 보유 여부를 확인하는 State Tree 조건
USTRUCT(meta = (DisplayName = "Has Gameplay Tag"))
struct TINYSURVIVOR_API FSTC_HasGameplayTag : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTC_HasGameplayTagInstanceData;

	FSTC_HasGameplayTag() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
