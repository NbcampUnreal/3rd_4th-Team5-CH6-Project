//
// rdInst_UIRandomSettings
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.00
//
// Creation Date: 3rd September 2024
// Last Modified: 3rd September 2024
//
// Constructs/Handles the rdInstTools Random Settings Window
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstOptions.h"
#include "Editor/PropertyEditor/Public/IDetailRootObjectCustomization.h"
#include "Editor/PropertyEditor/Public/IDetailCustomization.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Runtime/Slate/Public/Widgets/Input/SSpinBox.h"
#include "Runtime/Slate/Public/Widgets/Layout/SUniformGridPanel.h"
#include "IPropertyTypeCustomization.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include "rdPopUpActorPicker.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"
#include "Engine/Selection.h"
#include "rdUMGHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "rdInst.h"

#define LOCTEXT_NAMESPACE "FrdInstModule"

int32 getLockAxisComboInt(const FString& str) {
	if(str==TEXT("Lock All")) return 2;
	if(str==TEXT("Lock X&Y")) return 1;
	return 0;
}

FText getLockAxisComboString(int32 ret) {
	if(ret==2) return FText::FromString(TEXT("Lock All"));
	if(ret==1) return FText::FromString(TEXT("Lock X&Y"));
	return FText::FromString(TEXT("Free"));
}

class FrdInstRandomOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstRandomOptionsCustomization();
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	TSharedRef<SWidget>			MakeComboWidget(TSharedPtr<FString> InItem);
protected:
	FText						GetThemesText() const;
	void						SetThemesText(const FText& txt,ETextCommit::Type InTextCommit) const;
	void						OnRelyActorSelected(AActor* actor) const;
	AActor*						GetCurrentRelyActor() const;
	bool						GetIsMultipleValues() const;
	UrdInstOptions*				CurrentOptions;
	TArray<TSharedPtr<FString>> lockAxisComboList;
	TSharedPtr<STextBlock>		lockAxisComboBoxLabel;
};

class rdInstRandomOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstRandomOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstRandomOptions() : instClass(nullptr),CurrentOptions(nullptr),bUserCancelled(true) {};
	void			Construct(const FArguments& InArgs);

	virtual bool	SupportsKeyboardFocus() const override { return true; }
	virtual FReply	OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override { return (InKeyEvent.GetKey()==EKeys::Escape)?OnCancel():FReply::Unhandled(); }
	FReply			OnConfirm(){
						bUserCancelled=false;
						if(WidgetWindow.IsValid()) WidgetWindow.Pin()->HideWindow();
						if(instClass) instClass->ChangeRandomSettingsForSelectedActors();
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
							instClass->SetOptionDefaults(TEXT("Random"),CurrentOptions);
						}
						return FReply::Handled();
					}
	FReply			LoadDefault() {
						if(instClass) {
							instClass->FillFromOptionDefaults(TEXT("Random"),CurrentOptions);
						}
						return FReply::Handled();
					}
	bool			WasUserCancelled() { return bUserCancelled; }

	FrdInstEditorModule*			instClass;
	UrdInstOptions*					CurrentOptions;
private:
	TWeakPtr<SWindow>				WidgetWindow;
	bool							bUserCancelled;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdInstRandomOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstRandomOptionsCustomization());
}

//.............................................................................
// Constructor
//.............................................................................
FrdInstRandomOptionsCustomization::FrdInstRandomOptionsCustomization() {
	lockAxisComboList.Add(MakeShareable(new FString(TEXT("Free"))));
	lockAxisComboList.Add(MakeShareable(new FString(TEXT("Lock X&Y"))));
	lockAxisComboList.Add(MakeShareable(new FString(TEXT("Lock All"))));
}

void FrdInstRandomOptionsCustomization::OnRelyActorSelected(AActor* actor) const { 
	CurrentOptions->relyActor=actor;
	CurrentOptions->defRely=true;
	if(actor) {
		CurrentOptions->rndRelyOnActorName=actor->GetName();
	} else {
		CurrentOptions->rndRelyOnActorName=TEXT("");
	}
}

AActor* FrdInstRandomOptionsCustomization::GetCurrentRelyActor() const { 
	return CurrentOptions->relyActor; 
}

FText FrdInstRandomOptionsCustomization::GetThemesText() const {
	if(!CurrentOptions->defThemes) return FText::FromString(TEXT("(Multiple Values)"));
	return FText::FromString(CurrentOptions->themes);
}

