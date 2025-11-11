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
	//ASC 복제용 : 멀티 동기화
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//Base 값이 바뀌기 직전
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	// 값이 변경되기 직전 호출
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// GE 실행 후 호출
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:
	// 클램프 (헬퍼함수)
	void ClampBeforeChange(const FGameplayAttribute& Attribute, float& NewValue) const;
	void ClampAfterEffect(const struct FGameplayEffectModCallbackData& Data);

	
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

	//3. Hunger (배고픔)
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

public:
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
};
