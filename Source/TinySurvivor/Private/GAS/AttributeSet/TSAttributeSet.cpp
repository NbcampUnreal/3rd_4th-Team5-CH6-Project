#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameplayTags/AbilityGameplayTags.h"


//***********************************************
// 생성자 초기화
//***********************************************
UTSAttributeSet::UTSAttributeSet()
{
	// 기본 값 설정
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	
	InitStamina(100.0f);
	InitMaxStamina(100.0f);
	
	InitHunger(100.0f);
	InitMaxHunger(100.0f);
	
	InitThirst(100.0f);
	InitMaxThirst(100.0f);
	
	InitSanity(100.0f);
	InitMaxSanity(100.0f);
	
	InitTemperature(36.5f); //36.5??
	InitMaxTemperature(100.0f); //36.5? max는 한 39? 일단 100으로 다 맞춰둠
	
	InitMoveSpeed(600.f);
	InitMaxMoveSpeed(1000.0f);
}
//***********************************************
//복제 설정
//***********************************************
void UTSAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//모든 속성 네트워크로 복제하도록 설정
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxHunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Thirst, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxThirst, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Sanity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxSanity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Temperature, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxTemperature, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxMoveSpeed, COND_None, REPNOTIFY_Always);
}
//***********************************************
//값 변경 전 Clamp
//***********************************************
// 기본값(Base) 변경 전
void UTSAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampBeforeChange(Attribute, NewValue);
}
// 현재값(Current) 변경 전
void UTSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampBeforeChange(Attribute, NewValue);
}
//***********************************************
//GE 적용 후 Clamp
//***********************************************
void UTSAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	ClampAfterEffect(Data);
	
	if (Data.EvaluatedData.Attribute == GetThirstAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		FGameplayTag ThirstTag = AbilityTags::TAG_State_Status_Thirst;
		if (GetThirst() <= 0.0f)
		{
			if (!ASC->HasMatchingGameplayTag(ThirstTag))
			{
				ASC->AddLooseGameplayTag(ThirstTag);
			}
		}else
		{
			if (ASC->HasMatchingGameplayTag(ThirstTag))
			{
				ASC->RemoveLooseGameplayTag(ThirstTag);
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetHungerAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		FGameplayTag HungerStatusTag = AbilityTags::TAG_State_Status_Hunger;
		if (GetHunger() <= 0.0f)
		{
			if (!ASC->HasMatchingGameplayTag(HungerStatusTag))
			{
				ASC->AddLooseGameplayTag(HungerStatusTag);
			}
		}else
		{
			if (ASC->HasMatchingGameplayTag(HungerStatusTag))
			{
				ASC->RemoveLooseGameplayTag(HungerStatusTag);
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetTemperatureAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		FGameplayTag TempHotStatusTag = AbilityTags::TAG_State_Status_TempHot;
		FGameplayTag TempColdStatusTag = AbilityTags::TAG_State_Status_TempCold;
		
		if (GetTemperature() >= 40.0f)
		{
			// 더위 상태이상 -> Hot 태그 붙이고 Cold 태그 제거
			if (!ASC->HasMatchingGameplayTag(TempHotStatusTag))
			{
				ASC->AddLooseGameplayTag(TempHotStatusTag);
			}
			if (ASC->HasMatchingGameplayTag(TempColdStatusTag))
			{
				ASC->RemoveLooseGameplayTag(TempColdStatusTag);
			}
		}else if (GetTemperature() <= 30.0f)
		{
			// 추위 상태이상 -> Cold 태그 붙이고 Hot 태그 제거 
			if (!ASC->HasMatchingGameplayTag(TempColdStatusTag))
			{
				ASC->AddLooseGameplayTag(TempColdStatusTag);
			}
			if (ASC->HasMatchingGameplayTag(TempHotStatusTag))
			{
				ASC->RemoveLooseGameplayTag(TempHotStatusTag);
			}
		} else //정상 범위면 둘 다 제거
		{
			if (ASC->HasMatchingGameplayTag(TempHotStatusTag))
			{
				ASC->RemoveLooseGameplayTag(TempHotStatusTag);
			}

			if (ASC->HasMatchingGameplayTag(TempColdStatusTag))
			{
				ASC->RemoveLooseGameplayTag(TempColdStatusTag);
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (GetHealth() <= 0.0f)
		{
			/////////////////////////
			//내 캐릭터 죽는 함수 구현//
			////////////////////////
			UE_LOG(LogTemp, Log, TEXT("캐릭터 사망 로직 구현 전 -> 체력 0 이 되어 죽었음 !! 로그만 찍기"));
		}
	}
}
//***********************************************
//헬퍼 함수 구현
//***********************************************
void UTSAttributeSet::ClampBeforeChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 현재 값은 0 ~ Max값 사이로 제한
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetHungerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHunger());
	}
	else if (Attribute == GetThirstAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxThirst());
	}
	else if (Attribute == GetSanityAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSanity());
	}
	else if (Attribute == GetTemperatureAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxTemperature()); // 0도 ~ 100도 (기획에 따라 변경)
	}
	// MAX 방어 코드 : Max 가 0 이 되는걸 방지함
	else if (Attribute == GetMaxHealthAttribute() || 
			 Attribute == GetMaxStaminaAttribute() ||
			 Attribute == GetMaxHungerAttribute() || 
			 Attribute == GetMaxThirstAttribute() ||
			 Attribute == GetMaxSanityAttribute() || 
			 Attribute == GetMaxTemperatureAttribute())
	{
		NewValue = FMath::Max(1.0f, NewValue);
	}
}
void UTSAttributeSet::ClampAfterEffect(const struct FGameplayEffectModCallbackData& Data)
{
	// GE 적용 후 값 0~Max 로 제한
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetHungerAttribute())
	{
		SetHunger(FMath::Clamp(GetHunger(), 0.0f, GetMaxHunger()));
	}
	else if (Data.EvaluatedData.Attribute == GetThirstAttribute())
	{
		SetThirst(FMath::Clamp(GetThirst(), 0.0f, GetMaxThirst()));
	}
	else if (Data.EvaluatedData.Attribute == GetSanityAttribute())
	{
		SetSanity(FMath::Clamp(GetSanity(), 0.0f, GetMaxSanity()));
	}
	else if (Data.EvaluatedData.Attribute == GetTemperatureAttribute())
	{
		SetTemperature(FMath::Clamp(GetTemperature(), 0.0f, GetMaxTemperature()));
	}
}
//***********************************************
//OnRep 함수 구현
//***********************************************
void UTSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Health, OldHealth);
}
void UTSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxHealth, OldMaxHealth);
}
void UTSAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Stamina, OldStamina);
}
void UTSAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxStamina, OldMaxStamina);
}
void UTSAttributeSet::OnRep_Hunger(const FGameplayAttributeData& OldHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Hunger, OldHunger);
}
void UTSAttributeSet::OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxHunger, OldMaxHunger);
}
void UTSAttributeSet::OnRep_Thirst(const FGameplayAttributeData& OldThirst)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Thirst, OldThirst);
}
void UTSAttributeSet::OnRep_MaxThirst(const FGameplayAttributeData& OldMaxThirst)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxThirst, OldMaxThirst);
}
void UTSAttributeSet::OnRep_Sanity(const FGameplayAttributeData& OldSanity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Sanity, OldSanity);
}
void UTSAttributeSet::OnRep_MaxSanity(const FGameplayAttributeData& OldMaxSanity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxSanity, OldMaxSanity);
}
void UTSAttributeSet::OnRep_Temperature(const FGameplayAttributeData& OldTemperature)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Temperature, OldTemperature);
}
void UTSAttributeSet::OnRep_MaxTemperature(const FGameplayAttributeData& OldMaxTemperature)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxTemperature, OldMaxTemperature);
}

void UTSAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UTSAttributeSet::OnRep_MaxMoveSpeed(const FGameplayAttributeData& OldMaxMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxMoveSpeed, OldMaxMoveSpeed);
}
