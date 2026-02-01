// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatDisplay/Viewer/Item/ArmorStatViewer.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "GameplayTags/System/GameplayDisplaySubSystem.h"
#include "Item/Data/ItemData.h"
#include "UI/StatDisplay/Provider/IDisplayDataProvider.h"

void UArmorStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
                                    UGameplayTagDisplaySubsystem* DisplaySystem)
{
	Super::ShowStatList(ListView, DataProvider, DisplaySystem);
	FGameplayTag StatTag;
	// 장착 부위
	{
		StatTag = ItemTags::TAG_Display_Stat_EquipSlot;
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText EquipSlotText = FText();
		switch (DataProvider.GetEnumValue(StatTag))
		{
		case 0:
			EquipSlotText = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Display_Stat_Head);
			break;
		case 1:
			EquipSlotText = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Display_Stat_Torso);
			break;
		case 2:
			EquipSlotText = DisplaySystem->GetDisplayName_KR(ItemTags::TAG_Display_Stat_Leg);
			break;
		default:
			break;
		}
		FText StatValue = EquipSlotText;
		AddStatToList(ListView, StatName, StatValue);
	}
	// 장착 효과/효과 수치
	{
		StatTag = DataProvider.GetEffectTag();
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FNumberFormattingOptions NumberFormat;
		NumberFormat.AlwaysSign = true;
		FText StatValue = FText::AsNumber(FloatValue, &NumberFormat);
		FText StatUnit = DisplaySystem->GetUnit(StatTag);
		AddStatToList(ListView, StatName, StatValue, StatUnit);
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
