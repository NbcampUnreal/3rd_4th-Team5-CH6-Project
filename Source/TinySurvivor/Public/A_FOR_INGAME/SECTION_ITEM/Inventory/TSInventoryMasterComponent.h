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

	friend class UTSEqInvControlComponent;
	friend class UTSInventoryHelperLibrary;
	friend class UTSItemUseHelperLibrary;
	
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
	
	//--------------------
	// 아이템 소비
	//--------------------

public:
	// 아이템 소비 API 
	void ConsumeItem(int32 StaticDataID, int32 Quantity);
	
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

public:
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerActivateHotkeySlot(int32 SlotIndex);
	
	
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

	
	
	// ========================================
	// 핫 키
	// ========================================

	
	UPROPERTY(ReplicatedUsing = OnRep_ActiveHotkeyIndex, BlueprintReadOnly, Category = "Inventory")
	int32 ActiveHotkeyIndex = 0;

#pragma endregion
//======================================================================================================================
#pragma region 장착_아이템_데이터
	
public:

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

	
	

};
