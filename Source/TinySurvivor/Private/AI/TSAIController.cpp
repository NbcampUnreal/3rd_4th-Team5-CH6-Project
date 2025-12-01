// TSAIController.cpp

#include "AI/TSAIController.h"
#include "GameplayTagContainer.h"
#include "StateTreeEvents.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ATSAIController::ATSAIController()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	
	// 시각 설정
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.0f;
	SightConfig->LoseSightRadius = 2500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ATSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (HasAuthority())
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ATSAIController::OnTargetPerceptionUpdated);
		
		StateTreeComponent->StartLogic();
	}
}

void ATSAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!HasAuthority())
		return;
	
	if (Stimulus.WasSuccessfullySensed() && Actor->ActorHasTag(TEXT("Player")))
	{
		TargetActor = Actor;
		
		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("AI.Event.PlayerSpotted"));
		
		if (EventTag.IsValid())
		{
			FStateTreeEvent Event(EventTag);
			StateTreeComponent->SendStateTreeEvent(Event);
		}
	}
}