void FrdInstRandomOptionsCustomization::SetThemesText(const FText& txt,ETextCommit::Type InTextCommit) const {
	FString str=txt.ToString();
	if(str!=TEXT("(Multiple Values)")) {
		CurrentOptions->defThemes=true;
		CurrentOptions->themes=str;
	}
}

bool FrdInstRandomOptionsCustomization::GetIsMultipleValues() const {
	return !CurrentOptions->defRely;
}

TSharedRef<SWidget> FrdInstRandomOptionsCustomization::MakeComboWidget(TSharedPtr<FString> InItem) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#else
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#endif
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstRandomOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdInst options instance in currently edited objects
	CurrentOptions=Cast<UrdInstOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdInstOptions>(Object.Get()); }))->Get());

	FLinearColor lcRed=FLinearColor(1,0,0,1);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);
	FLinearColor lcBlue=FLinearColor(0,0,1,1);

	// .......................................................................
	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Transform"));

	// Location -----------------------------------------------------------------------------------
	rdFullRow(bldr1,lrow1,box1,"");

	rdCatLabel(box1,"rdInst_Random_LocationLabel","Location","rdInst_Random_FromAndToLoc","From and To Location offset: The minimum and maximum location variance for the actor/s.");
	rdNumBoxM(box1,"X",CurrentOptions->rndLocation1.X,CurrentOptions->defLocX1,-999.0f,999.0f,70,lcRed);
	rdNumBoxM(box1,"Y",CurrentOptions->rndLocation1.Y,CurrentOptions->defLocY1,-999.0f,999.0f,70,lcGreen);
	rdNumBoxM(box1,"Z",CurrentOptions->rndLocation1.Z,CurrentOptions->defLocZ1,-999.0f,999.0f,70,lcBlue);
	rdLabel2(box1,"->");
	rdNumBoxM(box1,"X",CurrentOptions->rndLocation2.X,CurrentOptions->defLocX2,-999.0f,999.0f,70,lcRed);
	rdNumBoxM(box1,"Y",CurrentOptions->rndLocation2.Y,CurrentOptions->defLocY2,-999.0f,999.0f,70,lcGreen);
	rdNumBoxM(box1,"Z",CurrentOptions->rndLocation2.Z,CurrentOptions->defLocZ2,-999.0f,999.0f,70,lcBlue);

	// Rotation -----------------------------------------------------------------------------------
	rdFullRow(bldr1,lrow2,box2,"");

	rdCatLabel(box2,"rdInst_Random_RotationLabel","Rotation","rdInst_Random_FromAndToRot","From and To Rotation: The minimum and maximum Rotation variance for the actor/s (-180.0 -> 180.0).");
	rdNumBoxM(box2,"X",CurrentOptions->rndRotation1.Roll,CurrentOptions->defRotX1,-180.0f,180.0f,70,lcRed);
	rdNumBoxM(box2,"Y",CurrentOptions->rndRotation1.Pitch,CurrentOptions->defRotY1,-180.0f,180.0f,70,lcGreen);
	rdNumBoxM(box2,"Z",CurrentOptions->rndRotation1.Yaw,CurrentOptions->defRotZ1,-180.0f,180.0f,70,lcBlue);
	rdLabel2(box2,"->");
	rdNumBoxM(box2,"X",CurrentOptions->rndRotation2.Roll,CurrentOptions->defRotX2,-180.0f,180.0f,70,lcRed);
	rdNumBoxM(box2,"Y",CurrentOptions->rndRotation2.Pitch,CurrentOptions->defRotY2,-180.0f,180.0f,70,lcGreen);
	rdNumBoxM(box2,"Z",CurrentOptions->rndRotation2.Yaw,CurrentOptions->defRotZ2,-180.0f,180.0f,70,lcBlue);

	// Flip Probability ---------------------------------------------------------------------------
	rdFullRow(bldr1,lrow3,box3,"");

	rdCatLabel(box3,"rdInst_Random_FlipLabel","Flip Probability","rdInst_Random_Flip","Flip Probability: The probability (0.0->1.0) the actor/s is/are Flipped 180 degrees in any direction.");
	rdNumBoxM(box3,"X",CurrentOptions->rndFlipProbability.X,CurrentOptions->defFlipX,0.0f,1.0f,70,lcRed);
	rdNumBoxM(box3,"Y",CurrentOptions->rndFlipProbability.Y,CurrentOptions->defFlipY,0.0f,1.0f,70,lcGreen);
	rdNumBoxM(box3,"Z",CurrentOptions->rndFlipProbability.Z,CurrentOptions->defFlipZ,0.0f,1.0f,70,lcBlue);

	// Scale --------------------------------------------------------------------------------------
	rdFullRow(bldr1,lrow4a,box4a,"");
	rdCatLabel(box4a,"rdInst_Random_LockAxisLabel","Lock Axis","rdInst_RandomizeActor_ToolTip4a1","Lock Scale Axis: Specify the Axis to lock for the Scale (if any).");
	box4a->AddSlot().Padding(2).AutoWidth()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&lockAxisComboList)
			.OnGenerateWidget(this,&FrdInstRandomOptionsCustomization::MakeComboWidget)
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo){ CurrentOptions->defLockedAxis=true; CurrentOptions->defScaleX1=true; CurrentOptions->defScaleY1=true; CurrentOptions->defScaleZ1=true;  CurrentOptions->defScaleX2=true; CurrentOptions->defScaleY2=true; CurrentOptions->defScaleZ2=true; CurrentOptions->lockAxis=getLockAxisComboInt(*Selection); lockAxisComboBoxLabel.Get()->SetText(FText::FromString(*Selection)); })
			.ToolTipText(LOCTEXT("rdInst_RandomizeActor_ToolTip4a","Lock Scale Axis: Specify the Axis to lock for the Scale (if any)."))
			.Content()
			[
				SAssignNew(lockAxisComboBoxLabel,STextBlock)
				.Text((!CurrentOptions->defLockedAxis)?FText::FromString(TEXT("Multi")):getLockAxisComboString(CurrentOptions->lockAxis))
				.MinDesiredWidth(80)
			]
		];

	rdFullRow(bldr1,lrow4,box4,"");

	rdCatLabel(box4,"rdInst_Random_ScaleLabel","Scale","rdInst_Random_FromAndToScale","From and To Scale: The minimum and maximum Scale variance for the actor/s.");
	rdNumBoxM(box4,"X",CurrentOptions->rndScale1.X,CurrentOptions->defScaleX1,0.001,99.0,70,lcRed);
	rdNumBoxMS(box4,"Y",CurrentOptions->rndScale1.Y,CurrentOptions->defScaleY1,(CurrentOptions->lockAxis>0),CurrentOptions->rndScale1.X,0.001,99.0,70,lcGreen);
	rdNumBoxMS(box4,"Z",CurrentOptions->rndScale1.Z,CurrentOptions->defScaleZ1,(CurrentOptions->lockAxis>1),CurrentOptions->rndScale1.X,0.001,99.0,70,lcBlue);
	rdLabel2(box4,"->");
	rdNumBoxM(box4,"X",CurrentOptions->rndScale2.X,CurrentOptions->defScaleX2,0.001,99.0,70,lcRed);
	rdNumBoxMS(box4,"Y",CurrentOptions->rndScale2.Y,CurrentOptions->defScaleY2,(CurrentOptions->lockAxis>0),CurrentOptions->rndScale2.X,0.001,99.0,70,lcGreen);
	rdNumBoxMS(box4,"Z",CurrentOptions->rndScale2.Z,CurrentOptions->defScaleZ2,(CurrentOptions->lockAxis>1),CurrentOptions->rndScale2.X,0.001,99.0,70,lcBlue);

	// Show ---------------------------------------------------------------------------------------
	rdFullRow(bldr1,lrow7,box7,"");

	rdCatLabel(box7,"rdInst_Random_ShowLabel","Show Probability","rdInst_Random_Show","Show Probability: The Probability (0.0->1.) of actor/s being visible. ");
	rdNumBoxM(box7,"",CurrentOptions->rndShowProbability,CurrentOptions->defShow,0.0,1.0,70,lcRed);

	// Themes ---------------------------------------------------------------------------------------
	rdFullRow(bldr1,lrow8,box8,"");

	rdCatLabel(box8,"rdInst_Random_ThemesLabel","Themes","rdInst_Random_ThemesActorText1","Actor/s will only be visible with the selected (comma separated) theme names.");

	box8->AddSlot()
		.Padding(FMargin(2.0f,2.0f,2.0f,2.0f))
		.AutoWidth()
		[
			SNew(SEditableTextBox)
			.Text(this,&FrdInstRandomOptionsCustomization::GetThemesText)
			.OnTextCommitted(this,&FrdInstRandomOptionsCustomization::SetThemesText)
			.MinDesiredWidth(450)
			.ToolTipText(LOCTEXT("rdInst_Random_ThemesActorText","This Actor will only be visible with the selected (comma separated) theme names."))
		];

	// Gap
	FDetailWidgetRow& lrow_gap=bldr1.AddCustomRow(FText::FromString(TEXT("")));

	// Rely ---------------------------------------------------------------------------------------
	rdFullRow(bldr1,lrow5,box5,"");

	rdCatLabel(box5,"rdInst_Random_RelyLabel","Rely","rdInst_Random_RelyActor1","Actor/s will only be visible if the Actor you select here is visible.");

	box5->AddSlot()
		.Padding(FMargin(2.0f,2.0f,2.0f,2.0f))
		.AutoWidth()
		[
			SNew(rdPopUpActorPicker)
			.OnActorSelected(this,&FrdInstRandomOptionsCustomization::OnRelyActorSelected)
			.CurrentActor(this,&FrdInstRandomOptionsCustomization::GetCurrentRelyActor)
			.IsMultipleValues(this,&FrdInstRandomOptionsCustomization::GetIsMultipleValues)
			.NameBoxWidth(410)
			.ToolTipText(LOCTEXT("rdInst_Random_RelyActor","Actor/s will only be visible if the Actor you select here is visible."))
		];
}

