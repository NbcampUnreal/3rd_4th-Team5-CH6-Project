// ChaserAttackTask.h

#pragma once

#include "CoreMinimal.h"
#include "AI/Common/TSAICharacter.h"
#include "ChaserTargetTaskData.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "StateTreeTaskBase.h"
#include "AI/Common/TSAIController.h"
#include "ChaserAttackTask.generated.h"

USTRUCT(meta = (DisplayName = "Chaser Attack"))
struct TINYSURVIVOR_API FChaserAttackTask : public FStateTreeTaskCommonBase
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
		ATSAICharacter* Monster = Cast<ATSAICharacter>(Controller->GetPawn());
		if (!Controller || !Monster)
			return EStateTreeRunStatus::Failed;
		
		// 이동 정지 & 상태 동기화
		Controller->StopMovement();
		Monster->CurrentState = EChaserState::Attack;
		
		// 공격 실행 (서버 -> 멀티캐스트 RPC)
		Monster->PerformAttack();
		
		return EStateTreeRunStatus::Running;
	}
	
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override
	{
		ATSAIController* Controller = &Context.GetExternalData(AIControllerHandle);
		ATSAICharacter* Monster = Cast<ATSAICharacter>(Controller->GetPawn());
		FChaserTargetTaskData& Data = Context.GetInstanceData<FChaserTargetTaskData>(*this);
		if (!Monster || !Controller)
			return EStateTreeRunStatus::Failed;
		
		// 공격 중에도 타겟을 향해 회전
		if (Data.TargetActor)
		{
			// 타겟을 향해 몸을 돌림
			FVector LookDir = Data.TargetActor->GetActorLocation() - Monster->GetActorLocation();
			LookDir.Z = 0.0f;
			if (!LookDir.IsNearlyZero())
			{
				FRotator TargetRot = LookDir.Rotation();
				// 부드러운 회전을 위해 RInterpTo 사용
				FRotator NewRot = FMath::RInterpTo(Monster->GetActorRotation(), TargetRot, DeltaTime, 5.0f);
				Monster->SetActorRotation(NewRot);
			}
		}
		
		// 몽타주 끝났는지 확인 후, 성공 반환. 다시 추적으로
		if (Monster->GetMesh()->GetAnimInstance() && Monster->AttackMontage)
		{
			if (!Monster->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Monster->AttackMontage))
			{
				// 공격 쿨타임 체크 로직
				return EStateTreeRunStatus::Succeeded;	
			}
		}
		
		return EStateTreeRunStatus::Running;
	}
};
