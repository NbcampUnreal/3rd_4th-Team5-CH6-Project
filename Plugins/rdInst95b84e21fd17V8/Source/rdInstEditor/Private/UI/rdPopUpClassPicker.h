// Copyright (c) 2022 Recourse Design ltd.
#pragma once

#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "AssetRegistry/AssetData.h"
#else
#include "AssetData.h"
#endif
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "SClassViewer.h"

DECLARE_DELEGATE_OneParam(FOnClassSelected,const UClass*);

class FriClassFilter : public IClassViewerFilter {
public:
	FriClassFilter(UClass* addClass)	{ classList.Add(addClass); }
	TSet<const UClass*> classList;
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& initOptions,const UClass* tstClass,TSharedRef<FClassViewerFilterFuncs> filterFuncs) override { 
		return filterFuncs->IfInChildOfClassesSet(classList,tstClass)==EFilterReturn::Passed;
	}
	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& initOptions,const TSharedRef<const IUnloadedBlueprintData> unloadedClassData,TSharedRef<FClassViewerFilterFuncs> filterFuncs) override { 
		return filterFuncs->IfInChildOfClassesSet(classList,unloadedClassData)==EFilterReturn::Passed;
	}
};

class rdPopUpClassPicker : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdPopUpClassPicker)
		: _CurrentClass(nullptr)
		, _OnClassSelected()
		, _FileBoxWidth(120)
		, _ClassFilter(UObject::StaticClass())
		, _DefaultEmptyValue(TEXT("(Select Class)"))
	{}
		SLATE_ATTRIBUTE(TWeakObjectPtr<UClass>,CurrentClass)
		SLATE_EVENT(FOnClassSelected,OnClassSelected)
		SLATE_ATTRIBUTE(float,FileBoxWidth)
		SLATE_ATTRIBUTE(UClass*,ClassFilter)
		SLATE_ATTRIBUTE(FString,DefaultEmptyValue)
	SLATE_END_ARGS()

	void						Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget>			createClassPicker();
	void						pickerClassSelected(UClass* uclass);
	FText						getClassName() const;
	FReply						selectClicked();

	TSharedPtr<SMenuAnchor>		mnuAnchor;

	FOnClassSelected			OnClassSelected;
	TAttribute<float>			fileBoxWidth;
	UClass*						classFilter;

	TSharedPtr<SClassViewer>	ClassViewer;
	TSharedPtr<FriClassFilter>	filter;

	TWeakObjectPtr<UClass>		currentClass;
	FString						defaultEmptyValue;
};
