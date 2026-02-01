//
// rdInst_UISettings.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.00
//
// Creation Date: 27th October 2024
// Last Modified: 27th October 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "rdInstOptions.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Interfaces/IMainFrameModule.h"
#include "Runtime/Slate/Public/Widgets/Layout/SUniformGridPanel.h"
#include "PropertyEditorModule.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

class FrdInstSettingsOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstSettingsOptionsCustomization();
	TSharedRef<SWidget>			MakeComboWidget(TSharedPtr<FString> InItem);
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	UrdInstOptions*				CurrentOptions;

protected:
};

class rdInstSettingsOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstSettingsOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstSettingsOptions();
	void			Construct(const FArguments& InArgs);
	TSharedRef<SWidget>	MakeComboWidget(TSharedPtr<FString> InItem);

	virtual bool	SupportsKeyboardFocus() const override { return true; }
	virtual FReply	OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override { return (InKeyEvent.GetKey()==EKeys::Escape)?OnCancel():FReply::Unhandled(); }
	FReply			OnConfirm(){
						bUserCancelled=false;
						if(WidgetWindow.IsValid()) {
							WidgetWindow.Pin()->RequestDestroyWindow();
						}
						return FReply::Handled();
					}
	FReply			OnCancel() {
						if(WidgetWindow.IsValid()) {
							WidgetWindow.Pin()->RequestDestroyWindow();
						}
						return FReply::Handled();
					}
	FReply			SetDefault() {
						if(instClass) {
							instClass->SetOptionDefaults(TEXT("MainSettings"),CurrentOptions);
						}
						return FReply::Handled();
					}
	bool			WasUserCancelled() { return bUserCancelled; }

	FrdInstEditorModule*			instClass;
	UrdInstOptions*					CurrentOptions;
	FrdInstSettingsOptionsCustomization* inst;

private:
	TWeakPtr<SWindow>				WidgetWindow;
	bool							bUserCancelled;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdInstSettingsOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstSettingsOptionsCustomization());
}

TSharedRef<SWidget> FrdInstSettingsOptionsCustomization::MakeComboWidget(TSharedPtr<FString> InItem) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#else
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#endif
}

//.............................................................................
// Constructor
//.............................................................................
FrdInstSettingsOptionsCustomization::FrdInstSettingsOptionsCustomization() {
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstSettingsOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdInst options instance in currently edited objects
	CurrentOptions=Cast<UrdInstOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdInstOptions>(Object.Get()); }))->Get());
	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=180.0f;
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);

	// Main Settings ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Settings"));
	rdFullRow(bldr1,lrow1,box1,"Settings");

	// Max Cull Distance (0=none)
	rdLabel(box1,100,"Cull Distance","Cull Distance");
	rdNumBox(box1,"",CurrentOptions->maxCullDistance,0.0f,9999999.0f,100,lcGreen);
	rdLabel(box1,150,"Cull Distance2","(0.0=no limit)");

	// Default BakedDataAsset Folder


	// .......................................................................
}

rdInstSettingsOptions::rdInstSettingsOptions() : instClass(nullptr),CurrentOptions(nullptr),bUserCancelled(true) {}

void rdInstSettingsOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=true;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(),FOnGetDetailCustomizationInstance::CreateLambda([this]() { 
		TSharedRef<IDetailCustomization> tinst=FrdInstSettingsOptionsCustomization::MakeInstance(); inst=(FrdInstSettingsOptionsCustomization*)&(tinst.Get()); return tinst;}));
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2).MaxHeight(700.0f) [ DetailsView->AsShared() ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
			+ SUniformGridPanel::Slot(0,0) [
				rdWinButton("rdInst_CreateSA_SetDef","Set as Default","rdInst_CreateSA_btnToolTip1","Set the current settings as the default",&rdInstSettingsOptions::SetDefault)
			]
			+ SUniformGridPanel::Slot(1,0) [
				rdWinButton("rdInst_CreateSA_Create","Create","rdInst_CreateSA_btnToolTip2","Creates the SpawnActor from the Selected Actors",&rdInstSettingsOptions::OnConfirm)
			]
			+ SUniformGridPanel::Slot(2,0) [
				rdWinButton("rdInst_CreateSA_Cancel","Cancel","rdInst_CreateSA_btnToolTip3","Closes without making any changes",&rdInstSettingsOptions::OnCancel)
			]
		]
	];
}

//.............................................................................
// ShowMainSettings
//.............................................................................
void FrdInstEditorModule::ShowMainSettings() {

	// Create the settings window...
	TSharedRef<SWindow> rdWin=SNew(SWindow)
										.Title(FText::FromString(TEXT("rdInst Settings")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
#if ENGINE_MAJOR_VERSION<5
										.ClientSize(FVector2D(460,290));
#else
										.ClientSize(FVector2D(460,290));
#endif

	FillFromOptionDefaults(TEXT("MainSettings"),rdInstOptions);

	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstSettingsOptions> Options;

	rdWin->SetContent(SAssignNew(Options,rdInstSettingsOptions)
						.WidgetWindow(rdWin)
						.SettingsObjects(OptionObjects)
					  );
	Options->CurrentOptions=rdInstOptions;
	Options->instClass=this;

	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return;
	}

	// Show Settings Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(rdWin,MainFrame.GetParentWindow(),false);
}

//.............................................................................

#undef LOCTEXT_NAMESPACE
