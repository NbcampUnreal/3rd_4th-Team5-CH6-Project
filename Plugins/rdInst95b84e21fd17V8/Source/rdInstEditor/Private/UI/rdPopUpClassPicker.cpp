//
// rdPopUpClassPicker.cpp
//
// Copyright (c) 2025 Recourse Design ltd. All rights reserved.
//
// Creation Date: 16th March 2025
// Last Modified: 19th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdPopUpClassPicker.h"
#include "rdInstBaseActor.h"
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
// getClassName
//.................................................................................................
FText rdPopUpClassPicker::getClassName() const {

	if(currentClass==nullptr) {
		return FText::FromString(TEXT("(Select Class)"));
//		return FText::FromString(TEXT("(Multi)"));
	}

//	if(currentClass==UrdMultiToken::StaticClass()) {
//		return FText::FromString(TEXT("(Multi)"));
//	}

	FText Value=FText::FromString(*currentClass->GetName());
	return Value;
}

//.................................................................................................
// Construct
//.................................................................................................
void rdPopUpClassPicker::Construct(const FArguments& InArgs) {

	currentClass=InArgs._CurrentClass.Get();
	fileBoxWidth=InArgs._FileBoxWidth;
	OnClassSelected=InArgs._OnClassSelected;
	defaultEmptyValue=InArgs._DefaultEmptyValue.Get();
	classFilter=InArgs._ClassFilter.Get();
	if(currentClass==nullptr) currentClass=classFilter;

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
				.Text(this,&rdPopUpClassPicker::getClassName)
				.MinDesiredWidth(fileBoxWidth)
			]
			.OnGetMenuContent(this,&rdPopUpClassPicker::createClassPicker)
		]
	];
}

//.................................................................................................
// createClassPicker
//.................................................................................................
TSharedRef<SWidget> rdPopUpClassPicker::createClassPicker() {

	//UClass* AllowedClass=classFilter;

	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions ClassViewerOptions;
	ClassViewerOptions.Mode							= EClassViewerMode::ClassPicker;
	ClassViewerOptions.DisplayMode					= EClassViewerDisplayMode::TreeView;
	ClassViewerOptions.bShowObjectRootClass			= true;
	ClassViewerOptions.bIsPlaceableOnly				= true;
	ClassViewerOptions.bIsBlueprintBaseOnly			= true;
	ClassViewerOptions.bShowUnloadedBlueprints		= true;
	ClassViewerOptions.bEnableClassDynamicLoading	= true;
	ClassViewerOptions.bShowNoneOption				= true;
	ClassViewerOptions.NameTypeToDisplay			= EClassViewerNameTypeToDisplay::Dynamic;

#if PLATFORM_WINDOWS
	filter=MakeShareable(new FriClassFilter(classFilter));
#if ENGINE_MAJOR_VERSION>4
	ClassViewerOptions.ClassFilters.Add(filter.ToSharedRef());
#else
	ClassViewerOptions.ClassFilter=filter.ToSharedRef();
#endif
#endif

	ClassViewerOptions.InitiallySelectedClass=currentClass.Get();

	return SNew(SBox)
					.HeightOverride(320)
					.WidthOverride(350)
					[
						SNew(SBorder)
						.BorderImage(GetAppBrush("Menu.Background"))
						[
							ClassViewerModule.CreateClassViewer(ClassViewerOptions,FOnClassPicked::CreateSP(this,&rdPopUpClassPicker::pickerClassSelected))
						]
					];

}

//.................................................................................................
// pickerClassSelected
//.................................................................................................
void rdPopUpClassPicker::pickerClassSelected(UClass* uclass) {

	if(currentClass!=uclass) {
		mnuAnchor->SetIsOpen(false);
		currentClass=uclass;
		if(OnClassSelected.IsBound()) {
			OnClassSelected.Execute(uclass);
		}
	}
}

//.................................................................................................

#undef LOCTEXT_NAMESPACE
