// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatDisplay/Viewer/Item/ConsumableStatViewer.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "GameplayTags/System/GameplayDisplaySubSystem.h"
#include "Item/Data/ItemData.h"
#include "UI/StatDisplay/Provider/IDisplayDataProvider.h"

void UConsumableStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
                                         UGameplayTagDisplaySubsystem* DisplaySystem)
{
	Super::ShowStatList(ListView, DataProvider, DisplaySystem);
	FGameplayTag StatTag;
	// 사용 효과/효과 유효 시간
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
	// 지속시간
	{
		StatTag = ItemTags::TAG_Display_Stat_EffectDuration;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		if (FloatValue > 0.f)
		{
			FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
			FText StatValue = FText::AsNumber(FloatValue);
			FText StatUnit = DisplaySystem->GetUnit(StatTag);
			AddStatToList(ListView, StatName, StatValue, StatUnit);
		}
	}
	// 최대 소지 개수
	{
		StatTag = ItemTags::TAG_Display_Stat_MaxStack;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = FText::AsNumber(FloatValue);
		FText StatUnit = DisplaySystem->GetUnit(StatTag);
		AddStatToList(ListView, StatName, StatValue, StatUnit);
	}
	// 부패까지 걸리는 시간
	{
		StatTag = ItemTags::TAG_Display_Stat_DecayRate;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = FText::AsNumber(FloatValue);
		FText StatUnit = DisplaySystem->GetUnit(StatTag);
		AddStatToList(ListView, StatName, StatValue, StatUnit);
	}
}
