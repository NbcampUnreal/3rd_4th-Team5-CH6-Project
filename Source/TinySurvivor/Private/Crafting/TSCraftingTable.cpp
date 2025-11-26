// Fill out your copyright notice in the Description page of Project Settings.


#include "Crafting/TSCraftingTable.h"

#include "Controller/TSPlayerController.h"
#include "Character/TSCharacter.h"
#include "Crafting/System/CraftingDataSubsystem.h"
#include "Inventory/TSInventoryMasterComponent.h"

// Sets default values
ATSCraftingTable::ATSCraftingTable()
{
	InventoryComp = CreateDefaultSubobject<UTSInventoryMasterComponent>(TEXT("InventoryComp"));
	InventoryComp->HotkeySlotCount = 0;
	InventoryComp->InitialBagSlotCount = 1;
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
	PC->ToggleContentsWidget(EContentWidgetIndex::CraftingMode);
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
	InventoryComp->AddItem(RecipeData.ResultItemID, RecipeData.ResultCount, RemainingQuantity);
}

// Called when the game starts or when spawned
void ATSCraftingTable::BeginPlay()
{
	Super::BeginPlay();
}
