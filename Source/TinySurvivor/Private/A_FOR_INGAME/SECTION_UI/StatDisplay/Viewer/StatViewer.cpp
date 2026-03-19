// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Viewer/StatViewer.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/StatDataObject.h"
#include "A_FOR_INGAME/SECTION_UI/TagDisplay/System/GameplayDisplaySubSystem.h"
#include "Components/ListView.h"

void UStatViewer::ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider,
                               UGameplayTagDisplaySubsystem* DisplaySystem)
{
	if (!ListView || !DisplaySystem)
	{
		return;
	}
}

void UStatViewer::AddStatToList(UListView* ListView, const FText& StatName, const FText& StatValue,
                                const FText& StatUnit)
{
	if (!ListView)
	{
		return;
	}
	UStatDataObject* StatData = NewObject<UStatDataObject>();
	StatData->StatName = StatName;
	StatData->StatValue = StatValue;
	StatData->StatUnit = StatUnit;
	ListView->AddItem(StatData);
}
