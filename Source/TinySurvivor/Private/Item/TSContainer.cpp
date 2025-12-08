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

// Called when the game starts or when spawned
void ATSContainer::BeginPlay()
{
	Super::BeginPlay();
}

void ATSContainer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetInventoryData();
}

void ATSContainer::SetInventoryData()
{
	UItemDataSubsystem* IDS = GetWorld()->GetGameInstance()->GetSubsystem<UItemDataSubsystem>();
	if (!IDS)
	{
		return;
	}

	FBuildingData ItemInfo;
	if (!IDS->GetBuildingDataSafe(ItemInstance.StaticDataID, ItemInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("TSContainer::SetInventoryData: Failed to get ItemData for ID: %d"), ItemInstance.StaticDataID);
		return;
	}
	// 가방 슬롯 개수 설정
	InventoryComp->MaxBagSlotCount = ItemInfo.StorageSlots;
	InventoryComp->InitialBagSlotCount = ItemInfo.StorageSlots;
}
