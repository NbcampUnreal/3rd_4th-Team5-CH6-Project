#pragma once

#include "CoreMinimal.h"
#include "GAS/GE/Item/Consumable/GEC_RestoreAttribute.h"
#include "Subsystems/WorldSubsystem.h"
#include "TSPlayerUIDataControllerSystem.generated.h"

class APlayerController;
class UAbilitySystemComponent;

// 1. Health (체력)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHealthChangeDelegate, float, Helath, float, MaxHealth);

// 2. Stamina (스태미나)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStaminaChangeDelegate, float, Stamina, float, MaxStamina);

// 3. Hunger (배고픔)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHungerChangeDelegate, float, Hunger, float, MaxHunger);

// 4. Thirst (갈증)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FThirstChangeDelegate, float, Thirst, float, MaxThirst);

// 5. Sanity (정신력)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSanityChangeDelegate, float, Sanity, float, MaxSanity);

// 6. Temperature (체온)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTemperatureChangeDelegate, float, Temperature, float, MaxTemperature);

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UTSPlayerUIDataControllerSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	// 플레이어가 모든 초기화 이후 1번째로 호출
	void InitControllerModel(APlayerController* Player, UAbilitySystemComponent* ASC);
	
	// 모든 초기화 이후 2번째로 호출 (타이밍 안 맞을 시 UI도 따로 호출)
	UFUNCTION(BlueprintCallable)
	void InitViewModel();

	// 게터 
	static UTSPlayerUIDataControllerSystem* Get(const UObject* WorldContextObject);
	
	// 1. Health (체력)
	UPROPERTY(BlueprintAssignable)
	FHealthChangeDelegate HealthChangeDelegate;
	
	// 2. Stamina (스태미나)
	UPROPERTY(BlueprintAssignable)
	FStaminaChangeDelegate StaminaChangeDelegate;
	
	// 3. Hunger (배고픔)
	UPROPERTY(BlueprintAssignable)
	FHungerChangeDelegate HungerChangeDelegate;
	
	// 4. Thirst (갈증)
	UPROPERTY(BlueprintAssignable)
	FThirstChangeDelegate ThirstChangeDelegate;
	
	// 5. Sanity (정신력)
	UPROPERTY(BlueprintAssignable)
	FSanityChangeDelegate SanityChangeDelegate;

	// 6. Temperature (체온)
	UPROPERTY(BlueprintAssignable)
	FTemperatureChangeDelegate TemperatureChangeDelegate;
	
	
protected:
	// 1. Health (체력)
	void UpdateHealth(const FOnAttributeChangeData& Data);
	void UpdateMaxHealth(const FOnAttributeChangeData& Data);
	
	// 2. Stamina (스태미나)
	void UpdateStamina(const FOnAttributeChangeData& Data);
	void UpdateMaxStamina(const FOnAttributeChangeData& Data);
	
	// 3. Hunger (배고픔)
	void UpdateHunger(const FOnAttributeChangeData& Data);
	void UpdateMaxHunger(const FOnAttributeChangeData& Data);
	
	// 4. Thirst (갈증)
	void UpdateThirst(const FOnAttributeChangeData& Data);
	void UpdateMaxThirst(const FOnAttributeChangeData& Data);
	
	// 5. Sanity (정신력)
	void UpdateSanity(const FOnAttributeChangeData& Data);
	void UpdateMaxSanity(const FOnAttributeChangeData& Data);
	
	// 6. Temperature (체온)
	void UpdateTemperature(const FOnAttributeChangeData& Data);
	void UpdateMaxTemperature(const FOnAttributeChangeData& Data);
	
	
	
	// 1. Health (체력)
	float Health = 0.0f;
	float MaxHealth = 0.0f;
	
	// 2. Stamina (스태미나)
	float Stamina =	0.0f;
	float MaxStamina = 0.0f;

	// 3. Hunger (배고픔)
	float Hunger = 0.0f;
	float MaxHunger = 0.0f;

	// 4. Thirst (갈증)
	float Thirst = 0.0f;
	float MaxThirst = 0.0f;

	// 5. Sanity (정신력)
	float Sanity = 0.0f;
	float MaxSanity = 0.0f;
	
	// 6. Temperature (체온)
	float Temperature =	0.0f;
	float MaxTemperature = 0.0f;
};
