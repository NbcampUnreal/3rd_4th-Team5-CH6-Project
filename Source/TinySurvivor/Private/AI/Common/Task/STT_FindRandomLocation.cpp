// STT_FindRandomLocation.cpp

#include "AI/Common/Task/STT_FindRandomLocation.h"

#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "AI/NavigationSystemBase.h"

EStateTreeRunStatus FSTT_FindRandomLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	
	if (!OwnerActor)
	{
		return EStateTreeRunStatus::Failed;	
	}
	
	FVector Center = (Data.Origin.IsZero()) ? OwnerActor->GetActorLocation() : Data.Origin;
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(OwnerActor->GetWorld());
	if (NavSys)
	{
		FNavLocation RandomPt;
		
		if (NavSys->GetRandomReachablePointInRadius(Center, Data.Radius, RandomPt))
		{
			Data.TargetLocation = RandomPt.Location;
			return EStateTreeRunStatus::Succeeded;
		}
	}
	
	Data.TargetLocation = OwnerActor->GetActorLocation();
	return EStateTreeRunStatus::Failed;
}
