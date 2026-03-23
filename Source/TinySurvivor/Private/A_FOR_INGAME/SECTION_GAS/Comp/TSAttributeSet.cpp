// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/Comp/TSAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

//======================================================================================================================	
#pragma region OnRep
	
	//━━━━━━━━━━━━━━━━━━━━
	// OnRep
	//━━━━━━━━━━━━━━━━━━━━	

// OnRep 구현부 (매크로 활용)
void UTSAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Health, OldHealth); }
void UTSAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxHealth, OldMaxHealth); }
void UTSAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Stamina, OldStamina); }
void UTSAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxStamina, OldMaxStamina); }
void UTSAttributeSet::OnRep_Mentality(const FGameplayAttributeData& OldMentality) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Mentality, OldMentality); }
void UTSAttributeSet::OnRep_MaxMentality(const FGameplayAttributeData& OldMaxMentality) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxMentality, OldMaxMentality); }
void UTSAttributeSet::OnRep_Hunger(const FGameplayAttributeData& OldHunger) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Hunger, OldHunger); }
void UTSAttributeSet::OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxHunger, OldMaxHunger); }
void UTSAttributeSet::OnRep_Thirst(const FGameplayAttributeData& OldThirst) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Thirst, OldThirst); }
void UTSAttributeSet::OnRep_MaxThirst(const FGameplayAttributeData& OldMaxThirst) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MaxThirst, OldMaxThirst); }
void UTSAttributeSet::OnRep_Temperature(const FGameplayAttributeData& OldTemperature) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Temperature, OldTemperature); }

void UTSAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, Strength, OldStrength); }
void UTSAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, AttackPower, OldAttackPower); }
void UTSAttributeSet::OnRep_DefensePower(const FGameplayAttributeData& OldDefensePower) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, DefensePower, OldDefensePower); }
void UTSAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, MovementSpeed, OldMovementSpeed); }
void UTSAttributeSet::OnRep_TotalWeight(const FGameplayAttributeData& OldTotalWeight) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldTotalWeight); }

void UTSAttributeSet::OnRep_SurvivalSkill(const FGameplayAttributeData& OldSurvivalSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldSurvivalSkill); }
void UTSAttributeSet::OnRep_MovementSkill(const FGameplayAttributeData& OldMovementSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldMovementSkill); }
void UTSAttributeSet::OnRep_LootingSkill(const FGameplayAttributeData& OldLootingSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldLootingSkill); }
void UTSAttributeSet::OnRep_MakingSkill(const FGameplayAttributeData& OldMakingSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldMakingSkill); }
void UTSAttributeSet::OnRep_FarmingSkill(const FGameplayAttributeData& OldFarmingSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldFarmingSkill); }
void UTSAttributeSet::OnRep_BuildingSkill(const FGameplayAttributeData& OldBuidlingSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldBuidlingSkill); }
void UTSAttributeSet::OnRep_AimingSkill(const FGameplayAttributeData& OldAimingSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldAimingSkill); }
void UTSAttributeSet::OnRep_BattleSkill(const FGameplayAttributeData& OldBattleSkill) { GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, TotalWeight, OldBattleSkill); }

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

UTSAttributeSet::UTSAttributeSet()
{
	// 초기값 기본 세팅
	InitHealth(100.f);		InitMaxHealth(100.f);
	InitStamina(100.f);		InitMaxStamina(100.f);
	InitMentality(100.f);	InitMaxMentality(100.f);
	InitHunger(100.f);		InitMaxHunger(100.f);
	InitThirst(100.f);		InitMaxThirst(100.f);
	InitTemperature(36.5f); 

	InitStrength(10.f);
	InitAttackPower(10.f);
	InitDefensePower(10.f);
	InitMovementSpeed(500.f);
	InitTotalWeight(0.f);
}

void UTSAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// 모든 스탯 복제 등록
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Mentality, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxMentality, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxHunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Thirst, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MaxThirst, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Temperature, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, DefensePower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, TotalWeight, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, SurvivalSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MovementSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, LootingSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, MakingSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, FarmingSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, BuildingSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, AimingSkill, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, BattleSkill, COND_None, REPNOTIFY_Always);
	
}

void UTSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UTSAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 이펙트 적용 후 최종 클램핑 확실히 처리
	if (Data.EvaluatedData.Attribute == GetHealthAttribute()) SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute()) SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
	else if (Data.EvaluatedData.Attribute == GetMentalityAttribute()) SetMentality(FMath::Clamp(GetMentality(), 0.f, GetMaxMentality()));
	else if (Data.EvaluatedData.Attribute == GetHungerAttribute()) SetHunger(FMath::Clamp(GetHunger(), 0.f, GetMaxHunger()));
	else if (Data.EvaluatedData.Attribute == GetThirstAttribute()) SetThirst(FMath::Clamp(GetThirst(), 0.f, GetMaxThirst()));
}

#pragma endregion
//======================================================================================================================	