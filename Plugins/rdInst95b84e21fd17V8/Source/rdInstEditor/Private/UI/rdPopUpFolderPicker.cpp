//
// rdPopUpFolderPicker.cpp
//
// Copyright (c) 2021 Recourse Design ltd. All rights reserved.
//
// Creation Date: 11th December 2021
// Last Modified: 19th March 2023
//
#include "rdPopUpFolderPicker.h"
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
#include "rdInstEditor.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//.................................................................................................
// getFolderName
//.................................................................................................
FText rdPopUpFolderPicker::getFolderName() const {

	FText Value;
	if(currentFolder.IsEmpty()) {
		Value=FText::FromString(TEXT("(Select a Folder)"));
	} else {
		if(purePath) {
			Value=FText::FromString(*currentFolder);
		} else {
			FString basePath=FPaths::ProjectContentDir();		
			FString fld=currentFolder.Right(currentFolder.Len()-basePath.Len()+1);
			Value=FText::FromString(*fld);
		}
	}
	return Value;
}

//.................................................................................................
// Construct
//.................................................................................................
void rdPopUpFolderPicker::Construct(const FArguments& InArgs) {

	currentFolder=*InArgs._CurrentFolder.Get();
	folderBoxWidth=InArgs._FolderBoxWidth.Get();
	purePath=InArgs._PurePath.Get();
	prefixGame=InArgs._PrefixGame.Get();
	showBtns=InArgs._ShowBtns.Get();

	OnFolderSelected=InArgs._OnFolderSelected;

	if(showBtns) {

		ChildSlot
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(FMargin(2.0f,2.0f,1.0f,1.0f))
			.AutoWidth()
			.MaxWidth(folderBoxWidth)
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
					.Text(this,&rdPopUpFolderPicker::getFolderName)
					.ToolTipText(this,&rdPopUpFolderPicker::getFolderName)
					.MinDesiredWidth(folderBoxWidth)
				]
				.OnGetMenuContent(this,&rdPopUpFolderPicker::createFolderPicker)
			]

			// Select Button
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(3.0f,3.0f,3.0f,3.0f))
			[
				SAssignNew(btnSelect,SButton)
				.ButtonStyle(GetAppStyle(),"NoBorder")
				.OnClicked(this,&rdPopUpFolderPicker::selectClicked)
				.ToolTipText(LOCTEXT("rdTexTool_SelectTooltip","Use the Selected Folder from the browser"))
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
				.OnClicked(this,&rdPopUpFolderPicker::browseClicked)
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

	} else {

		ChildSlot
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(FMargin(2.0f,2.0f,1.0f,1.0f))
			.AutoWidth()
			.MaxWidth(folderBoxWidth)
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
					.Text(this,&rdPopUpFolderPicker::getFolderName)
					.ToolTipText(this,&rdPopUpFolderPicker::getFolderName)
					.MinDesiredWidth(folderBoxWidth)
				]
				.OnGetMenuContent(this,&rdPopUpFolderPicker::createFolderPicker)
			]
		];
	}
}

//.................................................................................................
// selectClicked
//.................................................................................................
FReply rdPopUpFolderPicker::selectClicked() {

	FContentBrowserModule& ContentBrowserModule=FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
#if ENGINE_MAJOR_VERSION<5
	FString currFolder=ContentBrowserModule.Get().GetCurrentPath();
#else
	FContentBrowserItemPath cbi=ContentBrowserModule.Get().GetCurrentPath();
	FString currFolder=cbi.GetInternalPathString();
#endif

	if(purePath) {

	} else {
		currFolder=currFolder.Right(currFolder.Len()-6);

		FString basePath=FPaths::ProjectContentDir();
		currentFolder=basePath+currFolder+TEXT("/");
	}

	if(OnFolderSelected.IsBound()) {
		OnFolderSelected.Execute(currentFolder);
	}
	lastFolder=currFolder;

	return FReply::Handled();
}

//.................................................................................................
// browseClicked
//.................................................................................................
FReply rdPopUpFolderPicker::browseClicked() {

	FContentBrowserModule& ContentBrowserModule=FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	FString basePath=FPaths::ProjectContentDir();		
	FString fld=currentFolder.Right(currentFolder.Len()-basePath.Len()+1);
	TArray<FString> paths={TEXT("/Game")+fld};
	ContentBrowserModule.Get().SetSelectedPaths(paths);

	return FReply::Handled();
}

//.................................................................................................
// createFolderPicker
//.................................................................................................
TSharedRef<SWidget> rdPopUpFolderPicker::createFolderPicker() {

	FPathPickerConfig PathPickerConfig;

	PathPickerConfig.DefaultPath=*currentFolder;
	PathPickerConfig.OnPathSelected=FOnPathSelected::CreateSP(this,&rdPopUpFolderPicker::pickerFolderSelected);
	PathPickerConfig.bAllowClassesFolder=false;
	PathPickerConfig.bAddDefaultPath=false;
	PathPickerConfig.bAllowContextMenu=false;
	PathPickerConfig.bFocusSearchBoxWhenOpened=false;

	FContentBrowserModule& ContentBrowserModule=FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	return SNew(SBox)
					.HeightOverride(320)
					.WidthOverride(350)
					[
						SNew(SBorder)
						.BorderImage(GetAppBrush("Menu.Background"))
						[
							ContentBrowserModule.Get().CreatePathPicker(PathPickerConfig)
						]
					];
}

//.................................................................................................
// pickerFolderSelected
//.................................................................................................
void rdPopUpFolderPicker::pickerFolderSelected(const FString& FolderPath) {

	mnuAnchor->SetIsOpen(false);
	if(FolderPath.IsEmpty() || FolderPath==TEXT("/Game")) {

		if(purePath) {
			currentFolder=FolderPath+TEXT("/");
		} else {
			currentFolder=FPaths::ProjectContentDir();
		}
		if(OnFolderSelected.IsBound()) {
			OnFolderSelected.Execute(currentFolder);
		}
		lastFolder=currentFolder;

	} else if(FolderPath!=lastFolder) {

		if(purePath) {

			FString p=FolderPath;
			if(prefixGame) {

				if(p.StartsWith(TEXT("/All/"))) {
					p.RemoveAt(0,4);
				}
				if(!p.StartsWith(TEXT("/Game/"))) {
					p=TEXT("/Game/")+p;
				}

			} else {
				if(p.StartsWith(TEXT("/All/"))) p.RemoveAt(0,4);
				else if(p.StartsWith(TEXT("/Game/"))) p.RemoveAt(0,6);
				else if(p.StartsWith(TEXT("Game/"))) p.RemoveAt(0,5);
			}

			currentFolder=p+TEXT("/");
		} else {
			FString basePath=FPaths::ProjectContentDir();
			currentFolder=basePath+FolderPath.Right(FolderPath.Len()-6)+TEXT("/");
		}

		if(OnFolderSelected.IsBound()) {
			OnFolderSelected.Execute(currentFolder);
		}
		lastFolder=FolderPath;
	}
}

//.................................................................................................
#undef LOCTEXT_NAMESPACE