//.............................................................................
// Construct
//.............................................................................
void rdInstRandomOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=true;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(),FOnGetDetailCustomizationInstance::CreateLambda([=]() { return FrdInstRandomOptionsCustomization::MakeInstance(); }));
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2).MaxHeight(800.0f)	[ DetailsView->AsShared() ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
			+ SUniformGridPanel::Slot(0,0) [
				rdWinButton("rdBPtools_Random_LoadDef","Load Default","rdBPtools_Random_ToolTip0","Loads the default settings",&rdInstRandomOptions::LoadDefault)
			]
			+ SUniformGridPanel::Slot(1,0) [
				rdWinButton("rdBPtools_Random_SetDef","Set as Default","rdBPtools_Random_ToolTip1","Set the current settings as the default",&rdInstRandomOptions::SetDefault)
			]
			+ SUniformGridPanel::Slot(2,0) [
				rdWinButton("rdBPTools_Random_Set","Set","rdBPTools_Random_ToolTip2","Sets these Randomization settings in the selected Actors",&rdInstRandomOptions::OnConfirm)
			]
			+ SUniformGridPanel::Slot(3,0) [
				rdWinButton("rdBPTools_Random_Cancel","Cancel","rdBPTools_Random_ToolTip3","Closes without making any changes",&rdInstRandomOptions::OnCancel)
			]
		]
	];
}

