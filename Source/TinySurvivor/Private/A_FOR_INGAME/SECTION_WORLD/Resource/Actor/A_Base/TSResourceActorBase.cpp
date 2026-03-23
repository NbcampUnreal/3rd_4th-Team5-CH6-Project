// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/Resource/Actor/A_Base/TSResourceActorBase.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"


//======================================================================================================================	
#pragma region REP_API
	
	//━━━━━━━━━━━━━━━━━━━━
	// REP API
	//━━━━━━━━━━━━━━━━━━━━

void ATSResourceActorBase::OnRep_ResourceData()
{
}

#pragma endregion
//======================================================================================================================	
#pragma region 라이프_사이클
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 라이프 사이클
	//━━━━━━━━━━━━━━━━━━━━	

ATSResourceActorBase::ATSResourceActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ATSResourceActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void ATSResourceActorBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ATSResourceActorBase, ResourceData, COND_None);
}

#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트API_컴포넌트_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API_컴포넌트_데이터
	//━━━━━━━━━━━━━━━━━━━━	

void ATSResourceActorBase::ToggleInteractWidget_Implementation(bool InWantOn)
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
