//
// rdInst_UIProxySettings.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 30th June 2024
// Last Modified: 16th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "rdInstOptions.h"
#include "rdInstSubsystem.h"
#include "Interfaces/IMainFrameModule.h"
#include "PropertyEditorModule.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#if ENGINE_MAJOR_VERSION>4
#if ENGINE_MINOR_VERSION>2
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#elif ENGINE_MINOR_VERSION>0
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#endif
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"
#endif
#include "rdPopUpActorPicker.h"
#include "rdUMGHelpers.h"
#include "rdPopUpAssetPicker.h"
#include "rdPopUpClassPicker.h"
#include "LevelEditor.h"
#if ENGINE_MAJOR_VERSION>4
#include "LevelInstance/LevelInstanceActor.h"
#endif
#include "rdActor.h"
#include "rdMaterialList.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

class FrdInstProxySettingsOptionsCustomization : public IDetailCustomization {
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
								FrdInstProxySettingsOptionsCustomization();
	TSharedRef<SWidget>			MakeComboWidget(TSharedPtr<FString> InItem);
	virtual void				CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void						ShowRelevantSections();

	IDetailCategoryBuilder*		pBldr1=nullptr;
	IDetailCategoryBuilder*		pBldr2=nullptr;
	IDetailCategoryBuilder*		pBldr3=nullptr;
	IDetailCategoryBuilder*		pBldr4=nullptr;
	IDetailCategoryBuilder*		pBldr5=nullptr;
	IDetailCategoryBuilder*		pBldr6=nullptr;

	FReply						ShowColorPicker1(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent);
	FLinearColor				GetSourceColor1() const;
	void						OnColorPickerPicked1(FLinearColor col);
	FReply						ShowColorPicker2(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent);
	FLinearColor				GetSourceColor2() const;
	void						OnColorPickerPicked2(FLinearColor col);

	void						OnPAClassSelected(const UClass* uclass) const { CurrentOptions->proxyActor=uclass; CurrentOptions->defProxyActor=true; }
	TWeakObjectPtr<UClass>		GetPACurrentClass() const {	
									if(!CurrentOptions->defProxyActor) {
										return UrdMultiToken::StaticClass();
									}
									return CurrentOptions->proxyActor.LoadSynchronous(); 
								}

	void						OnDLAssetSelected(const FAssetData& InAssetData) const { CurrentOptions->ProxyDataLayer=Cast<UDataLayerAsset>(InAssetData.GetAsset()); CurrentOptions->defProxyDataLayerName=true; }
	TWeakObjectPtr<UObject>		GetDLCurrentAsset() const {	
									if(!CurrentOptions->defProxyDataLayerName) {
										return rdNonDeterminantInstance;
									}
									return CurrentOptions->ProxyDataLayer.LoadSynchronous(); 
								}

	void						OnSMAssetSelected(const FAssetData& InAssetData) const { 
									CurrentOptions->ProxyStaticMesh=Cast<UStaticMesh>(InAssetData.GetAsset());
									matList1.Get()->SetMaterialList(CurrentOptions->ProxyStaticMesh.Get());
									CurrentOptions->defProxyStaticMesh=true;
								}
	TWeakObjectPtr<UObject>		GetSMCurrentAsset() const {	
									if(!CurrentOptions->defProxyStaticMesh) {
										return rdNonDeterminantInstance;
									}
									return CurrentOptions->ProxyStaticMesh.LoadSynchronous(); 
								}

	void						OnDMAssetSelected(const FAssetData& InAssetData) const { 
									CurrentOptions->ProxyDestroyedMesh=Cast<UStaticMesh>(InAssetData.GetAsset()); 
									matList2.Get()->SetMaterialList(CurrentOptions->ProxyDestroyedMesh.Get());
									CurrentOptions->defProxyDestroyedMesh=true;
								}
	TWeakObjectPtr<UObject>		GetDMCurrentAsset() const {	
									if(!CurrentOptions->defProxyDestroyedMesh) {
										return rdNonDeterminantInstance;
									}
									return CurrentOptions->ProxyDestroyedMesh.LoadSynchronous(); 
								}

	void						OnDPClassSelected(const UClass* uclass) const { CurrentOptions->ProxyDestroyedPrefab=uclass; CurrentOptions->defProxyDestroyedPrefab=true; }
	TWeakObjectPtr<UClass>		GetDPCurrentClass() const {	
									if(!CurrentOptions->defProxyDestroyedPrefab) {
										return UrdMultiToken::StaticClass();
									}
									return CurrentOptions->ProxyDestroyedPrefab.LoadSynchronous(); 
								}

	void						OnMatList1Changed(const TArray<UMaterialInterface*>& mats) { 
									CurrentOptions->materialList=mats; 
									CurrentOptions->ProxyStaticMeshMaterials.Empty();
									for(auto& m:mats) {
										CurrentOptions->ProxyStaticMeshMaterials.Add(m);
									}
									CurrentOptions->defProxyMeshMaterials=true;
								}
	void						OnMatList2Changed(const TArray<UMaterialInterface*>& mats) { 
									CurrentOptions->materialList2=mats; 
									CurrentOptions->ProxyDestroyedMeshMaterials.Empty(); 
									for(auto& m:mats) {
										CurrentOptions->ProxyDestroyedMeshMaterials.Add(m);
									}
									CurrentOptions->defProxyDestroyedMaterials=true;
								}

