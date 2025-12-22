// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TSBuildingActorBase.h"
#include "TSContainer.generated.h"

class UTSInventoryMasterComponent;

UCLASS()
class TINYSURVIVOR_API ATSContainer : public ATSBuildingActorBase
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
	virtual void Multicast_PlayDestroyEffect_Implementation() const override;
	void DropAllItems() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UTSInventoryMasterComponent> InventoryComp;
};
