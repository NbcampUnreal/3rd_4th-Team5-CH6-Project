// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_WORLD/Resource/Actor/A_Base/TSResourceActorBase.h"
#include "A_FOR_INGAME/SECTION_UI/Interact/TSInteractUIBase.h"
#include "A_FOR_INGAME/SECTION_UI/Interact/TSResourceInteractUI.h"
#include "A_FOR_INGAME/SECTION_WORLD/Resource/System/TSResourceDataSystem.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "SECTION_LOOT/Comp/TSLootHandleComponent.h"


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

void ATSResourceActorBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ATSResourceActorBase, ResourceData, COND_None);
}

void ATSResourceActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitInteractUI(ResourceData);
}
#pragma endregion
//======================================================================================================================	
#pragma region 인터렉트API_컴포넌트_데이터
	
	
	//━━━━━━━━━━━━━━━━━━━━
	// 인터렉트API_컴포넌트_데이터
	//━━━━━━━━━━━━━━━━━━━━	

void ATSResourceActorBase::InitInteractUI(FTSResourceRuntimeData& ItemRuntimeData)
{
	if (!IsValid(InteractWidgetComp)) return;
	if (!IsValid(InteractWidgetComp->GetUserWidgetObject())) return;
	
	UTSInteractUIBase* InteractUI = CastChecked<UTSInteractUIBase>(InteractWidgetComp->GetUserWidgetObject());
	if (!IsValid(InteractUI)) return;
	
	if (!IsValid(GetWorld())) return;
	UTSResourceDataSystem* DataSubSystem = UTSResourceDataSystem::Get(GetWorld());
	if (!IsValid(DataSubSystem)) return;
	
	FTSResourceStaticData* ResourceStaticData = DataSubSystem->GetResourceStaticData(ItemRuntimeData.StaticDataID);
	if (!ResourceStaticData) return;

	FString InteractKey = TEXT("Left Mouse Button");
	FText InteractText = FText::FromString(InteractKey);
	InteractUI->SetInteractInfo(ResourceStaticData->ResourceUIInfoTable.ResourceName, InteractText);
	
	UTSResourceInteractUI* ResourceInteractUI = CastChecked<UTSResourceInteractUI>(InteractUI);
	if (!IsValid(ResourceInteractUI)) return;
	
	ResourceInteractUI->SetResourceRemainStackInfo(ResourceData.DynamicData.CurrentAmount, ResourceStaticData->ResourceLootInfoTable.TotalCount);
	
	ResourceInteractUI->SetVisibility(ESlateVisibility::Collapsed);
}
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
		InteractWidgetComp->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Collapsed);
	}
}

#pragma endregion
//======================================================================================================================	
#pragma region 자원_API_및_데이터

	//━━━━━━━━━━━━━━━━━━━━
	// 자원_API_및_데이터
	//━━━━━━━━━━━━━━━━━━━━	
	
bool ATSResourceActorBase::TryInteractLogicOnResource_Implementation(float InAttackDamage, FGameplayTag InInteractType, FVector InHitImpactPoint)
{
	if (!IsValid(LootHandleComponent)) return false;
	return LootHandleComponent->RequestSpawnLootFromResource(InAttackDamage, InInteractType, ResourceData, InHitImpactPoint);
}

#pragma endregion
//======================================================================================================================