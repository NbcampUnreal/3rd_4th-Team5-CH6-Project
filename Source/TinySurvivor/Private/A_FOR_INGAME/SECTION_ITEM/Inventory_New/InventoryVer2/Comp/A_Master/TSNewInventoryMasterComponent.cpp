// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Comp/A_Master/TSNewInventoryMasterComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory_New/InventoryVer2/Library/TSNewInventoryInternalHelperLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item_New/Library/TSNewItemCheckLibrary.h"
#include "Net/UnrealNetwork.h"

//======================================================================================================================	
#pragma region REP_API
	

	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━

void UTSNewInventoryMasterComponent::OnRep_InventoryData()
{
	
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클


	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━


UTSNewInventoryMasterComponent::UTSNewInventoryMasterComponent()
{
	// tick
	PrimaryComponentTick.bCanEverTick = false;
	
	// network
	SetIsReplicatedByDefault(true);
}

void UTSNewInventoryMasterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UTSNewInventoryMasterComponent, InventoryData, COND_None);
}

void UTSNewInventoryMasterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTSNewInventoryMasterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


#pragma endregion
//======================================================================================================================	
#pragma region 인벤토리_외부_API
	

	//━━━━━━━━━━━━━━━━━━━━
	// 인벤토리 외부 API
	//━━━━━━━━━━━━━━━━━━━━

bool UTSNewInventoryMasterComponent::AddItemToInventory_Implementation(FTSNewItemRuntimeData InItemRuntimeData)
{
	// 오너 검증 및 서버에서 진행 중인지 판단 
	if (!IsValid(GetOwner()) || !GetOwner()->HasAuthority()) return false;
	
	// 유효한 아이템인지 판단 
	if (UTSNewItemCheckLibrary::IsThisItemValid_Lib(this, InItemRuntimeData) == false) return false;
	
	// 이 인벤토리에 들어올 수 있는지 판단 
	if (UTSNewItemCheckLibrary::IsThisItemCanPlaceInThisInventory_Lib(this, InItemRuntimeData, InventoryData) == false) return false;
	
	// 스택아이템인지 아닌지 판단 
	bool bIsStackItem = UTSNewItemCheckLibrary::IsThisITemCanStack_Lib(this,InItemRuntimeData);

	// Case: 비 스택 
	if (bIsStackItem == false) return TrayAddNoneStackItem_internal(InItemRuntimeData);
	
	// Case: 스택 
	if (bIsStackItem == true) return TrayAddStackItem_internal(InItemRuntimeData);
	
	return false;
}

bool UTSNewInventoryMasterComponent::RemoveItemFromInventory_Implementation(FTSNewItemRuntimeData InItemRuntimeData)
{
	
	
	return false;
}

bool UTSNewInventoryMasterComponent::DropItemToFromInventory_Implementation(FTSNewItemRuntimeData InItemRuntimeData)
{
	
	
	
	return false;
}

bool UTSNewInventoryMasterComponent::SwapItemFromThisInventoryToAnotherInventory_Implementation(FTSNewItemRuntimeData InItemRuntimeData)
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
	
bool UTSNewInventoryMasterComponent::TrayAddNoneStackItem_internal(FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 빈 슬롯 찾기 (배열 인덱스는 0부터 시작하므로 0보다 작을 수 없음) 
	int32 FoundSlotIndex = UTSNewInventoryInternalHelperLibrary::FindEmptySlot_Lib(this, InventoryData, InItemRuntimeData);
	if (FoundSlotIndex < 0) return false;

	// 인덱스 검증
	if (!InventoryData.InventoryDataArray.IsValidIndex(FoundSlotIndex)) return false;

	// 빈 슬롯에 넣기 	
	return UTSNewInventoryInternalHelperLibrary::AddItemToEmptySlot_Lib(this, InventoryData.InventoryDataArray[FoundSlotIndex], InItemRuntimeData);
}

bool UTSNewInventoryMasterComponent::TrayAddStackItem_internal(FTSNewItemRuntimeData& InItemRuntimeData)
{
	// 스택 가능한 슬롯 찾기 (배열 인덱스는 0부터 시작하므로 0보다 작을 수 없음)
	int32 FoundSlotIndex = UTSNewInventoryInternalHelperLibrary::FindStackSlot_Lib(this, InventoryData, InItemRuntimeData);
	if (FoundSlotIndex >= 0)
	{
		// 인덱스 검증
		if (!InventoryData.InventoryDataArray.IsValidIndex(FoundSlotIndex)) return false;
	
		// 스택 슬롯에 넣기
		return UTSNewInventoryInternalHelperLibrary::AddItemToStackSlot_Lib(this, InventoryData.InventoryDataArray[FoundSlotIndex], InItemRuntimeData);
	}
	// 스택 못 찾았을 경우 빈 슬롯에 넣기 시도
	else
	{
		return TrayAddNoneStackItem_internal(InItemRuntimeData);
	}
}

#pragma endregion
//======================================================================================================================	
	