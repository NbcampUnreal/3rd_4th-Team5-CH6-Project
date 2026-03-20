// All CopyRight From YulRyongGameStudio //


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Struct/TSInventorySlot.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/ItemData.h"
#include "TSInventoryMasterComponent.generated.h"

#pragma region 전방선언
class UGameplayTagDisplaySubsystem;
class ATSPlayerController;
class ATSEquippedItem;
struct FItemData;
struct FItemInstance;
class UItemDataSubsystem;
class UAbilitySystemComponent;
struct FEquippedArmor;
#pragma endregion

#pragma region 델리게이트_선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryInitialized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventorySlotUpdated, const FInventoryStructMaster&, HotkeyInventory, const FInventoryStructMaster&, EquipmentInventory, const FInventoryStructMaster&, BagInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHotkeyActivated, int32, SlotIndex, const FSlotStructMaster&, ActiveSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBagSizeChanged, int32, NewSize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, int32, ItemID, int32, Quantity);
#pragma endregion

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSInventoryMasterComponent : public UActorComponent
{
	GENERATED_BODY()

	
	
	
	//```````````````````````
	// 게터, 델리게이트, Rep 섹션
	//.......................
	
//======================================================================================================================
#pragma region 델리게이트

	
	//━━━━━━━━━━━━━━━━━━━━
	// 델리게이트
	//━━━━━━━━━━━━━━━━━━━━
public:
	
	UPROPERTY(BlueprintAssignable) FOnInventoryInitialized OnInventoryInitialized;
	UPROPERTY(BlueprintAssignable) FOnInventorySlotUpdated OnInventoryUpdated;
	UPROPERTY(BlueprintAssignable) FOnHotkeyActivated OnHotkeyActivated;
	UPROPERTY(BlueprintAssignable) FOnBagSizeChanged OnBagSizeChanged;
	UPROPERTY(BlueprintAssignable) FOnItemAdded OnItemAdded;

	
#pragma endregion
//======================================================================================================================
#pragma region REP_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━
protected:

	UFUNCTION() void OnRep_ActiveHotkeyIndex();
	UFUNCTION() void OnRep_HotkeyInventory();
	UFUNCTION() void OnRep_EquipmentInventory();
	UFUNCTION() void OnRep_BagInventory();
	UFUNCTION() void OnRep_CurrentEquippedItem();
	
	
#pragma endregion	
//======================================================================================================================
	
	
	
	
	//```````````````````````
	// 사이클 섹션
	//.......................
	
//======================================================================================================================
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━
public:
	
	UTSInventoryMasterComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
#pragma endregion
//======================================================================================================================

	
	
	
	//```````````````````````
	// API 섹션
	//.......................
	
//======================================================================================================================	
#pragma region UI_관련
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// UI 관련
	//━━━━━━━━━━━━━━━━━━━━
protected:
	
	// 아이템 습득 HUD 표시용 이벤트 브로드캐스트
	UFUNCTION(Client, Reliable)
	void ClientRPC_NotifyItemAdded_internal(int32 ItemID, int32 Quantity);

	
#pragma endregion
//======================================================================================================================
#pragma region 아이템_관련_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// 아이템 관련 API
	//━━━━━━━━━━━━━━━━━━━━

	
	//--------------------
	// 아이템 사용 
	//--------------------
	
public:
	// 아이템 사용 요청 외부 API 
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerUseItem(int32 SlotIndex);
	
protected:
	// 아이템 사용 처리 내부 API
	void UseItem_internal(int32 SlotIndex);
	
	// 가방 아이템 사용 시 액션 함수 
	void ActionWithBagItem_internal(FSlotStructMaster& InTargetSlot);
	
	// 소모품 사용 시 액션 함수
	void ActionWithConsumableItem_internal(FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex);

	// 방어구 사용 시 액션 함수
	void ActionWithArmorItem_internal(FSlotStructMaster& InTargetSlot, int32& InTargetSlotIndex);

	//--------------------
	// 아이템 소비
	//--------------------

public:
	// 아이템 소비 API 
	void ConsumeItem(int32 StaticDataID, int32 Quantity);
	
	//--------------------
	// 아이템 장착
	//--------------------

protected:
	
	// 아이템을 장착하고 있는지 체크
	bool HasItemEquipped_internal() const;
	
	// 핫키 버튼에 따른 아이템 창작
	void EquipActiveHotkeyItem_internal();

	// 핫키 버튼에 따른 아이템 장착 해제
	void UnequipCurrentItem_internal();

	// 장착 아이템 변환 발생 시 호출
	void HandleCurrentEquippedItemChanged_internal();

	//--------------------
	// 방어구
	//--------------------

protected:
		
	// 방어구 장착
	void EquipArmor_internal(const FItemData& ItemInfo, int32 ArmorSlotIndex);
	
	// 방어구 해제
	void UnequipArmor_internal(int32 ArmorSlotIndex);
	
	// 방어구 피격 이벤트 수신 함수
	void OnArmorHitEvent_internal(const FGameplayEventData* Payload);
	
	//--------------------
	// 무기 
	//--------------------
	
protected:
	
	// 무기 장착
	void ApplyWeaponStats_internal(const FItemData& ItemInfo);
	
	// 무기 해제
	void RemoveWeaponStats_internal();
	
	// 무기 사용 이벤트 수신 함수
	void OnWeaponAttackEvent_internal(const FGameplayEventData* Payload);
	
	//--------------------
	// 도구 관련
	//--------------------
	
protected:
	
	// 도구 장착
	void ApplyToolTags_internal(const FItemData& ItemInfo);

	// 도구 해제
	void RemoveToolTags_internal();
	
	// 도구 사용 이벤트 수신 함수
	void OnToolHarvestEvent_internal(const FGameplayEventData* Payload);
	
#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_관련_API

	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 관련 API
	//━━━━━━━━━━━━━━━━━━━━
	
	//--------------------
	// 초기화
	//--------------------
protected:
	
	void InitializeInventory_internal();
	
	//--------------------
	// 습득
	//--------------------
public:
	
	UFUNCTION(BlueprintCallable)
	bool AddItem(const FItemInstance& ItemData, int32 Quantity, int32& OutRemainingQuantity);

protected:
	
	bool TryStackSlots_internal(FSlotStructMaster& FromSlot, FSlotStructMaster& ToSlot, bool bIsFullStack);
	
	int32 FindEmptySlot_internal(EInventoryType InventoryType);
	
	int32 FindEquipmentSlot_internal(EEquipSlot ArmorSlot) const;
	
	//--------------------
	// 드랍
	//--------------------
public:
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerDropItemToWorld(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity);

	//--------------------
	// 아이템 제거
	//--------------------
protected:
	
	bool RemoveItem_internal(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity = 0);
	
	virtual void ClearSlot_internal(FSlotStructMaster& Slot);
	
	//--------------------
	// 사용
	//--------------------
protected:
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerActivateHotkeySlot_internal(int32 SlotIndex);
	
	//--------------------
	// 게터
	//--------------------
public:

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FSlotStructMaster GetSlot(EInventoryType InventoryType, int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable)
	int32 GetItemCount(int32 StaticDataID) const;

protected:
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE int32 GetActiveHotkeyIndex_internal() const { return ActiveHotkeyIndex; }
	
	FInventoryStructMaster* GetInventoryByType_internal(EInventoryType InventoryType);
	
	FSlotStructMaster GetActiveHotkeySlot_internal() const;
	
	//--------------------
	// 검증
	//--------------------
protected:
	
	bool IsSlotEmpty_internal(EInventoryType InventoryType, int32 SlotIndex);
	
	bool IsValidSlotIndex_internal(EInventoryType InventoryType, int32 SlotIndex);
	
	virtual bool CanPlaceItemInSlot_internal(int32 StaticDataID, EInventoryType InventoryType, int32 SlotIndex, bool IsTarget);
	
	//--------------------
	// 유틸
	//--------------------
public:
	
	virtual void TransferItem(UTSInventoryMasterComponent* SourceInventory, UTSInventoryMasterComponent* TargetInventory, EInventoryType FromInventoryType, int32 FromSlotIndex, EInventoryType ToInventoryType, int32 ToSlotIndex, bool bIsFullStack = true, ATSPlayerController* RequestingPlayer = nullptr);
	
protected:
	
	static void CopySlotData_internal(const FSlotStructMaster& Source, FSlotStructMaster& Target, int32 Quantity = -1);
	
	
	//--------------------
	// 후 처리 알림 (for UI)
	//--------------------
protected:
	// 인벤토리 변환 발생 시  
	void HandleInventoryChanged_internal();

	// 핫 키 변환 발생 시   
	void HandleActiveHotkeyIndexChanged_internal();
	
	
#pragma endregion
//======================================================================================================================
#pragma region 가방_시스템_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 관련 API
	//━━━━━━━━━━━━━━━━━━━━
public:
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 GetCurrentBagSlotCount() const { return BagInventory.InventorySlotContainer.Num(); }

protected:
	
	bool ExpandBagInventory_internal(int32 AdditionalSlots);

	
#pragma endregion
//======================================================================================================================
#pragma region GAS_관련_API
    	
	
	//━━━━━━━━━━━━━━━━━━━━
	// GAS 관련 API
	//━━━━━━━━━━━━━━━━━━━━
protected:
    	// GameplayEvent 수신 함수
    	void OnItemConsumedEvent_internal(const FGameplayEventData* Payload);
    
    	// 소모품 Ability 관련 리소스 정리
    	void ClearConsumableAbilityResources_internal();
    	
    	/*
    		소모품 Ability를 부여하고 Trigger 예약
    		@param ItemInfo 아이템 정보
    		@param SlotIndex 슬롯 인덱스
    		@param ASC AbilitySystemComponent
    		@return 성공 여부
    	*/
    	bool GrantAndScheduleConsumableAbility_internal(const FItemData& ItemInfo, int32 SlotIndex, UAbilitySystemComponent* ASC);
    	
    	// ASC 준비되면 이벤트 리스너 등록
    	void TryRegisterEventListeners_internal();
    	
	
#pragma endregion	
//======================================================================================================================
#pragma region 부패도_관련_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 부패도 관련 API
	//━━━━━━━━━━━━━━━━━━━━
protected:
	// 부패도 매니저 델리게이트 바인딩 함수
	void BindDecayManagerDelegate_internal();
	
	UFUNCTION()
	void OnDecayTick_internal();
	
	void ConvertToDecayedItem_internal(EInventoryType InventoryType);
	
	double UpdateExpirationTime_internal(double CurrentExpirationTime, int CurrentStack, int NewItemStack, float DecayRate) const;
	
	float UpdateDecayPercent_internal(double CurrentExpirationTime, float DecayRate) const;

	
#pragma endregion
//======================================================================================================================
	
	
	
	
	//```````````````````````
	// 데이터 섹션
	//.......................

//======================================================================================================================
#pragma region 인벤토리_관련 데이터
	
	// ========================================
	// 초기 설정
	// ========================================

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 HotkeySlotCount = 10;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	ESlotAccessType SlotAccessType = ESlotAccessType::ReadWrite ;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	TMap<ESlotType, EEquipSlot> EquipmentSlotTypes = {{ESlotType::Head, EEquipSlot::HEAD},{ESlotType::Torso, EEquipSlot::TORSO},{ESlotType::Leg, EEquipSlot::LEG}};

public:
	
	/** 가방 초기 슬롯 개수 (0이면 가방 아이템 사용 전까지 사용 불가) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 InitialBagSlotCount = 0;

	
	/** 가방 최대 슬롯 개수 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 MaxBagSlotCount = 16;
protected:

	
	/** 가방 아이템 사용 시 증가하는 슬롯 개수 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 BagSlotIncrement = 4;

	/** 가방 아이템의 StaticDataID */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 BagItemID = 999;
	
	
	// ========================================
	// 인벤토리 데이터
	// ========================================

public:

	UPROPERTY(ReplicatedUsing = OnRep_HotkeyInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster HotkeyInventory;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster EquipmentInventory;

	UPROPERTY(ReplicatedUsing = OnRep_BagInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster BagInventory;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveHotkeyIndex, BlueprintReadOnly, Category = "Inventory")
	int32 ActiveHotkeyIndex = 0;

#pragma endregion
//======================================================================================================================
#pragma region 장착_아이템_데이터
	
	// 현재 장착된 아이템 ID
	int32 CachedEquippedItemID = 0;
	
	// ■ 소모품 회복약 사이즈 조정 관련 내용 추가
	UPROPERTY(ReplicatedUsing = OnRep_CurrentEquippedItem)
	ATSEquippedItem* CurrentEquippedItem = nullptr;
	
	// 현재 장착 아이템의 스케일 값
	UPROPERTY(Replicated)
	FVector EquippedItemScale = FVector(1.0f);
	
	UPROPERTY(Replicated)
	TArray<FEquippedArmor> EquippedArmors;
	
	// 현재 장착된 무기의 스탯 이펙트 핸들
	FActiveGameplayEffectHandle CurrentWeaponEffectHandle;
	
	// 무기 스탯 적용용 GameplayEffect 클래스 : 블루프린트에서 설정 (GE_WeaponStats_Base를 상속한 BP)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
	TSubclassOf<UGameplayEffect> WeaponStatEffectClass;
	
	// 방어구 스탯 적용용 GameplayEffect 클래스 : 블루프린트에서 설정 -> GE_ArmorCommonStats_Base
	UPROPERTY(EditDefaultsOnly, Category = "Armor Stats")
	TSubclassOf<UGameplayEffect> ArmorCommonStatEffectClass;

	// GE_ArmorEffectStats_Base
	UPROPERTY(EditDefaultsOnly, Category = "Armor Stats")
	TSubclassOf<UGameplayEffect> ArmorEffectStatEffectClass; 
	
#pragma endregion	
//======================================================================================================================
#pragma region 부패도_관련_데이터

protected:
	
	int32 CachedDecayedItemID;
	
	FItemData CachedDecayedItemInfo;

#pragma endregion	
//======================================================================================================================
#pragma region GAS_관련_데이터
	
private:
	// 현재 활성화된 소모품 Ability의 SpecHandle
	FGameplayAbilitySpecHandle ActiveConsumableAbilityHandle;
	
	// Timer 취소용 핸들
	FTimerHandle ConsumableAbilityTriggerTimer;
	
	// 타이머 핸들
	FTimerHandle ASCCheckTimerHandle;
	
	// ASC 초기화 대기 중 플래그
	bool bEventListenersRegistered = false;
	
	
#pragma endregion
//======================================================================================================================	
	
	
	
	
	

};
