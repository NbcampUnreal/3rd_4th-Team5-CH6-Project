// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TSInteractionActorBase.h"
#include "GameFramework/Actor.h"
#include "TSPickupItem.generated.h"

UCLASS()
class TINYSURVIVOR_API ATSPickupItem : public ATSInteractionActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATSPickupItem();

#pragma region IInteraction
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) override;
	virtual void Interact(ATSCharacter* InstigatorCharacter) override;
	virtual bool RunOnServer() override;
#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
