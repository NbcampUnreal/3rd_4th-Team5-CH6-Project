// All CopyRight From YulRyongGameStudio //


#pragma once

#include "CoreMinimal.h"
#include "A_FOR_INGAME/SECTION_UI/StatDisplay/Viewer/StatViewer.h"
#include "CraftingStatViewer.generated.h"

/**
 * 
 */
UCLASS()
class TINYSURVIVOR_API UCraftingStatViewer : public UStatViewer
{
	GENERATED_BODY()
public:
	virtual void ShowStatList(UListView* ListView, const IDisplayDataProvider& DataProvider, UGameplayTagDisplaySubsystem* DisplaySystem) override;
};
