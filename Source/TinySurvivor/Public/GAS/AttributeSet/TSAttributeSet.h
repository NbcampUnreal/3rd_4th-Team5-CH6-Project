#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TSAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TINYSURVIVOR_API UTSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UTSAttributeSet();
	// ASC 복제용 : 멀티 동기화
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Base 값이 바뀌기 직전
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	// 값이 변경되기 직전 호출
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// GE 실행 후 호출
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:
	// 클램프 (헬퍼함수)
	void ClampBeforeChange(const FGameplayAttribute& Attribute, float& NewValue) const;
	void ClampAfterEffect(const struct FGameplayEffectModCallbackData& Data);

public:
	// 1. Health (체력)
	UPROPERTY(BlueprintReadOnly,Category = "Vital", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly,Category = "Vital", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	
	// 2. Stamina (스태미나)
	UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	
	UPROPERTY(BlueprintReadOnly, Category = "Vital", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	
	// 3. Hunger (배고픔)
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Hunger)
	FGameplayAttributeData Hunger;
	
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_MaxHunger)
	FGameplayAttributeData MaxHunger;
	
	// 4. Thirst (갈증)
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Thirst)
	FGameplayAttributeData Thirst;
	
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_MaxThirst)
	FGameplayAttributeData MaxThirst;
	
	// 5. Sanity (정신력)
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Sanity)
	FGameplayAttributeData Sanity;
	
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_MaxSanity)
	FGameplayAttributeData MaxSanity;
	
	// 6. Temperature (체온)
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_Temperature)
	FGameplayAttributeData Temperature;
	
	UPROPERTY(BlueprintReadOnly, Category = "Survival", ReplicatedUsing = OnRep_MaxTemperature)
	FGameplayAttributeData MaxTemperature;
	
	// 7. Speed (속도)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MaxMoveSpeed)
	FGameplayAttributeData MaxMoveSpeed;
	
	// 8. Damage (공격력)
	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_DamageBonus)
	FGameplayAttributeData DamageBonus;

	// 9. AttackSpeed (공격속도)
	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_BaseAttackSpeed)
	FGameplayAttributeData BaseAttackSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_AttackSpeedBonus)
	FGameplayAttributeData AttackSpeedBonus;

	// 10. AttackRange (공격반경)
	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_BaseAttackRange)
	FGameplayAttributeData BaseAttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_AttackRangeBonus)
	FGameplayAttributeData AttackRangeBonus;
	
	// 매크로
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Stamina)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxStamina)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Hunger)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxHunger)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Thirst)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxThirst)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Sanity)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxSanity)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Temperature)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxTemperature)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MoveSpeed)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxMoveSpeed)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, BaseDamage)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, DamageBonus)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, BaseAttackSpeed)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, AttackSpeedBonus)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, BaseAttackRange)
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, AttackRangeBonus)
	
protected:
	//OnRep 클라 반영
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	UFUNCTION()
	void OnRep_Hunger(const FGameplayAttributeData& OldHunger);
	UFUNCTION()
	void OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger);
	UFUNCTION()
	void OnRep_Thirst(const FGameplayAttributeData& OldThirst);
	UFUNCTION()
	void OnRep_MaxThirst(const FGameplayAttributeData& OldMaxThirst);
	UFUNCTION()
	void OnRep_Sanity(const FGameplayAttributeData& OldSanity);
	UFUNCTION()
	void OnRep_MaxSanity(const FGameplayAttributeData& OldMaxSanity);
	UFUNCTION()
	void OnRep_Temperature(const FGameplayAttributeData& OldTemperature);
	UFUNCTION()
	void OnRep_MaxTemperature(const FGameplayAttributeData& OldMaxTemperature);
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
	UFUNCTION()
	void OnRep_MaxMoveSpeed(const FGameplayAttributeData& OldMaxMoveSpeed);
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage);
	UFUNCTION()
	void OnRep_DamageBonus(const FGameplayAttributeData& OldDamageBonus);
	UFUNCTION()
	void OnRep_BaseAttackSpeed(const FGameplayAttributeData& OldBaseAttackSpeed);
	UFUNCTION()
	void OnRep_AttackSpeedBonus(const FGameplayAttributeData& OldAttackSpeedBonus);
	UFUNCTION()
	void OnRep_BaseAttackRange(const FGameplayAttributeData& OldBaseAttackRange);
	UFUNCTION()
	void OnRep_AttackRangeBonus(const FGameplayAttributeData& OldAttackRangeBonus);
};
