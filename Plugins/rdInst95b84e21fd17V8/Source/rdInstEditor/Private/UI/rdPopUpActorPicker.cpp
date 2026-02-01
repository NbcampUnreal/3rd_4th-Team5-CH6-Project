//
// rdPopUpAssetPicker.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 19th September 2022
// Last Modified: 3rd July 2022
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdPopUpActorPicker.h"
#include "Editor.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include "SceneOutlinerModule.h"
#include "ActorPickerMode.h"
#if ENGINE_MAJOR_VERSION>4
#include "SceneOutlinerPublicTypes.h"
#include "SceneOutlinerFilters.h"
#include "SceneOutlinerMenuContext.h"
#include "SceneOutlinerDelegates.h"
#include "ActorMode.h"
#include "ActorPickingMode.h"
#include "ActorTreeItem.h"
#endif
#include "rdInstOptions.h"

#define LOCTEXT_NAMESPACE "FrdInstModule"

//.................................................................................................
// getActorName
//.................................................................................................
FText rdPopUpActorPicker::getActorName() const {

	FText val=FText::FromString(TEXT("Select Actor"));
	if(isMultipleValues) val=FText::FromString(TEXT("(Multiple Values)"));
	else if(currentActor) val=FText::FromString(*currentActor->GetName());

	return val;
}

//.................................................................................................
// Construct
//.................................................................................................
void rdPopUpActorPicker::Construct(const FArguments& InArgs) {

	currentActor=InArgs._CurrentActor.Get();
	nameBoxWidth=InArgs._NameBoxWidth.Get();
	onActorSelected=InArgs._OnActorSelected;
	isMultipleValues=InArgs._IsMultipleValues.Get();
	showPicker=InArgs._ShowPicker.Get();
	actorList=InArgs._ActorList.Get();

	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(FMargin(2.0f,2.0f,1.0f,1.0f))
		.AutoWidth()
		.MaxWidth(nameBoxWidth)
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
				.Text(this,&rdPopUpActorPicker::getActorName)
				.MinDesiredWidth(nameBoxWidth)
			]
			.OnGetMenuContent(this,&rdPopUpActorPicker::createActorPicker)
		]

		// Select Button
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(3.0f,3.0f,3.0f,3.0f))
		[
			SNew(SButton)
			.ButtonStyle(GetAppStyle(),"HoverHintOnly")
			.OnClicked(FOnClicked::CreateRaw(this,&rdPopUpActorPicker::browseClicked))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			.IsEnabled(this,&rdPopUpActorPicker::ShouldShowPicker)
			.IsFocusable(false)
			[
				SNew(SImage)
#if ENGINE_MAJOR_VERSION>4
				.Image(GetAppBrush("Icons.EyeDropper"))
#else
				.Image(GetAppBrush("PropertyWindow.Button_PickActorInteractive"))
#endif
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]

		// Clear Button
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(3.0f,3.0f,3.0f,3.0f))
		[
			SNew(SButton)
			.ToolTipText(LOCTEXT("ClearButtonLabel","Clear Selection") )
			.ButtonStyle(GetAppStyle(),"HoverHintOnly")
			.OnClicked(FOnClicked::CreateRaw(this,&rdPopUpActorPicker::clearClicked))
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			.IsFocusable(false)
			[
				SNew(SImage)
#if ENGINE_MAJOR_VERSION>4
				.Image(GetAppBrush("Icons.Delete"))
#else
				.Image(FPluginStyle::Get()->GetBrush("Icons.Cross"))
#endif
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]

	];
}

//.................................................................................................
// clearClicked
//.................................................................................................
FReply rdPopUpActorPicker::clearClicked() {

	if(onActorSelected.IsBound()) onActorSelected.Execute(nullptr);
	currentActor=nullptr;
	isMultipleValues=false;

	return FReply::Handled();
}

//.................................................................................................
// browseClicked
//.................................................................................................
FReply rdPopUpActorPicker::browseClicked() {

	FSlateApplication::Get().DismissAllMenus();

	FActorPickerModeModule& actorPickerMode=FModuleManager::Get().GetModuleChecked<FActorPickerModeModule>("ActorPickerMode");
	actorPickerMode.BeginActorPickingMode(
											FOnGetAllowedClasses(), 
											FOnShouldFilterActor::CreateRaw(this,&rdPopUpActorPicker::IsSelectableActor), 
											FOnActorSelected::CreateRaw(this,&rdPopUpActorPicker::OnActorPickedFromPicker)
											);

	return FReply::Handled();
}

bool rdPopUpActorPicker::IsSelectableActor(const AActor* actor) const {
	if(actorList.Num()==0) return true;
	if(actorList.Contains(actor)) return true;
	return false;
}

void rdPopUpActorPicker::OnActorPickedFromPicker(AActor* actor) {

	if(currentActor!=actor) {
		if(onActorSelected.IsBound()) onActorSelected.Execute(actor);
		currentActor=actor;
		isMultipleValues=false;
	}
}

//.................................................................................................
// createAssetPicker
//.................................................................................................
TSharedRef<SWidget> rdPopUpActorPicker::createActorPicker() {

	FSceneOutlinerModule& outlinerModule=FModuleManager::LoadModuleChecked<FSceneOutlinerModule>("SceneOutliner");

#if ENGINE_MAJOR_VERSION>4
	FSceneOutlinerInitializationOptions options;
	options.bShowHeaderRow=false;
	options.bFocusSearchBoxWhenOpened=true;
	options.Filters->AddFilterPredicate<FActorTreeItem>(FActorTreeItem::FFilterPredicate::CreateRaw(this,&rdPopUpActorPicker::IsSelectableActor));

	return SNew(SBox)
					.HeightOverride(320)
					.WidthOverride(350)
					[
						SNew(SBorder)
						.BorderImage(GetAppBrush("Menu.Background"))
						[
							outlinerModule.CreateActorPicker(options,FOnActorPicked::CreateRaw(this,&rdPopUpActorPicker::pickerActorSelected))
						]
					];
#else
	SceneOutliner::FInitializationOptions options;
	options.Mode=ESceneOutlinerMode::ActorPicker;			
	options.bShowHeaderRow=false;
	options.bFocusSearchBoxWhenOpened=true;
	options.Filters->AddFilterPredicate(SceneOutliner::FActorFilterPredicate::CreateRaw(this,&rdPopUpActorPicker::IsSelectableActor));

	return SNew(SBox)
					.HeightOverride(320)
					.WidthOverride(350)
					[
						SNew(SBorder)
						.BorderImage(GetAppBrush("Menu.Background"))
						[
							outlinerModule.CreateSceneOutliner(options,FOnActorPicked::CreateRaw(this,&rdPopUpActorPicker::pickerActorSelected))
						]
					];
#endif
}

//.................................................................................................
// pickerActorSelected
//.................................................................................................
void rdPopUpActorPicker::pickerActorSelected(AActor* actor) {

	mnuAnchor->SetIsOpen(false);
	if(currentActor!=actor) {
		if(onActorSelected.IsBound()) onActorSelected.Execute(actor);
		currentActor=actor;
		isMultipleValues=false;
	}
}

//.................................................................................................

#undef LOCTEXT_NAMESPACE
