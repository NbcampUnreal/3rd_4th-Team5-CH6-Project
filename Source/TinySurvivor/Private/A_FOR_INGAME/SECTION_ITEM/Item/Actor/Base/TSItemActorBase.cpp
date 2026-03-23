// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/Actor/Base/TSItemActorBase.h"
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

void ATSItemActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void ATSItemActorBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ATSItemActorBase, ItemData, COND_None);
}

#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트API_컴포넌트_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API_컴포넌트_데이터
	//━━━━━━━━━━━━━━━━━━━━	
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
		InteractWidgetComp->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	}
}

#pragma endregion
//======================================================================================================================	
