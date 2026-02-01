//
// rdInst_UIInstanceSettings
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 18th August 2024
// Last Modified: 12th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "rdInstances.h"
#include "rdInstOptions.h"
#include "rdInstSubsystem.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SVectorInputBox.h"
#include "Widgets/Input/SComboBox.h"
#include "rdUMGHelpers.h"
#include "Interfaces/IMainFrameModule.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Interfaces/IMainFrameModule.h"
#include "Runtime/Slate/Public/Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

#define rdBitbox(box,var,dvr,bitVal,l1,l2,t1,t2) box->AddSlot().Padding(margin).AutoWidth()[SNew(SCheckBox) \
			.IsChecked_Lambda([this](){ if(!(dvr&bitVal)) return ECheckBoxState::Undetermined; return (var&bitVal)?ECheckBoxState::Checked:ECheckBoxState::Unchecked; }) \
			.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState){ dvr|=bitVal; if(NewState==ECheckBoxState::Checked) var|=bitVal; else var&=~bitVal; }) \
			.ToolTipText(LOCTEXT(t1,t2))]; \
			box->AddSlot().Padding(labMargin).AutoWidth()[SNew(STextBlock).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(t1,t2)).MinDesiredWidth(labWidth)]

#define rdBitboxEn(box,var,dvr,bitVal,l1,l2,t1,t2,en) box->AddSlot().Padding(margin).AutoWidth()[SNew(SCheckBox) \
			.IsChecked_Lambda([this](){ if(!(dvr&bitVal)) return ECheckBoxState::Undetermined; return ((var&bitVal)||!en)?ECheckBoxState::Checked:ECheckBoxState::Unchecked; }) \
			.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState){ dvr|=bitVal; if(NewState==ECheckBoxState::Checked) var|=bitVal; else var&=~bitVal; }) \
			.IsEnabled_Lambda([this](){return en;}) \
			.ToolTipText(LOCTEXT(t1,t2))]; \
			box->AddSlot().Padding(labMargin).AutoWidth()[SNew(STextBlock).Text(LOCTEXT(l1,l2)).ToolTipText(LOCTEXT(t1,t2)).MinDesiredWidth(labWidth)]

class FrdInstInstanceSettingsOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstInstanceSettingsOptionsCustomization();
	TSharedRef<SWidget>			MakeComboWidget(TSharedPtr<FString> InItem);
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	int32						getComboCollision(const FString& text) { 
									if(text==TEXT("No Collision")) return 0; 
									if(text==TEXT("Query Only")) return 1; 
									if(text==TEXT("Physics Only")) return 2; 
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
									if(text==TEXT("Probe Only")) return 4; 
									if(text==TEXT("Query and Probe")) return 5; 
#endif
									return 3; 
								}
	FText						getComboCollisionString(int32 col) { 
									if(col==0) return FText::FromString(TEXT("No Collision"));
									if(col==1) return FText::FromString(TEXT("Query Only"));
									if(col==2) return FText::FromString(TEXT("Physics Only")); 
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
									if(col==4) return FText::FromString(TEXT("Probe Only")); 
									if(col==5) return FText::FromString(TEXT("Query and Probe")); 
#endif
									return FText::FromString(TEXT("Collision Enabled")); 
								}
	int32						getComboMobility(const FString& text) { 
									if(text==TEXT("Static")) return 0; 
									if(text==TEXT("Stationary")) return 1; 
									return 2; 
								}
	FText						getComboMobilityString(int32 mob) { 
									if(mob==0) return FText::FromString(TEXT("Static"));
									if(mob==1) return FText::FromString(TEXT("Stationary"));
									return FText::FromString(TEXT("Movable")); 
								}
	int32						getComboShadowInvalidation(const FString& text) { 
									if(text==TEXT("Always")) return 1; 
									if(text==TEXT("Rigid")) return 2; 
									if(text==TEXT("Static")) return 3; 
									return 0; 
								}
	FText						getComboShadowInvalidationString(int32 iv) { 
									if(iv==1) return FText::FromString(TEXT("Always"));
									if(iv==2) return FText::FromString(TEXT("Rigid"));
									if(iv==3) return FText::FromString(TEXT("Static"));
									return FText::FromString(TEXT("Auto")); 
								}
	FText						getProfile() const;
	void						setProfile(const FText& txt,ETextCommit::Type InTextCommit) const;

	TSharedPtr<STextBlock>		collision_ComboBoxLabel;
	TSharedPtr<STextBlock>		mobility_ComboBoxLabel;
	TSharedPtr<STextBlock>		invalidation_ComboBoxLabel;
