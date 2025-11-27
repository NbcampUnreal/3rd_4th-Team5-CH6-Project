// GEC_RemovePoisonTag.cpp
#include "GAS/GE/Item/Consumable/GEC_RemovePoisonTag.h"

#include "AbilitySystemComponent.h"
#include "GameplayTags/AbilityGameplayTags.h"

// 로그 카테고리 정의
DEFINE_LOG_CATEGORY_STATIC(LogRemovePoisonTagExecution, Log, All);

UGEC_RemovePoisonTag::UGEC_RemovePoisonTag()
{
	// 생성자에서 특별한 설정 불필요
}

void UGEC_RemovePoisonTag::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UE_LOG(LogRemovePoisonTagExecution, Warning, TEXT("========================================"));
	UE_LOG(LogRemovePoisonTagExecution, Warning, TEXT("[Antidote] GEC Execute 호출됨!!!"));
	UE_LOG(LogRemovePoisonTagExecution, Warning, TEXT("========================================"));
   
	// Target ASC 가져오기
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!TargetASC)
	{
		// TargetASC가 없을 경우
		UE_LOG(LogRemovePoisonTagExecution, Warning, TEXT("[Antidote] TargetASC가 null입니다."));
		return;
	}
	
	// 독 태그 정의
	FGameplayTag PoisonTag = AbilityTags::TAG_State_Status_Poison;
	
	// // 독 상태 태그가 있으면 제거
	// if (TargetASC->HasMatchingGameplayTag(PoisonTag))
	// {
	// 	UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 독 상태 태그 발견"));
	// 	
	// 	// 방법 1: Loose 태그 제거
	// 	TargetASC->RemoveLooseGameplayTag(PoisonTag);
	// 	UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 독 Loose 태그 제거"));
	// 	
	// 	// 방법 2: 독 GE가 있다면 제거 (나중에 독 시스템 구현 시)
	// 	FGameplayTagContainer PoisonTags;
	// 	PoisonTags.AddTag(PoisonTag);
	// 	TargetASC->RemoveActiveEffectsWithTags(PoisonTags);
	// 	UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 독 GE 제거 완료"));
	// 	
	// 	UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 독 상태 제거 완료"));
	// }
	// else
	// {// 독 상태 태그가 없는 경우
	// 	UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 독 상태 태그가 없습니다."));
	// }
	
	// 현재 태그 확인
	FGameplayTagContainer CurrentTags;
	TargetASC->GetOwnedGameplayTags(CurrentTags);
	UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 현재 태그: %s"),
			*CurrentTags.ToStringSimple());
	
	if (TargetASC->HasMatchingGameplayTag(PoisonTag))
	{
		UE_LOG(LogRemovePoisonTagExecution, Warning, TEXT("[Antidote] 독 태그 발견! 제거 시작"));
		
		// 1. Loose 태그 제거
		TargetASC->RemoveLooseGameplayTag(PoisonTag);
		UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] Loose 태그 제거 완료"));
		
		// 2. 독 GE 제거
		FGameplayTagContainer PoisonTags;
		PoisonTags.AddTag(PoisonTag);
		int32 RemovedCount = TargetASC->RemoveActiveEffectsWithTags(PoisonTags);
		UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 독 GE %d개 제거"), RemovedCount);
		
		// 제거 후 태그 확인
		TargetASC->GetOwnedGameplayTags(CurrentTags);
		UE_LOG(LogRemovePoisonTagExecution, Log, TEXT("[Antidote] 제거 후 태그: %s"),
				*CurrentTags.ToStringSimple());
	}
	else
	{
		UE_LOG(LogRemovePoisonTagExecution, Warning, TEXT("[Antidote] 독 태그 없음"));
	}
}