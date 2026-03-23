// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "TSAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)		\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region OnRep
	
	//━━━━━━━━━━━━━━━━━━━━
	// OnRep
	//━━━━━━━━━━━━━━━━━━━━	
	
public:
	
	UFUNCTION() virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION() virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION() virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	UFUNCTION() virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	UFUNCTION() virtual void OnRep_Mentality(const FGameplayAttributeData& OldMentality);
	UFUNCTION() virtual void OnRep_MaxMentality(const FGameplayAttributeData& OldMaxMentality);
	UFUNCTION() virtual void OnRep_Hunger(const FGameplayAttributeData& OldHunger);
	UFUNCTION() virtual void OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger);
	UFUNCTION() virtual void OnRep_Thirst(const FGameplayAttributeData& OldThirst);
	UFUNCTION() virtual void OnRep_MaxThirst(const FGameplayAttributeData& OldMaxThirst);
	UFUNCTION() virtual void OnRep_Temperature(const FGameplayAttributeData& OldTemperature);
	UFUNCTION() virtual void OnRep_MaxTemperature(const FGameplayAttributeData& OldMaxTemperature);

	UFUNCTION() virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);
	UFUNCTION() virtual void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);
	UFUNCTION() virtual void OnRep_DefensePower(const FGameplayAttributeData& OldDefensePower);
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	
public:
	UTSAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;		// 값이 바뀌기 직전에 호출 
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override; // Effect(데미지, 힐 등)가 실행된 직후 호출 
	
#pragma endregion
//======================================================================================================================	
#pragma region 어트리뷰트_매크로
	
	//━━━━━━━━━━━━━━━━━━━━
	// 어트리뷰트
	//━━━━━━━━━━━━━━━━━━━━	
public:
	// 체력
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxHealth);
	// 지구력
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxStamina);
	// 정신력
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Mentality);
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxMentality);
	// 배고픔
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Hunger);
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxHunger);
	// 갈증
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Thirst);
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxThirst);
	// 체온
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Temperature);
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, MaxTemperature);

	ATTRIBUTE_ACCESSORS(UTSAttributeSet, Strength);     // 근력
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, AttackPower);  // 공격력
	ATTRIBUTE_ACCESSORS(UTSAttributeSet, DefensePower); // 방어력
	
#pragma endregion
//======================================================================================================================	
#pragma region 어트리뷰트_매크로
	
	//━━━━━━━━━━━━━━━━━━━━
	// 어트리뷰트
	//━━━━━━━━━━━━━━━━━━━━	

protected:
	//-----------------------
	// 체력 
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	//-----------------------
	// 지구력  
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	//-----------------------
	// 정신력
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Mentality)
	FGameplayAttributeData Mentality;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxMentality)
	FGameplayAttributeData MaxMentality;
	//-----------------------
	// 배고픔
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Hunger)
	FGameplayAttributeData Hunger;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHunger)
	FGameplayAttributeData MaxHunger;
	//-----------------------
	// 갈증 
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Thirst)
	FGameplayAttributeData Thirst;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxThirst)
	FGameplayAttributeData MaxThirst;
	//-----------------------
	// 체온
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Temperature)
	FGameplayAttributeData Temperature;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxTemperature)
	FGameplayAttributeData MaxTemperature;
	//-----------------------
	// 근력  
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	//-----------------------
	// 공격력 
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	//-----------------------
	// 방어력
	//-----------------------
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_DefensePower)
	FGameplayAttributeData DefensePower;
	
#pragma endregion
//======================================================================================================================	
	
	
	
	
	
	
	
	
	
	
	
};
