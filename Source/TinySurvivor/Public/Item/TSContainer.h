// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TSInteractionActorBase.h"
#include "TSContainer.generated.h"

class UTSInventoryMasterComponent;

UCLASS()
class TINYSURVIVOR_API ATSContainer : public ATSInteractionActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSContainer();
#pragma region IInteraction
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
#pragma endregion
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	void SetInventoryData();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UTSInventoryMasterComponent> InventoryComp;
};
