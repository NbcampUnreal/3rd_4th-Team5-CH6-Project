// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GA/LeftClick/DefaultAction/OnTarget/TSGA_LCDefaultOnResource.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "A_FOR_COMMON/Library/Debug/Net/TSDebugNetLibrary.h"
#include "A_FOR_COMMON/Tag/Input/TSInputTag.h"

UTSGA_LCDefaultOnResource::UTSGA_LCDefaultOnResource()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	// 에셋 태그
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnResource);
	SetAssetTags(AssetTags);
	
	// 부여 태그
	ActivationOwnedTags.AddTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnResource);
	
	// 트리커 설정
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnResource;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	// 트리거 추가
	AbilityTriggers.Add(TriggerData);
}

void UTSGA_LCDefaultOnResource::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!TriggerEventData) { K2_EndAbility(); return; }
	
	// 애니메이션 재생 
	PlayStandardMontageAndWait(OnResourceMontage);
	
	// 실제 상호작용 로직 (서버에서만 실행)
	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		if (TriggerEventData)
		{
			UE_LOG(LogTemp, Log, TEXT("[TS_Debug] C++ Trigger Success! Event Tag: %s"), *TriggerEventData->EventTag.ToString());
			UTSDebugNetLibrary::CheckCurrentNetContext(this);
			UTSDebugNetLibrary::GetActorNetworkRoles(GetAvatarActorFromActorInfo());
		
			if (IsValid(TriggerEventData->Target))
			{
				UE_LOG(LogTemp, Log, TEXT("[TS_Debug] Target Actor: %s"), *TriggerEventData->Target->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[TS_Debug] Target Actor is NULL!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[TS_Debug] C++ Trigger Failed!"));
			UTSDebugNetLibrary::CheckCurrentNetContext(this);
			UTSDebugNetLibrary::GetActorNetworkRoles(GetAvatarActorFromActorInfo());
		}
	}
}
