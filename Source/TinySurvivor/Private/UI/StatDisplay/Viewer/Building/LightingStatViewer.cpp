// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatDisplay/Viewer/Building/LightingStatViewer.h"
#include "GameplayTags/ItemGameplayTags.h"
#include "GameplayTags/System/GameplayDisplaySubSystem.h"
#include "Item/Data/ItemData.h"
#include "UI/StatDisplay/Provider/IDisplayDataProvider.h"

void ULightingStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
                                       UGameplayTagDisplaySubsystem* DisplaySystem)
{
	Super::ShowStatList(ListView, DataProvider, DisplaySystem);
	FGameplayTag StatTag;
	// 조명 범위
	{
		StatTag = ItemTags::TAG_Display_Stat_LightRadius;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = FText::AsNumber(FloatValue);
		FText StatUnit = DisplaySystem->GetUnit(StatTag);
		AddStatToList(ListView, StatName, StatValue, StatUnit);
	}
	// 침식도 감소량
	{
		StatTag = ItemTags::TAG_Display_Stat_ErosionReduction;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = FText::AsNumber(FloatValue);
		FText StatUnit = DisplaySystem->GetUnit(StatTag);
		AddStatToList(ListView, StatName, StatValue, StatUnit);
	}
	// 정신력 회복
	{
		StatTag = ItemTags::TAG_Item_Effect_SANITY_RESTORE;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = FText::AsNumber(FloatValue);
		FText StatUnit = DisplaySystem->GetUnit(StatTag);
		AddStatToList(ListView, StatName, StatValue, StatUnit);
	}
	// 모닥불: 최대 동작 시간
	{
		StatTag = ItemTags::TAG_Display_Stat_MaxMaintenance;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		if (FloatValue > 0.f)
		{
			FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
			FText StatValue = FText::AsNumber(FloatValue);
			FText StatUnit = DisplaySystem->GetUnit(StatTag);
			AddStatToList(ListView, StatName, StatValue, StatUnit);
		}
	}
	// 연료 재료/소모 간격
	{
		if (DataProvider.GetMaintenanceCostID() > 0)
		{
			StatTag = ItemTags::TAG_Display_Stat_MaintenanceInterval;
			float FloatValue = DataProvider.GetStatValue(StatTag);
			FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
			FText StatValue = FText::Format(FText::FromString("{0} | {1}"),
			                                DataProvider.GetItemName(StatTag),
			                                FText::AsNumber(FloatValue));
			FText StatUnit = DisplaySystem->GetUnit(StatTag);
			AddStatToList(ListView, StatName, StatValue, StatUnit);
		}
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
