// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/ActorComponent.h"
#include "TSInventoryGASControlComponent.generated.h"


class UTSInventoryMasterComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSInventoryGASControlComponent : public UActorComponent
{
	GENERATED_BODY()
	
	friend class UTSItemUseHelperLibrary;

//======================================================================================================================
#pragma region 라이프_사이클
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	
	UTSInventoryGASControlComponent();
	virtual void BeginPlay() override;
	
	
#pragma endregion
//======================================================================================================================
#pragma region GAS_관련_API

	
	//━━━━━━━━━━━━━━━━━━━━
	// GAS 관련 API
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	
	// ASC 준비되면 이벤트 리스너 등록
	void TryRegisterEventListeners_internal();
    
	// 리스너 이벤트 함수들
	void OnItemConsumedEvent_internal(const FGameplayEventData* GameplayEventData);
	void OnToolHarvestEvent_internal(const FGameplayEventData* GameplayEventData);
	void OnWeaponAttackEvent_internal(const FGameplayEventData* GameplayEventData);
	void OnArmorHitEvent_internal(const FGameplayEventData* GameplayEventData);
	
#pragma endregion	
//======================================================================================================================
#pragma region GAS_관련_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// GAS 관련 데이터
	//━━━━━━━━━━━━━━━━━━━━
	
private:
	// ASC 초기화 대기 중 플래그
	bool bEventListenersRegistered = false;
	
	FGameplayAbilitySpecHandle ActiveConsumableAbilityHandle;	// 현재 활성화된 소모품 Ability의 SpecHandle
	FTimerHandle ConsumableAbilityTriggerTimer;					// Timer 취소용 핸들
	FTimerHandle ASCCheckTimerHandle;							// 타이머 핸들
	
	
#pragma endregion
//======================================================================================================================	
#pragma region 관리할_인벤토리


	//━━━━━━━━━━━━━━━━━━━━
	// 관리할_인벤토리
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	
	// 관리할 인벤토리 마스터 컴포넌트
	UPROPERTY()
	UTSInventoryMasterComponent* InventoryMasterComp = nullptr;
	
	
#pragma endregion
//======================================================================================================================
};
