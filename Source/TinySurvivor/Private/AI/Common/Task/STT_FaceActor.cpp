// STT_FaceActor.cpp

#include "AI/Common/Task/STT_FaceActor.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FSTT_FaceActor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType Data = Context.GetInstanceData<FInstanceDataType>(*this);
	
	AActor* Owner = Cast<AActor>(Context.GetOwner());
	APawn* Pawn = Cast<APawn>(Owner);
	AAIController* AIC = Pawn ? Pawn->GetController<AAIController>() : nullptr;
	
	if (AIC && Data.TargetActor)
	{
		AIC->SetFocus(Data.TargetActor);
		return EStateTreeRunStatus::Running;
	}
	
	return EStateTreeRunStatus::Failed;
}

void FSTT_FaceActor::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AActor* Owner = Cast<AActor>(Context.GetOwner());
	APawn* Pawn = Cast<APawn>(Owner);
	AAIController* AIC = Pawn ? Pawn->GetController<AAIController>() : nullptr;
	
	if (AIC)
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}
}
