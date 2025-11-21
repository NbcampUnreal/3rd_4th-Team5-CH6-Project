// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteraction.generated.h"

class UWidgetComponent;
class ATSCharacter;

UINTERFACE()
class UIInteraction : public UInterface
{
	GENERATED_BODY()
};

class TINYSURVIVOR_API IIInteraction
{
	GENERATED_BODY()

public:
	virtual void ShowInteractionWidget(ATSCharacter* InstigatorCharacter) = 0;
	virtual void HideInteractionWidget() = 0;
	virtual void SetInteractionText(FText InteractionText) = 0;
	virtual bool CanInteract(ATSCharacter* InstigatorCharacter) = 0;
	virtual void Interact(ATSCharacter* InstigatorCharacter) = 0;
	virtual bool RunOnServer() = 0;
};
