// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TSInteractionActorBase.h"
#include "TSCraftingTable.generated.h"

class UTSCraftingTableInventory;
class UCraftingDataSubsystem;
class ATSCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftComplete, int32, SlotIndex);
UCLASS()
class TINYSURVIVOR_API ATSCraftingTable : public ATSInteractionActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSCraftingTable();

#pragma region IInteraction
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
#pragma endregion

#pragma region Crafting
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerRequestCraft(int32 RecipeID, ATSCharacter* InstigatorCharacter);
	UFUNCTION()
	bool CanCraft(int32 RecipeID, ATSCharacter* InstigatorCharacter);
	UFUNCTION()
	void StartCrafting(int32 RecipeID, ATSCharacter* InstigatorCharacter);
	// 제작 완료 브로드캐스트 RPC
	UFUNCTION(Client, Reliable)
	void ClientNotifyCraftResult(int32 SlotIndex);
	
	UPROPERTY(BlueprintAssignable)
	FOnCraftComplete OnCraftComplete;
#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UTSCraftingTableInventory> CraftingInventory;
};
