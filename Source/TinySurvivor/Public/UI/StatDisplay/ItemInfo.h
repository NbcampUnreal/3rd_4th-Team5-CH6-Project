// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInfo.generated.h"

class IDisplayDataProvider;
class UStatViewerFactory;
struct FGameplayTag;
struct FBuildingData;
struct FItemData;
class UListView;
class UStatDataObject;
class UGameplayTagDisplaySubsystem;

UCLASS()
class TINYSURVIVOR_API UItemInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	TObjectPtr<UListView> ItemStatView;

	UFUNCTION(BlueprintCallable)
	void UpdateCraftingItemStatView(const FItemData& ItemData);
	UFUNCTION(BlueprintCallable)
	void UpdateBuildingItemStatView(const FBuildingData& BuildingData);

	void Internal_UpdateStatView(const IDisplayDataProvider& DataProvider);

	UPROPERTY()
	TObjectPtr<UGameplayTagDisplaySubsystem> TagDisplaySubsystem = nullptr;
	UPROPERTY()
	TObjectPtr<UStatViewerFactory> StatViewerFactory = nullptr;
};
