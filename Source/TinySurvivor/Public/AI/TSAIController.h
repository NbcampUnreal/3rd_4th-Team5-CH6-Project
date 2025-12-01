// TSAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/StateTreeComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TSAIController.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ATSAIController();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStateTreeComponent* StateTreeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* TargetActor = nullptr;
	// 배회 기준점 (스포너 위치)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	FVector HomeLocation;
	// 배회 반경 (이 범위를 벗어나지 않음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolRadius = 1500.0f;
	
	// 감각 업데이트 이벤트
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
