// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_GAS/GA/LeftClick/TSGA_CharacterLeftClick.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/HotKey/TSHotKeyInventoryComponent.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Interface/TSPlayerCompGetterInterface.h"
#include "A_FOR_COMMON/Tag/Input/TSInputTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

UTSGA_CharacterLeftClick::UTSGA_CharacterLeftClick()
{
	// 에셋 태그
	FGameplayTagContainer AssetTags;
	AssetTags.AddTag(InputTag::LeftClick::TS_Input_LeftClickCall);
	SetAssetTags(AssetTags);
	
	// 부여 태그
	ActivationOwnedTags.AddTag(InputTag::LeftClick::TS_Input_LeftClickCall);
	
	// 인스턴싱, 네트워크 실행, 네트워크 보안 실행 규칙 
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UTSGA_CharacterLeftClick::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 캐릭터 클래스 체크
	ACharacter* AbilityOwner = CastChecked<ACharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(AbilityOwner)) { K2_EndAbility(); return; }
	
	// 플레이어 인터페이스 체크
	ITSPlayerCompGetterInterface* InterfaceInstance = Cast<ITSPlayerCompGetterInterface>(AbilityOwner);
	if (!InterfaceInstance) { K2_EndAbility(); return; }

	// 핫키 컴포넌트 체크 
	UTSHotKeyInventoryComponent* HotKeyInventoryComponent = ITSPlayerCompGetterInterface::Execute_GetHotKeyInventoryComponent(AbilityOwner);
	if (!IsValid(HotKeyInventoryComponent)) { K2_EndAbility(); return; }
	
	// 현재 활성화되어 있는 핫키가 있는가?
	if (HotKeyInventoryComponent->IsAnyHotKeyActive() == false)
	{
		// 기본 좌클릭 어빌리티 입력  
		GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_Default.GetTag().GetSingleTagContainer());
	}
	else
	{
		// 핫키 사용 어빌리티 입력
		GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(InputTag::LeftClick::TS_Input_LeftClick_HotKey.GetTag().GetSingleTagContainer());
	}
	
	K2_EndAbility();
	return;
}

/*
현재 플레이어의 상태 체크할 목록 
	
< 우클릭을 할 수 있는 상태인가? >
1. 차단 태그로 설정 : 우클릭 발동 자체가 불가능한 상태. ex) 기절함, 죽음, 건축 모드, 상저 모드 등등.
2. possess에 의한 설정 : 자동차를 타고 있으면 posses 가 바뀌어 다른 left Click 가 발동됨. ex) 자동차 기관총 발사 

3. 현재 활성화되어 있는 핫키가 있는가?

3-a : 현재 활성화되어 있는 핫키가 없습니다. 	
설명 -> 들고 있는 아이템이 없다는 의미 = 기본 공격 어빌리티가 발동되어야 한다.
:: NPC, 몬스터, 플레이어, 자원, 거인, 건축물, 가구 등등은 해당 어빌리티 내에서 switch 를 통해서 다르게 발동하며.
공통된 상호작용 인터페이스를 상속하고 내부 API를 다르게 설정하여 같은 자원, NCP, 몬스터, 건축물 등등이라도 다르게 반응하도록 설정.

3-b. 현재 보고 있는 타겟의 역할이 무엇인가?
설명 -> 들고 있는 아이템이 있다는 의미 = 아이템을 사용(Left Click) 하면 발동되는 어빌리티가 발동되어야 한다.
:: NPC, 몬스터, 플레이어, 자원, 거인, 건축물, 가구 등등은 해당 어빌리티 내에서 switch 를 통해서 다르게 발동하며.
공통된 상호작용 인터페이스를 상속하고 내부 API를 다르게 설정하여 같은 자원, NCP, 몬스터, 건축물 등등이라도 다르게 반응하도록 설정.
	
**** NOTE *****
현재 어떤 타겟을 보고 있든, 보고 있는 타겟이 없든 (인터렉트의 CurrentWatchingTargetActor == nullptr) 간에 상관없이
이 어빌리티는 3-a, 또는 3-b에 따라 발동되는 어빌리티가 알아서 확인한다.
	
*/