//
// rdInst_BrowserContextMenu.cpp
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 18th August 2024
// Last Modified: 14th April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Dialogs/Dialogs.h"
#include "Misc/MessageDialog.h"
#include "rdInstEditor.h"
#include "rdInstOptions.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstModule"

//----------------------------------------------------------------------------------------------------------------
// ContentBrowser Menu Extender
//----------------------------------------------------------------------------------------------------------------
FContentBrowserMenuExtension::FContentBrowserMenuExtension(const TArray<FAssetData,FDefaultAllocator> assets) {

	for(auto asset:assets) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		if(asset.AssetClassPath==UBlueprint::StaticClass()->GetClassPathName()) selectedBPs.Add(asset);
		if(asset.AssetClassPath==UStaticMesh::StaticClass()->GetClassPathName()) selectedStaticMeshes.Add(asset);
		if(asset.AssetClassPath==ULevel::StaticClass()->GetClassPathName()) selectedLevels.Add(asset);
#else
		if(asset.AssetClass==TEXT("Blueprint")) selectedBPs.Add(asset);
		if(asset.AssetClass==TEXT("StaticMesh")) selectedStaticMeshes.Add(asset);
		if(asset.AssetClass==TEXT("Level")) selectedLevels.Add(asset);
#endif
	}
}

//----------------------------------------------------------------------------------------------------------------
// AddMenuEntry
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::AddMenuEntry(FMenuBuilder& MenuBuilder) {

	rdAddSubMenu("rdInst","rdInst.Icon",&FContentBrowserMenuExtension::ShowContentBrowserMenu);
}

//----------------------------------------------------------------------------------------------------------------
// ShowContentBrowserMenu
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ShowContentBrowserMenu(FMenuBuilder& MenuBuilder) {

	if(selectedBPs.Num()<1 && selectedStaticMeshes.Num()<1) return;

	// Create our Section
	MenuBuilder.BeginSection("rdInst"); 

	if(selectedStaticMeshes.Num()>0) {
		rdAddMenuItem("Instance Settings...","Opens the Static Mesh Instance Settings Window.","rdInst.InstanceSettings",&FContentBrowserMenuExtension::ContextMenuItem_InstanceSettings_Selected);
		rdAddMenuItem("Random Settings...","Opens the Randomization Settings Window.","rdInst.RandomSettings",&FContentBrowserMenuExtension::ContextMenuItem_RandomSettings_Selected);
		rdAddMenuItem("Proxy Settings...","Set the Proxy options for use with the Create SpawnActor tool","rdInst.ProxySettings",&FContentBrowserMenuExtension::ContextMenuItem_ProxySettings_Selected);
	}

	if(selectedBPs.Num()>0) {
		rdAddMenuItem("Move Prefab data to Arrays","Moves the Prefab build data from the Fast Tables to the customizable Instance Arrays","rdInst.ProxySettings",&FContentBrowserMenuExtension::ContextMenuItem_PrefabDataToArrays_Selected);
		rdAddMenuItem("Move Prefab data to Fast Tables","Moves the Prefab build data from the customizable Instance Arrays to the Fast Tables","rdInst.ProxySettings",&FContentBrowserMenuExtension::ContextMenuItem_PrefabDataToTables_Selected);
	}

	if(selectedLevels.Num()>0) {
		rdAddMenuItem("Convert Level to SpawnActor...","Converts the Levels in the selection to SpawnActors.","rdInst.ConvertLevel",&FContentBrowserMenuExtension::ContextMenuItem_ConvertLevels_Selected);
		rdAddMenuItem("Update SpawnActor from Level...","Updates the SpawnActors from the selected Levels.","rdInst.ConvertLevel",&FContentBrowserMenuExtension::ContextMenuItem_UpdateFromLevels_Selected);
	}

	MenuBuilder.EndSection();
}

