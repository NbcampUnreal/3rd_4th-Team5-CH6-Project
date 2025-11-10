//
// rdMaterialList.cpp
//
// Copyright (c) 2021 Recourse Design ltd. All rights reserved.
//
// Creation Date: 5th December 2024
// Last Modified: 7th March 2025
//
#include "rdMaterialList.h"
#include "Modules/ModuleManager.h"
#include "Editor.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "PropertyEditorModule.h"
#include "Engine/Selection.h"
#include "DetailLayoutBuilder.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"
#include "Runtime/SlateCore/Public/Widgets/Images/SImage.h"
#include "PropertyCustomizationHelpers.h"
#include "rdInstEditor.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//.................................................................................................
// Construct
//.................................................................................................
void rdMaterialList::Construct(const FArguments& InArgs) {

	catBuilder=InArgs._CatBuilder.Get();
	materialList=InArgs._MaterialList.Get();
	textBoxWidth=InArgs._TextBoxWidth.Get();
	onMaterialsUpdated=InArgs._OnMaterialsUpdated;

	origMaterialList=materialList;

	if(!catBuilder) {
		return;
	}

	FMaterialListDelegates matDelegates;
	matDelegates.OnGetMaterials.BindSP(this,&rdMaterialList::OnGetMaterials,0);
	matDelegates.OnMaterialChanged.BindSP(this,&rdMaterialList::OnMaterialListChanged,0);
	matDelegates.OnGenerateCustomNameWidgets.BindSP(this,&rdMaterialList::OnGenerateCustomNameWidgetsForMaterialList);
	matDelegates.OnGenerateCustomMaterialWidgets.BindSP(this,&rdMaterialList::OnGenerateCustomMaterialWidgetsForMaterialList,0);
	matDelegates.OnMaterialListDirty.BindSP(this,&rdMaterialList::OnMaterialListDirty);

	TArray<FAssetData> matOwners={nullptr};
	matListWidget=new FMaterialList(catBuilder->GetParentLayout(),matDelegates,matOwners,true,false);

	catBuilder->AddCustomBuilder(MakeShareable(matListWidget));
}

//.................................................................................................
// OnCopyMaterialList
//.................................................................................................
void rdMaterialList::OnCopyMaterialList() {
}

//.................................................................................................
// OnPasteMaterialList
//.................................................................................................
void rdMaterialList::OnPasteMaterialList() {
}

//.................................................................................................
// OnCanCopyMaterialList
//.................................................................................................
bool rdMaterialList::OnCanCopyMaterialList() const {	
	
	return false; 
}

//.................................................................................................
// OnGetMaterials
//.................................................................................................
void rdMaterialList::OnGetMaterials(class IMaterialListBuilder& outMaterials,int32 lod) {

	int i=0;
	for(auto& m:materialList) {
		outMaterials.AddMaterial(i++,m,true);
	}
}

//.................................................................................................
// OnMaterialListChanged
//.................................................................................................
void rdMaterialList::OnMaterialListChanged(UMaterialInterface* newMaterial, UMaterialInterface* prevMaterial,int32 slot,bool bReplaceAll,int32 lod) {

	materialList[slot]=newMaterial;
	onMaterialsUpdated.ExecuteIfBound(materialList);
}

//.................................................................................................
// OnGenerateCustomNameWidgetsForMaterialList
//.................................................................................................
TSharedRef<SWidget> rdMaterialList::OnGenerateCustomNameWidgetsForMaterialList(UMaterialInterface* Material,int32 matIndex) {
	return SNew(SVerticalBox);
}

//.................................................................................................
// GetMaterialSlotName
//.................................................................................................
FText rdMaterialList::GetMaterialSlotName(int32 matIndex) const {
	//if(IsMaterialValid(MaterialIndex)) {
	//	return FText::FromName(GroomAsset->GetHairGroupsMaterials()[MaterialIndex].SlotName);
	//}
	return FText::FromName(NAME_None);
}

//.................................................................................................
// OnMaterialNameCommitted
//.................................................................................................
void rdMaterialList::OnMaterialNameCommitted(const FText& value, ETextCommit::Type commitType, int32 matIndex) {

}

//.................................................................................................
// CanDeleteMaterial
//.................................................................................................
bool rdMaterialList::CanDeleteMaterial(int32 matIndex) const {
	return false;
}
	
//.................................................................................................
// OnDeleteMaterial
//.................................................................................................
void rdMaterialList::OnDeleteMaterial(int32 matIndex) {
}

//.................................................................................................
// OnGenerateCustomMaterialWidgetsForMaterialList
//.................................................................................................
TSharedRef<SWidget> rdMaterialList::OnGenerateCustomMaterialWidgetsForMaterialList(UMaterialInterface* material,int32 matIndex,int32 lod) {

	return
		SNew(SMaterialSlotWidget,matIndex,true)
		.MaterialName(this,&rdMaterialList::GetMaterialSlotName,matIndex)
		.OnMaterialNameCommitted(this,&rdMaterialList::OnMaterialNameCommitted,matIndex);
		//.CanDeleteMaterial(this,&rdMaterialList::CanDeleteMaterial,matIndex)
		//.OnDeleteMaterial(this,&rdMaterialList::OnDeleteMaterial,matIndex);
}

//.................................................................................................
// AddMaterial
//.................................................................................................
FReply rdMaterialList::AddMaterial() {

	return FReply::Handled();
}

//.................................................................................................
// OnMaterialListDirty
//.................................................................................................
bool rdMaterialList::OnMaterialListDirty() {

	return false;
}

//.................................................................................................
// SetMaterialList
//.................................................................................................
void rdMaterialList::SetMaterialList(UStaticMesh* mesh) {

	materialList.Empty();
	if(!mesh) {
		return;
	}

	TArray<FStaticMaterial>& staticMats=mesh->GetStaticMaterials();
	for(auto& sm:staticMats) {
		materialList.Add(sm.MaterialInterface);
	}
}

//.................................................................................................
#undef LOCTEXT_NAMESPACE
