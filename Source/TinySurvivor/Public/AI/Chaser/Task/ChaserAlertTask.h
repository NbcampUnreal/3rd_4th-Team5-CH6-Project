// ChaserAlertTask.h

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AI/Common/TSAICharacter.h"
#include "AI/Common/TSAIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "ChaserTargetTaskData.h"
#include "ChaserAlertTask.generated.h"

USTRUCT(meta = (DisplayName = "Chaser Alert (Roar)"))
struct TINYSURVIVOR_API FChaserAlertTask : public FStateTreeTaskCommonBase
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
		ATSAIController* Controller	= &Context.GetExternalData(AIControllerHandle);
		ATSAICharacter* Monster = Cast<ATSAICharacter>(Controller->GetPawn());
		if (!Controller || !Monster)
			return EStateTreeRunStatus::Failed;
		
		// 이동 정지 & 포커스
		Controller->StopMovement();
		if (InstanceData.TargetActor)
			Controller->SetFocus(InstanceData.TargetActor);
		
		Monster->CurrentState = EChaserState::Chase;

		// 몽타주 재생
		if (Monster->RoarMontage)
			Monster->PlayAnimMontage(Monster->RoarMontage);
		
		return EStateTreeRunStatus::Running;
	}
	
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override
	{
		ATSAIController* Controller	= &Context.GetExternalData(AIControllerHandle);
		ATSAICharacter* Monster = Cast<ATSAICharacter>(Controller->GetPawn());
		
		// 몽타주가 끝났는지 확인 후, 성공 반환. Chase 상태로 전이
		if (Monster && Monster->GetMesh()->GetAnimInstance() && Monster->RoarMontage)
		{
			if (!Monster->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Monster->RoarMontage))
				return EStateTreeRunStatus::Succeeded;
		}
		
		return EStateTreeRunStatus::Running;
	}
	
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override
	{
		ATSAIController* Controller	= &Context.GetExternalData(AIControllerHandle);
		if (Controller)
			// Focus 해제. 다음 추적 단계 준비
				Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
};