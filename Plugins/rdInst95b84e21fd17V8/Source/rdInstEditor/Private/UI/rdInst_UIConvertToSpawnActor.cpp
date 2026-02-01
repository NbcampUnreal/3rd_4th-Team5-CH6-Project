//
// rdInst_UIConvertToSpawnActor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 3rd March 2024
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
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "rdPopUpFolderPicker.h"
#include "rdInstSubsystem.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

bool fileExists(const FString& sname);
bool fileDelete(const FString& spath,const FString& file);

//.............................................................................
// fileExists
//.............................................................................
bool fileExists(const FString& spath,const FString& file) {

	FString path=spath;
	if(path.StartsWith(TEXT("/All/"))) path.RemoveAt(0,4);
	else if(path.StartsWith(TEXT("/Game/"))) path.RemoveAt(0,6);
	else if(path.StartsWith(TEXT("Game/"))) path.RemoveAt(0,5);
	FString realfile=FPaths::ProjectContentDir()+path+file+TEXT(".uasset");
	bool exists=FPlatformFileManager::Get().GetPlatformFile().FileExists(*realfile);

	if(!exists) { // check to see if it resides only in memory
		UPackage* package=FindPackage(NULL,*(spath/file));
		if(package) {
			exists=true;
		}
	}

	return exists;
}

//.............................................................................
// fileDelete
//.............................................................................
bool fileDelete(const FString& spath,const FString& file) {
/*
	FString path=spath;
	if(path.StartsWith(TEXT("/All/"))) path.RemoveAt(0,4);
	if(path.StartsWith(TEXT("/Game/"))) path.RemoveAt(0,6);
	FString fullName=(FPaths::ProjectContentDir()+path+file+TEXT(".uasset"));

	{
		UPackage* package=FindPackage(NULL,*(spath/file));
		if(package) {
			package->SetDirtyFlag(false);
			TArray<UPackage*> list={package};
			FText errmsg;
			bool ret=UPackageTools::UnloadPackages(list,errmsg);

			package=FindPackage(NULL,*(spath/file));
			if(!ret || package) {
				UE_LOG(LogTemp,Display,TEXT("Failed to Unload Package \"%s\" - %s"),*(spath/file),*errmsg.ToString());
				return false;
			}
		}
	}

	IPlatformFile& pf=FPlatformFileManager::Get().GetPlatformFile();
	if(pf.FileExists(*fullName)) {
		bool b=pf.DeleteFile(*fullName);
		if(!b) {
			UE_LOG(LogTemp,Display,TEXT("Failed to Delete file \"%s\""),*fullName);
			return false;
		}
	}
*/
	return true;
}

class FrdInstConvertToSpawnActorOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstConvertToSpawnActorOptionsCustomization();
	TSharedRef<SWidget>			MakeComboWidget(TSharedPtr<FString> InItem);
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	UrdInstOptions*				CurrentOptions;

	void						OnSpawnModeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo);

	TSharedPtr<STextBlock>			spawnModeLabel;
	TArray<TSharedPtr<FString>>		spawnMode_ComboList;

	FText			GetFileName() const {
						return FText::FromString(CurrentOptions->dataAssetFilename);
					}
	void			SetFileName(const FText& txt,ETextCommit::Type InTextCommit) const {
						CurrentOptions->dataAssetFilename=txt.ToString();
						CurrentOptions->outputExists=fileExists(CurrentOptions->dataAssetFolder,CurrentOptions->dataAssetFilename);
					}
	void			OnFolderSelected(const FString& FolderPath) {
						CurrentOptions->dataAssetFolder=FolderPath;
						if(FolderPath.Len()>1 && FolderPath[FolderPath.Len()-1]!=L'/') CurrentOptions->dataAssetFolder+=TEXT("/");
						CurrentOptions->outputExists=fileExists(CurrentOptions->dataAssetFolder,CurrentOptions->dataAssetFilename);
					}

protected:
};

class rdInstConvertToSpawnActorOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstConvertToSpawnActorOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstConvertToSpawnActorOptions();
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
							instClass->SetOptionDefaults(TEXT("ConvertToSpawnActor"),CurrentOptions);
						}
						return FReply::Handled();
					}
	bool			WasUserCancelled() { return bUserCancelled; }

	int32			getComboSpawnMode(const FString& text) { 
						if(text==TEXT("None")) return 0; 
						if(text==TEXT("Spherical Difference Population")) return 2; 
						if(text==TEXT("Frustum Difference Population")) return 3; 
						return 1; // All
					}
	FText			getComboSpawnModeString(int32 sm) { 
						if(sm==0) return FText::FromString(TEXT("None"));
						if(sm==2) return FText::FromString(TEXT("Spherical Difference Population"));
						if(sm==3) return FText::FromString(TEXT("Frustum Difference Population"));
						return FText::FromString(TEXT("All")); 
					}

	FrdInstEditorModule*			instClass;
	UrdInstOptions*					CurrentOptions;
	FrdInstConvertToSpawnActorOptionsCustomization* inst;