protected:
	UrdInstOptions*				CurrentOptions;

	TArray<TSharedPtr<FString>> collision_ComboList;
	TArray<TSharedPtr<FString>> mobility_ComboList;
	TArray<TSharedPtr<FString>> invalidation_ComboList;
};

class rdInstInstanceSettingsOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstInstanceSettingsOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstInstanceSettingsOptions() : rdInstModule(nullptr),CurrentOptions(nullptr),inst(nullptr),bUserCancelled(true) {};
	void			Construct(const FArguments& InArgs);

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
	FReply			LoadDefault() {
						if(rdInstModule) {
							rdInstModule->FillFromOptionDefaults(TEXT("InstanceSettings"),CurrentOptions);
							if(inst) {
								inst->mobility_ComboBoxLabel.Get()->SetText((!CurrentOptions->defMobility)?FText::FromString(TEXT("Multi")):inst->getComboMobilityString(CurrentOptions->mobility));
								inst->collision_ComboBoxLabel.Get()->SetText((!CurrentOptions->defCollision)?FText::FromString(TEXT("Multi")):inst->getComboCollisionString(CurrentOptions->collision));
							}
						}
						return FReply::Handled();
					}
	FReply			SetDefault() {
						if(rdInstModule) {
							rdInstModule->SetOptionDefaults(TEXT("InstanceSettings"),CurrentOptions);
						}
						return FReply::Handled();
					}
	bool			WasUserCancelled() { return bUserCancelled; }

	FrdInstEditorModule*			rdInstModule;
	UrdInstOptions*					CurrentOptions;
	FrdInstInstanceSettingsOptionsCustomization* inst;
private:
	TWeakPtr<SWindow>				WidgetWindow;
	bool							bUserCancelled;
	TSharedPtr<class IDetailsView>	DetailsView;
	TSharedPtr<SButton>				ConfirmButton;
};

TSharedRef<IDetailCustomization> FrdInstInstanceSettingsOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstInstanceSettingsOptionsCustomization());
}

TSharedRef<SWidget> FrdInstInstanceSettingsOptionsCustomization::MakeComboWidget(TSharedPtr<FString> InItem) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#else
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#endif
}

FText FrdInstInstanceSettingsOptionsCustomization::getProfile() const {

	if(!CurrentOptions->defCollisionProfile) {
		return FText::FromString(TEXT("Multi"));
	}
	return FText::FromString(CurrentOptions->collisionProfile);
}

void FrdInstInstanceSettingsOptionsCustomization::setProfile(const FText& txt,ETextCommit::Type InTextCommit) const {
	CurrentOptions->defCollisionProfile=true;
	CurrentOptions->collisionProfile=txt.ToString();
}