//.............................................................................
// ShowRandomSettings
//.............................................................................
void FrdInstEditorModule::ShowRandomSettings(bool bulk) {

	TSharedPtr<SWindow> rdWin;

	// Create the settings window...
	SAssignNew(rdWin,SWindow)
							.Title(FText::FromString(TEXT("rdInst Randomization")))
							.SizingRule(ESizingRule::UserSized)
							.AutoCenter(EAutoCenter::PreferredWorkArea)
							.IsTopmostWindow(true)
#if ENGINE_MAJOR_VERSION<5
							.ClientSize(FVector2D(610,320));
#else
							.ClientSize(FVector2D(650,320));
#endif

//	rdInstOptions->instClass=this;
//	rdInstOptions->bulkAction=bulk;
	//rdInstOptions->bp=nullptr;
	//rdInstOptions->bpEditor=nullptr;
	rdInstOptions->relyActor=nullptr;
	if(!rdInstOptions->rndRelyOnActorName.IsEmpty()) {

		FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
		UWorld* World=world->World();
		for(TActorIterator<AActor> actor(World);actor;++actor) {
			if(actor->GetName()==rdInstOptions->rndRelyOnActorName) {
				rdInstOptions->relyActor=*actor;
				break;
			}
		}
	}

	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstRandomOptions> Options;

	rdWin->SetContent(SAssignNew(Options,rdInstRandomOptions)
						.WidgetWindow(rdWin)
						.SettingsObjects(OptionObjects)
					  );

	Options->CurrentOptions=rdInstOptions;
	Options->instClass=this;

	// Show Window
	FSlateApplication::Get().AddWindow(rdWin.ToSharedRef());
}
//.............................................................................

#undef LOCTEXT_NAMESPACE
