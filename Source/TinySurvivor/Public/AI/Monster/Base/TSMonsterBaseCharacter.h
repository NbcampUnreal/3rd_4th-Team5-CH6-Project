#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "TSMonsterBaseCharacter.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class UTSMonsterAS;
class UTSMonsterASC;

UCLASS()
class TINYSURVIVOR_API ATSMonsterBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// UTSMonsterSpawnSystem 라이프 사이클
	//--------------------------------------
	
public:
	ATSMonsterBaseCharacter();
	virtual void BeginPlay() override;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//--------------------------------------
	// UTSMonsterSpawnSystem 라이프 사이클
	//--------------------------------------

public:
	// IAbilitySystemInterface ~ 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ IAbilitySystemInterface
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UTSMonsterASC> MonsterASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UTSMonsterAS> MonsterAS;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TArray<TSubclassOf<UGameplayAbility>> GiveAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TArray<TSubclassOf<UGameplayEffect>> GiveGameplayEffects;
};
