// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TSPickupItem.h"

#include "Character/TSCharacter.h"
#include "Inventory/TSInventoryMasterComponent.h"


// Sets default values
ATSPickupItem::ATSPickupItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ATSPickupItem::CanInteract(ATSCharacter* InstigatorCharacter)
{
	UTSInventoryMasterComponent* InventoryComp = Cast<UTSInventoryMasterComponent>(
		InstigatorCharacter->GetComponentByClass(
			UTSInventoryMasterComponent::StaticClass()));
	if (!InventoryComp)
	{
		return false;
	}
	return true;
}

void ATSPickupItem::Interact(ATSCharacter* InstigatorCharacter)
{
	if (!HasAuthority() || !InstigatorCharacter || ItemInstance.StaticDataID == 0)
	{
		return;
	}

	UTSInventoryMasterComponent* InventoryComp = Cast<UTSInventoryMasterComponent>(
		InstigatorCharacter->GetComponentByClass(
			UTSInventoryMasterComponent::StaticClass()));
	if (!InventoryComp)
	{
		return;
	}
	int32 RemainingQuantity = 0;
	bool bCanAdd = InventoryComp->AddItem(ItemInstance, Quantity, RemainingQuantity);
	if (bCanAdd)
	{
		Destroy();
		// TODO: 추후 풀로 반환하도록 수정
	}
	else
	{
		Quantity = RemainingQuantity;
	}
}

bool ATSPickupItem::RunOnServer()
{
	return true;
}

// Called when the game starts or when spawned
void ATSPickupItem::BeginPlay()
{
	Super::BeginPlay();
}
