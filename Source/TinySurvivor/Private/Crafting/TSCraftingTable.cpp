// Fill out your copyright notice in the Description page of Project Settings.


#include "Crafting/TSCraftingTable.h"

#include "Controller/TSPlayerController.h"
#include "Character/TSCharacter.h"
#include "Crafting/System/CraftingDataSubsystem.h"
#include "Inventory/TSCraftingTableInventory.h"
#include "Inventory/TSInventoryMasterComponent.h"

// Sets default values
ATSCraftingTable::ATSCraftingTable()
{
	CraftingInventory = CreateDefaultSubobject<UTSCraftingTableInventory>(TEXT("CraftingInventory"));
}

bool ATSCraftingTable::CanInteract(ATSCharacter* InstigatorCharacter)
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

void ATSCraftingTable::Interact(ATSCharacter* InstigatorCharacter)
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
	PC->ToggleContainer(EContentWidgetIndex::CraftingMode, this);
}

bool ATSCraftingTable::RunOnServer()
{
	return false;
}

void ATSCraftingTable::ServerRequestCraft_Implementation(int32 RecipeID, ATSCharacter* InstigatorCharacter)
{
	UCraftingDataSubsystem* CraftingDataSub = UCraftingDataSubsystem::GetCraftingDataSubsystem(GetWorld());
	if (!CraftingDataSub)
	{
		return;
	}
	FCraftingData RecipeData;
	if (!CraftingDataSub->GetCraftingDataSafe(RecipeID, RecipeData))
	{
		return;
	}
	if (!CanCraft(RecipeID, InstigatorCharacter))
	{
		return;
	}
	StartCrafting(RecipeID, InstigatorCharacter);
}

bool ATSCraftingTable::ServerRequestCraft_Validate(int32 RecipeID, ATSCharacter* InstigatorCharacter)
{
	return RecipeID > 0 && InstigatorCharacter;
}

bool ATSCraftingTable::CanCraft(int32 RecipeID, ATSCharacter* InstigatorCharacter)
{
	if (!HasAuthority())
	{
		return false;
	}
	UCraftingDataSubsystem* CraftingDataSub = UCraftingDataSubsystem::GetCraftingDataSubsystem(GetWorld());
	FCraftingData RecipeData;
	if (!CraftingDataSub->GetCraftingDataSafe(RecipeID, RecipeData))
	{
		return false;
	}

	UTSInventoryMasterComponent* PlayerInventoryComp = Cast<UTSInventoryMasterComponent>(
		InstigatorCharacter->GetComponentByClass(UTSInventoryMasterComponent::StaticClass()));
	if (!PlayerInventoryComp)
	{
		return false;
	}

	for (const FIngredientData& Ingredient : RecipeData.Ingredients)
	{
		// 아이템 개수 확인
		int32 ItemCount = PlayerInventoryComp->GetItemCount(Ingredient.MaterialID);
		if (ItemCount < Ingredient.Count)
		{
			return false;
		}
	}
	return true;
}

void ATSCraftingTable::StartCrafting(int32 RecipeID, ATSCharacter* InstigatorCharacter)
{
	if (!HasAuthority())
	{
		return;
	}
	UCraftingDataSubsystem* CraftingDataSub = UCraftingDataSubsystem::GetCraftingDataSubsystem(GetWorld());
	FCraftingData RecipeData;
	if (!CraftingDataSub->GetCraftingDataSafe(RecipeID, RecipeData))
	{
		return;
	}

	UTSInventoryMasterComponent* PlayerInventoryComp = Cast<UTSInventoryMasterComponent>(
		InstigatorCharacter->GetComponentByClass(UTSInventoryMasterComponent::StaticClass()));
	if (!PlayerInventoryComp)
	{
		return;
	}

	for (const FIngredientData& Ingredient : RecipeData.Ingredients)
	{
		// 아이템 소비
		PlayerInventoryComp->ConsumeItem(Ingredient.MaterialID, Ingredient.Count);
	}
	// 제작대 인벤토리 가방에 제작한 아이템 추가
	int32 RemainingQuantity = RecipeData.ResultCount;
	ATSPlayerController* PC = Cast<ATSPlayerController>(InstigatorCharacter->GetController());
	if (!PC)
	{
		return;
	}

	int32 SlotIndex = CraftingInventory->PlaceCraftResult(PC, RecipeData.ResultItemID, RemainingQuantity);
	if (SlotIndex == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to place craft result"));
		return;
	}
	// 제작 완료 브로드캐스트
	PC->ClientNotifyCraftResult(SlotIndex);
}