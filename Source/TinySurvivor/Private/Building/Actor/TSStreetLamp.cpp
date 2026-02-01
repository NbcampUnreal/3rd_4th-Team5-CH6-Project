// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/Actor/TSStreetLamp.h"

#include "Character/TSCharacter.h"
#include "Controller/TSPlayerController.h"
#include "Inventory/TSLampInventory.h"
#include "Item/Data/BuildingData.h"
#include "Net/UnrealNetwork.h"
#include "System/Erosion/TSErosionSubSystem.h"


// Sets default values
ATSStreetLamp::ATSStreetLamp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	LampInventory = CreateDefaultSubobject<UTSLampInventory>(TEXT("LampInventory"));
	LampInventory->HotkeySlotCount = 0;
	LampInventory->InitialBagSlotCount = 1;
}

void ATSStreetLamp::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATSStreetLamp, StartTime);
	DOREPLIFETIME(ATSStreetLamp, bIsFueling);
	DOREPLIFETIME(ATSStreetLamp, MaintenanceInterval);
}

void ATSStreetLamp::OnRep_IsFueling()
{
	OnFuelModeChanged.Broadcast(bIsFueling);
}

bool ATSStreetLamp::CanInteract(ATSCharacter* InstigatorCharacter)
{
	UTSInventoryMasterComponent* CharacterInventoryComp = Cast<UTSInventoryMasterComponent>(
		InstigatorCharacter->GetComponentByClass(
			UTSInventoryMasterComponent::StaticClass()));
	if (!CharacterInventoryComp)
	{
		return false;
	}
	return true;
}

void ATSStreetLamp::Interact(ATSCharacter* InstigatorCharacter)
{
	if (!InstigatorCharacter || !InstigatorCharacter->IsLocallyControlled())
	{
		return;
	}
	ATSPlayerController* PC = Cast<ATSPlayerController>(InstigatorCharacter->GetController());
	if (!PC)
	{
		return;
	}
	PC->ToggleContainer(EContentWidgetIndex::FuelMode, this);
}

bool ATSStreetLamp::RunOnServer()
{
	return false;
}

void ATSStreetLamp::InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID)
{
	Super::InitializeFromBuildingData(BuildingInfo, StaticDataID);
	if (HasAuthority())
	{
		MaintenanceCostID = BuildingInfo.MaintenanceCostID;
		MaintenanceInterval = BuildingInfo.MaintenanceInterval;
		MaintenanceCostQty = BuildingInfo.MaintenanceCostQty;
	}
}

void ATSStreetLamp::BeginPlay()
{
	Super::BeginPlay();
	SetNetUpdateFrequency(30.f);
	if (HasAuthority())
	{
		LampInventory->SetMaintenanceCostID(MaintenanceCostID);
		LampInventory->SetMaintenanceCostQty(MaintenanceCostQty);

		// 델리게이트 바인딩
		LampInventory->OnFuelTransferred.AddDynamic(this, &ATSStreetLamp::SetFuelTimer);

		// 처음 연료 넣기
		FItemInstance TempItemInstance(MaintenanceCostID, GetWorld()->GetTimeSeconds());
		int32 TempRemainingQuantity = 0;
		LampInventory->AddItem(TempItemInstance, 1, TempRemainingQuantity);

		// 연료 타이머 설정
		SetFuelTimer();
	}
}

void ATSStreetLamp::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FuelTimerHandle);
	bIsFueling = false;
	Super::EndPlay(EndPlayReason);
}

void ATSStreetLamp::ChangeLightScaleByErosion(float CurrentErosionScale)
{
	if (!bIsFueling)
	{
		return;
	}
	Super::ChangeLightScaleByErosion(CurrentErosionScale);
}

void ATSStreetLamp::SetFuelTimer()
{
	if (bIsFueling)
	{
		return;
	}
	GetWorldTimerManager().SetTimer(FuelTimerHandle, this, &ATSStreetLamp::UseFuel, MaintenanceInterval, true, 0.f);
	bIsFueling = true;
}

void ATSStreetLamp::UseFuel()
{
	if (LampInventory->GetItemCount(MaintenanceCostID) > 0)
	{
		// 연료 사용
		UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
		LampInventory->ConsumeItem(MaintenanceCostID, 1);
		ChangeLightScaleByErosion(ErosionSubSystem->GetCurrentErosion());
		StartTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		// 연료 없으면 불 끄기
		SetLightScale(0);
		GetWorldTimerManager().ClearTimer(FuelTimerHandle);
		bIsFueling = false;
	}
	OnFuelModeChanged.Broadcast(bIsFueling);
}
