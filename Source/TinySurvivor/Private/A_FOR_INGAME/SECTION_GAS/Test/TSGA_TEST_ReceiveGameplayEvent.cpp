// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/Test/TSGA_TEST_ReceiveGameplayEvent.h"
#include "A_FOR_COMMON/Library/Debug/Net/TSDebugNetLibrary.h"
#include "A_FOR_COMMON/Tag/Input/TSInputTag.h"

UTSGA_TEST_ReceiveGameplayEvent::UTSGA_TEST_ReceiveGameplayEvent()
{
	// 인스턴스
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 에셋
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(InputTag::LeftClick::TS_Input_LeftClick_Default);
	SetAssetTags(AssetTags);
	
	// 부여 태그
	ActivationOwnedTags.AddTag(InputTag::LeftClick::TS_Input_LeftClick_Default);
	
	// 트리커 설정
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = InputTag::LeftClick::TS_Input_LeftClick_Default;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	// 트리거 추가
	AbilityTriggers.Add(TriggerData);
}

void UTSGA_TEST_ReceiveGameplayEvent::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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
		
		const UObject* ReceivedObj = TriggerEventData->OptionalObject;
		FString ObjName = ReceivedObj ? ReceivedObj->GetName() : TEXT("NULL");

		// 2. 로그 출력
		UE_LOG(LogTemp, Warning, TEXT("[TS_Payload_Test] Magnitude: %f"), TriggerEventData->EventMagnitude);
		UE_LOG(LogTemp, Warning, TEXT("[TS_Payload_Test] OptionalObject Name: %s"), *ObjName);
		
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[TS_Debug] C++ Trigger Failed!"));
		UTSDebugNetLibrary::CheckCurrentNetContext(this);
		UTSDebugNetLibrary::GetActorNetworkRoles(GetAvatarActorFromActorInfo());
	}
	
	K2_EndAbility();
}
