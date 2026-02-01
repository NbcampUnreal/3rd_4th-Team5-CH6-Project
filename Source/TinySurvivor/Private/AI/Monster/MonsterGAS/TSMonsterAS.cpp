#include "AI/Monster/MonsterGAS/TSMonsterAS.h"
#include "Net/UnrealNetwork.h"

UTSMonsterAS::UTSMonsterAS()
{
}

void UTSMonsterAS::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UTSMonsterAS, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSMonsterAS, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSMonsterAS, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTSMonsterAS, MonsterSpeed, COND_None, REPNOTIFY_Always);
}

void UTSMonsterAS::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UTSMonsterAS::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSMonsterAS, Health, OldHealth);
}

void UTSMonsterAS::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSMonsterAS, MaxHealth, OldMaxHealth);
}

void UTSMonsterAS::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSMonsterAS, AttackDamage, OldAttackDamage);
}

void UTSMonsterAS::OnRep_MonsterSpeed(const FGameplayAttributeData& OldMonsterSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTSMonsterAS, MonsterSpeed, OldMonsterSpeed);
}
