// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GA/Inventory/A_Base/TSGA_TryAddItemBase.h"

//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

UTSGA_TryAddItemBase::UTSGA_TryAddItemBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UTSGA_TryAddItemBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 습득 몽타주 실행 
	
	
	// 실제 추가 로직은 서버에서만 
	if (!IsValid(GetAvatarActorFromActorInfo()) && !GetAvatarActorFromActorInfo()->HasAuthority())
	{
		K2_EndAbility();
		return;
	}
	
	// 타겟 체크 
	if (CheckValidAbilityAndTarget(TriggerEventData) == false)
	{
		K2_EndAbility();
		return;
	}
	
	// 2. 인벤토리 컴포넌트 추출
	//TargetActor->
	
}

#pragma endregion
//======================================================================================================================	
#pragma region 내부 동작 API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 몽타주
	//━━━━━━━━━━━━━━━━━━━━

bool UTSGA_TryAddItemBase::CheckValidAbilityAndTarget(const FGameplayEventData* InTriggerEventData)
{
	// 페이로드 체크 
	if (!InTriggerEventData) return false;
	
	// 타겟 체크
	if (!IsValid(InTriggerEventData->Target) || !IsValid(InTriggerEventData->Target.Get())) return false;
	
	// 캐싱 체크 
	TargetActor = const_cast<AActor*>(InTriggerEventData->Target.Get());
	if (!IsValid(TargetActor)) return false;
	
	return true;
}

#pragma endregion
//======================================================================================================================	
#pragma region 몽타주
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 몽타주
	//━━━━━━━━━━━━━━━━━━━━

#pragma endregion
//======================================================================================================================	