//.............................................................................
// Constructor
//.............................................................................
FrdInstInstanceSettingsOptionsCustomization::FrdInstInstanceSettingsOptionsCustomization() {

	mobility_ComboList.Add(MakeShareable(new FString(TEXT("Static"))));
	mobility_ComboList.Add(MakeShareable(new FString(TEXT("Stationary"))));
	mobility_ComboList.Add(MakeShareable(new FString(TEXT("Movable"))));

	collision_ComboList.Add(MakeShareable(new FString(TEXT("No Collision"))));
	collision_ComboList.Add(MakeShareable(new FString(TEXT("Query Only"))));
	collision_ComboList.Add(MakeShareable(new FString(TEXT("Physics Only"))));
	collision_ComboList.Add(MakeShareable(new FString(TEXT("Collision Enabled"))));
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	collision_ComboList.Add(MakeShareable(new FString(TEXT("Probe Only"))));
	collision_ComboList.Add(MakeShareable(new FString(TEXT("Query and Probe"))));
#endif

	invalidation_ComboList.Add(MakeShareable(new FString(TEXT("Auto"))));
	invalidation_ComboList.Add(MakeShareable(new FString(TEXT("Always"))));
	invalidation_ComboList.Add(MakeShareable(new FString(TEXT("Rigid"))));
	invalidation_ComboList.Add(MakeShareable(new FString(TEXT("Static"))));
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstInstanceSettingsOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdBPtools options instance in currently edited objects
	CurrentOptions=Cast<UrdInstOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdInstOptions>(Object.Get()); }))->Get());
	FMargin labMargin(0.0f,5.0f,2.0f,2.0f),margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=160.0f;
	FLinearColor lcBlack=FLinearColor(0,0,0,0);

	// Culling -----------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr0=DetailBuilder.EditCategory(TEXT("Culling"));
	rdFullRow(bldr0,lrow0,box0,"Culling");

	rdLabel(box0,80,"StartCull","Start Cull");
	rdNumBoxM(box0,"",CurrentOptions->startCull,CurrentOptions->defStartCull,-9999999.0f,9999999.0f,120,lcBlack);

	rdLabel(box0,80,"EndCull","End Cull");
	rdNumBoxM(box0,"",CurrentOptions->endCull,CurrentOptions->defEndCull,-9999999.0f,9999999.0f,120,lcBlack);

	rdLabel(box0,80,"WPO Cull","WPO Cull");
	rdNumBoxM(box0,"",CurrentOptions->WPODisableDistance,CurrentOptions->defWPODisableDistance,0.0f,9999999.0f,120,lcBlack);

	rdFullRow(bldr0,lrow0a,box0a,"Min Draw");

	rdLabel(box0a,80,"MinDraw","Min Draw");
	rdNumBoxM(box0a,"",CurrentOptions->minDrawDistance,CurrentOptions->defMinDrawDistance,0.0f,9999999.0f,120,lcBlack);

	rdLabel(box0a,80,"NaniteDraw","Nanite Prog");
	rdNumBoxM(box0a,"",CurrentOptions->naniteProgDistance,CurrentOptions->defNaniteProgDistance,0.0f,9999999.0f,120,lcBlack);

	// Mesh -----------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Mesh"));
	rdFullRow(bldr1,lrow1,box1,"Mesh");

	rdLabel(box1,80,"MinLOD","MinLOD");
	rdNumBoxMI(box1,"",CurrentOptions->minLOD,CurrentOptions->defMinLOD,0,100,120,lcBlack);
	rdLabel(box1,80,"RandomSeed","RandomSeed");
	rdNumBoxMI(box1,"",CurrentOptions->randomSeed,CurrentOptions->defRandomSeed,0,999999,120,lcBlack);
	
	rdLabel(box1,80,"LightChannels","LightChannels");
	labWidth=16.0f;
	rdBitbox(box1,CurrentOptions->lightChannel,CurrentOptions->defLightChannel,1,"rdInst_InstanceSettings_LightChannel1","0","rdInst_InstanceSettings_LC_ToolTip1","Use Channel 1 for lighting");
	rdBitbox(box1,CurrentOptions->lightChannel,CurrentOptions->defLightChannel,2,"rdInst_InstanceSettings_LightChannel2","1","rdInst_InstanceSettings_LC_ToolTip2","Use Channel 2 for lighting");
	rdBitbox(box1,CurrentOptions->lightChannel,CurrentOptions->defLightChannel,4,"rdInst_InstanceSettings_LightChannel3","2","rdInst_InstanceSettings_LC_ToolTip3","Use Channel 3 for lighting");
	labWidth=160.0f;

	rdFullRow(bldr1,lrow1a,box1a,"");
	rdLabel(box1a,80,"Mobility","Mobility");
	box1a->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&mobility_ComboList)
			.OnGenerateWidget(this,&FrdInstInstanceSettingsOptionsCustomization::MakeComboWidget)
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo){ CurrentOptions->defMobility=true; CurrentOptions->mobility=getComboMobility(*Selection); mobility_ComboBoxLabel.Get()->SetText(FText::FromString(*Selection));
			})
			.ToolTipText(LOCTEXT("rdInst_Options_InstanceSettings_ToolTip_Mob1","Mobility: "))
				.Content()
				[
					SAssignNew(mobility_ComboBoxLabel,STextBlock)
					.Text((!CurrentOptions->defMobility)?FText::FromString(TEXT("Multi")):getComboMobilityString(CurrentOptions->mobility))
#if ENGINE_MAJOR_VERSION>4
					.MinDesiredWidth(82)
#else
					.MinDesiredWidth(94)
#endif
				]
		];

	rdLabel(box1a,80,"Collision","Collision");
	box1a->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&collision_ComboList)
			.OnGenerateWidget(this,&FrdInstInstanceSettingsOptionsCustomization::MakeComboWidget)
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo){ CurrentOptions->defCollision=true; CurrentOptions->collision=getComboCollision(*Selection); collision_ComboBoxLabel.Get()->SetText(FText::FromString(*Selection));
			})
			.ToolTipText(LOCTEXT("rdInst_Options_InstanceSettings_ToolTip_Col1","Collision: "))
				.Content()
				[
					SAssignNew(collision_ComboBoxLabel,STextBlock)
					.Text((!CurrentOptions->defCollision)?FText::FromString(TEXT("Multi")):getComboCollisionString(CurrentOptions->collision))
#if ENGINE_MAJOR_VERSION>4
					.MinDesiredWidth(82)
#else
					.MinDesiredWidth(94)
#endif
				]
		];

	rdLabel(box1a,80,"CustData","CustData");
	rdNumBoxMI(box1a,"",CurrentOptions->numPerInstCustData,CurrentOptions->defNumPerInstCustData,0,64,120,lcBlack);

	rdFullRow(bldr1,lrow1b,box1b,"");

	rdLabel(box1b,100,"CollisionProfile","Collision Profile");
	rdEditBox(box1b,&FrdInstInstanceSettingsOptionsCustomization::getProfile,&FrdInstInstanceSettingsOptionsCustomization::setProfile,150);


	// Shadows-------------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr2=DetailBuilder.EditCategory(TEXT("Shadows"));

	rdFullRow(bldr2,lrow2,box2,"");
	rdBitbox(box2,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastShadow,"rdInst_InstanceSettings_CastShadow","CastShadow","rdBPtools_InstanceSettings_ToolTip1","CastShadow: ");
	rdBitbox(box2,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastStaticShadow,"rdInst_InstanceSettings_CastStaticShadow","CastStaticShadow","rdBPtools_InstanceSettings_ToolTip2","CastStaticShadow: ");
	rdBitbox(box2,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastDynamicShadow,"rdInst_InstanceSettings_CastDynamicShadow","CastDynamicShadow","rdBPtools_InstanceSettings_ToolTip3","CastDynamicShadow: ");

	rdFullRow(bldr2,lrow3,box3,"");
	rdBitbox(box3,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastFarShadow,"rdInst_InstanceSettings_CastFarShadow","CastFarShadow","rdBPtools_InstanceSettings_ToolTip4","CastFarShadow: ");
	rdBitbox(box3,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastInsetShadow,"rdInst_InstanceSettings_CastInsetShadow","CastInsetShadow","rdBPtools_InstanceSettings_ToolTip5","CastInsetShadow: ");
	rdBitbox(box3,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastHiddenShadow,"rdInst_InstanceSettings_CastHiddenShadow","CastHiddenShadow","rdBPtools_InstanceSettings_ToolTip6","CastHiddenShadow: ");

	rdFullRow(bldr2,lrow4,box4,"");
	rdBitbox(box4,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastShadowAsTwoSided,"rdInst_InstanceSettings_CastShadowAsTwoSided","CastShadowAsTwoSided","rdBPtools_InstanceSettings_ToolTip7","CastShadowAsTwoSided: ");
	rdBitbox(box4,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastVolumetrixTranslucentShadow,"rdInst_InstanceSettings_CastVolumetricTranslucentShadow","CastVolumetricTranslucentShadow","rdBPtools_InstanceSettings_ToolTip8","CastVolumetricTranslucentShadow: ");

	rdFullRow(bldr2,lrow4a,box4a,"");
	rdBitbox(box4a,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_SelfShadowOnly,"rdInst_InstanceSettings_SelfShadowOnly","SelfShadowOnly","rdBPtools_InstanceSettings_ToolTip8b","SelfShadowOnly: ");
	rdBitbox(box4a,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CastContactShadow,"rdInst_InstanceSettings_CastContactShadow","CastContactShadow","rdBPtools_InstanceSettings_ToolTip8c","CastContactShadow: ");
	rdBitbox(box4a,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_ReceiveMobileCSMShadow,"rdInst_InstanceSettings_ReceiveMobileCSMShadows","ReceiveMobileCSMShadows","rdBPtools_InstanceSettings_ToolTip8a","ReceiveMobileCSMShadows: ");

	rdFullRow(bldr2,lrow4b,box4b,"");

	rdLabel(box4b,80,"Invalidation","Invalidation");
	box4b->AddSlot()
		.Padding(margin)
		.AutoWidth()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&invalidation_ComboList)
			.OnGenerateWidget(this,&FrdInstInstanceSettingsOptionsCustomization::MakeComboWidget)
			.OnSelectionChanged_Lambda([this](TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo){ CurrentOptions->defShadowInvalidation=true; CurrentOptions->shadowInvalidation=getComboShadowInvalidation(*Selection); invalidation_ComboBoxLabel.Get()->SetText(FText::FromString(*Selection));
			})
			.ToolTipText(LOCTEXT("rdBPTools_Options_InstanceSettings_ToolTip_Inval1","Shadow Invalidation Type: "))
				.Content()
				[
					SAssignNew(invalidation_ComboBoxLabel,STextBlock)
					.Text((!CurrentOptions->defShadowInvalidation)?FText::FromString(TEXT("Multi")):getComboShadowInvalidationString(CurrentOptions->shadowInvalidation))
#if ENGINE_MAJOR_VERSION>4
					.MinDesiredWidth(82)
#else
					.MinDesiredWidth(94)
#endif
				]
		];

	// Flags --------------------------------------------------------------------------------------
	IDetailCategoryBuilder& bldr3=DetailBuilder.EditCategory(TEXT("Flags"));

	rdFullRow(bldr3,lrow5,box5,"");
	rdBitbox(box5,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_ReceivesDecals,"rdInst_InstanceSettings_ReceivesDecals","ReceivesDecals","rdBPtools_InstanceSettings_ToolTip9","ReceivesDecals: ");
	rdBitbox(box5,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_DisableCollision,"rdInst_InstanceSettings_DisableCollision","DisableCollision","rdBPtools_InstanceSettings_ToolTip10","DisableCollision: ");
	rdBitbox(box5,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_CanEverAffectNavigation,"rdInst_InstanceSettings_CanEverAffectNavigation","CanEverAffectNavigation","rdBPtools_InstanceSettings_ToolTip11","CanEverAffectNavigation: ");

	rdFullRow(bldr3,lrow6,box6,"");
	rdBitbox(box6,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_HasPerInstanceHitProxies,"rdInst_InstanceSettings_HasPerInstanceHitProxies","HasPerInstanceHitProxies","rdBPtools_InstanceSettings_ToolTip12","HasPerInstanceHitProxies: ");
	rdBitbox(box6,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_RayTracingFarField,"rdInst_InstanceSettings_RayTracingFarField","RayTracingFarField","rdBPtools_InstanceSettings_ToolTip13","RayTracingFarField: ");
	rdBitbox(box6,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_AffectDynamicIndirectLighting,"rdInst_InstanceSettings_AffectDynamicIndirectLighting","AffectDynamicIndirectLighting","rdBPtools_InstanceSettings_ToolTip14","AffectDynamicIndirectLighting: ");

	rdFullRow(bldr3,lrow7,box7,"");
	rdBitbox(box7,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_AffectDistanceFieldLighting,"rdInst_InstanceSettings_AffectDistanceFieldLighting","AffectDistanceFieldLighting","rdBPtools_InstanceSettings_ToolTip15","AffectDistanceFieldLighting: ");
	rdBitbox(box7,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_Selectable,"rdInst_InstanceSettings_Selectable","Selectable","rdBPtools_InstanceSettings_ToolTip16","Selectable: ");
	rdBitbox(box7,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_WPOWritesVelocity,"rdInst_InstanceSettings_WPOWritesVelocity","WPO Writes Velocity","rdBPtools_InstanceSettings_ToolTip17","WPOWritesVelocity: ");

	rdFullRow(bldr3,lrow8,box8,"");
	rdBitbox(box8,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_RenderInMainPass,"rdInst_InstanceSettings_RenderInMainPass","RenderInMainPass","rdBPtools_InstanceSettings_ToolTip18","RenderInMainPass: ");
	rdBitbox(box8,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_RenderInDepthPass,"rdInst_InstanceSettings_RenderInDepthPass","RenderInDepthPass","rdBPtools_InstanceSettings_ToolTip19","RenderInDepthPass: ");
	rdBitbox(box8,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_EvaluateWPO,"rdInst_InstanceSettings_EvaluateWPO","EvaluateWPO","rdBPtools_InstanceSettings_ToolTip20","EvaluateWPO: ");

	rdFullRow(bldr3,lrow9,box9,"");
	rdBitbox(box9,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_FillUnderForNav,"rdInst_InstanceSettings_FillUnderForNav","FillUnderForNav","rdBPtools_InstanceSettings_ToolTip21","FillUnderForNav: Blocks Navigation for anything under");
	rdBitbox(box9,CurrentOptions->instFlags,CurrentOptions->defInstFlags,rdInst_FillNavConvex,"rdInst_InstanceSettings_FillNavConvex","FillNavConvex","rdBPtools_InstanceSettings_ToolTip22","FillNavConvex: Use 3D structure for multiple height navigation");
}

