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
	virtual void InitializeFromBuildingData(const FBuildingData& BuildingInfo, const int32 StaticDataID) override;
#pragma region IInteraction
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
#pragma endregion
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UTSInventoryMasterComponent> InventoryComp;
};
