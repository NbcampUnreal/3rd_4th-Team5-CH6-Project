// MonsterAttributeSet.cpp

#include "AI/Common/MonsterAttributeSet.h"
#include "AI/Common/TSAICharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UMonsterAttributeSet::UMonsterAttributeSet()
{
	// 기본값 초기화
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitAttackDamage(10.0f);
	InitErosionReward(5.0f);
}

// 변수 동기화 등록
void UMonsterAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UMonsterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMonsterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMonsterAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMonsterAttributeSet, ErosionReward, COND_None, REPNOTIFY_Always);
}

// 사망 판정
void UMonsterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 변경된 속성이 체력
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 다시 한번 값 제한
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		
		float DamageTaken = Data.EvaluatedData.Magnitude;
		
		if (DamageTaken < 0.0f)
		{
			AActor* TargetActor = Data.Target.GetAvatarActor();
			ATSAICharacter* Monster = Cast<ATSAICharacter>(TargetActor);
			
			if (Monster)
			{
				AActor* Attacker = Data.EffectSpec.GetEffectContext().GetInstigator();
				
				FGameplayTagContainer AssetTags;
				Data.EffectSpec.GetAllAssetTags(AssetTags);
				
				Monster->OnDamaged(FMath::Abs(DamageTaken), AssetTags, Attacker);
			}
		}
		
		// 체력이 0 이하가 되면 사망 처리
		if (GetHealth() <= 0.0f)
		{
			ATSAICharacter* TargetMonster = Cast<ATSAICharacter>(Data.Target.GetAvatarActor());
			
			if (TargetMonster)
			{
				// 막타 친 플레이어
				AActor* KillerActor = Data.EffectSpec.GetEffectContext().GetInstigator();
				
				// 몬스터에게 사망 함수 호출
				TargetMonster->OnDeath(KillerActor);
			}
		}
	}
}

// 클라이언트에서 값이 갱신되었을 때 GAS 시스템에 알림
void UMonsterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMonsterAttributeSet, Health, OldHealth);
}

void UMonsterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMonsterAttributeSet, MaxHealth, OldMaxHealth);
}

void UMonsterAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMonsterAttributeSet, AttackDamage, OldAttackDamage);
}

void UMonsterAttributeSet::OnRep_ErosionReward(const FGameplayAttributeData& OldErosionReward)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMonsterAttributeSet, ErosionReward, OldErosionReward);
}
