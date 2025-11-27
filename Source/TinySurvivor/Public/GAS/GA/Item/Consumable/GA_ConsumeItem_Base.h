// GA_ConsumeItem_Base.h
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Item/Data/ItemData.h"
#include "GA_ConsumeItem_Base.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitDelay;
class UItemDataSubsystem;
//struct FItemData;

/*
	소모품 아이템 사용을 위한 베이스 GameplayAbility
	
	사용 방법:
	1. 이 클래스를 부모로 하는 블루프린트 생성
	2. 블루프린트에서 ItemID 설정
	3. 블루프린트에서 GameplayEffect 설정
	4. DT_Item의 해당 아이템에 블루프린트 클래스 매핑
	
	동작 흐름:
	1. ItemID로 ItemDataSubsystem에서 아이템 정보 조회
	2. 몽타주 길이만큼 또는 ConsumptionTime만큼 대기 (0이면 스킵)
	3. GE_RestoreAttribute_Base를 적용하여 스탯 회복
	
	네트워크:
	- Server/Client 모두 동일한 정적 데이터 조회 가능
	- GameplayEffect 적용은 서버에서만 실행되므로 동기화 문제 없음
*/
UCLASS(Abstract, Blueprintable)
class TINYSURVIVOR_API UGA_ConsumeItem_Base : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ConsumeItem_Base();
	
#pragma region AbilityLifecycle
public:
	//========================================
	// Ability Lifecycle
	//========================================
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
#pragma endregion
	
#pragma region ItemConfiguration
protected:
	//========================================
	// Blueprint 설정 변수
	//========================================
	
	/*
		블루프린트에서 설정할 아이템 ID
		이 ID로 ItemDataSubsystem에서 모든 정보 자동 조회
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Configuration",
		meta = (DisplayName = "Item ID", ToolTip = "DT_Item에 정의된 아이템 ID"))
	int32 ItemID = 0;
	
	/*
		스탯 회복용 GameplayEffect 클래스
		블루프린트에서 GE_RestoreAttribute_Base를 상속한 클래스 설정
		
		일반적으로는 GE_RestoreAttribute_Base를 직접 사용하지만,
		커스텀 로직이 필요한 경우 블루프린트로 확장 가능
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Configuration",
		meta = (DisplayName = "Restore Effect Class", ToolTip = "스탯 회복용 GameplayEffect 클래스"))
	TSubclassOf<UGameplayEffect> RestoreEffectClass;
#pragma endregion
	
#pragma region InternalState
protected:
	//========================================
	// Internal State
	//========================================
	
	/*
		아이템 사용 중 상태 플래그
	*/
	bool bIsConsuming = false;
	
	/*
		조회된 아이템 정보 캐싱
	*/
	FItemData CachedItemData;
	
	/*
		움직임 체크 타이머 핸들
	*/
	//FTimerHandle MovementCheckTimerHandle;
	
	/*
		슬롯 인덱스 저장 (EventData에서 수신)
	*/
	int32 ConsumedSlotIndex = -1;
	
	/*
		Task 포인터 캐싱
	*/
	UPROPERTY()
	TWeakObjectPtr<UAbilityTask_PlayMontageAndWait> ActiveMontageTask;
	
	/*
		Task 포인터 캐싱
	*/
	UPROPERTY()
	TWeakObjectPtr<UAbilityTask_WaitDelay> ActiveDelayTask;
	
	/*
		Cancel 태그 목록
	*/
	UPROPERTY()
	TArray<FGameplayTag> CancelTags;
#pragma endregion
	
#pragma region Multicast
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayConsumeMontage(UAnimMontage* Montage, float ServerStartTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopConsumeMontage();
#pragma endregion
	
#pragma region LoadItemData
protected:
	
	/*
		ItemDataSubsystem에서 아이템 정보 조회
		@return 조회 성공 여부
	*/
	UFUNCTION(BlueprintCallable, Category = "Consume Item")
	bool LoadItemData();
#pragma endregion
	
#pragma region Consumption
protected:
	/*
		아이템 사용 시간 대기 (비동기)
	*/
	UFUNCTION(BlueprintCallable, Category = "Consume Item")
	void WaitForConsumption();
	
	/*
		ConsumptionTime 대기 완료 콜백
	*/
	UFUNCTION()
	void OnConsumptionCompleted();
	
	/*
		ConsumptionTime 대기 취소 콜백
	*/
	UFUNCTION()
	void OnConsumptionCancelled();
	
	/*
		외부 이벤트로 강제 취소
	*/
	void OnCancelTagChanged(const FGameplayTag Tag, int32 NewCount);
#pragma endregion
	
#pragma region CheckMovement
protected:
	/*
		움직임 감지 
	*/
	//void CheckMovement();
#pragma endregion
	
#pragma region EffectApplication
protected:
	/*
		GameplayEffect 적용하여 스탯 회복
		SetByCaller로 EffectValue 전달
	*/
	UFUNCTION(BlueprintCallable, Category = "Consume Item")
	void ApplyRestoreEffect();
#pragma endregion
	
#pragma region InventoryNotification
protected:
	/*
		인벤토리에 아이템 소비 알림 (GameplayEvent 발송)
	*/
	void NotifyItemConsumed();
#pragma endregion
	
#pragma region Debug
public:
	//========================================
	// 디버그 설정
	//========================================
	
	/*
		디버그용 로그 출력
	*/
	void LogItemInfo() const;
#pragma endregion
};