// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_ITEM/Item/TSPickupItem.h"
#include "A_FOR_INGAME/SECTION_ITEM/Inventory/TSInventoryMasterComponent.h"
#include "A_FOR_INGAME/SECTION_ITEM/Item/Runtime/ItemInstance.h"
#include "A_FOR_INGAME/SECTION_PLAYER/Character/TSCharacter.h"


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
