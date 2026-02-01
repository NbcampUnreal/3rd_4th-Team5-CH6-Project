//
// rdInst_UIDataAssetFilename.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.00
//
// Creation Date: 28th October 2024
// Last Modified: 2nd November 2024
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
#include "rdPopUpFolderPicker.h"
#include "rdInstSubsystem.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

bool fileExists(const FString& spath,const FString& file);

class FrdInstDataAssetFilenameOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstDataAssetFilenameOptionsCustomization();
	TSharedRef<SWidget>			MakeComboWidget(TSharedPtr<FString> InItem);
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	UrdInstOptions*				CurrentOptions;

	FText						GetFileName() const {
		return FText::FromString(CurrentOptions->dataAssetFilename);
	}
	void						SetFileName(const FText& txt,ETextCommit::Type InTextCommit) const {
		CurrentOptions->dataAssetFilename=txt.ToString();
		CurrentOptions->outputExists=fileExists(CurrentOptions->dataAssetFolder,CurrentOptions->dataAssetFilename);
	}
	void						OnFolderSelected(const FString& FolderPath) {
		CurrentOptions->dataAssetFolder=FolderPath;
		if(FolderPath.Len()>1 && FolderPath[FolderPath.Len()-1]!=L'/') CurrentOptions->dataAssetFolder+=TEXT("/");
		CurrentOptions->outputExists=fileExists(CurrentOptions->dataAssetFolder,CurrentOptions->dataAssetFilename);
	}

protected:
};

class rdInstDataAssetFilenameOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstDataAssetFilenameOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstDataAssetFilenameOptions();
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
							instClass->SetOptionDefaults(TEXT("DataAssetFilename"),CurrentOptions);
						}
						return FReply::Handled();
					}
	bool			WasUserCancelled() { return bUserCancelled; }

	FrdInstEditorModule*			instClass;
	UrdInstOptions*					CurrentOptions;
	FrdInstDataAssetFilenameOptionsCustomization* inst;

private:
	TWeakPtr<SWindow>				WidgetWindow;
	bool							bUserCancelled;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdInstDataAssetFilenameOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstDataAssetFilenameOptionsCustomization());
}

TSharedRef<SWidget> FrdInstDataAssetFilenameOptionsCustomization::MakeComboWidget(TSharedPtr<FString> InItem) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#else
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#endif
}

//.............................................................................
// Constructor
//.............................................................................
FrdInstDataAssetFilenameOptionsCustomization::FrdInstDataAssetFilenameOptionsCustomization() {
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstDataAssetFilenameOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdInst options instance in currently edited objects
	CurrentOptions=Cast<UrdInstOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdInstOptions>(Object.Get()); }))->Get());
	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=180.0f;
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);

	// Filename and Folder ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Filename"));
	rdFullRow(bldr1,lrow1,box1,"Filename");
	rdLabel(box1,50,"rdInst_CreateSA_NameLabel","Name");
	box1->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(SEditableTextBox)
			.Text(this,&FrdInstDataAssetFilenameOptionsCustomization::GetFileName)
			.OnTextCommitted(this,&FrdInstDataAssetFilenameOptionsCustomization::SetFileName)
			.MinDesiredWidth(368)
#if ENGINE_MAJOR_VERSION>4
			.ForegroundColor_Lambda([this](){ return ((CurrentOptions->outputExists&&!CurrentOptions->overwrite))?FLinearColor(0.4f,0.0f,0.0f):FLinearColor(0.0f,0.4f,0.0f); })
#else
			.ForegroundColor_Lambda([this](){ return ((CurrentOptions->outputExists&&!CurrentOptions->overwrite))?FLinearColor(0.4f,0.0f,0.0f):FLinearColor(0.0f,0.25f,0.0f); })
