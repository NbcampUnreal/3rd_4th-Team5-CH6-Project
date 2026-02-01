//
// rdInst_UIUpdateSpawnActor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.00
//
// Creation Date: 2nd November 2024
// Last Modified: 10th April 2025
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
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "rdPopUpFolderPicker.h"
#include "rdInstSubsystem.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

class FrdInstUpdateSpawnActorOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstUpdateSpawnActorOptionsCustomization();
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	UrdInstOptions*				CurrentOptions;
protected:
};

class rdInstUpdateSpawnActorOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstUpdateSpawnActorOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstUpdateSpawnActorOptions();
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
							instClass->SetOptionDefaults(TEXT("UpdateSpawnActor"),CurrentOptions);
						}
						return FReply::Handled();
					}
	bool			WasUserCancelled() { return bUserCancelled; }

	FrdInstEditorModule*			instClass;
	UrdInstOptions*					CurrentOptions;
	FrdInstUpdateSpawnActorOptionsCustomization* inst;

private:
	TWeakPtr<SWindow>				WidgetWindow;
	bool							bUserCancelled;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdInstUpdateSpawnActorOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstUpdateSpawnActorOptionsCustomization());
}


//.............................................................................
// Constructor
//.............................................................................
FrdInstUpdateSpawnActorOptionsCustomization::FrdInstUpdateSpawnActorOptionsCustomization() {
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstUpdateSpawnActorOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdInst options instance in currently edited objects
	CurrentOptions=Cast<UrdInstOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdInstOptions>(Object.Get()); }))->Get());
	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=180.0f;
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);

	// Create Settings ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Update Settings"));

	// Add Items from Selection not already in the SpawnActor
	rdFullRow(bldr1,lrow1a,box1a,"");
	rdCheckbox(box1a,CurrentOptions->updateSA_addItems,"rdInst_UpdateSA_AddItems","Add Items from Selection","rdInst_UpdateSA_ToolTip5","When ticked, Add Items from Selection not already in the SpawnActor");

	// Remove Items that exist in the SpawnActor, but not in the selection
	rdFullRow(bldr1,lrow1b,box1b,"");
	rdCheckbox(box1b,CurrentOptions->updateSA_removeItems,"rdInst_UpdateSA_RemoveItems","Remove Items not in Selection","rdInst_UpdateSA_ToolTip6","When ticked, Remove Items that exist in the SpawnActor, but not in the Selection");

	// Update transforms of Selection
	rdFullRow(bldr1,lrow1c,box1c,"");
	rdCheckbox(box1c,CurrentOptions->updateSA_updateTransforms,"rdInst_UpdateSA_UpdateTransforms","Update Transforms of Selection","rdInst_UpdateSA_ToolTip7","When ticked, Updates the Transforms in the SpawnActor from the Selection");

	// Harvest Instances
	rdFullRow(bldr1,lrow1d,box1d,"");
	rdCheckbox(box1d,CurrentOptions->harvestInstances,"rdInst_UpdateSA_HarvestInstances","Harvest Instances","rdInst_UpdateSA_ToolTip8","When ticked, instances contained in actors are converted to rdInstances");

	// Grid Settings ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr3=DetailBuilder.EditCategory(TEXT("Grid Settings"));
	rdFullRow(bldr3,lrow3a,box3a,"Grid Settings");

	// Grid Cell Numbers
	rdLabel(box3a,90,"NumRows","Grid Rows:");
	rdSpinBoxI(box3a,CurrentOptions->spawnActorGridX,1,256,60,"SetNumGridRows_TT1","The number of rows to split the GridArray into. Each row/column cell is a cluster of entities collated by location");
	rdLabel(box3a,90,"NumCols","Grid Columns:");
	rdSpinBoxI(box3a,CurrentOptions->spawnActorGridY,1,256,60,"SetNumGridCols_TT1","The number of columns to split the GridAwway into. Each row/column cell is a cluster of entities collated by location");

	// .......................................................................
}

rdInstUpdateSpawnActorOptions::rdInstUpdateSpawnActorOptions() : instClass(nullptr),CurrentOptions(nullptr),bUserCancelled(true) {}

void rdInstUpdateSpawnActorOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=true;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(),FOnGetDetailCustomizationInstance::CreateLambda([this]() { 
		TSharedRef<IDetailCustomization> tinst=FrdInstUpdateSpawnActorOptionsCustomization::MakeInstance(); inst=(FrdInstUpdateSpawnActorOptionsCustomization*)&(tinst.Get()); return tinst;}));
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2).MaxHeight(700.0f) [ DetailsView->AsShared() ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
			+ SUniformGridPanel::Slot(0,0) [
				rdWinButton("rdInst_UpdateSA_SetDef","Set as Default","rdInst_UpdateSA_btnToolTip1","Set the current settings as the default",&rdInstUpdateSpawnActorOptions::SetDefault)
			]
			+ SUniformGridPanel::Slot(1,0) [
				rdWinButton("rdInst_UpdateSA_Update","Update","rdInst_UpdateSA_btnToolTip2","Updates the SpawnActor from the Selected Actors",&rdInstUpdateSpawnActorOptions::OnConfirm)
			]
			+ SUniformGridPanel::Slot(2,0) [
				rdWinButton("rdInst_UpdateSA_Cancel","Cancel","rdInst_UpdateSA_btnToolTip3","Closes without making any changes",&rdInstUpdateSpawnActorOptions::OnCancel)
			]
		]
	];
}

//.............................................................................
// ShowUpdateSpawnActorSettings
//.............................................................................
bool FrdInstEditorModule::ShowUpdateSpawnActorSettings() {

	// Create the settings window...
	TSharedRef<SWindow> rdWin=SNew(SWindow)
										.Title(FText::FromString(TEXT("Update rdSpawnActor")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
#if ENGINE_MAJOR_VERSION<5
										.ClientSize(FVector2D(560,220));
#else
										.ClientSize(FVector2D(560,220));
#endif

	FillFromOptionDefaults(TEXT("UpdateSpawnActor"),rdInstOptions);

	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstUpdateSpawnActorOptions> Options;

	rdWin->SetContent(SAssignNew(Options,rdInstUpdateSpawnActorOptions)
						.WidgetWindow(rdWin)
						.SettingsObjects(OptionObjects)
					  );
	Options->CurrentOptions=rdInstOptions;
	Options->instClass=this;

	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return false;
	}

	// Show Settings Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(rdWin,MainFrame.GetParentWindow(),false);

	return !Options->WasUserCancelled();
}

//.............................................................................

#undef LOCTEXT_NAMESPACE
