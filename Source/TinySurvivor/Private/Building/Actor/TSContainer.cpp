// Fill out your copyright notice in the Description page of Project Settings.


#include "Building/Actor/TSContainer.h"

#include "Character/TSCharacter.h"
#include "Controller/TSPlayerController.h"
#include "Inventory/TSInventoryMasterComponent.h"
#include "Item/Data/BuildingData.h"
#include "Item/System/WorldItemPoolSubsystem.h"


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
		
		// 명시적으로 가방 인벤토리 초기화
		InventoryComp->BagInventory.InventorySlotContainer.SetNum(BuildingInfo.StorageSlots);
        
		for (int32 i = 0; i < BuildingInfo.StorageSlots; ++i)
		{
			InventoryComp->BagInventory.InventorySlotContainer[i].SlotType = ESlotType::Any;
		}
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

void ATSContainer::Multicast_PlayDestroyEffect_Implementation() const
{
	Super::Multicast_PlayDestroyEffect_Implementation();
	DropAllItems();
}

void ATSContainer::DropAllItems() const
{
	FTransform DropTransform = GetActorTransform();
	FVector DropLocation = GetActorLocation();
	for (auto Slot :InventoryComp->BagInventory.InventorySlotContainer)
	{
		UWorldItemPoolSubsystem* IPS = GetWorld()->GetSubsystem<UWorldItemPoolSubsystem>();
		if (IPS)
		{
			IPS->DropItem(Slot, DropTransform, DropLocation);
		}
	}
}
