// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Viewer/Item/ToolStatViewer.h"
#include "GameplayTagContainer.h"
#include "A_FOR_COMMON/Tag/ItemGameplayTags.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Provider/IDisplayDataProvider.h"
#include "A_FOR_INGAME/SECTION_UI/TagDisplay/System/GameplayDisplaySubSystem.h"

void UToolStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
                                   UGameplayTagDisplaySubsystem* DisplaySystem)
{
	Super::ShowStatList(ListView, DataProvider, DisplaySystem);
	FGameplayTag StatTag;
	// 채취 유형
	{
		StatTag = ItemTags::TAG_Display_Stat_HarvestTarget;
		FGameplayTagContainer HarvestTargets = DataProvider.GetTagContainer(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = DisplaySystem->GetDisplayNamesFromContainer_KR(HarvestTargets);
		AddStatToList(ListView, StatName, StatValue);
	}
	// 채취 공격
	{
		StatTag = ItemTags::TAG_Display_Stat_AttackDamage;
		float FloatValue = DataProvider.GetStatValue(StatTag);
		FText StatName = DisplaySystem->GetDisplayName_KR(StatTag);
		FText StatValue = FText::AsNumber(FloatValue);
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
