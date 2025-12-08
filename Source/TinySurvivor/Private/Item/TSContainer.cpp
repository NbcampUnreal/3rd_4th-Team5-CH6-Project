// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TSContainer.h"

#include "Character/TSCharacter.h"
#include "Controller/TSPlayerController.h"
#include "Inventory/TSInventoryMasterComponent.h"
#include "Item/Data/BuildingData.h"
#include "Item/System/ItemDataSubsystem.h"


// Sets default values
ATSContainer::ATSContainer()
{
	PrimaryActorTick.bCanEverTick = false;
	InventoryComp = CreateDefaultSubobject<UTSInventoryMasterComponent>(TEXT("InventoryComp"));
}

void ATSContainer::InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID)
{
	Super::InitializeFromBuildingData(BuildingInfo, StaticDataID);
	if (HasAuthority())
	{
		// 가방 슬롯 개수 설정
		InventoryComp->MaxBagSlotCount = BuildingInfo.StorageSlots;
		InventoryComp->InitialBagSlotCount = BuildingInfo.StorageSlots;
	}
}

bool ATSContainer::CanInteract(ATSCharacter* InstigatorCharacter)
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

void ATSContainer::Interact(ATSCharacter* InstigatorCharacter)
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
	PC->ToggleContainer(EContentWidgetIndex::Container,this);
}

bool ATSContainer::RunOnServer()
{
	return false;
}

