// TSAttributeSet.cpp
#include "GAS/AttributeSet/TSAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Character/TSCharacter.h"
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
	
	InitSanity(70.0f);
	InitMaxSanity(100.0f);
	
	InitTemperature(36.5f); 
	InitMaxTemperature(100.0f); 
	
	InitMoveSpeed(600.f);
	InitMaxMoveSpeed(1000.0f);
	
	InitBaseDamage(10.0f);		// 기본 공격력
	InitDamageBonus(0.0f);		// 무기 추가 공격력 (초기값 0)
	
	InitBaseAttackSpeed(1.0f);	// 기본 공격속도
	InitAttackSpeedBonus(1.0f);	// 무기 추가 속도, 적용 전 기본 배율 1
	
	InitBaseAttackRange(100.0f);	// 기본 공격반경
	InitAttackRangeBonus(1.0f);	// 무기 추가 반경, 적용 전 기본 배율 1
	
	InitBaseDamageReduction(0.0f);	// 0% 기본
	InitDamageReductionBonus(0.0f);	// 장비/버프 추가
	
	InitBaseDamageReflection(0.0f);	// 0% 기본
	InitDamageReflectionBonus(0.0f);	// 장비/버프 추가
	
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
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, DamageBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, BaseAttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, AttackSpeedBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, BaseAttackRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, AttackRangeBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, BaseDamageReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, DamageReductionBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, BaseDamageReflection, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSAttributeSet, DamageReflectionBonus, COND_None, REPNOTIFY_Always);
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
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	AActor* AvatarActor = ASC ? ASC->GetAvatarActor() : nullptr;
	ATSCharacter* Char = Cast<ATSCharacter>(AvatarActor);
	if  (!Char)
	{
		return;
	}
	
	if (Data.EvaluatedData.Attribute == GetThirstAttribute())
	{		
		FGameplayTag ThirstTag = AbilityTags::TAG_State_Status_Thirst;
		if (GetThirst() <= 0.0f)
		{
			if (!ASC->HasMatchingGameplayTag(ThirstTag))
			{
				if (Char->ThirstTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->ThirstTagEffectClass, 1, ContextHandle);
            
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}else
		{
			if (ASC->HasMatchingGameplayTag(ThirstTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(ThirstTag));
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetHungerAttribute())
	{
		FGameplayTag HungerTag = AbilityTags::TAG_State_Status_Hunger;
		if (GetHunger() <= 0.0f)
		{
			if (!ASC->HasMatchingGameplayTag(HungerTag))
			{
				if (Char->HungerTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->HungerTagEffectClass, 1, ContextHandle);
            
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}else
		{
			if (ASC->HasMatchingGameplayTag(HungerTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(HungerTag));
			}
		}
		
		// Full 상태 처리
		FGameplayTag FullTag = AbilityTags::TAG_State_Status_Full;
		if (GetHunger() >= 100.0f)
		{
			// 배부름 태그 부착
			if (!ASC->HasMatchingGameplayTag(FullTag))
			{
				if (Char->FullTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->FullTagEffectClass, 1, ContextHandle);
            
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
			if (Char  && Char->FullRecoverHealthEffectClass)
			{
				FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				ContextHandle.AddSourceObject(this);
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->FullRecoverHealthEffectClass, 1, ContextHandle);
            
				if (SpecHandle.IsValid())
				{
					ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		} else
		{
			if (ASC->HasMatchingGameplayTag(FullTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FullTag));
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetTemperatureAttribute())
	{
		FGameplayTag TempHotTag = AbilityTags::TAG_State_Status_TempHot;
		FGameplayTag TempColdTag = AbilityTags::TAG_State_Status_TempCold;
		
		if (GetTemperature() >= 40.0f)
		{
			// 더위 상태이상 -> Hot 태그 붙이고 Cold 태그 제거
			if (!ASC->HasMatchingGameplayTag(TempHotTag))
			{
				if (Char->TempHotTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->TempHotTagEffectClass, 1, ContextHandle);
         
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
			if (ASC->HasMatchingGameplayTag(TempColdTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(TempColdTag));
			}
		}else if (GetTemperature() <= 30.0f)
		{
			// 추위 상태이상 -> Cold 태그 붙이고 Hot 태그 제거 
			if (!ASC->HasMatchingGameplayTag(TempColdTag))
			{
				if (Char->TempColdTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->TempColdTagEffectClass, 1, ContextHandle);
         
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
			if (ASC->HasMatchingGameplayTag(TempHotTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(TempHotTag));
			}
		} else //정상 범위면 둘 다 제거
		{
			if (ASC->HasMatchingGameplayTag(TempHotTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(TempHotTag));
			}

			if (ASC->HasMatchingGameplayTag(TempColdTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(TempColdTag));
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (GetHealth() <= 0.0f)
		{
			// 캐릭터 기절
			if (Char && !Char -> IsDowned()) 
			{
				Char -> BecomeDowned();
			}
		}
	}
	if (Data.EvaluatedData.Attribute == GetSanityAttribute())
	{
		FGameplayTag AnxietyTag = AbilityTags::TAG_State_Status_Anxiety;
		FGameplayTag PanicTag = AbilityTags::TAG_State_Status_Panic;
		FGameplayTag SanityBlockTag = AbilityTags::TAG_State_Sanity_InLightBlock;
		
		if (GetSanity() <= 0.0f) // 정신력 0 -> 패닉 상태
		{
			// 블락 태그 갖고있다면 제거
			if (ASC->HasMatchingGameplayTag(SanityBlockTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(SanityBlockTag));
			}
			// 불안 태그 갖고있다면 제거
			if (ASC->HasMatchingGameplayTag(AnxietyTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AnxietyTag));
			}
			// 패닉 태그 안갖고 있다면 부착
			if (!ASC->HasMatchingGameplayTag(PanicTag))
			{
				if (Char->PanicTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->PanicTagEffectClass, 1, ContextHandle);
         
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		} else if (GetSanity() > 0.0f && GetSanity() <= 30.0f) // 정신력 1 ~30 -> 불안 상태
		{
			// 블락 태그 갖고있다면 제거
			if (ASC->HasMatchingGameplayTag(SanityBlockTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(SanityBlockTag));
			}
			// 패닉 태그 갖고있다면 제거
			if (ASC->HasMatchingGameplayTag(PanicTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(PanicTag));
			}
			// 불안 태그 안갖고 있다면 부착
			if (!ASC->HasMatchingGameplayTag(AnxietyTag))
			{
				if (Char->AnxietyTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->AnxietyTagEffectClass, 1, ContextHandle);
         
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		} else if (GetSanity() > 30.0f) // 30 초과 -> 정상 범위
		{
			// 패닉 불안 갖고있다면 태그 제거
			if (ASC->HasMatchingGameplayTag(PanicTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(PanicTag));
			}
			if (ASC->HasMatchingGameplayTag(AnxietyTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AnxietyTag));
			}
		}


		if (GetSanity() >= 80.0f)
		{
			if (!ASC->HasMatchingGameplayTag(SanityBlockTag))
			{
				if (Char->SanityBlockTagEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Char->SanityBlockTagEffectClass, 1, ContextHandle);
         
					if (SpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
		else
		{
			if (ASC->HasMatchingGameplayTag(SanityBlockTag))
			{
				ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(SanityBlockTag));
			}
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
	else if (Attribute == GetBaseDamageAttribute())
	{// 공격력은 0 이상 가능
		NewValue = FMath::Max(0.0f, NewValue);
	}
	else if (Attribute == GetBaseAttackSpeedAttribute())
	{// 공격 속도는 0이면 공격 불가이므로 최소 0.01로 제한
		NewValue = FMath::Max(0.01f, NewValue);
	}
	else if (Attribute == GetBaseAttackRangeAttribute())
	{// 공격 반경도 0이면 공격 불가, 최소 1로 제한
		NewValue = FMath::Max(1.0f, NewValue);
	}
	// Bonus는 음수 허용 (디버프 가능)
	else if (Attribute == GetBaseDamageReductionAttribute() ||
		 Attribute == GetDamageReductionBonusAttribute() ||
		 Attribute == GetBaseDamageReflectionAttribute() ||
		 Attribute == GetDamageReflectionBonusAttribute())
	{
		NewValue = FMath::Clamp(NewValue, -1.0f, 1.0f); 
		// 또는 필요 시 0~1만 허용
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

void UTSAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, BaseDamage, OldBaseDamage);
}

void UTSAttributeSet::OnRep_DamageBonus(const FGameplayAttributeData& OldDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, DamageBonus, OldDamageBonus);
}

void UTSAttributeSet::OnRep_BaseAttackSpeed(const FGameplayAttributeData& OldBaseAttackSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, BaseAttackSpeed, OldBaseAttackSpeed);
}

void UTSAttributeSet::OnRep_AttackSpeedBonus(const FGameplayAttributeData& OldAttackSpeedBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, AttackSpeedBonus, OldAttackSpeedBonus);
}

void UTSAttributeSet::OnRep_BaseAttackRange(const FGameplayAttributeData& OldBaseAttackRange)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, BaseAttackRange, OldBaseAttackRange);
}

void UTSAttributeSet::OnRep_AttackRangeBonus(const FGameplayAttributeData& OldAttackRangeBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, AttackRangeBonus, OldAttackRangeBonus);
}

void UTSAttributeSet::OnRep_BaseDamageReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, BaseDamageReduction, OldValue);
}

void UTSAttributeSet::OnRep_DamageReductionBonus(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, DamageReductionBonus, OldValue);
}

void UTSAttributeSet::OnRep_BaseDamageReflection(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, BaseDamageReflection, OldValue);
}

void UTSAttributeSet::OnRep_DamageReflectionBonus(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSAttributeSet, DamageReflectionBonus, OldValue);
}