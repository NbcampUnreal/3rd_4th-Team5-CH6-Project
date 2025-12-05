// TSAIController.cpp

#include "AI/Common/TSAIController.h"

#include "BehaviorTree/BlackboardComponent.h"

ATSAIController::ATSAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	
	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ATSAIController::OnTargetDetected);
}

void ATSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ATSAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor->ActorHasTag("Player") && Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
	}
}
