// rdDebugHud.h - Copyright (c) 2024 Recourse Design ltd.
//
#pragma once
#include "Blueprint/UserWidget.h"
#include "Types/PaintArgs.h"
#include "Layout/Geometry.h"
#include "Layout/SlateRect.h"
#include "Rendering/DrawElements.h"
#include "Styling/WidgetStyle.h"
#include "rdDebugHUD.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

class UTextBlock;

UCLASS()
class RDINST_PLUGIN_API UrdDebugGraph : public UUserWidget {
	GENERATED_BODY()
public:
//#if !UE_BUILD_SHIPPING
	virtual int32 NativePaint(const FPaintArgs& args,const FGeometry& geo,const FSlateRect& cullRect,FSlateWindowElementList& drawElements,int32 layerId,const FWidgetStyle& widgetStyle,bool bParentEnabled) const override;
	TArray<int32> storedData;
	int32 currentCyclicIndex=0;
	TObjectPtr<UTextBlock> stats0label=nullptr;
	TObjectPtr<UTextBlock> stats1label=nullptr;
	TObjectPtr<UTextBlock> stats2label=nullptr;
//#endif
};

UCLASS()
class RDINST_PLUGIN_API UrdDebugHUD : public UUserWidget {
	GENERATED_BODY()
public:
#if !UE_BUILD_SHIPPING
	TObjectPtr<UTextBlock> statsText=nullptr;
	TObjectPtr<UrdDebugGraph> statsGraph=nullptr;
#endif
};