	TArray<TSharedPtr<FString>> proxyType_ComboList;
	TSharedPtr<STextBlock>		proxyTypeLabel;
	TArray<TSharedPtr<FString>> collision1_ComboList;
	TSharedPtr<STextBlock>		collision1Label;
	TArray<TSharedPtr<FString>> collision2_ComboList;
	TSharedPtr<STextBlock>		collision2Label;
	TArray<TSharedPtr<FString>> pickupType_ComboList;
	TSharedPtr<STextBlock>		pickupTypeLabel;
	TArray<TSharedPtr<FString>> pickupHighlightType_ComboList;
	TSharedPtr<STextBlock>		pickupHighlightTypeLabel;
	TArray<TSharedPtr<FString>> pickupHighlightStyle_ComboList;
	TSharedPtr<STextBlock>		pickupHighlightStyleLabel;

	FString						GetProxyTypeString();
	void						OnProxyTypeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo);
	FString						GetCollision1String();
	void						OnCollision1Changed(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo);
	FString						GetCollision2String();
	void						OnCollision2Changed(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo);
	FString						GetPickupTypeString();
	void						OnPickupTypeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo);
	FString						GetHighlightTypeString();
	void						OnPickupHighlightTypeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo);
	FString						GetHighlightStyleString();
	void						OnPickupHighlightStyleChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo);

	TSharedPtr<SWidget>			color1PickerWidget=nullptr;
	TSharedPtr<SWidget>			color2PickerWidget=nullptr;

	TSharedPtr<rdMaterialList>	matList1;
	TSharedPtr<rdMaterialList>	matList2;

protected:
	UrdInstOptions*				CurrentOptions=nullptr;
};

class rdInstProxySettingsOptions : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(rdInstProxySettingsOptions)
		: _WidgetWindow()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>,WidgetWindow)
		SLATE_ARGUMENT(TArray<TWeakObjectPtr<UObject>>,SettingsObjects)
		SLATE_END_ARGS()

