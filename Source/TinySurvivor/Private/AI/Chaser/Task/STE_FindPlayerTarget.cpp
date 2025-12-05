// STE_FindPlayerTarget.cpp

#include "AI/Chaser/Task/STE_FindPlayerTarget.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void FSTE_FindPlayerTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData<FInstanceDataType>(*this);
    
	// State Tree의 주인 가져오기
	APawn* OwnerPawn = Cast<APawn>(Context.GetOwner());
	if (!OwnerPawn)
		return;
	
	// Pawn을 통해 AIController 가져오기
	AAIController* AIC = Cast<AAIController>(Context.GetOwner());
	if (!AIC) 
		return;

	// AI Perception 컴포넌트 가져오기
	UAIPerceptionComponent* Perception = AIC->GetPerceptionComponent();
	if (!Perception)
	{
		Data.TargetActor = nullptr;
		return;	
	}
	
	// 인지된 모든 액터 가져오기
	TArray<AActor*> PerceiveActors;
	Perception->GetCurrentlyPerceivedActors(nullptr, PerceiveActors);
	
	AActor* BestTarget = nullptr;
	float MinDistSq = FLT_MAX;
	FVector MyLoc = OwnerPawn->GetActorLocation();
	
	// 가장 가까운 플레이어 찾기
	for (AActor* Actor : PerceiveActors)
	{
		// 유효하고 죽지 않았고, Player 태그가 있는 대상만
		if (IsValid(Actor) && Actor->ActorHasTag(FName("Player")))
		{
			// 거리 제곱 계산
			float DistSq = FVector::DistSquared(MyLoc, Actor->GetActorLocation());
			
			// 기존보다 더 가까우면 갱신
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				BestTarget = Actor;
			}
		}
	}
	
	Data.TargetActor = BestTarget;
	
	// 거리 저장
	if (BestTarget)
	{
		// 타겟이 있으면 실제 거리 계산
		Data.DistanceToTarget = FVector::Distance(MyLoc, BestTarget->GetActorLocation());
	}
	else
	{
		// 타겟이 없으면 아주 먼 거리로 설정 (공격 조건 발동 안되게)
		Data.DistanceToTarget = 99999.0f;
	}
}
