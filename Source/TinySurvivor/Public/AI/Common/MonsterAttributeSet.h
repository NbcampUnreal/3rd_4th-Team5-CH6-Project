// MonsterAttributeSet.h

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MonsterAttributeSet.generated.h"

// 매크로 : Getter, Setter, Init 자동 생성
#define ATTRIBUTE_ACCESSORS(Classname, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(Classname, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TINYSURVIVOR_API UMonsterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMonsterAttributeSet();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	// 체력
	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, Health);
	
	// 최대체력
	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, MaxHealth);
	
	// 공격력
	UPROPERTY(BlueprintReadOnly, Category = "Stats", ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, AttackDamage);
	
	// 처치 시 침식도 보상
	UPROPERTY(BlueprintReadOnly, Category = "Erosion", ReplicatedUsing = OnRep_ErosionReward)
	FGameplayAttributeData ErosionReward;
	ATTRIBUTE_ACCESSORS(UMonsterAttributeSet, ErosionReward);
	
protected:
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	
	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);
	
	UFUNCTION()
	void OnRep_ErosionReward(const FGameplayAttributeData& OldErosionReward);
};
