// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatDisplay/Viewer/Building/DefenseStatViewer.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "GameplayTags/System/GameplayDisplaySubSystem.h"
#include "Item/Data/ItemData.h"
#include "UI/StatDisplay/Provider/IDisplayDataProvider.h"

void UDefenseStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
	UGameplayTagDisplaySubsystem* DisplaySystem)
{
	Super::ShowStatList(ListView, DataProvider, DisplaySystem);
	FGameplayTag StatTag;
	// 등급
	{
		StatTag = ItemTags::TAG_Display_Stat_Rarity;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText Rarity = FText();
		switch (DataProvider.GetEnumValue(StatTag))
		{
		case 0:
			Rarity = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Item_Rarity_None);
			break;
		case 1:
			Rarity = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Item_Rarity_Common);
			break;
		case 2:
			Rarity = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Item_Rarity_Normal);
			break;
		case 3:
			Rarity = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Item_Rarity_Rare);
			break;
		case 4:
			Rarity = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Item_Rarity_Unique);
			break;
		default:
			break;
		}
		FText StatValue = Rarity;
		AddStatToList(ListView, StatName, StatValue);
	}
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
