#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "MonsterInterface.h"
#include "AI/Gaint/TSGiantCompInterface.h"
#include "TSAICBase.generated.h"

class UStateTreeAIComponent;

UCLASS()
class TINYSURVIVOR_API ATSAICBase : public AAIController, public IAbilitySystemInterface, public IMonsterInterface
{
	GENERATED_BODY()

public:
	ATSAICBase();
	
	// IAbilitySystemInterface ~ 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ IAbilitySystemInterface
	
	// IMonsterInterface ~
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void SetSpawningPoint(FVector InSpawnPoint) override { CachingSpawnPoint = InSpawnPoint;}

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual FVector GetSpawningPoint() override { return CachingSpawnPoint; }
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void StartLogic() override;
	// ~ IMonsterInterface
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> ThisAIPerceptionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UStateTreeAIComponent> ThisStateTreeAIComponent;
	
	// 스폰 위치 
	FVector CachingSpawnPoint;
};