private:
	TWeakPtr<SWindow>				WidgetWindow;
	bool							bUserCancelled;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdInstConvertToSpawnActorOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstConvertToSpawnActorOptionsCustomization());
}

TSharedRef<SWidget> FrdInstConvertToSpawnActorOptionsCustomization::MakeComboWidget(TSharedPtr<FString> InItem) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#else
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#endif
}

//.............................................................................
// Constructor
//.............................................................................
FrdInstConvertToSpawnActorOptionsCustomization::FrdInstConvertToSpawnActorOptionsCustomization() {

	spawnMode_ComboList.Add(MakeShareable(new FString(TEXT("None"))));
	spawnMode_ComboList.Add(MakeShareable(new FString(TEXT("All"))));
	spawnMode_ComboList.Add(MakeShareable(new FString(TEXT("Spherical Difference Population"))));
	spawnMode_ComboList.Add(MakeShareable(new FString(TEXT("Frustum Difference Population"))));
}

//.............................................................................
// OnSpawnModeChanged
//.............................................................................
void FrdInstConvertToSpawnActorOptionsCustomization::OnSpawnModeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo) {

	spawnModeLabel.Get()->SetText(FText::FromString(*Selection));

	if(*Selection==TEXT("None")) {
		CurrentOptions->spawnMode=0;
	} else if(*Selection==TEXT("All")) {
		CurrentOptions->spawnMode=1;
	} else if(*Selection==TEXT("Spherical Difference Population")) {
		CurrentOptions->spawnMode=2;
	} else if(*Selection==TEXT("Frustum Difference Population")) {
		CurrentOptions->spawnMode=3;
	}
}


//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstConvertToSpawnActorOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdInst options instance in currently edited objects
	CurrentOptions=Cast<UrdInstOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdInstOptions>(Object.Get()); }))->Get());
	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=220.0f;
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);

	// Create Settings ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Create Settings"));

	// Harvest Instances
	rdFullRow(bldr1,lrow1a,box1a,"");
	rdCheckbox(box1a,CurrentOptions->harvestInstances,"rdInst_CreateSA_HarvestInstances","Harvest Instances","rdInst_CreateSA_ToolTip5","When ticked, instances contained in actors are converted to rdInstances");

	// Rows and Columns
	rdFullRow(bldr1,lrow1b,box1b,"");
	rdLabel(box1b,90,"NumRows","Rows:");
	rdSpinBoxI(box1b,CurrentOptions->numSpawnActorRows,1,9999,60,"SetNumRows_TT1","The number of rows to split the selection into. Each row/column cell is a SpawnActor");
	rdLabel(box1b,90,"NumCols","Columns:");
	rdSpinBoxI(box1b,CurrentOptions->numSpawnActorCols,1,9999,60,"SetNumCols_TT1","The number of columns to split the selection into. Each row/column cell is a SpawnActor");


	// GridArray Settings ------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr2=DetailBuilder.EditCategory(TEXT("GridArray Settings"));

	// Grid Cell Numbers
	rdFullRow(bldr2,lrow2a,box2a,"");
	rdLabel(box2a,90,"NumRows","Grid Rows:");
	rdSpinBoxI(box2a,CurrentOptions->spawnActorGridX,1,256,60,"SetNumGridRows_TT1","The number of rows to split the GridArray into. Each row/column cell is a cluster of entities collated by location");
	rdLabel(box2a,90,"NumCols","Grid Columns:");
	rdSpinBoxI(box2a,CurrentOptions->spawnActorGridY,1,256,60,"SetNumGridCols_TT1","The number of columns to split the GridAwway into. Each row/column cell is a cluster of entities collated by location");


	// Filename Settings ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr3=DetailBuilder.EditCategory(TEXT("Filename"));

	rdFullRow(bldr3,lrow3a,box3a,"Filename");
	rdLabel(box3a,90,"rdInst_CreateSA_NameLabel","Name");
	box3a->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(SEditableTextBox)
			.Text(this,&FrdInstConvertToSpawnActorOptionsCustomization::GetFileName)
			.OnTextCommitted(this,&FrdInstConvertToSpawnActorOptionsCustomization::SetFileName)
			.MinDesiredWidth(300)
#if ENGINE_MAJOR_VERSION>4
			.ForegroundColor_Lambda([this](){ return ((CurrentOptions->outputExists&&!CurrentOptions->overwrite))?FLinearColor(0.4f,0.0f,0.0f):FLinearColor(0.0f,0.4f,0.0f); })
