// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Comp/A_Master/TSInventoryMasterComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Inventory/Library/TSInventoryInternalHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Library/TSItemCheckLibrary.h"
#include "Net/UnrealNetwork.h"

//======================================================================================================================	
#pragma region REP_API
	

	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━

void UTSInventoryMasterComponent::OnRep_InventoryData()
{
	
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클


	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━


UTSInventoryMasterComponent::UTSInventoryMasterComponent()
{
	// tick
	PrimaryComponentTick.bCanEverTick = false;
	
	// network
	SetIsReplicatedByDefault(true);
}

void UTSInventoryMasterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UTSInventoryMasterComponent, InventoryData, COND_None);
}

void UTSInventoryMasterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTSInventoryMasterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_외부_API
	

	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 외부 API
	//━━━━━━━━━━━━━━━━━━━━

bool UTSInventoryMasterComponent::AddItemToInventory_Implementation(FTSItemRuntimeData InItemRuntimeData)
{
	// 오너 검증 및 서버에서 진행 중인지 판단 
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return false;
	
	// 유효한 아이템인지 판단 
	if (UTSItemCheckLibrary::IsThisItemValid_Lib(this, InItemRuntimeData) == false) return false;
	
	// 이 인벤토리에 들어올 수 있는지 판단 
	if (UTSItemCheckLibrary::IsThisItemCanPlaceInThisInventory_Lib(this, InItemRuntimeData, InventoryData) == false) return false;
	
	// 스택아이템인지 아닌지 판단 
	bool bIsStackItem = UTSItemCheckLibrary::IsThisITemCanStack_Lib(this,InItemRuntimeData);

	// Case: 비 스택 
	if (bIsStackItem == false) return TrayAddNoneStackItem_internal(InItemRuntimeData);
	
	// Case: 스택 
	if (bIsStackItem == true) return TrayAddStackItem_internal(InItemRuntimeData);
	
	return false;
}

bool UTSInventoryMasterComponent::RemoveItemFromInventory_Implementation(FTSItemRuntimeData InItemRuntimeData)
{
	
	
	return false;
}

bool UTSInventoryMasterComponent::DropItemToFromInventory_Implementation(FTSItemRuntimeData InItemRuntimeData)
{
	
	
	
	return false;
}

bool UTSInventoryMasterComponent::SwapItemFromThisInventoryToAnotherInventory_Implementation(FTSItemRuntimeData InItemRuntimeData)
{
	
	
	
	return false;
}



#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_내부_API
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 내부 API
	//━━━━━━━━━━━━━━━━━━━━

	//--------------------
	// 아이템 습득 
	//-------------------
	
bool UTSInventoryMasterComponent::TrayAddNoneStackItem_internal(FTSItemRuntimeData& InItemRuntimeData)
{
	// 빈 슬롯 찾기 (배열 인덱스는 0부터 시작하므로 0보다 작을 수 없음) 
	int32 FoundSlotIndex = UTSInventoryInternalHelperLibrary::FindEmptySlot_Lib(this, InventoryData, InItemRuntimeData);
	if (FoundSlotIndex < 0) return false;

	// 인덱스 검증
	if (!InventoryData.InventoryDataArray.IsValidIndex(FoundSlotIndex)) return false;

	// 빈 슬롯에 넣기 	
	return UTSInventoryInternalHelperLibrary::AddItemToEmptySlot_Lib(this, InventoryData.InventoryDataArray[FoundSlotIndex], InItemRuntimeData);
}

bool UTSInventoryMasterComponent::TrayAddStackItem_internal(FTSItemRuntimeData& InItemRuntimeData)
{
	// 스택 가능한 슬롯 찾기 (배열 인덱스는 0부터 시작하므로 0보다 작을 수 없음)
	int32 FoundSlotIndex = UTSInventoryInternalHelperLibrary::FindStackSlot_Lib(this, InventoryData, InItemRuntimeData);
	if (FoundSlotIndex >= 0)
	{
		// 인덱스 검증
		if (!InventoryData.InventoryDataArray.IsValidIndex(FoundSlotIndex)) return false;
	
		// 스택 슬롯에 넣기
		return UTSInventoryInternalHelperLibrary::AddItemToStackSlot_Lib(this, InventoryData.InventoryDataArray[FoundSlotIndex], InItemRuntimeData);
	}
	// 스택 못 찾았을 경우 빈 슬롯에 넣기 시도
	else
	{
		return TrayAddNoneStackItem_internal(InItemRuntimeData);
	}
}

#pragma endregion
//======================================================================================================================	
	