public:
					rdInstProxySettingsOptions();
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
						bUserCancelled=true;
						if(WidgetWindow.IsValid()) {
							WidgetWindow.Pin()->RequestDestroyWindow();
						}
						return FReply::Handled();
					}
	FReply			SetDefault() {
						if(instClass) {
							instClass->SetOptionDefaults(TEXT("ProxySettings"),CurrentOptions);
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

TSharedRef<IDetailCustomization> FrdInstProxySettingsOptionsCustomization::MakeInstance() {
	return MakeShareable(new FrdInstProxySettingsOptionsCustomization());
}

FLinearColor FrdInstProxySettingsOptionsCustomization::GetSourceColor1() const {

	if(!CurrentOptions->defPickupHighlightColor1) {
		return FLinearColor(0,0,0,0);
	}
	return CurrentOptions->pickupHighlightColor1;
}
void FrdInstProxySettingsOptionsCustomization::OnColorPickerPicked1(FLinearColor col) {
	CurrentOptions->pickupHighlightColor1=col;
	CurrentOptions->defPickupHighlightColor1=true;
}

FLinearColor FrdInstProxySettingsOptionsCustomization::GetSourceColor2() const {

	if(!CurrentOptions->defPickupHighlightColor2) {
		return FLinearColor(0,0,0,0);
	}
	return CurrentOptions->pickupHighlightColor2;
}
void FrdInstProxySettingsOptionsCustomization::OnColorPickerPicked2(FLinearColor col) {
	CurrentOptions->pickupHighlightColor2=col;
	CurrentOptions->defPickupHighlightColor2=true;
}

void FrdInstProxySettingsOptionsCustomization::ShowRelevantSections() {

	if(pBldr2) pBldr2->SetCategoryVisibility(CurrentOptions->proxyType==1 || CurrentOptions->proxyType==2);
	if(pBldr3) pBldr3->SetCategoryVisibility(CurrentOptions->proxyType==1 || CurrentOptions->proxyType==2);
	if(pBldr4) pBldr4->SetCategoryVisibility(CurrentOptions->proxyType==1 || CurrentOptions->proxyType==2);
	if(pBldr5) pBldr5->SetCategoryVisibility(CurrentOptions->proxyType>0 && CurrentOptions->proxyType<4);
	if(pBldr6) pBldr6->SetCategoryVisibility(CurrentOptions->proxyType==4);
}

FString FrdInstProxySettingsOptionsCustomization::GetProxyTypeString() {

	if(!CurrentOptions->defProxyType) {
		return TEXT("(Multi)");
	}
	switch(CurrentOptions->proxyType) {
		case 1: return TEXT("Short Distance Proxy");
		case 2: return TEXT("Long Distance Proxy");
		case 3: return TEXT("Destruction-Only Proxy");
		case 4: return TEXT("Pickup Proxy");
	}
	return TEXT("None");
}

void FrdInstProxySettingsOptionsCustomization::OnProxyTypeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo) {
	proxyTypeLabel.Get()->SetText(FText::FromString(*Selection));
	if(*Selection==TEXT("None")) {
		CurrentOptions->bProxyUseDestruction=false;
		CurrentOptions->proxyType=0;
	} else if(*Selection==TEXT("Short Distance Proxy")) {
		CurrentOptions->bProxyUseDestruction=false;
		CurrentOptions->proxyType=1;
	} else if(*Selection==TEXT("Long Distance Proxy")) {
		CurrentOptions->bProxyUseDestruction=false;
		CurrentOptions->proxyType=2;
	} else if(*Selection==TEXT("Destruction-Only Proxy")) {
		CurrentOptions->proxyType=3;
		CurrentOptions->bProxyUseDestruction=true;
	} else if(*Selection==TEXT("Pickup Proxy")) {
		CurrentOptions->bProxyUseDestruction=false;
		CurrentOptions->proxyType=4;
	}
	CurrentOptions->defProxyUseDestruction=true;
	CurrentOptions->defProxyType=true;

	ShowRelevantSections();
}

FString FrdInstProxySettingsOptionsCustomization::GetCollision1String() {
	if(!CurrentOptions->defProxyMeshCollision) {
		return TEXT("(Multi)");
	}
	switch(CurrentOptions->ProxyStaticMeshCollision) {
		case 1: return TEXT("No Collision");
		case 2: return TEXT("Query Only");
		case 3: return TEXT("Physics Only");
		case 4: return TEXT("Collision Enabled");
		case 5: return TEXT("Probe Only");
		case 6: return TEXT("Query and Probe");
	}
	return TEXT("Use Default");
}

void FrdInstProxySettingsOptionsCustomization::OnCollision1Changed(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo) {
	collision1Label.Get()->SetText(FText::FromString(*Selection));
	if(*Selection==TEXT("Use Default")) {
		CurrentOptions->ProxyStaticMeshCollision=0;
	} else if(*Selection==TEXT("No Collision")) {
		CurrentOptions->ProxyStaticMeshCollision=1;
	} else if(*Selection==TEXT("Query Only")) {
		CurrentOptions->ProxyStaticMeshCollision=2;
	} else if(*Selection==TEXT("Physics Only")) {
		CurrentOptions->ProxyStaticMeshCollision=3;
	} else if(*Selection==TEXT("Collision Enabled")) {
		CurrentOptions->ProxyStaticMeshCollision=4;
	}
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	else if(*Selection==TEXT("Probe Only")) {
		CurrentOptions->ProxyStaticMeshCollision=5;
	} else if(*Selection==TEXT("Query and Probe")) {
		CurrentOptions->ProxyStaticMeshCollision=6;
	}
#endif
	CurrentOptions->defProxyMeshCollision=true;
}

FString FrdInstProxySettingsOptionsCustomization::GetCollision2String() {
	if(!CurrentOptions->defProxyDestroyedCollision) {
		return TEXT("(Multi)");
	}
	switch(CurrentOptions->ProxyDestroyedMeshCollision) {
		case 1: return TEXT("No Collision");
		case 2: return TEXT("Query Only");
		case 3: return TEXT("Physics Only");
		case 4: return TEXT("Collision Enabled");
		case 5: return TEXT("Probe Only");
		case 6: return TEXT("Query and Probe");
	}
	return TEXT("Use Default");
}

void FrdInstProxySettingsOptionsCustomization::OnCollision2Changed(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo) {
	collision2Label.Get()->SetText(FText::FromString(*Selection));
	if(*Selection==TEXT("Use Default")) {
		CurrentOptions->ProxyDestroyedMeshCollision=0;
	} else if(*Selection==TEXT("No Collision")) {
		CurrentOptions->ProxyDestroyedMeshCollision=1;
	} else if(*Selection==TEXT("Query Only")) {
		CurrentOptions->ProxyDestroyedMeshCollision=2;
	} else if(*Selection==TEXT("Physics Only")) {
		CurrentOptions->ProxyDestroyedMeshCollision=3;
	} else if(*Selection==TEXT("Collision Enabled")) {
		CurrentOptions->ProxyDestroyedMeshCollision=4;
	}
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	else if(*Selection==TEXT("Probe Only")) {
		CurrentOptions->ProxyDestroyedMeshCollision=5;
	} else if(*Selection==TEXT("Query and Probe")) {
		CurrentOptions->ProxyDestroyedMeshCollision=6;
	}
#endif
	CurrentOptions->defProxyDestroyedCollision=true;
}

FString	FrdInstProxySettingsOptionsCustomization::GetPickupTypeString() {

	if(!CurrentOptions->defPickupType) {
		return TEXT("(Multi)");
	}
	switch(CurrentOptions->pickupType) {
		case 0: return TEXT("None");
		case 1: return TEXT("Overlap");
		case 2: return TEXT("Interact");
		case 3: return TEXT("Long Interact");
	}
	return TEXT("None");
}

void FrdInstProxySettingsOptionsCustomization::OnPickupTypeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo) {
	pickupTypeLabel.Get()->SetText(FText::FromString(*Selection));
	if(*Selection==TEXT("None")) {
		CurrentOptions->pickupType=0;
	} else if(*Selection==TEXT("Overlap")) {
		CurrentOptions->pickupType=1;
	} else if(*Selection==TEXT("Interact")) {
		CurrentOptions->pickupType=2;
	} else if(*Selection==TEXT("Long Interact")) {
		CurrentOptions->pickupType=3;
	}
	CurrentOptions->defPickupType=true;
}

FString	FrdInstProxySettingsOptionsCustomization::GetHighlightTypeString() {

	if(!CurrentOptions->defPickupHighlightType) {
		return TEXT("(Multi)");
	}
	switch(CurrentOptions->pickupHighlightType) {
		case 0: return TEXT("No Highlight");
		case 1: return TEXT("Outline");
		case 2: return TEXT("LookAt Outline");
	}
	return TEXT("None");
}

void FrdInstProxySettingsOptionsCustomization::OnPickupHighlightTypeChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo) {
	pickupHighlightTypeLabel.Get()->SetText(FText::FromString(*Selection));
	if(*Selection==TEXT("No Highlight")) {
		CurrentOptions->pickupHighlightType=0;
	} else if(*Selection==TEXT("Outline")) {
		CurrentOptions->pickupHighlightType=1;
	} else if(*Selection==TEXT("LookAt Outline")) {
		CurrentOptions->pickupHighlightType=2;
	}
	CurrentOptions->defPickupHighlightType=true;
}

