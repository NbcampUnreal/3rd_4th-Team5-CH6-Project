// All CopyRight From YulRyongGameStudio //

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Data/Struct/InventoryCompData/TSInventoryCompData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Interface/ForComp/TSInventoryCompActionInterface.h"
#include "TSInventoryMasterComponent.generated.h"


/**
 * 인벤토리 컴포넌트 설명
 * : NOTE -2026-03-22 : 
 * 인벤토리 컴포넌트는 UI에 직접적인 관련을 절대 하지 않는다. 대신 모든 반환값을 bool로 하고 반드시 어빌리티를 통해 API를 발동시킨다. 
 * 즉, 인벤토리의 어떤 성공 여부는 bool 값 반환에 따라 실행한 어빌리티가 게임 플레이 메시지로 다른 시스템에게 알린다. 
 * 이를 통해 UI는 물론이고 EquipVisual 컴포넌트 또는 무게 컴포넌트에게도 알려진다. 따라서 인벤토리 컴포넌트는 절대는 델리게이트가 없다. 
 * 직접적인 예시 -> 게임 플레이 메시지를 받은 EquipVisual 컴포넌트는 받은 메시지에 따라서 맞는 아이템을 생성, 비쥬얼적으로 보여주기만 한다. 
 * 직접적인 예시 -> 부패도 경우에는 friend class를 선언한 컴포넌트가 내부 데이터를 알아서 주기적으로 체크하는 시스템을 가진다. (최적화 필요 1초마다 X)
 * 직접적인 예시 -> 무게 경우에는 주기적인 체크 대신 아이템을 습득하거나 제거, 드랍하는 경우에 bool에 따라 어빌리티가 게임 플레이 메시지로 알린다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class TINYSURVIVOR_API UTSInventoryMasterComponent : public UActorComponent, public ITSInventoryCompActionInterface
{
	GENERATED_BODY()
	
//======================================================================================================================	
#pragma region REP_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	
	UFUNCTION() void OnRep_InventoryData();
	
#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

public:
	UTSInventoryMasterComponent();											// 리플리케이트 On, 틱 false
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; 
	virtual void BeginPlay() override;											// 게임 플레이 메시지 시스템 바인딩 등록
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	// 게임 플레이 메시지 바인딩 해제
	
	
#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_외부_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 외부 API
	//━━━━━━━━━━━━━━━━━━━━
	
public:
	// ITSNewEquipVisualCompActionInterface ~
	virtual bool AddItemToInventory_Implementation(FTSItemRuntimeData InItemRuntimeData) override;							// 아이템 습득 
	virtual bool RemoveItemFromInventory_Implementation(FTSItemRuntimeData InItemRuntimeData) override;						// 아이템 제거 
	virtual bool DropItemToFromInventory_Implementation(FTSItemRuntimeData InItemRuntimeData) override;						// 아이템 드랍
	virtual bool SwapItemFromThisInventoryToAnotherInventory_Implementation(FTSItemRuntimeData InItemRuntimeData) override;	// 아이템 스왑 
	// ~ ITSNewEquipVisualCompActionInterface
	
	
#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_내부_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 내부 API
	//━━━━━━━━━━━━━━━━━━━━
	
protected:
	
	//--------------------
	// 아이템 습득 
	//-------------------
	
	bool TrayAddNoneStackItem_internal(FTSItemRuntimeData& InItemRuntimeData);	// 비 스택 아이템 넣기 
	bool TrayAddStackItem_internal(FTSItemRuntimeData& InItemRuntimeData);		// 스택 아이템 넣기

#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 데이터 
	//━━━━━━━━━━━━━━━━━━━━	
	
protected:
	
	// 인벤토리 데이터 
	UPROPERTY(ReplicatedUsing = OnRep_InventoryData, EditAnywhere, BlueprintReadWrite, Category = "TS | Inventory")
	FTSInventoryCompData InventoryData;
	
	
#pragma endregion
//======================================================================================================================	
	
};