//.............................................................................
// Construct
//.............................................................................
void rdInstInstanceSettingsOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=true;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(),FOnGetDetailCustomizationInstance::CreateLambda([this]() { TSharedRef<IDetailCustomization> tinst=FrdInstInstanceSettingsOptionsCustomization::MakeInstance(); inst=(FrdInstInstanceSettingsOptionsCustomization*)&(tinst.Get()); return tinst; }));
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2) [	DetailsView->AsShared()	]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
			+ SUniformGridPanel::Slot(0,0) [
				rdWinButton("rdBPtools_InstanceSettings_LoadDef","Load Default","rdBPtools_InstanceSettings_ToolTip0","Loads the default settings",&rdInstInstanceSettingsOptions::LoadDefault)
			]
			+ SUniformGridPanel::Slot(1,0) [
				rdWinButton("rdBPtools_InstanceSettings_SetDef","Set as Default","rdBPtools_InstanceSettings_ToolTip1","Set the current settings as the default",&rdInstInstanceSettingsOptions::SetDefault)
			]
			+ SUniformGridPanel::Slot(2,0) [
				rdWinButton("rdBPTools_InstanceSettings_Set","Set","rdBPTools_InstanceSettings_ToolTip2","Sets the Instance settings in the StaticMesh",&rdInstInstanceSettingsOptions::OnConfirm)
			]
			+ SUniformGridPanel::Slot(3,0) [
				rdWinButton("rdBPTools_InstanceSettings_Cancel","Cancel","rdBPTools_InstanceSettings_ToolTip3","Closes without making any changes",&rdInstInstanceSettingsOptions::OnCancel)
			]
		]
	];
}

//.............................................................................
// ShowStaticMeshInstanceSettings
//.............................................................................
bool FrdInstEditorModule::ShowStaticMeshInstanceSettings(bool bulk) {

	// Create the settings window...
	TSharedRef<SWindow> rdWin=SNew(SWindow)
										.Title(FText::FromString(TEXT("rdInst Instance Settings")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
#if ENGINE_MAJOR_VERSION<5
										.ClientSize(FVector2D(660,560));
#else
										.ClientSize(FVector2D(700,540));
#endif

	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstInstanceSettingsOptions> Options;

	rdWin->SetContent(SAssignNew(Options,rdInstInstanceSettingsOptions)
						.WidgetWindow(rdWin)
						.SettingsObjects(OptionObjects)
					  );

	Options->CurrentOptions=rdInstOptions;
	Options->rdInstModule=this;

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