FString	FrdInstProxySettingsOptionsCustomization::GetHighlightStyleString() {

	if(!CurrentOptions->defPickupHighlightStyle) {
		return TEXT("(Multi)");
	}
	switch(CurrentOptions->pickupHighlightStyle) {
		case 0: return TEXT("Solid Color");
		case 1: return TEXT("Pulsing between 2 Colors");
		case 2: return TEXT("Flashing between 2 Colors");
	}
	return TEXT("None");
}

void FrdInstProxySettingsOptionsCustomization::OnPickupHighlightStyleChanged(TSharedPtr<FString> Selection,ESelectInfo::Type SelectInfo) {
	pickupHighlightStyleLabel.Get()->SetText(FText::FromString(*Selection));
	if(*Selection==TEXT("Solid Color")) {
		CurrentOptions->pickupHighlightStyle=0;
	} else if(*Selection==TEXT("Pulsing between 2 Colors")) {
		CurrentOptions->pickupHighlightStyle=1;
	} else if(*Selection==TEXT("Flashing between 2 Colors")) {
		CurrentOptions->pickupHighlightStyle=2;
	}
	CurrentOptions->defPickupHighlightStyle=true;
}

//.............................................................................
// ShowColorPicker1
//.............................................................................
FReply FrdInstProxySettingsOptionsCustomization::ShowColorPicker1(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdInstProxySettingsOptionsCustomization::OnColorPickerPicked1);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetSourceColor1();
#else
	cargs.InitialColorOverride=GetSourceColor1();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=color1PickerWidget;
	cargs.ParentWidget=color1PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// ShowColorPicker2
