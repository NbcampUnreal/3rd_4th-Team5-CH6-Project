#include "UI/TSPlayerUIDataControllerSystem.h"
#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "System/Erosion/ErosionStateInfo.h"

void UTSPlayerUIDataControllerSystem::InitControllerModel(APlayerController* Player, UAbilitySystemComponent* ASC)
{
	if (IsValid(Player) && Player->IsLocalPlayerController() && IsValid(ASC))
	{
		// 1. Health (체력)
		Health = ASC->GetNumericAttribute(UTSAttributeSet::GetHealthAttribute());
		MaxHealth = ASC->GetNumericAttribute(UTSAttributeSet::GetMaxHealthAttribute());
		
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetHealthAttribute())
		.AddUObject(this, &UTSPlayerUIDataControllerSystem::UpdateHealth);
		
		// 2. Stamina (스태미나)
		Stamina = ASC->GetNumericAttribute(UTSAttributeSet::GetStaminaAttribute());
		MaxStamina = ASC->GetNumericAttribute(UTSAttributeSet::GetMaxStaminaAttribute());
		
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetStaminaAttribute())
		.AddUObject(this, &UTSPlayerUIDataControllerSystem::UpdateStamina);
		
		// 3. Hunger (배고픔)
		Hunger = ASC->GetNumericAttribute(UTSAttributeSet::GetHungerAttribute());
		MaxHunger = ASC->GetNumericAttribute(UTSAttributeSet::GetMaxHungerAttribute());
		
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetHungerAttribute())
		.AddUObject(this, &UTSPlayerUIDataControllerSystem::UpdateHunger);
		
		// 4. Thirst (갈증)
		Thirst = ASC->GetNumericAttribute(UTSAttributeSet::GetThirstAttribute());
		MaxThirst = ASC->GetNumericAttribute(UTSAttributeSet::GetMaxThirstAttribute());
		
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetThirstAttribute())
		.AddUObject(this, &UTSPlayerUIDataControllerSystem::UpdateThirst);
		
		// 5. Sanity (정신력)
		Sanity = ASC->GetNumericAttribute(UTSAttributeSet::GetSanityAttribute());
		MaxSanity = ASC->GetNumericAttribute(UTSAttributeSet::GetMaxSanityAttribute());
		
		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetSanityAttribute())
		.AddUObject(this, &UTSPlayerUIDataControllerSystem::UpdateSanity);
		
		// 6. Temperature (체온)
		Temperature = ASC->GetNumericAttribute(UTSAttributeSet::GetTemperatureAttribute());
		MaxTemperature = ASC->GetNumericAttribute(UTSAttributeSet::GetMaxTemperatureAttribute());

		ASC->GetGameplayAttributeValueChangeDelegate(UTSAttributeSet::GetTemperatureAttribute())
		.AddUObject(this, &UTSPlayerUIDataControllerSystem::UpdateTemperature);
		
		// 7. 침식도
		if (!IsValid(GetWorld())) return;
		
		AActor* FoundErosionStateInfo = UGameplayStatics::GetActorOfClass(this, AErosionStateInfo::StaticClass());
		if (!IsValid(FoundErosionStateInfo)) return;
		
		AErosionStateInfo* ErosionStateInfo = Cast<AErosionStateInfo>(FoundErosionStateInfo);
		if (!IsValid(ErosionStateInfo)) return;
		
		Erosion = ErosionStateInfo->GetCurrentErosion();
		MaxErosion = 100.f;
		
		ErosionStateInfo->OnErosionChangedUIDelegate.AddUObject(this, &ThisClass:: UpdateErosion);
		
	}
}

void UTSPlayerUIDataControllerSystem::InitViewModel()
{
	HealthChangeDelegate.Broadcast(Health, MaxHealth);
	StaminaChangeDelegate.Broadcast(Stamina, MaxStamina);
	HungerChangeDelegate.Broadcast(Hunger, MaxHunger);
	ThirstChangeDelegate.Broadcast(Thirst, MaxThirst);
	SanityChangeDelegate.Broadcast(Sanity, MaxSanity);
	TemperatureChangeDelegate.Broadcast(Temperature, MaxTemperature);
	ErosionChangeDelegate.Broadcast(Erosion, MaxErosion);
}

UTSPlayerUIDataControllerSystem* UTSPlayerUIDataControllerSystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}
			
	return World->GetSubsystem<UTSPlayerUIDataControllerSystem>();
	
}

void UTSPlayerUIDataControllerSystem::UpdateHealth(const FOnAttributeChangeData& Data)
{
	Health = Data.NewValue;
	HealthChangeDelegate.Broadcast(Health, MaxHealth);
}

void UTSPlayerUIDataControllerSystem::UpdateMaxHealth(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue;
	HealthChangeDelegate.Broadcast(Health, MaxHealth);
}


void UTSPlayerUIDataControllerSystem::UpdateStamina(const FOnAttributeChangeData& Data)
{
	Stamina = Data.NewValue;
	StaminaChangeDelegate.Broadcast(Stamina, MaxStamina);
}

void UTSPlayerUIDataControllerSystem::UpdateMaxStamina(const FOnAttributeChangeData& Data)
{
	MaxStamina = Data.NewValue;
	StaminaChangeDelegate.Broadcast(Stamina, MaxStamina);
}

void UTSPlayerUIDataControllerSystem::UpdateHunger(const FOnAttributeChangeData& Data)
{
	Hunger = Data.NewValue;
	HungerChangeDelegate.Broadcast(Hunger, MaxHunger);
}

void UTSPlayerUIDataControllerSystem::UpdateMaxHunger(const FOnAttributeChangeData& Data)
{
	MaxHunger = Data.NewValue;
	HungerChangeDelegate.Broadcast(Hunger, MaxHunger);
}

void UTSPlayerUIDataControllerSystem::UpdateThirst(const FOnAttributeChangeData& Data)
{
	Thirst = Data.NewValue;
	ThirstChangeDelegate.Broadcast(Thirst, MaxThirst);
}

void UTSPlayerUIDataControllerSystem::UpdateMaxThirst(const FOnAttributeChangeData& Data)
{
	MaxThirst = Data.NewValue;
	ThirstChangeDelegate.Broadcast(Thirst, MaxThirst);
}

void UTSPlayerUIDataControllerSystem::UpdateSanity(const FOnAttributeChangeData& Data)
{
	Sanity = Data.NewValue;
	SanityChangeDelegate.Broadcast(Sanity, MaxSanity);
}

void UTSPlayerUIDataControllerSystem::UpdateMaxSanity(const FOnAttributeChangeData& Data)
{
	MaxSanity = Data.NewValue;
	SanityChangeDelegate.Broadcast(Sanity, MaxSanity);
}

void UTSPlayerUIDataControllerSystem::UpdateTemperature(const FOnAttributeChangeData& Data)
{
	Temperature = Data.NewValue;
	TemperatureChangeDelegate.Broadcast(Temperature, MaxTemperature);
}

void UTSPlayerUIDataControllerSystem::UpdateMaxTemperature(const FOnAttributeChangeData& Data)
{
	MaxTemperature = Data.NewValue;
	TemperatureChangeDelegate.Broadcast(Temperature, MaxTemperature);
}

void UTSPlayerUIDataControllerSystem::UpdateErosion(float NewValue)
{
	Erosion = NewValue;
	ErosionChangeDelegate.Broadcast(Erosion, MaxErosion);
}

void UTSPlayerUIDataControllerSystem::UpdateMaxErosion(float NewValue)
{
	MaxErosion = NewValue;
	ErosionChangeDelegate.Broadcast(Erosion, MaxErosion);
}