#else
			.ForegroundColor_Lambda([this](){ return ((CurrentOptions->outputExists&&!CurrentOptions->overwrite))?FLinearColor(0.4f,0.0f,0.0f):FLinearColor(0.0f,0.25f,0.0f); })
#endif
		];

	// Overwrite
	rdCheckbox(box3a,CurrentOptions->overwrite,"rdInst_Create_overwrite","Overwrite","rdInst_Create_OverwriteToolTip","Overwrite: Replace the existing BakedDataAsset with this folder and name.");

	rdFullRow(bldr3,lrow3b,box3b,"Folder");
	rdLabel(box3b,90,"rdInstBaseAssetFolderLabel","Folder");
	box3b->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(rdPopUpFolderPicker)
			.OnFolderSelected(this,&FrdInstConvertToSpawnActorOptionsCustomization::OnFolderSelected)
			.CurrentFolder(*CurrentOptions->dataAssetFolder)
			.FolderBoxWidth(350)
			.PurePath(true)
			.PrefixGame(true)
			.ShowBtns(false)
		];

	// Spawn Settings ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr4=DetailBuilder.EditCategory(TEXT("Spawn Settings"));

	rdFullRow(bldr4,lrow4a,box4a,"");
	rdCheckbox(box4a,CurrentOptions->bSpawnOnStaticMeshes,"rdInst_CreateSA_SpawnSMs","Spawn on StaticMeshes","rdInst_CreateSA_ToolTip10","Spawn on StaticMeshes: When Unticked, only the Landscape is used for LineTraces.");

	// LocalSpawn
	rdFullRow(bldr4,lrow4b,box4b,"");
	rdLabel(box4b,120,"Spawn Mode","Spawn Mode");
	rdInt32Combo(box4b,"All",&spawnMode_ComboList,&FrdInstConvertToSpawnActorOptionsCustomization::OnSpawnModeChanged,&FrdInstConvertToSpawnActorOptionsCustomization::MakeComboWidget,spawnModeLabel,"rdInst_SpawnMode","Select the type of Spawn Behavior",260);

	// 
	rdFullRow(bldr4,lrow4c,box4c,"");
	rdLabel(box4c,120,"SDPRadius","Spherical Radius");
	rdNumBox(box4c,"",CurrentOptions->localRadius,0.0f,9999999.0f,100,lcGreen);
	
	rdFullRow(bldr4,lrow4d,box4d,"");
	rdLabel(box4d,120,"FDPDistance","Frustum Distance");
	rdNumBox(box4d,"",CurrentOptions->viewFrustumDistance,0.0f,9999999.0f,100,lcGreen);
	rdLabel(box4d,120,"FDPExtend","Frustum Extend");
	rdNumBox(box4d,"",CurrentOptions->viewFrustumExtend,0.0f,90.0f,100,lcGreen);

	// Distribution Frames
	rdFullRow(bldr4,lrow4e,box4e,"");
	rdLabel(box4e,120,"Distribution Frames","Distribute");
	rdNumBoxI(box4e,"",CurrentOptions->distFrames,0.0f,120.0f,60,lcGreen);
	rdLabel(box4e,220,"Distribution Frames2","(Frames to distribute spawn over)");

	// Stream Settings ------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr5=DetailBuilder.EditCategory(TEXT("Streaming Settings"));

	rdFullRow(bldr5,lrow5a,box5a,"Streaming Settings");

	// StreamIn Distance (0=always)
	rdLabel(box5a,120,"StreamIn Distance","Stream In Distance");
	rdNumBox(box5a,"",CurrentOptions->spawnDistance,0.0f,9999999.0f,100,lcGreen);
	rdLabel(box5a,150,"StreamIn Distance2","(0.0=Load Immediately)");

	// Stream Data on Demand
	rdFullRow(bldr5,lrow5b,box5b,"");
	rdCheckbox(box5b,CurrentOptions->loadOnDemandBakedData,"rdInst_CreateSA_LoadOnDemandBakedData","Load on Demand","rdInst_CreateSA_ToolTip4","When ticked, the baked data transforms and assets are not loaded into memory until within the proximity distance");
	rdCheckbox(box5b,CurrentOptions->freeOnHideBakedData,"rdInst_CreateSA_FreeOnHideBakedData","Free on Hide","rdInst_CreateSA_ToolTip5","When ticked, the baked data transforms and assets are freed from memory when their SpawnActor is hidden");


	// Calc Spawn Distance usage by object size
	rdFullRow(bldr5,lrow5c,box5c,"");
	rdCheckbox(box5c,CurrentOptions->calcSpawnDistanceBySize,"rdInst_CreateSA_CalcBySize","Calc By Size","rdInst_CreateSA_ToolTip6","When ticked, Entity Spawn Distance index (from above) is calculated from the object size");
	rdCheckbox(box5c,CurrentOptions->calcSpawnDistanceDeepScan,"rdInst_CreateSA_DeepCalc","Deep Calc","rdInst_CreateSA_ToolTip7","When ticked, the object size is calculated from the max Scale of each instance (otherwise the objects default scale is used, faster)");

	// Spatially loaded and Relative Spawning
	rdFullRow(bldr5,lrow5d,box5d,"");
	rdCheckbox(box5d,CurrentOptions->spatial,"rdInst_CreateSA_Spatial","Spatially Loaded (for WP)","rdInst_CreateSA_ToolTip2","Spatially Loaded: The SpawnActor is streamed in with WP tiles.");
	rdCheckbox(box5d,CurrentOptions->bRelativeSpawning,"rdInst_CreateSA_Relative","Relative Spawning","rdInst_CreateSA_ToolTip8","Relative Spawning: The Entities are spawned relative to the SpawnActor.");

	// .......................................................................
	// Misc

	IDetailCategoryBuilder& bldr6=DetailBuilder.EditCategory(TEXT("Misc"));

	rdFullRow(bldr6,lrow6a,box6a,"");
	rdCheckbox(box6a,CurrentOptions->replace,"rdInst_CreateSA_Replace","Replace Selected Actors with SpawnActor","rdInst_CreateSA_ToolTip3","Replace: Replaces the selected actors with the newly made SpawnActor.");

	// .......................................................................
}

