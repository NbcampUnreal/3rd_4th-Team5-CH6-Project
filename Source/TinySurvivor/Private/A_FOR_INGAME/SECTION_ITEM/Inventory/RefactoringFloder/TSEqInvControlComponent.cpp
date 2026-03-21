// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Inventory/RefactoringFloder/TSEqInvControlComponent.h"

#include "A_FOR_COMMON/Library/Item/TSInventoryHelperLibrary.h"
#include "A_FOR_COMMON/Library/Item/TSItemHelperLibrary.h"
#include "A_FOR_COMMON/Library/System/TSDecayLibrary.h"
#include "A_FOR_COMMON/Library/System/TSSystemGetterLibrary.h"
#include "A_FOR_COMMON/Library/System/TSTimeLibrary.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSInventoryMasterComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/Struct/TSInventorySlot.h"

//======================================================================================================================
#pragma region 라이프_사이클


	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━

UTSEqInvControlComponent::UTSEqInvControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTSEqInvControlComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BindDecayManagerDelegate_internal();
}


#pragma endregion
//======================================================================================================================
#pragma region 부패도_관리


	//━━━━━━━━━━━━━━━━━━━━
	// 부패도 관리
	//━━━━━━━━━━━━━━━━━━━━

void UTSEqInvControlComponent::BindDecayManagerDelegate_internal()
{
	if (!IsValid(UTSSystemGetterLibrary::GetDecayManager(this))) return;
	UTSSystemGetterLibrary::GetDecayManager(this)->OnDecayTick.AddDynamic(this, &UTSEqInvControlComponent::OnDecayTick_internal);
}

void UTSEqInvControlComponent::OnDecayTick_internal()
{
	if (!IsValid(InventoryMasterComp)) return;
	
	ConvertToDecayedItem_internal(EInventoryType::HotKey);
	ConvertToDecayedItem_internal(EInventoryType::BackPack);
	InventoryMasterComp->HandleInventoryChanged_internal();
}

void UTSEqInvControlComponent::ConvertToDecayedItem_internal(EInventoryType InventoryType)
{
	if (!IsValid(InventoryMasterComp)) return;
	
	// 타입에 맞는 인벤토리 가져오기
	FInventoryStructMaster* Inventory = UTSInventoryHelperLibrary::GetInventoryByType_Lib(InventoryMasterComp, InventoryType);
	if (!Inventory) return;
	
	for (int32 SlotIndex = 0; SlotIndex < Inventory->InventorySlotContainer.Num(); ++SlotIndex)
	{
		// 슬롯 캐싱, 부패물 생성용 데이터 변수 생성
		FSlotStructMaster& Slot = Inventory->InventorySlotContainer[SlotIndex];
		FItemData ItemInfo;
		
		// 빈 슬롯은 건너뛰기
		if (Slot.ItemData.StaticDataID == 0) continue;

		// 아이템 정보 조회 실패 시 건너뛰기
		if (!UTSItemHelperLibrary::GetItemData_Lib(this, Slot.ItemData.StaticDataID, ItemInfo)) continue;

		// 부패 가능한 아이템만 건너뛰기
		if (!ItemInfo.IsDecayEnabled()) continue;
		
		// 아직 만료되지 않았으면 부패도만 업데이트
		if (UTSTimeLibrary::GetCurrentTime(this) < Slot.ExpirationTime)
		{
			Slot.CurrentDecayPercent = UTSDecayLibrary::CalculateDecayPercent(this,Slot.ExpirationTime, ItemInfo.ConsumableData.DecayRate);
			continue;
		}

		//=======================================================================
		// 부패 시간 만료: 부패물로 전환
		//=======================================================================
		
		// 부패물 정보 캐싱
		FItemData DecayedItemInfo;
		UTSItemHelperLibrary::GetDecayedItemData_Lib(this, DecayedItemInfo);
			
		// 슬롯 데이터를 부패물로 변경
		Slot.ItemData.StaticDataID = UTSItemHelperLibrary::GetDecayedItemID_Lib(this);
		Slot.ExpirationTime = 0;
		Slot.bCanStack = DecayedItemInfo.IsStackable();
		Slot.MaxStackSize = DecayedItemInfo.MaxStack;

		// 현재 손에 들고 있는 슬롯인 경우 부패물 메시로 재장착
		if (InventoryType == EInventoryType::HotKey && SlotIndex == InventoryMasterComp->ActiveHotkeyIndex) InventoryMasterComp->HandleActiveHotkeyIndexChanged_internal();
	}
}


#pragma endregion
//======================================================================================================================