#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TSGiantAbilitySystemComponent.generated.h"

class UGameplayAbility;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSGiantAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UTSGiantAbilitySystemComponent();

	UFUNCTION(BlueprintCallable)	
	void SetGiantAbilityList();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbility>> GiantAbilityClassList;
};
