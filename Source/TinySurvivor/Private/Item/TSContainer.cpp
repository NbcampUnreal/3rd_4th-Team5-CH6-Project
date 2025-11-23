// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TSContainer.h"

#include "Character/TSCharacter.h"
#include "Controller/TSPlayerController.h"
#include "Inventory/TSInventoryMasterComponent.h"


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
	PC->ToggleContainer(this);
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
