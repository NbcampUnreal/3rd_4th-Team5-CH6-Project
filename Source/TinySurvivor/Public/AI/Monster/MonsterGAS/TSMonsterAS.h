#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AI/Common/MonsterAttributeSet.h"
#include "TSMonsterAS.generated.h"

#define ATTRIBUTE_ACCESSORS(Classname, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(Classname, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSMonsterAS : public UAttributeSet
{
	GENERATED_BODY()
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	ATTRIBUTE_ACCESSORS(UTSMonsterAS, Health);
	ATTRIBUTE_ACCESSORS(UTSMonsterAS, MaxHealth);
	ATTRIBUTE_ACCESSORS(UTSMonsterAS, AttackDamage);
	
protected:
	
	// 체력
	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	
	// 최대체력
	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	
	// 공격력
	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	
	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);
	
};
