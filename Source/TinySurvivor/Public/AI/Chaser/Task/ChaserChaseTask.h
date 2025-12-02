// ChaserChaseTask.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AIController.h"
#include "AI/Common/TSAICharacter.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "AI/Common/TSAIController.h"
#include "ChaserTargetTaskData.h"
#include "Navigation/PathFollowingComponent.h"
#include "ChaserChaseTask.generated.h"

// 태스크 구조체
USTRUCT(meta = (DisplayName = "Chaser Chase"))
struct FChaserChaseTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "InstanceData")
	FChaserTargetTaskData InstanceData;
	
	TStateTreeExternalDataHandle<ATSAIController> AIControllerHandle;
	
	virtual bool Link(FStateTreeLinker& Linker) override
	{
		Linker.LinkExternalData(AIControllerHandle);
		return true;
	}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{
		ATSAIController* Controller = &Context.GetExternalData(AIControllerHandle);
		if (Controller && Controller->GetPawn())
		{
			if (ATSAICharacter* Character = Cast<ATSAICharacter>(Controller->GetPawn()))
				Character->CurrentState = EChaserState::Chase;
		}
		
		return EStateTreeRunStatus::Running;
	}
	
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override
	{
		FChaserTargetTaskData& Data = Context.GetInstanceData<FChaserTargetTaskData>(*this);
		ATSAIController* Controller = &Context.GetExternalData(AIControllerHandle);
		
		if (Controller && Data.TargetActor)
		{
			EPathFollowingRequestResult::Type Result = Controller->MoveToActor(Data.TargetActor, 150.0f);
			
			if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
				return EStateTreeRunStatus::Succeeded;
		}
		
		return EStateTreeRunStatus::Running;
	}
};
