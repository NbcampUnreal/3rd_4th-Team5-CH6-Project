// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/Base/TSItemActorBase.h"

#include "A_FOR_INGAME/SECTION_ITEM/Item/Data/Struct/TSITemStaticData.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/System/TSItemDataSubSystem.h"
#include "A_FOR_INGAME/SECTION_UI/Interact/TSInteractUIBase.h"
#include "A_FOR_INGAME/SECTION_UI/Interact/TSItemInteractUI.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"


//======================================================================================================================	
#pragma region REP_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━
	
void ATSItemActorBase::OnRep_ItemData()
{
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSItemActorBase::ATSItemActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	SetReplicatingMovement(true);
	
}

void ATSItemActorBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ATSItemActorBase, ItemData, COND_None);
}

void ATSItemActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitInteractUI(ItemData);
}
#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트API_컴포넌트_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API_컴포넌트_데이터
	//━━━━━━━━━━━━━━━━━━━━	

void ATSItemActorBase::InitInteractUI(FTSItemRuntimeData& ItemRuntimeData)
{
	if (!IsValid(InteractWidgetComp)) return;
	if (!IsValid(InteractWidgetComp->GetUserWidgetObject())) return;
	
	UTSInteractUIBase* InteractUI = CastChecked<UTSInteractUIBase>(InteractWidgetComp->GetUserWidgetObject());
	if (!IsValid(InteractUI)) return;
	
	if (!IsValid(GetWorld())) return;
	UTSItemDataSubSystem* DataSubSystem = UTSItemDataSubSystem::Get(GetWorld());
	if (!IsValid(DataSubSystem)) return;
	
	FTSITemStaticData* ItemStaticData = DataSubSystem->GetItemStaticData(ItemRuntimeData.StaticDataID);
	if (!ItemStaticData) return;

	FString InteractKey = TEXT("F");
	FText InteractText = FText::FromString(InteractKey);
	InteractUI->SetInteractInfo(ItemStaticData->ItemUIInfoTable.ItemName, InteractText);
	
	UTSItemInteractUI* ItemInteractUI = CastChecked<UTSItemInteractUI>(InteractUI);
	if (!IsValid(ItemInteractUI)) return;
	
	ItemInteractUI->SetItemRemainStackInfo(ItemRuntimeData.DynamicData.CurrentStack);
	
	ItemInteractUI->SetVisibility(ESlateVisibility::Collapsed);
}

void ATSItemActorBase::ToggleInteractWidget_Implementation(bool InWantOn)
{
	if (!IsValid(InteractWidgetComp)) return;
	if (!IsValid(InteractWidgetComp->GetUserWidgetObject())) return;
	
	if (InWantOn == true)
	{
		InteractWidgetComp->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
	}
	else if (InWantOn == false)
	{
		InteractWidgetComp->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Collapsed);
	}
}

#pragma endregion
//======================================================================================================================	
