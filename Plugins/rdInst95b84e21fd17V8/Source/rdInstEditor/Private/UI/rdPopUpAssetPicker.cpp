//
// rdPopUpAssetPicker.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 11th November 2021
// Last Modified: 23rd April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdPopUpAssetPicker.h"
#include "rdInstEditor.h"
#include "Modules/ModuleManager.h"
#include "Editor.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "PropertyEditorModule.h"
#include "Engine/Selection.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include "Runtime/SlateCore/Public/Widgets/Images/SImage.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstModule"

//.................................................................................................
// getAssetDetails
//.................................................................................................
FText rdPopUpAssetPicker::getAssetDetails() const {

	UObject* obj=currentAsset.Get().Get();
	if(obj==rdNonDeterminantInstance) {
		return FText::FromString(TEXT("(Multi)"));
	}

	if(!getAssetData().IsValid()) {
		return FText::FromString(TEXT("(Nothing Selected)"));
	}

	return FText::FromString(getAssetData().GetFullName());
}

//.................................................................................................
// getAssetName
//.................................................................................................
FText rdPopUpAssetPicker::getAssetName() const {

	UObject* obj=currentAsset.Get().Get();
	if(obj==rdNonDeterminantInstance) {
		return FText::FromString(TEXT("(Multi)"));
	}
	FText Value=FText::FromString(*defaultEmptyValue);
	FAssetData& CurrentAssetData=getAssetData();

	if(UField* field=Cast<UField>(currentAsset.Get().Get())) {
		Value=field->GetDisplayNameText();
	} else if (CurrentAssetData.IsValid()) {
		Value=FText::FromString(CurrentAssetData.AssetName.ToString());
	}

	return Value;
}

//.................................................................................................
// getAssetData
//.................................................................................................
FAssetData& rdPopUpAssetPicker::getAssetData() const {

	UObject* obj=currentAsset.Get().Get();
	if(obj) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		if(!obj->GetPathName().Equals(lastAssetData.GetSoftObjectPath().ToString(),ESearchCase::CaseSensitive)) {
#else
		if(!obj->GetPathName().Equals(lastAssetData.ObjectPath.ToString(),ESearchCase::CaseSensitive)) {
#endif
			lastAssetData=FAssetData(obj,true);
		}
	} else {
		lastAssetData=FAssetData();
	}

	return lastAssetData;
}

//.................................................................................................
// Construct
//.................................................................................................
void rdPopUpAssetPicker::Construct(const FArguments& InArgs) {

	currentAsset=InArgs._CurrentAsset;
	fileBoxWidth=InArgs._FileBoxWidth;
	OnAssetSelected=InArgs._OnAssetSelected;
	defaultEmptyValue=InArgs._DefaultEmptyValue.Get();
	assetFilter=InArgs._AssetFilter.Get();

	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(FMargin(2.0f,2.0f,1.0f,1.0f))
		.AutoWidth()
		.MaxWidth(fileBoxWidth)
		[
			SAssignNew(mnuAnchor,SComboButton)
			.ForegroundColor(FLinearColor::White)
			.ContentPadding(FMargin(2.0f,1.0f,1.0f,1.0f))
			.MenuPlacement(MenuPlacement_BelowAnchor)
			.ButtonStyle(GetAppStyle(),"PropertyEditor.AssetComboStyle")
			.ButtonContent()
			[
				SNew(STextBlock)
				.TextStyle(GetAppStyle(),"PropertyEditor.AssetClass")
				.Text(this,&rdPopUpAssetPicker::getAssetName)
				.ToolTipText(this,&rdPopUpAssetPicker::getAssetDetails)
				.MinDesiredWidth(fileBoxWidth)
			]
			.OnGetMenuContent(this,&rdPopUpAssetPicker::createAssetPicker)
		]

		// Select Button
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(3.0f,3.0f,3.0f,3.0f))
		[
			SAssignNew(btnSelect,SButton)
			.ButtonStyle(GetAppStyle(),"NoBorder")
			.OnClicked(this,&rdPopUpAssetPicker::selectClicked)
			.ToolTipText(LOCTEXT("rdTexTool_SelectTooltip","Use the Selected Asset from the browser"))
			[
				SNew(SImage)
#if ENGINE_MAJOR_VERSION<5
				.Image(GetAppBrush(TEXT("PropertyWindow.Button_Use")))
#else
				.Image(GetAppBrush(TEXT("Icons.CircleArrowLeft")))
#endif
			]
		]

		// Browse Button
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(3.0f,3.0f,3.0f,3.0f))
		[
			SAssignNew(btnBrowse,SButton)
			.ButtonStyle(GetAppStyle(),"NoBorder")
			.OnClicked(this,&rdPopUpAssetPicker::browseClicked)
			.ToolTipText(LOCTEXT("rdTexTool_BrowseTooltip","Browse"))
			[
				SNew(SImage)
#if ENGINE_MAJOR_VERSION<5
				.Image(GetAppBrush(TEXT("PropertyWindow.Button_Browse")))
#else
				.Image(GetAppBrush(TEXT("Icons.Search")))
#endif
			]
		]
	];
}

//.................................................................................................
// selectClicked
//.................................................................................................
FReply rdPopUpAssetPicker::selectClicked() {

	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

	UObject* obj=GEditor->GetSelectedObjects()->GetTop(assetFilter);
	if(obj && OnAssetSelected.IsBound()) {
		OnAssetSelected.Execute(FAssetData(obj,true));
	}

	return FReply::Handled();
}

//.................................................................................................
// browseClicked
//.................................................................................................
FReply rdPopUpAssetPicker::browseClicked() {

	const FAssetData& AssetData=getAssetData();
	if(AssetData.IsValid()) {
		TArray<FAssetData> Objects;
		Objects.Add(AssetData);
		GEditor->SyncBrowserToObjects(Objects);
	}
	return FReply::Handled();
}

//.................................................................................................
// createAssetPicker
//.................................................................................................
TSharedRef<SWidget> rdPopUpAssetPicker::createAssetPicker() {

	UClass* AllowedClass=assetFilter;

	FContentBrowserModule& ContentBrowserModule=FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	FAssetPickerConfig AssetPickerConfig;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	AssetPickerConfig.Filter.ClassPaths.Add(AllowedClass->GetClassPathName());
#else
	AssetPickerConfig.Filter.ClassNames.Add(AllowedClass->GetFName());
#endif
	AssetPickerConfig.bAllowNullSelection=true;
	AssetPickerConfig.Filter.bRecursiveClasses=true;
	AssetPickerConfig.OnAssetSelected=FOnAssetSelected::CreateSP(this,&rdPopUpAssetPicker::pickerAssetSelected);
	AssetPickerConfig.InitialAssetViewType=EAssetViewType::List;
	AssetPickerConfig.bAllowDragging=false;

	return SNew(SBox)
					.HeightOverride(320)
					.WidthOverride(350)
					[
						SNew(SBorder)
						.BorderImage(GetAppBrush("Menu.Background"))
						[
							ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
						]
					];
}

//.................................................................................................
// pickerAssetSelected
//.................................................................................................
void rdPopUpAssetPicker::pickerAssetSelected(const struct FAssetData& AssetData) {

	const FAssetData& CurrentAssetData=getAssetData();
	if(CurrentAssetData!=AssetData) {
		mnuAnchor->SetIsOpen(false);
		if(OnAssetSelected.IsBound()) {
			OnAssetSelected.Execute(AssetData);
		}
	}
}

//.................................................................................................

#undef LOCTEXT_NAMESPACE
