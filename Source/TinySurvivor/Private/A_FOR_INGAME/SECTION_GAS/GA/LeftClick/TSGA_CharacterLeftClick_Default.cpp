// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GA/LeftClick/TSGA_CharacterLeftClick_Default.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Interface/TSPlayerCompGetterInterface.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Comp/Player/TSPlayerInteractComponent.h"
#include "A_FOR_INGAME/SECTION_INTERACT/Interface/TSCommonInteractInterface.h"
#include "A_FOR_COMMON/Tag/Input/TSInputTag.h"
#include "AbilitySystemComponent.h"

UTSGA_CharacterLeftClick_Default::UTSGA_CharacterLeftClick_Default()
{
	// 에셋 태그
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(InputTag::LeftClick::TS_Input_LeftClick_Default);
	SetAssetTags(AssetTags);
	
	// 부여 태그
	ActivationOwnedTags.AddTag(InputTag::LeftClick::TS_Input_LeftClick_Default);
	
	// 인스턴싱, 네트워크 실행, 네트워크 보안 실행 규칙 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UTSGA_CharacterLeftClick_Default::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 인터렉트 컴포넌트 체크
	UTSPlayerInteractComponent* InteractComponent = ITSPlayerCompGetterInterface::Execute_GetPlayerInteractComponent(GetAvatarActorFromActorInfo());
	if (!IsValid(InteractComponent)) { K2_EndAbility(); return; }
	
	// 현재 보고 있는 액터가 있을 경우 
	if (IsValid(InteractComponent->GetCurrentInteractActor()))
	{
		// 공통 인터페이스 확인
		ITSCommonInteractInterface* CommonInteractInterface = Cast<ITSCommonInteractInterface>(InteractComponent->GetCurrentInteractActor());
		if (!CommonInteractInterface) { K2_EndAbility(); return; }
		
		switch (CommonInteractInterface->GetPlayRole())
		{
		case ETSPlayRole::None:
			break;
			
		case ETSPlayRole::Player:
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnPlayer.GetTag().GetSingleTagContainer());
			break;
			
		case ETSPlayRole::Giant:
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnGiant.GetTag().GetSingleTagContainer());
			break;
			
		case ETSPlayRole::Monster:
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnMonster.GetTag().GetSingleTagContainer());
			break;
			
		case ETSPlayRole::Resource:
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnResource.GetTag().GetSingleTagContainer());
			break;
			
		case ETSPlayRole::Item:
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction_OnItem.GetTag().GetSingleTagContainer());
			break;
		}
	}
	// 현재 보고 있는 액터가 없을 경우 
	else
	{
		GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_DefaultAction.GetTag().GetSingleTagContainer());
	}
	
	K2_EndAbility(); 
	return;
}

/*
// *********설명 *********
CharacterLefClick 어빌리티 발동에 의해 이 기본 좌클릭 어빌리티가 실행됨.
이 어빌리티가 실행되는 것은 다음과 같은 전체를 기본으로 함.
1. 특수 상황이 아니다 : 죽었다. 기절했다, 건축모드 On 같은 거. -> CharacterLeftClick 에서 거름.
2. 현재 활성화된 핫 키가 없다. -> CharacterLefClick 에서 거름.

자세한 설명 

----------------------------------
1. 현재 보고 있는 액터가 있냐? (playerInteractComp 의 CurrentTarget 이 null 이냐?)
설명 : 만약 현재 보고 있는 액터가 있다면 그 액터의 PlayRoel 타입에 따라서 실행해야하는 기본 좌클릭 어빌리티가 다를 수 있음.

예시 [1] 보고 있는 액터가 "아이템" 이라고 가정해보자.
플레이어는 좌클릭을 통해서 아이템의 Interact 인터페이스를 통해서 아이템마다 각기 다른 인터페이스를 실행시킬 수 있음.
그러나 "아이템"이라는 액터에 대한 공통적인 부분이 존재할 수 있음. 애니메이션 몽타주, VFX 등등.

예시 [2] 보고 있는 액터가 "기믹 (ex 비밀문)" 이라고 가정해보자. 
플레이너는 좌클릭을 통해서 기믹의 interact 인퍼에시를 통해서 기믹마다 각기 다른 인터페이스를 실행시킬 수 있음.
하지만 기믹이기 때문에 플레이어가 소통했다. 라는 것을 게임 플레이 메시지로 전부 알려야할 수 있음.
이 경우 이 LeftDefault 에 모든 코드를 넣게 되면 코드가 늘어남. 모듈화가 실패함.

따러서 1의 경우에는 Switch문을 통해서 PlayRole에 따라 분류해서 기본 어빌리티를 실행함.
----------------------------------


----------------------------------
1-1. 현재 보고 있는 액터가 없음.
설명 : 만약 현재 보고 있는 액터가 없다면 솔직히 뭐 아무것도 할 필요가 없음. 
따라서 그냥 기본 좌클릭 몽타주를 실행해도 됨. 하지만 여기서 실행하지 않음. 
이 역시 모듏화를 위해서임.
----------------------------------


*/