rdInstConvertToSpawnActorOptions::rdInstConvertToSpawnActorOptions() : instClass(nullptr),CurrentOptions(nullptr),bUserCancelled(true) {}

void rdInstConvertToSpawnActorOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=true;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(),FOnGetDetailCustomizationInstance::CreateLambda([this]() { 
		TSharedRef<IDetailCustomization> tinst=FrdInstConvertToSpawnActorOptionsCustomization::MakeInstance(); inst=(FrdInstConvertToSpawnActorOptionsCustomization*)&(tinst.Get()); return tinst;}));
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2).MaxHeight(700.0f) [ DetailsView->AsShared() ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
			+ SUniformGridPanel::Slot(0,0) [
				rdWinButton("rdInst_CreateSA_SetDef","Set as Default","rdInst_CreateSA_btnToolTip1","Set the current settings as the default",&rdInstConvertToSpawnActorOptions::SetDefault)
			]
			+ SUniformGridPanel::Slot(1,0) [
				rdWinButtonEn("rdInst_CreateSA_Create","Create","rdInst_CreateSA_btnToolTip2","Creates the SpawnActor from the Selected Actors",&rdInstConvertToSpawnActorOptions::OnConfirm,((!CurrentOptions->outputExists||CurrentOptions->overwrite) && !CurrentOptions->dataAssetFolder.IsEmpty() && !CurrentOptions->dataAssetFilename.IsEmpty()))
			]
			+ SUniformGridPanel::Slot(2,0) [
				rdWinButton("rdInst_CreateSA_Cancel","Cancel","rdInst_CreateSA_btnToolTip3","Closes without making any changes",&rdInstConvertToSpawnActorOptions::OnCancel)
			]
		]
	];
}

//.............................................................................
// ShowConvertToSpawnActorSettings
//.............................................................................
bool FrdInstEditorModule::ShowConvertToSpawnActorSettings() {

	// Create the settings window...
	TSharedRef<SWindow> rdWin=SNew(SWindow)
										.Title(FText::FromString(TEXT("Create rdSpawnActor")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
#if ENGINE_MAJOR_VERSION<5
										.ClientSize(FVector2D(560,620));
#else
										.ClientSize(FVector2D(560,620));
#endif

	FillFromOptionDefaults(TEXT("ConvertToSpawnActor"),rdInstOptions);
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

	if(rdInstOptions->dataAssetFilename.IsEmpty()) {
		int32 cnt=1;
		FString dan=TEXT("BakedDataAsset_1");
		while(fileExists(rdInstOptions->dataAssetFolder,dan)) {
			dan=FString::Printf(TEXT("BakedDataAsset_%d"),cnt++);
		}
		rdInstOptions->dataAssetFilename=dan;
	}
	rdInstOptions->outputExists=fileExists(rdInstOptions->dataAssetFolder,rdInstOptions->dataAssetFilename);

	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstConvertToSpawnActorOptions> Options;

	rdWin->SetContent(SAssignNew(Options,rdInstConvertToSpawnActorOptions)
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
