// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/StatDisplay/Viewer/StatViewer.h"
#include "ConsumableStatViewer.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UConsumableStatViewer : public UStatViewer
{
	GENERATED_BODY()
public:
	virtual void ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider, UGameplayTagDisplaySubsystem* DisplaySystem) override;

};
