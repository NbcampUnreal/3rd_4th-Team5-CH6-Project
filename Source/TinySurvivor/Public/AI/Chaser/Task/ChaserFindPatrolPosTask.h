// ChaserFindPatrolPosTask.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AI/Common/TSAIController.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Navigation/PathFollowingComponent.h"
#include "StateTreeInstanceData.h"
#include "ChaserFindPatrolPosTask.generated.h"

USTRUCT()
struct FChaserFindPatrolPosTaskInstanceData
{
	GENERATED_BODY()
	
};

USTRUCT(meta = (DisplayName = "Chaser Find Patrol Pos"))
struct TINYSURVIVOR_API FChaserFindPatrolPosTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Data")
	FChaserFindPatrolPosTaskInstanceData InstanceData;
	
	TStateTreeExternalDataHandle<ATSAIController> AIControlerHandle;
	
	virtual bool Link(FStateTreeLinker& Linker) override
	{
		// 외부 데이터 연결
		Linker.LinkExternalData(AIControlerHandle);
		return true;
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{
		ATSAIController* Controller = &Context.GetExternalData(AIControlerHandle);
		UWorld* World = Context.GetWorld();
		if (!Controller || !World)
			return EStateTreeRunStatus::Failed;
		
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Context.GetWorld());
		if (!NavSys)
			return EStateTreeRunStatus::Failed;
		
		FNavLocation ResultLocation;
		bool bFound = NavSys->GetRandomReachablePointInRadius(
			Controller->HomeLocation,
			Controller->PatrolRadius,
			ResultLocation
		);
		
		if (bFound)
		{
			Controller->MoveToLocation(ResultLocation.Location);
			return EStateTreeRunStatus::Running;
		}
		
		return EStateTreeRunStatus::Failed;
	}
	
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override
	{
		ATSAIController* Controller = &Context.GetExternalData(AIControlerHandle);
		
		if (Controller && Controller->GetMoveStatus() == EPathFollowingStatus::Idle)
			return EStateTreeRunStatus::Succeeded;
		
		return EStateTreeRunStatus::Running;
	}
};