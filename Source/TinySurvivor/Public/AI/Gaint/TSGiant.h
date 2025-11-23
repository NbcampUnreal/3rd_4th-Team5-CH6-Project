#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "TSGiantCompInterface.h"
#include "GameFramework/Character.h"
#include "TSGiant.generated.h"

class UTSGiantAttributeSet;
class UTSGiantAbilitySystemComponent;

UCLASS()
class TINYSURVIVOR_API ATSGiant : public ACharacter, public IAbilitySystemInterface, public ITSGiantCompInterface
{
	GENERATED_BODY()

public:
	ATSGiant();
	virtual void BeginPlay() override;
	
	// IAbilitySystemInterface ~
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ IAbilitySystemInterface
	
	// ITSGiantCompInterface ~ 
	virtual UStateTreeAIComponent* GetStateTreeAIComponent_Implementation() override;
	// ~ ITSGiantCompInterface
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTSGiantAbilitySystemComponent> GiantAbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTSGiantAttributeSet> GiantAttributeSet;
	
};
