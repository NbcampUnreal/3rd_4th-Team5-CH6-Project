// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Viewer/Building/StorageStatViewer.h"
#include "GameplayTagContainer.h"
#include "A_FOR_COMMON/Tag/ItemGameplayTags.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Provider/IDisplayDataProvider.h"
#include "A_FOR_INGAME/SECTION_UI/TagDisplay/System/GameplayDisplaySubSystem.h"

void UStorageStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
	UGameplayTagDisplaySubsystem* DisplaySystem)
{
	Super::ShowStatList(ListView, DataProvider, DisplaySystem);
	FGameplayTag StatTag;
	// 용량
	{
		StatTag = ItemTags::TAG_Display_Stat_StorageSlots;
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