//.............................................................................
FReply FrdInstProxySettingsOptionsCustomization::ShowColorPicker2(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {

	FColorPickerArgs cargs;
	cargs.bIsModal=false;
	cargs.OnColorCommitted=FOnLinearColorValueChanged::CreateSP(this,&FrdInstProxySettingsOptionsCustomization::OnColorPickerPicked2);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	cargs.InitialColor=GetSourceColor2();
#else
	cargs.InitialColorOverride=GetSourceColor2();
#endif
	cargs.bUseAlpha=true;
	cargs.OptionalOwningDetailsView=color2PickerWidget;
	cargs.ParentWidget=color2PickerWidget;
	cargs.bOpenAsMenu=true;

	OpenColorPicker(cargs);

	return FReply::Handled();
}

//.............................................................................
// Constructor
//.............................................................................
FrdInstProxySettingsOptionsCustomization::FrdInstProxySettingsOptionsCustomization() {

	proxyType_ComboList.Add(MakeShareable(new FString(TEXT("None"))));
	proxyType_ComboList.Add(MakeShareable(new FString(TEXT("Short Distance Proxy"))));
	proxyType_ComboList.Add(MakeShareable(new FString(TEXT("Long Distance Proxy"))));
	proxyType_ComboList.Add(MakeShareable(new FString(TEXT("Destruction-Only Proxy"))));
	proxyType_ComboList.Add(MakeShareable(new FString(TEXT("Pickup Proxy"))));
	
	collision1_ComboList.Add(MakeShareable(new FString(TEXT("Use Default"))));
	collision1_ComboList.Add(MakeShareable(new FString(TEXT("No Collision"))));
	collision1_ComboList.Add(MakeShareable(new FString(TEXT("Query Only"))));
	collision1_ComboList.Add(MakeShareable(new FString(TEXT("Physics Only"))));
	collision1_ComboList.Add(MakeShareable(new FString(TEXT("Collision Enabled"))));
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	collision1_ComboList.Add(MakeShareable(new FString(TEXT("Probe Only"))));
	collision1_ComboList.Add(MakeShareable(new FString(TEXT("Query and Probe"))));
#endif
	
	collision2_ComboList.Add(MakeShareable(new FString(TEXT("Use Default"))));
	collision2_ComboList.Add(MakeShareable(new FString(TEXT("No Collision"))));
	collision2_ComboList.Add(MakeShareable(new FString(TEXT("Query Only"))));
	collision2_ComboList.Add(MakeShareable(new FString(TEXT("Physics Only"))));
	collision2_ComboList.Add(MakeShareable(new FString(TEXT("Collision Enabled"))));
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	collision2_ComboList.Add(MakeShareable(new FString(TEXT("Probe Only"))));
	collision2_ComboList.Add(MakeShareable(new FString(TEXT("Query and Probe"))));
#endif
	
	pickupType_ComboList.Add(MakeShareable(new FString(TEXT("None"))));
	pickupType_ComboList.Add(MakeShareable(new FString(TEXT("Overlap"))));
	pickupType_ComboList.Add(MakeShareable(new FString(TEXT("Interact"))));
	pickupType_ComboList.Add(MakeShareable(new FString(TEXT("Long Interact"))));
	
	pickupHighlightType_ComboList.Add(MakeShareable(new FString(TEXT("No Highlight"))));
	pickupHighlightType_ComboList.Add(MakeShareable(new FString(TEXT("Outline"))));
	pickupHighlightType_ComboList.Add(MakeShareable(new FString(TEXT("LookAt Outline"))));
	
	pickupHighlightStyle_ComboList.Add(MakeShareable(new FString(TEXT("Solid Color"))));
	pickupHighlightStyle_ComboList.Add(MakeShareable(new FString(TEXT("Pulsing between 2 Colors"))));
	pickupHighlightStyle_ComboList.Add(MakeShareable(new FString(TEXT("Flashing between 2 Colors"))));
}

TSharedRef<SWidget> FrdInstProxySettingsOptionsCustomization::MakeComboWidget(TSharedPtr<FString> InItem) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#else
	return SNew(STextBlock).Text(FText::FromString(*InItem)).Font(FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")));
#endif
}

//.............................................................................
// CustomizeDetails
//.............................................................................
void FrdInstProxySettingsOptionsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) {

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	// Try and find rdInst options instance in currently edited objects
	CurrentOptions=Cast<UrdInstOptions>((WeakObjects.FindByPredicate([](TWeakObjectPtr<UObject> Object) { return Cast<UrdInstOptions>(Object.Get()); }))->Get());

	FMargin margin(2.0f,2.0f,2.0f,2.0f);
	float labWidth=130.0f;
	FMargin labMargin(0.0f, 5.0f, 2.0f, 2.0f);
	FLinearColor lcRed=FLinearColor(1,0,0,1);
	FLinearColor lcGreen=FLinearColor(0,1,0,1);
	FLinearColor lcBlue=FLinearColor(0,0,1,1);
	FLinearColor lcBlack=FLinearColor(0.2,0.2,0.2,0);
	FLinearColor lcGrey=FLinearColor(0.4,0.4,0.4,0);

	// .......................................................................
	// Main Settings
	IDetailCategoryBuilder& bldr1=DetailBuilder.EditCategory(TEXT("Main Settings")).RestoreExpansionState(false).InitiallyCollapsed(false);
	pBldr1=&bldr1;

	// ProxyType
	rdFullRow(bldr1,lrow1,box1,"");
	rdLabel(box1,120,"ProxyType","ProxyType");
	rdInt32Combo(box1,GetProxyTypeString(),&proxyType_ComboList,&FrdInstProxySettingsOptionsCustomization::OnProxyTypeChanged,&FrdInstProxySettingsOptionsCustomization::MakeComboWidget,proxyTypeLabel,"rdInst_ProxyType","Select the type of Proxy",260);

	rdFullRow(bldr1,lrow1a,box1a,"");
	// All Mesh proxy
	rdCheckboxMEn(box1a,CurrentOptions->bProxyAllMesh,CurrentOptions->defProxyAllMesh,"ProxyAllInstances","Proxy All Instances","ProxyAllInstancesTT","When ticked, all instances of the mesh or prefab will be swapped to the proxy.",CurrentOptions->proxyType!=0);
	// Don't Remove
	rdCheckboxMEn(box1a,CurrentOptions->bProxyDontRemove,CurrentOptions->defProxyDontRemove,"ProxyDontRemove","Don't remove proxy","ProxyDontRemoveTT","Don't remove the proxy when out of proximity.",(CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4));
	// Don't Hide
	rdCheckboxMEn(box1a,CurrentOptions->bProxyDontHide,CurrentOptions->defProxyDontHide,"ProxyDontHide","Don't hide Mesh","ProxyDontHideTT","Don't hide the main Mesh when the proxy is swapped in.",(CurrentOptions->proxyType>0&&CurrentOptions->proxyType<3));

	// Call Swap Delegates
	rdFullRow(bldr1,lrow1ba,box1ba,"");
	rdCheckboxMEn(box1ba,CurrentOptions->ProxyCallSwapEvent,CurrentOptions->defCallSwapEvent,"ProxyCallDelegates","Call Swap Delegates","ProxyCallDelegatesTT","When ticked, any Delegates added will be called when swapped in and out.",(CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4));

	// Physics Timeout
	rdFullRow(bldr1,lrow1b,box1b,"");
	rdNumBoxMEn(box1b,"Physics Timeout",CurrentOptions->ProxyPhysicsTimeout,CurrentOptions->defProxyPhysicsTimeout,0.0f,99.9f,240,lcBlack,CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4);
	// Proxy Scan Distance
	rdNumBoxMEn(box1b,"Scan Distance",CurrentOptions->ProxyLongDistance,CurrentOptions->defProxyLongDistance,-1.0f,999999.9f,220,lcBlack,CurrentOptions->proxyType==1 || CurrentOptions->proxyType==2);

	// .......................................................................
	// Proxy Actor Settings
	IDetailCategoryBuilder& bldr2=DetailBuilder.EditCategory(TEXT("Actor Proxy")).RestoreExpansionState(false).InitiallyCollapsed(false);
	pBldr2=&bldr2;

	rdFullRow(bldr2,lrow2a,box2a,"");
	rdLabel(box2a,120,"ProxyActor","Actor");
	rdClassPickerEn(box2a,&FrdInstProxySettingsOptionsCustomization::OnPAClassSelected,&FrdInstProxySettingsOptionsCustomization::GetPACurrentClass,290,AActor::StaticClass(),CurrentOptions->proxyType==1||CurrentOptions->proxyType==2);

	// Proxy Actor, Pooled, Num to Pool, Simple, Saved State
	rdFullRow(bldr2,lrow2b,box2b,"");
	rdCheckboxMEn(box2b,CurrentOptions->bProxyPool,CurrentOptions->defProxyPool,"ProxyPooled","Pool Proxy Actor","ProxyPooledTT","Pool the proxy actor that is swapped to within proximity.",(CurrentOptions->proxyType==1||CurrentOptions->proxyType==2));
	rdCheckboxMEn(box2b,CurrentOptions->bProxySimplePool,CurrentOptions->defProxySimplePool,"ProxySimplePool","Use Simple Pooling","ProxySimplePoolTT","Use Simple pooling, no component activating or initializing is done.",(CurrentOptions->proxyType==1||CurrentOptions->proxyType==2));
	rdNumBoxMIEn(box2b,"Num to Pool",CurrentOptions->ProxyNumToPool,CurrentOptions->defProxyNumToPool,0,10000,150,lcBlack,(CurrentOptions->proxyType==1||CurrentOptions->proxyType==2));

	// .......................................................................
	// Proxy DataLayer Settings
	IDetailCategoryBuilder& bldr3=DetailBuilder.EditCategory(TEXT("Datalayer Proxy")).RestoreExpansionState(false).InitiallyCollapsed(false);
	pBldr3=&bldr3;

	rdFullRow(bldr3,lrow3a,box3a,"");
	rdLabel(box3a,120,"ProxyDataLayer","Proxy DataLayer");
	rdAssetPickerEn(box3a,&FrdInstProxySettingsOptionsCustomization::OnDLAssetSelected,&FrdInstProxySettingsOptionsCustomization::GetDLCurrentAsset,290,UDataLayerAsset::StaticClass(),CurrentOptions->proxyType==1||CurrentOptions->proxyType==2);

	// .......................................................................
	// Proxy StaticMesh Settings
	IDetailCategoryBuilder& bldr4=DetailBuilder.EditCategory(TEXT("StaticMesh Proxy")).RestoreExpansionState(false).InitiallyCollapsed(false);
	pBldr4=&bldr4;

	rdFullRow(bldr4,lrow4a,box4a,"");
	rdLabel(box4a,120,"ProxyStaticMesh","StaticMesh");
	rdAssetPickerEn(box4a,&FrdInstProxySettingsOptionsCustomization::OnSMAssetSelected,&FrdInstProxySettingsOptionsCustomization::GetSMCurrentAsset,290,UStaticMesh::StaticClass(),CurrentOptions->proxyType==1||CurrentOptions->proxyType==2);

	// Material Overrides
	SAssignNew(matList1,rdMaterialList).TextBoxWidth(350).CatBuilder(&bldr4).MaterialList(CurrentOptions->materialList);
	matList1->onMaterialsUpdated.BindSP(this,&FrdInstProxySettingsOptionsCustomization::OnMatList1Changed);

	// Collision
	rdFullRow(bldr4,lrow4c,box4c,"");
	rdLabel(box4c,120,"Coll1","Collision Type");
	rdInt32ComboEn(box4c,GetCollision1String(),&collision1_ComboList,&FrdInstProxySettingsOptionsCustomization::OnCollision1Changed,&FrdInstProxySettingsOptionsCustomization::MakeComboWidget,collision1Label,"rdInst_CollisionType","Select the type of collision for the StaticMesh",260,CurrentOptions->proxyType==1||CurrentOptions->proxyType==2);

	// .......................................................................
	// Destruction Settings
	IDetailCategoryBuilder& bldr5=DetailBuilder.EditCategory(TEXT("Destruction Settings")).RestoreExpansionState(false).InitiallyCollapsed(false);
	pBldr5=&bldr5;

	// Use with Destruction
	rdFullRow(bldr5,lrow5a,box5a,"");
	rdCheckboxMEn(box5a,CurrentOptions->bProxyUseDestruction,CurrentOptions->defProxyUseDestruction,"ProxyUseDestruction","Add Destruction Proxy","ProxyUseDestructionTT","When ticked, this actor is scanned with the Destruction Scanner.",(CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4));

	// Destroyed mesh
	rdFullRow(bldr5,lrow5b,box5b,"");
	rdLabel(box5b,120,"ProxyDestructionMesh","Destroyed Mesh");
	rdAssetPickerEn(box5b,&FrdInstProxySettingsOptionsCustomization::OnDMAssetSelected,&FrdInstProxySettingsOptionsCustomization::GetDMCurrentAsset,290,UStaticMesh::StaticClass(),CurrentOptions->bProxyUseDestruction);

	// Material Overrides
	SAssignNew(matList2,rdMaterialList).TextBoxWidth(350).CatBuilder(&bldr5).MaterialList(CurrentOptions->materialList2);
	matList2->onMaterialsUpdated.BindSP(this,&FrdInstProxySettingsOptionsCustomization::OnMatList2Changed);

	// Collision
	rdFullRow(bldr5,lrow5d,box5d,"");
	rdLabel(box5d,120,"Coll1","Collision Type");
	rdInt32ComboEn(box5d,GetCollision2String(),&collision2_ComboList,&FrdInstProxySettingsOptionsCustomization::OnCollision2Changed,&FrdInstProxySettingsOptionsCustomization::MakeComboWidget,collision2Label,"rdInst_CollisionType","Select the type of collision for the StaticMesh",260,CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4&&CurrentOptions->bProxyUseDestruction);

	// Destroyed prefab
	rdFullRow(bldr5,lrow5e,box5e,"");
	rdLabel(box5e,120,"ProxyDestroyedPrefab","Destroyed Prefab");
	rdClassPickerEn(box5e,&FrdInstProxySettingsOptionsCustomization::OnDPClassSelected,&FrdInstProxySettingsOptionsCustomization::GetDPCurrentClass,290,ArdActor::StaticClass(),CurrentOptions->proxyType!=0&&CurrentOptions->bProxyUseDestruction);

	// Destroyed offset
	rdFullRow(bldr5,lrow5f,box5f,"");
	rdLabel(box5f,120,"Offset","Destroyed Offset");
	rdNumBoxMEn(box5f,"X",CurrentOptions->pivotOffset.X,CurrentOptions->defProxyDestroyedOffsetX,-99999.0f,99999.0f,110,lcRed,CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4&&CurrentOptions->bProxyUseDestruction);
	rdNumBoxMEn(box5f,"Y",CurrentOptions->pivotOffset.Y,CurrentOptions->defProxyDestroyedOffsetY,-99999.0f,99999.0f,110,lcGreen,CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4&&CurrentOptions->bProxyUseDestruction);
	rdNumBoxMEn(box5f,"Z",CurrentOptions->pivotOffset.Z,CurrentOptions->defProxyDestroyedOffsetZ,-99999.0f,99999.0f,110,lcBlue,CurrentOptions->proxyType>0&&CurrentOptions->proxyType<4&&CurrentOptions->bProxyUseDestruction);

	// .......................................................................
	// Pickup Settings
	IDetailCategoryBuilder& bldr6=DetailBuilder.EditCategory(TEXT("Pickup Settings")).RestoreExpansionState(false).InitiallyCollapsed(false);
	pBldr6=&bldr6;

	rdFullRow(bldr6,lrow6a,box6a,"");
	rdLabel(box6a,120,"puType","Pickup Type");
	rdInt32ComboEn(box6a,GetPickupTypeString(),&pickupType_ComboList,&FrdInstProxySettingsOptionsCustomization::OnPickupTypeChanged,&FrdInstProxySettingsOptionsCustomization::MakeComboWidget,pickupTypeLabel,"rdInst_PickupType","Select the Pickup Behavior Type",200,CurrentOptions->proxyType==4);

	rdNumBoxMIEn(box6a,"ID",CurrentOptions->pickupID,CurrentOptions->defPickupID,0,9999999,110,lcGrey,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	rdFullRow(bldr6,lrow6b,box6b,"");
	rdLabel(box6b,120,"puHiType","Highlight Type");
	rdInt32ComboEn(box6b,GetHighlightTypeString(),&pickupHighlightType_ComboList,&FrdInstProxySettingsOptionsCustomization::OnPickupHighlightTypeChanged,&FrdInstProxySettingsOptionsCustomization::MakeComboWidget,pickupHighlightTypeLabel,"rdInst_PickupHighlightType","Select the Pickup Highlight Type",200,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	rdFullRow(bldr6,lrow6c,box6c,"");
	rdLabel(box6c,120,"puHiStyle","Highlight Style");
	rdInt32ComboEn(box6c,GetHighlightStyleString(),&pickupHighlightStyle_ComboList,&FrdInstProxySettingsOptionsCustomization::OnPickupHighlightStyleChanged,&FrdInstProxySettingsOptionsCustomization::MakeComboWidget,pickupHighlightStyleLabel,"rdInst_PickupHighlightStyle","Select the Pickup Highlight Style Type",200,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	// StencilBuffer and Highlight Thickness
	rdFullRow(bldr6,lrow6c2,box6c2,"");
	rdCheckboxMEn(box6c2,CurrentOptions->bPickupUseStencilBuffer,CurrentOptions->defPickupUseStencilBuffer,"UseStencilBuffer","Use StencilBuffer","UseStencilBufferTT","When ticked, uses a StencilBuffer for rendering the highlights rather than a inverse-hull mesh",CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0&&CurrentOptions->pickupHighlightType>0);
	rdNumBoxMEn(box6c2," Highlight Thickness ",CurrentOptions->pickupHighlightThickness,CurrentOptions->defPickupHighlightThickness,0.0f,5.0f,200,lcBlack,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0&&CurrentOptions->pickupHighlightType>0);

	// Highlight Distance
	rdFullRow(bldr6,lrow6c1,box6c1,"");
	rdNumBoxMEn(box6c1," Highlight Distance ",CurrentOptions->pickupHighlightDistance,CurrentOptions->defPickupHighlightDistance,0.0f,999999.9f,200,lcBlack,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0&&CurrentOptions->pickupHighlightType>0);

	// Pickup Distance
	rdNumBoxMEn(box6c1," Pickup Distance ",CurrentOptions->pickupPickupDistance,CurrentOptions->defPickupPickupDistance,0.0f,999999.9f,200,lcBlack,CurrentOptions->proxyType==4&&CurrentOptions->pickupType==1);

	// Color pickers
	rdFullRow(bldr6,lrow6d,box6d,"");
	rdLabel(box6d,60,"puHiCol1","  Color 1 ");
	rdColorPickerMEn(box6d,color1PickerWidget,CurrentOptions->pickupHighlightColor1,CurrentOptions->defPickupHighlightColor1,&FrdInstProxySettingsOptionsCustomization::ShowColorPicker1,"puoc1","Pickup Outline Color 1",CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);
	rdLabel(box6d,70," puHiCol1","   Color 2 ");
	rdColorPickerMEn(box6d,color2PickerWidget,CurrentOptions->pickupHighlightColor2,CurrentOptions->defPickupHighlightColor2,&FrdInstProxySettingsOptionsCustomization::ShowColorPicker2,"puoc2","Pickup Outline Color 2",CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	// Strobe Speed
	rdFullRow(bldr6,lrow6e,box6e,"");
	rdNumBoxMEn(box6e,"Strobe Speed ",CurrentOptions->pickupStrobeSpeed,CurrentOptions->defPickupStrobeSpeed,0.0f,999999.9f,200,lcBlack,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	// Ebb Speed
	rdNumBoxMEn(box6e," Ebb Speed ",CurrentOptions->pickupEbbSpeed,CurrentOptions->defPickupEbbSpeed,0.0f,999999.9f,200,lcBlack,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	// Fade Highlight
	rdFullRow(bldr6,lrow6g,box6g,"");
	rdCheckboxMEn(box6g,CurrentOptions->bPickupFadeHighlight,CurrentOptions->defPickupFadeHighlight,"FadeHighlight","Fade to ground","FadeHighlightTT","When ticked, Fades the pickup highlight to transparent as it nears the ground",CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);
	rdCheckboxMEn(box6g,CurrentOptions->bPickupDistanceFadeHighlight,CurrentOptions->defPickupDistanceFadeHighlight,"DistanceFadeHighlight","Distance Fade","FadeHighlightTT2","When ticked, Fades the pickup highlight to transparent when close to its maximum highlight distance",CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0&&CurrentOptions->pickupHighlightType==1);

	// Respawn Time
	rdFullRow(bldr6,lrow6h,box6h,"");
	rdNumBoxMEn(box6h,"Respawn Time",CurrentOptions->pickupRespawnTime,CurrentOptions->defPickupRespawnTime,0.0f,999999.9f,240,lcBlack,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	// Lonely Respawn
	rdCheckboxMEn(box6h,CurrentOptions->bPickupRespawnWhenLonely,CurrentOptions->defPickupRespawnWhenLonely,"LonelyRespawn","Respawn when lonely","LonelyRespawnTT","When ticked, only respawn the pickup when on players are around",CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0);

	// Respawn Distance
	rdFullRow(bldr6,lrow6i,box6i,"");
	rdNumBoxMEn(box6i," Lonely Distance ",CurrentOptions->pickupRespawnDistance,CurrentOptions->defPickupRespawnDistance,0.0f,999999.9f,200,lcBlack,CurrentOptions->proxyType==4&&CurrentOptions->pickupType>0&&CurrentOptions->bPickupRespawnWhenLonely);


	// .......................................................................

	ShowRelevantSections();
}

rdInstProxySettingsOptions::rdInstProxySettingsOptions() : instClass(nullptr),CurrentOptions(nullptr),bUserCancelled(true) {}

void rdInstProxySettingsOptions::Construct(const FArguments& InArgs) {

	WidgetWindow = InArgs._WidgetWindow;

	// Retrieve property editor module and create a SDetailsView
	FPropertyEditorModule& PropertyEditorModule=FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch=false;
	DetailsViewArgs.NameAreaSettings=FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.bAllowMultipleTopLevelObjects=true;

	DetailsView=PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->RegisterInstancedCustomPropertyLayout(UrdInstOptions::StaticClass(),FOnGetDetailCustomizationInstance::CreateLambda([=]() { return FrdInstProxySettingsOptionsCustomization::MakeInstance(); }));
	DetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FSimpleRootObjectCustomization));
	DetailsView->SetObjects(InArgs._SettingsObjects,true);

	this->ChildSlot [ SNew(SVerticalBox)
		+ SVerticalBox::Slot().Padding(2).MaxHeight(900.0f) [ DetailsView->AsShared() ]
		+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(2) [
			SNew(SUniformGridPanel).SlotPadding(2)
			+ SUniformGridPanel::Slot(0,0) [
				rdWinButton("rdInst_ProxySettings_SetDef","Set as Default","rdInst_ProxySettings_ToolTip1","Set the current settings as the default",&rdInstProxySettingsOptions::SetDefault)
			]
			+ SUniformGridPanel::Slot(1,0) [
				rdWinButton("rdInst_ProxySettings_Set","Set","rdInst_ProxySettings_ToolTip2","Set the Proxy values in the asset",&rdInstProxySettingsOptions::OnConfirm)
			]
			+ SUniformGridPanel::Slot(2,0) [
				rdWinButton("rdInst_ProxySettings_Cancel","Cancel","rdInst_ProxySettings_ToolTip3","Closes without making any changes",&rdInstProxySettingsOptions::OnCancel)
			]
		]
	];
}

//.............................................................................
// ShowProxySettings
//.............................................................................
bool FrdInstEditorModule::ShowProxySettings() {

	// Create the settings window...
	TSharedRef<SWindow> rdWin=SNew(SWindow)
										.IsTopmostWindow(true)
										.Title(FText::FromString(TEXT("rdProxy Settings")))
										.SizingRule(ESizingRule::UserSized)
										.AutoCenter(EAutoCenter::PreferredWorkArea)
										.ClientSize(FVector2D(550,700));

	TArray<TWeakObjectPtr<UObject>> OptionObjects{ rdInstOptions };
	TSharedPtr<rdInstProxySettingsOptions> Options;

	rdWin->SetContent(SAssignNew(Options,rdInstProxySettingsOptions)
						.WidgetWindow(rdWin)
						.SettingsObjects(OptionObjects)
					  );
	Options->CurrentOptions=rdInstOptions;
	Options->instClass=this;

	if(!FModuleManager::Get().IsModuleLoaded("MainFrame")) {
		return false;
	}

	// Show Window
	IMainFrameModule& MainFrame=FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
	FSlateApplication::Get().AddModalWindow(rdWin,MainFrame.GetParentWindow(),false);

	if(Options->WasUserCancelled()) return false;

	rdInstOptions->ProxyDestroyedOffset.SetTranslation(rdInstOptions->pivotOffset); //@ for now we're just focusing on the translation, rot and scale later

	return true;
}

//.............................................................................

#undef LOCTEXT_NAMESPACE
