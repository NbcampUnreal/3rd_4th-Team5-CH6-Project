// All CopyRight From YulRyongGameStudio //


#include "A_FOR_INGAME/SECTION_UI/StatDisplay/ItemInfo.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/StatViewerFactory.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Provider/BuildingDataProvider.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Provider/ItemDataProvider.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Viewer/StatViewer.h"
#include "A_FOR_INGAME/SECTION_UI/TagDisplay/System/GameplayDisplaySubSystem.h"
#include "Components/ListView.h"

void UItemInfo::NativeConstruct()
{
	Super::NativeConstruct();
	StatViewerFactory = NewObject<UStatViewerFactory>(this);
	TagDisplaySubsystem = UGameplayTagDisplaySubsystem::Get(this);
}

void UItemInfo::UpdateCraftingItemStatView(const FItemData& ItemData)
{
	FItemDataProvider DataProvider(ItemData);
	Internal_UpdateStatView(DataProvider);
}

void UItemInfo::UpdateBuildingItemStatView(const FBuildingData& BuildingData)
{
	FBuildingDataProvider DataProvider(BuildingData);
	DataProvider.SetWorldContext(GetWorld());
	Internal_UpdateStatView(DataProvider);
}

void UItemInfo::Internal_UpdateStatView(const IDisplayDataProvider& DataProvider)
{
	if (!ItemStatView || !TagDisplaySubsystem || !StatViewerFactory)
	{
		return;
	}
	// 리스트 초기화
	ItemStatView->ClearListItems();
	
	// 카테고리 태그로 스탯 뷰어 가져오기
	FGameplayTag CategoryTag = DataProvider.GetCategoryTag();
	UStatViewer* StatViewer = StatViewerFactory->GetBuilder(CategoryTag);
	if (!StatViewer)
	{
		return;
	}
	StatViewer->ShowStatList(ItemStatView, DataProvider, TagDisplaySubsystem);
}
