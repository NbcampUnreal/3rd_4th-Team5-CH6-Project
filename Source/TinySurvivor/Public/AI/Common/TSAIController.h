// TSAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "TSAIController.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ATSAIController();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
	
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* AIPerception;
	
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;
};
