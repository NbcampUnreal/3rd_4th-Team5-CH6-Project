// STT_FindRandomLocation.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_FindRandomLocation.generated.h"

USTRUCT()
struct FSTT_FindRandomLocationInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Input")
	float Radius = 1500.0f;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FVector Origin = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category = "Output")
	FVector TargetLocation = FVector::ZeroVector;
};

USTRUCT(meta = (DisplayName = "Find Random Location", Category = "AI"))
struct TINYSURVIVOR_API FSTT_FindRandomLocation : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FSTT_FindRandomLocationInstanceData;
	
	FSTT_FindRandomLocation() = default;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
