#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TSPlayerState.generated.h"

class UAbilitySystemComponent;
class UTSAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class TINYSURVIVOR_API ATSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATSPlayerState();

	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTSAttributeSet* GetAttributeSet() const; 

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UTSAttributeSet> Attributes;
	
};
