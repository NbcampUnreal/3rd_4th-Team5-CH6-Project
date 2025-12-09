// STE_FindPlayerTarget.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "STE_FindPlayerTarget.generated.h"

USTRUCT()
struct FSTE_FindPlayerTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Output")
	TObjectPtr<AActor> TargetActor = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Output")
	float DistanceToTarget = 0.0f;
};

// Perception을 사용하여 가장 가까운 플레이어를 찾는 Evaluator
USTRUCT(meta = (DisplayName = "Find Player Target"))
struct TINYSURVIVOR_API FSTE_FindPlayerTarget : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTE_FindPlayerTargetInstanceData;

	FSTE_FindPlayerTarget() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};
