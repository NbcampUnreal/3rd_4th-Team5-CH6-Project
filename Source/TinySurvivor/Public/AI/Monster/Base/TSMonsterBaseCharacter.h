#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "MonsterCharacterInterface.h"
#include "GameFramework/Character.h"
#include "TSMonsterBaseCharacter.generated.h"

class ULootComponent;
class UGameplayEffect;
class UGameplayAbility;
class UTSMonsterAS;
class UTSMonsterASC;

UCLASS()
class TINYSURVIVOR_API ATSMonsterBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IMonsterCharacterInterface
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//---------------------------------------
	// ATSMonsterBaseCharacter 라이프 사이클
	//--------------------------------------
	
public:
	ATSMonsterBaseCharacter();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//--------------------------------------
	// ATSMonsterBaseCharacter 기타 
	//--------------------------------------

public:
	// IAbilitySystemInterface ~ 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ IAbilitySystemInterface
	
	// IMonsterCharacterInterface ~ 
	virtual void SetSpeedIncrease() override;
	virtual void ResetSpeed() override;
	virtual void StopWalk() override;
	virtual void RegainSpeed() override;
	virtual void SetDropRootItems(FTSMonsterTable& MonsterTable) override;
	virtual void RequestSpawnDropRooItems() override;
	virtual void MakeTimeToDead() override;
	// ~ IMonsterCharacterInterface
	
	void DeadTime();
	
protected:
	int32 CurrentSpeedIncreaseLevel = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UTSMonsterASC> MonsterASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
	TObjectPtr<UTSMonsterAS> MonsterAS;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TArray<TSubclassOf<UGameplayAbility>> GiveAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TArray<TSubclassOf<UGameplayEffect>> GiveGameplayEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	float DelayTimeToDead = 10.0f;
	
	UPROPERTY()
	TObjectPtr<ULootComponent> SpawnedLootComp;

};