//----------------------------------------------------------------------------------------------------------------
// ContextMenuItem_InstanceSettings_Selected
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ContextMenuItem_InstanceSettings_Selected() {

	rdInstEditorMod->selectedAssets=selectedStaticMeshes;
	bool ret=rdInstEditorMod->ChangeInstanceSettings();
	if(!ret) {
		return;
	}

	rdInstEditorMod->ChangeInstanceSettingsForSelectedFiles();

}

//----------------------------------------------------------------------------------------------------------------
// ContextMenuItem_RandomSettings_Selected
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ContextMenuItem_RandomSettings_Selected() {

	rdInstEditorMod->selectedAssets=selectedStaticMeshes;
	rdInstEditorMod->rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());
	rdInstEditorMod->ShowRandomSettings(true);
}

//----------------------------------------------------------------------------------------------------------------
// ContextMenuItem_RandomSettings_Selected
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ContextMenuItem_ProxySettings_Selected() {

	rdInstEditorMod->selectedAssets=selectedStaticMeshes;
	rdInstEditorMod->ProxySettingsFromSelectedAssets();
}

//----------------------------------------------------------------------------------------------------------------
// ContextMenuItem_PrefabDataToArrays_Selected
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ContextMenuItem_PrefabDataToArrays_Selected() {
	rdInstEditorMod->selectedAssets=selectedStaticMeshes;
	rdInstEditorMod->ConvertSelectedAssetPrefabsToArrays();
}

//----------------------------------------------------------------------------------------------------------------
// ContextMenuItem_PrefabDataToTables_Selected
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ContextMenuItem_PrefabDataToTables_Selected() {
	rdInstEditorMod->selectedAssets=selectedStaticMeshes;
	rdInstEditorMod->ConvertSelectedAssetPrefabsToTables();
}

//----------------------------------------------------------------------------------------------------------------
// ContextMenuItem_ConvertLevels_Selected
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ContextMenuItem_ConvertLevels_Selected() {

	rdInstEditorMod->selectedAssets=selectedLevels;
	rdInstEditorMod->CreateSpawnActorsFromSelectedLevelAssets();
}

//----------------------------------------------------------------------------------------------------------------
// ContextMenuItem_ConvertLevels_Selected
//----------------------------------------------------------------------------------------------------------------
void FContentBrowserMenuExtension::ContextMenuItem_UpdateFromLevels_Selected() {

	rdInstEditorMod->selectedAssets=selectedLevels;
	rdInstEditorMod->UpdateSpawnActorsFromSelectedLevelAssets();
}

//----------------------------------------------------------------------------------------------------------------
// rdMenuExtender Constructor
//----------------------------------------------------------------------------------------------------------------
TSharedRef<FExtender> FrdInstEditorModule::rdMenuExtender(const TArray<FAssetData>& assets) {

	// see if there are any blueprints selected, if not we don't need to show our menu
	bool hasBlueprints=false,hasStaticMeshes=false;
	for(auto asset:assets) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		if(asset.AssetClassPath==UBlueprint::StaticClass()->GetClassPathName()) {
#else
		if(asset.AssetClass==TEXT("Blueprint")) {
#endif
			hasBlueprints=true;
		}
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		if(asset.AssetClassPath==UStaticMesh::StaticClass()->GetClassPathName()) {
#else
		if(asset.AssetClass==TEXT("StaticMesh")) {
#endif
			hasStaticMeshes=true;
		}
	}

	// Create extender that contains a delegate that will be called to get information about new context menu items
	TSharedPtr<FExtender> MenuExtender=MakeShareable(new FExtender());

	if(hasBlueprints || hasStaticMeshes) {

		rdMenuExtension=MakeShareable(new FContentBrowserMenuExtension(assets));
		rdMenuExtension->rdInstEditorMod=this;

		// Create a Shared-pointer delegate that keeps a weak reference to object
		MenuExtender->AddMenuExtension("CommonAssetActions",EExtensionHook::After,TSharedPtr<FUICommandList>(),
										FMenuExtensionDelegate::CreateSP(rdMenuExtension.ToSharedRef(),
										&FContentBrowserMenuExtension::AddMenuEntry));
	}

	return MenuExtender.ToSharedRef();
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
