#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "MonsterAICInterface.h"
#include "Perception/AIPerceptionTypes.h"
#include "TSAICBase.generated.h"

class UAISenseConfig_Sight;
class UStateTreeAIComponent;

UCLASS()
class TINYSURVIVOR_API ATSAICBase : public AAIController, public IAbilitySystemInterface, public IMonsterAICInterface
{
	GENERATED_BODY()

public:
	ATSAICBase();
	virtual void BeginPlay() override;
	
	// IAbilitySystemInterface ~ 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ IAbilitySystemInterface
	
	// IMonsterAICInterface ~
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void SetSpawningPoint(FVector InSpawnPoint) override { CachingSpawnPoint = InSpawnPoint;}

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual FVector GetSpawningPoint() override { return CachingSpawnPoint; }
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void StartLogic() override;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void SetInstigator(AActor* InInstigator) override;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual AActor* GetCauseInstigator() override;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual UStateTreeAIComponent* GetStateTreeAIComponent() override { return ThisStateTreeAIComponent; }
	// ~ IMonsterAICInterface
	
protected:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> ThisAIPerceptionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UStateTreeAIComponent> ThisStateTreeAIComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	
	// 스폰 위치 
	FVector CachingSpawnPoint;
	
	// 초기 타겟
	TWeakObjectPtr<AActor> FirstInstigator;

};

