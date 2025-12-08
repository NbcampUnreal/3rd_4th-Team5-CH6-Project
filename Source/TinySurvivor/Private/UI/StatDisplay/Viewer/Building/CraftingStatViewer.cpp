// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatDisplay/Viewer/Building/CraftingStatViewer.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "GameplayTags/System/GameplayDisplaySubSystem.h"
#include "Item/Data/ItemData.h"
#include "UI/StatDisplay/Provider/IDisplayDataProvider.h"

void UCraftingStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
                                       UGameplayTagDisplaySubsystem* DisplaySystem)
{
	Super::ShowStatList(ListView, DataProvider, DisplaySystem);
	FGameplayTag StatTag;
	
	// 내구도
	{
		StatTag = ItemTags::TAG_Display_Stat_Durability;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = FText::AsNumber(FloatValue);
		FText StatUnit = DisplaySystem->GetUnit(StatTag);
		AddStatToList(ListView, StatName, StatValue, StatUnit);
	}
}
