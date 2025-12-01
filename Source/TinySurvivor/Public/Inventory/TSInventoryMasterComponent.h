// TSInventoryMasterComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Struct/TSInventorySlot.h"
#include "Item/Data/ItemData.h"
#include "TSInventoryMasterComponent.generated.h"


class ATSEquippedItem;
struct FItemData;
struct FItemInstance;
class UItemDataSubsystem;
class UAbilitySystemComponent;
// ========================================
// 장착한 방어구 구조체
// ========================================
USTRUCT()
struct FEquippedArmor
{
	GENERATED_BODY()

	UPROPERTY()
	EEquipSlot SlotType = EEquipSlot::HEAD;

	UPROPERTY()
	TObjectPtr<ATSEquippedItem> EquippedArmor = nullptr;
};

// ========================================
// 델리게이트 선언
// ========================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventorySlotUpdated, const FInventoryStructMaster&, HotkeyInventory,
                                               const FInventoryStructMaster&, EquipmentInventory,
                                               const FInventoryStructMaster&, BagInventory);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHotkeyActivated, int32, SlotIndex, const FSlotStructMaster&, ActiveSlot)
;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBagSizeChanged, int32, NewSize);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryInitialized);

UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class TINYSURVIVOR_API UTSInventoryMasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTSInventoryMasterComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// ========================================
	// 인벤토리 데이터
	// ========================================

	UPROPERTY(ReplicatedUsing = OnRep_HotkeyInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster HotkeyInventory;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster EquipmentInventory;

	UPROPERTY(ReplicatedUsing = OnRep_BagInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster BagInventory;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveHotkeyIndex, BlueprintReadOnly, Category = "Inventory")
	int32 ActiveHotkeyIndex = 0;

	// ========================================
	// 초기 설정
	// ========================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 HotkeySlotCount = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	TMap<ESlotType, EEquipSlot> EquipmentSlotTypes = {
		{ESlotType::Head, EEquipSlot::HEAD},
		{ESlotType::Torso, EEquipSlot::TORSO},
		{ESlotType::Leg, EEquipSlot::LEG}
	};

	/** 가방 초기 슬롯 개수 (0이면 가방 아이템 사용 전까지 사용 불가) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 InitialBagSlotCount = 0;

	/** 가방 최대 슬롯 개수 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 MaxBagSlotCount = 16;

	/** 가방 아이템 사용 시 증가하는 슬롯 개수 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 BagSlotIncrement = 4;

	/** 가방 아이템의 StaticDataID */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 BagItemID = 999;

	// ========================================
	// 델리게이트
	// ========================================

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventorySlotUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnHotkeyActivated OnHotkeyActivated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnBagSizeChanged OnBagSizeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryInitialized OnInventoryInitialized;

	// ========================================
	// 리플리케이션 콜백
	// ========================================

	UFUNCTION()
	void OnRep_HotkeyInventory();

	UFUNCTION()
	void OnRep_EquipmentInventory();

	UFUNCTION()
	void OnRep_BagInventory();

	UFUNCTION()
	void OnRep_ActiveHotkeyIndex();

	// ========================================
	// Server RPC
	// ========================================

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|RPC")
	void ServerDropItemToWorld(
		EInventoryType InventoryType, int32 SlotIndex, int32 Quantity);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|RPC")
	void ServerActivateHotkeySlot(int32 SlotIndex);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|RPC")
	void ServerUseItem(int32 SlotIndex);

	// ========================================
	// Internal 함수
	// ========================================

	UFUNCTION(BlueprintCallable, Category = "Inventory|Internal")
	void Internal_TransferItem(
		UTSInventoryMasterComponent* SourceInventory,
		UTSInventoryMasterComponent* TargetInventory,
		EInventoryType FromInventoryType, int32 FromSlotIndex,
		EInventoryType ToInventoryType, int32 ToSlotIndex,
		bool bIsFullStack = true);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Internal")
	void Internal_UseItem(int32 SlotIndex);

	// ========================================
	// 아이템 추가/제거
	// ========================================

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FItemInstance& ItemData, int32 Quantity, int32& OutRemainingQuantity);

	UFUNCTION(Category = "Inventory")
	bool RemoveItem(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity = 0);

	// ========================================
	// 슬롯 조회
	// ========================================

	UFUNCTION(BlueprintPure, Category = "Inventory|Search")
	FSlotStructMaster GetSlot(EInventoryType InventoryType, int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Search")
	bool IsSlotEmpty(EInventoryType InventoryType, int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Search")
	int32 FindEmptySlot(EInventoryType InventoryType) const;

	// ========================================
	// 타입 검증
	// ========================================

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanPlaceItemInSlot(
		int32 StaticDataID,
		EInventoryType InventoryType,
		int32 SlotIndex) const;

	// ========================================
	// 가방 시스템
	// ========================================

	UFUNCTION(BlueprintPure, Category = "Inventory|Bag")
	int32 GetCurrentBagSlotCount() const { return BagInventory.InventorySlotContainer.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Inventory|Bag")
	bool ExpandBagInventory(int32 AdditionalSlots);

	// ========================================
	// 핫키 아이템 장착 시스템
	// ========================================

	UPROPERTY(Replicated)
	ATSEquippedItem* CurrentEquippedItem = nullptr;

	UFUNCTION(BlueprintPure, Category = "Inventory|Hotkey")
	int32 GetActiveHotkeyIndex() const { return ActiveHotkeyIndex; }

	UFUNCTION(BlueprintPure, Category = "Inventory|Hotkey")
	FSlotStructMaster GetActiveHotkeySlot() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Hotkey")
	bool HasItemEquipped() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Hotkey")
	void EquipActiveHotkeyItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Hotkey")
	void UnequipCurrentItem();

	// ========================================
	// 아이템 검색/소비
	// ========================================
	UFUNCTION(BlueprintCallable, Category = "Inventory|Search")
	int32 GetItemCount(int32 StaticDataID) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory|Search")
	void ConsumeItem(int32 StaticDataID, int32 Quantity);

protected:
	// 부패도 매니저 델리게이트 바인딩 함수
	UFUNCTION()
	void OnDecayTick();
	// ========================================
	// 헬퍼 함수 - 부패
	// ========================================
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Settings")
	int32 CachedDecayedItemID;
	FItemData CachedDecayedItemInfo;
	void ConvertToDecayedItem(EInventoryType InventoryType);

	// ========================================
	// 헬퍼 함수 - 슬롯 조작
	// ========================================
	static void ClearSlot(FSlotStructMaster& Slot);
	static void CopySlotData(const FSlotStructMaster& Source, FSlotStructMaster& Target, int32 Quantity = -1);
	bool TryStackSlots(FSlotStructMaster& FromSlot, FSlotStructMaster& ToSlot, bool bIsFullStack);

	// ========================================
	// 헬퍼 함수 - 인벤토리
	// ========================================
	FInventoryStructMaster* GetInventoryByType(EInventoryType InventoryType);
	const FInventoryStructMaster* GetInventoryByType(EInventoryType InventoryType) const;
	bool IsValidSlotIndex(EInventoryType InventoryType, int32 SlotIndex) const;

	// ========================================
	// 헬퍼 함수 - 아이템 정보
	// ========================================
	mutable class UItemDataSubsystem* CachedIDS = nullptr;
	UItemDataSubsystem* GetItemDataSubsystem() const;
	bool GetItemData(int32 StaticDataID, FItemData& OutData) const;
	bool IsItemBagType(int32 StaticDataID) const;
	double UpdateExpirationTime(double CurrentExpirationTime, int CurrentStack, int NewItemStack,
	                            float DecayRate) const;
	float UpdateDecayPercent(double CurrentExpirationTime, float DecayRate) const;

	// ========================================
	// 헬퍼 함수 - 방어구/무기 착용
	// ========================================
	UPROPERTY(Replicated)
	TArray<FEquippedArmor> EquippedArmors;
	
	// 방어구 관련
	int32 FindEquipmentSlot(EEquipSlot ArmorSlot) const;
	void EquipArmor(const FItemData& ItemInfo, int32 ArmorSlotIndex);
	void UnequipArmor(int32 ArmorSlotIndex);
	void RemoveArmorStats(int32 ArmorSlotIndex);
	
	// 무기 관련
	void ApplyWeaponStats(const FItemData& ItemInfo);
	void RemoveWeaponStats();

	// ========================================
	// 헬퍼 함수 - ASC
	// ========================================
	UAbilitySystemComponent* GetASC();

	// ========================================
	// 헬퍼 함수 - 델리게이트
	// ========================================
	void HandleInventoryChanged();
	void HandleActiveHotkeyIndexChanged();
	
	// ■ ItemConsumed
	//[S]=====================================================================================
	// GameplayEvent 수신 함수
	void OnItemConsumedEvent(const FGameplayEventData* Payload);
	//[E]=====================================================================================

#pragma region WeaponData
protected:
	/*
		현재 장착된 무기의 스탯 이펙트 핸들
	*/
	FActiveGameplayEffectHandle CurrentWeaponEffectHandle;
	
	/*
		무기 스탯 적용용 GameplayEffect 클래스
		블루프린트에서 설정 (GE_WeaponStats_Base를 상속한 BP)
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
	TSubclassOf<UGameplayEffect> WeaponStatEffectClass;
#pragma endregion
	
private:
	int32 CachedEquippedItemID = 0; // 현재 장착된 아이템 ID
};
