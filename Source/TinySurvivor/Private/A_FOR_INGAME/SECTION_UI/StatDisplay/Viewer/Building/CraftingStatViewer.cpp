// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Viewer/Building/CraftingStatViewer.h"
#include "GameplayTagContainer.h"
#include "A_FOR_COMMON/Tag/ItemGameplayTags.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Provider/IDisplayDataProvider.h"
#include "A_FOR_INGAME/SECTION_UI/TagDisplay/System/GameplayDisplaySubSystem.h"

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