#endif
		];

	// Overwrite
	rdCheckbox(box1,CurrentOptions->overwrite,"rdInst_Create_overwrite","Overwrite","rdInst_Create_OverwriteToolTip","Overwrite: Replace the existing BakedDataAsset with this folder and name.");

	// Folder
	rdFullRow(bldr1,lrow2,box2,"Folder");
	rdLabel(box2,50,"rdInstBaseAssetFolderLabel","Folder");
	box2->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(rdPopUpFolderPicker)
			.OnFolderSelected(this,&FrdInstDataAssetFilenameOptionsCustomization::OnFolderSelected)
			.CurrentFolder(*CurrentOptions->dataAssetFolder)
			.FolderBoxWidth(490)
			.PurePath(true)
			.ShowBtns(false)
		];

	// .......................................................................
}

rdInstDataAssetFilenameOptions::rdInstDataAssetFilenameOptions() : instClass(nullptr),CurrentOptions(nullptr),bUserCancelled(true) {}

void rdInstDataAssetFilenameOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=true;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(),FOnGetDetailCustomizationInstance::CreateLambda([this]() { 
		TSharedRef<IDetailCustomization> tinst=FrdInstDataAssetFilenameOptionsCustomization::MakeInstance(); inst=(FrdInstDataAssetFilenameOptionsCustomization*)&(tinst.Get()); return tinst;}));
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2).MaxHeight(700.0f) [ DetailsView->AsShared() ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
			+ SUniformGridPanel::Slot(0,0) [
				rdWinButton("rdInst_CreateSA_SetDef","Set as Default","rdInst_CreateSA_btnToolTip1","Set the current settings as the default",&rdInstDataAssetFilenameOptions::SetDefault)
			]
			+ SUniformGridPanel::Slot(1,0) [
				rdWinButtonEn("rdInst_CreateSA_Create","Create","rdInst_CreateSA_btnToolTip2","Creates the SpawnActor from the Selected Actors",&rdInstDataAssetFilenameOptions::OnConfirm,((!CurrentOptions->outputExists||CurrentOptions->overwrite) && !CurrentOptions->dataAssetFolder.IsEmpty() && !CurrentOptions->dataAssetFilename.IsEmpty()))
			]
			+ SUniformGridPanel::Slot(2,0) [
				rdWinButton("rdInst_CreateSA_Cancel","Cancel","rdInst_CreateSA_btnToolTip3","Closes without making any changes",&rdInstDataAssetFilenameOptions::OnCancel)
			]
		]
	];
}

//.............................................................................
// ShowDataAssetFilename
//.............................................................................
FString FrdInstEditorModule::ShowDataAssetFilename() {

	FString str;

	// Create the settings window...
	TSharedRef<SWindow> rdWin=SNew(SWindow)
										.Title(FText::FromString(TEXT("Baked DataAsset")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
#if ENGINE_MAJOR_VERSION<5
										.ClientSize(FVector2D(500,120));
#else
										.ClientSize(FVector2D(500,120));
#endif

	FillFromOptionDefaults(TEXT("DataAssetFilename"),rdInstOptions);
	if(rdInstOptions->dataAssetFolder.IsEmpty()) {
		UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
		bool beingEdited=false;
		if(rdInstSubsystem) {
			ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
			if(rdBase) {
				rdInstOptions->dataAssetFolder=rdBase->baseBakedAssetFolder;
			}
		}
		if(rdInstOptions->dataAssetFolder.IsEmpty()) {
			rdInstOptions->dataAssetFolder=TEXT("/Game/");
		}
	}

	rdInstOptions=DuplicateObject(GetMutableDefault<UrdInstOptions>(),GetTransientPackage());
	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstDataAssetFilenameOptions> Options;

	rdWin->SetContent(SAssignNew(Options,rdInstDataAssetFilenameOptions)
						.WidgetWindow(rdWin)
						.SettingsObjects(OptionObjects)
					  );
	Options->CurrentOptions=rdInstOptions;
	Options->instClass=this;

	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return str;
	}

	// Show Settings Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(rdWin,MainFrame.GetParentWindow(),false);

	if(!rdInstOptions->dataAssetFolder.IsEmpty() && ! rdInstOptions->dataAssetFilename.IsEmpty()) {
		str=rdInstOptions->dataAssetFolder;
		if(!str.EndsWith("/")) str+=TEXT("/");
		str+=rdInstOptions->dataAssetFilename;
	}

	return str;
}

//.............................................................................

#undef LOCTEXT_NAMESPACE
