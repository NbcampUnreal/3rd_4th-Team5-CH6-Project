#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "TSGiantCompInterface.h"
#include "Perception/AIPerceptionTypes.h"
#include "TSGiantAIController.generated.h"

class UStateTreeAIComponent;
class UStateTree;
class UAIPerceptionComponent;
class UAISense_Hearing;
class UAISense_Sight;

UCLASS()
class TINYSURVIVOR_API ATSGiantAIController : public AAIController, public IAbilitySystemInterface, public ITSGiantCompInterface
{
	GENERATED_BODY()
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//----------------------------------------
	// ATSGiantAIController 라이프 사이클
	//----------------------------------------
	
public:
	ATSGiantAIController();
	virtual void BeginPlay() override;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//----------------------------------------
	// ATSGiantAIController 퍼셉션
	//----------------------------------------
protected:
	// 퍼셉션 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaint | Comp")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	
	// 눈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaint | Sense")
	TObjectPtr<UAISense_Sight> GiantAISightSense;
	
	// 귀
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaint | Sense")
	TObjectPtr<UAISense_Hearing> GiantAIHearingSense;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//----------------------------------------
	// ATSGiantAIController 스테이트 트리
	//----------------------------------------
	
public:
	// ITSGiantCompInterface ~ 
	virtual UStateTreeAIComponent* GetStateTreeAIComponent_Implementation() override;
	// ~ ITSGiantCompInterface
	
	// StateTree AI Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Giant | StateTree")
	TObjectPtr<UStateTreeAIComponent> GiantStateTreeComponent;

	// StateTree Asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Giant | StateTree")
	TObjectPtr<UStateTree> GiantStateTreeAsset;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//----------------------------------------
	// ATSGiantAIController GAS
	//----------------------------------------
	
public:
	// IAbilitySystemInterface ~
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ IAbilitySystemInterface
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//----------------------------------------
	// ATSGiantAIController 침식도
	//----------------------------------------
	
protected:
	UFUNCTION()
	void ChangeStateTreeLogicByErosionValue(float ErosionValue);
	
	UFUNCTION(BlueprintCallable, Category = "Giant | Erosion")
	FGameplayTag CheckErosionTag(float& ErosionValue);
};
