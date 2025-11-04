// TSInventoryMasterComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Struct/TSInventorySlot.h"
#include "TSInventoryMasterComponent.generated.h"

// ========================================
// 델리게이트 선언
// ========================================

class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotUpdated, EInventoryType, InventoryType, int32, SlotIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHotkeyActivated, int32, SlotIndex, const FSlotStructMaster&, ActiveSlot)
;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBagSizeChanged, int32, NewSize);

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

	/** 핫키 인벤토리 */
	UPROPERTY(ReplicatedUsing = OnRep_HotkeyInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster HotkeyInventory;

	/** 장비 인벤토리 */
	UPROPERTY(ReplicatedUsing = OnRep_EquipmentInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster EquipmentInventory;

	/** 가방 인벤토리 */
	UPROPERTY(ReplicatedUsing = OnRep_BagInventory, BlueprintReadOnly, Category = "Inventory")
	FInventoryStructMaster BagInventory;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveHotkeyIndex, BlueprintReadOnly, Category = "Inventory")
	int32 ActiveHotkeyIndex = -1;

	// ========================================
	// 초기 설정
	// ========================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 HotkeySlotCount = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	int32 EquipmentSlotCount = 4;

	/** 장비 슬롯 타입 정의 (인덱스 순서) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
	TArray<ESlotType> EquipmentSlotTypes = {
		ESlotType::Helmet,
		ESlotType::Chest,
		ESlotType::Boot,
		ESlotType::BackPack
	};

	// ========================================
	// 델리게이트
	// ========================================

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventorySlotUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnHotkeyActivated OnHotkeyActivated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnBagSizeChanged OnBagSizeChanged;

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
	void ServerSwapSlots(
		EInventoryType FromInventoryType, int32 FromSlotIndex,
		EInventoryType ToInventoryType, int32 ToSlotIndex,
		bool bIsFullStack = true);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|RPC")
	void ServerTransferItem(
		UTSInventoryMasterComponent* TargetInventory,
		EInventoryType FromInventoryType, int32 FromSlotIndex,
		EInventoryType ToInventoryType, int32 ToSlotIndex,
		bool bIsFullStack = true);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|RPC")
	void ServerDropItemToWorld(
		EInventoryType InventoryType, int32 SlotIndex, int32 Quantity);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory|RPC")
	void ServerActivateHotkeySlot(int32 SlotIndex);

	// ========================================
	// Internal 함수
	// ========================================

	UFUNCTION(BlueprintCallable, Category = "Inventory|Internal")
	void Internal_SwapSlots(
		EInventoryType FromInventoryType, int32 FromSlotIndex,
		EInventoryType ToInventoryType, int32 ToSlotIndex,
		bool bIsFullStack = true);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Internal")
	void Internal_TransferItem(
		UTSInventoryMasterComponent* TargetInventory,
		EInventoryType FromInventoryType, int32 FromSlotIndex,
		EInventoryType ToInventoryType, int32 ToSlotIndex,
		bool bIsFullStack = true);

	// ========================================
	// 아이템 추가/제거
	// ========================================

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(int32 StaticDataID, int32 DynamicDataID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(EInventoryType InventoryType, int32 SlotIndex, int32 Quantity = 0);

	// ========================================
	// 슬롯 조회
	// ========================================

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FSlotStructMaster GetSlot(EInventoryType InventoryType, int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsSlotEmpty(EInventoryType InventoryType, int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
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
	bool IsBagEquipped() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Bag")
	int32 GetEquippedBagSlotCount() const;

	UFUNCTION()
	void UpdateBagInventoryState();

	UFUNCTION(BlueprintPure, Category = "Inventory|Bag")
	bool CanAccessBagInventory() const;

	// ========================================
	// 핫키 아이템 장착 시스템
	// ========================================

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

private:
	// ========================================
	// 헬퍼 함수 - 슬롯 조작
	// ========================================

	/** 슬롯 초기화 */
	static void ClearSlot(FSlotStructMaster& Slot);

	/** 슬롯 데이터 복사 */
	static void CopySlotData(const FSlotStructMaster& Source, FSlotStructMaster& Target, int32 Quantity = -1);

	/**
	 * 두 슬롯 간 스택 처리 시도
	 * @return true면 스택 처리 완료, false면 일반 교환 필요
	 */
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

	bool GetItemSlotInfo(int32 StaticDataID, FItemData& OutData) const;
	int32 GetEquippedBagItemID() const;
	int32 GetBagSlotCount(int32 BagItemID) const;

	// ========================================
	// 헬퍼 함수 - ASC
	// ========================================
	UAbilitySystemComponent* GetASC();
};
