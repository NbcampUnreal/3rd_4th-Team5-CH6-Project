//
// rdInst_WorldContextMenu.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 15th September 2024
// Last Modified: 26th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "Runtime/Core/Public/Misc/ScopedSlowTask.h"
#include "Editor/UnrealEd/Public/ScopedTransaction.h"
#include "ToolMenus.h"
#include "SSceneOutliner.h"
#include "rdInstOptions.h"
#include "Dialogs/Dialogs.h"
#include "Misc/MessageDialog.h"
#include "Engine/Selection.h"
#include "Misc/ConfigCacheIni.h"
#include "LevelEditor.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "Misc/ConfigCacheIni.h"
#include "EngineUtils.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "rdInstOptions.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//----------------------------------------------------------------------------------------------------------------
// LevelMenuSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::LevelMenuSubMenu(UToolMenu* menu) {

	FToolMenuSection& Section=menu->AddSection("rdInstSec",LOCTEXT("rdInst_LevelMenu_Label","rdInst"));
	rdAddMenuItemT("Randomize Settings...","Change the Randomize Settings used for the selected actors (for when used with the Container Blueprint)","rdInst.RandomSettings",&FrdInstEditorModule::ChangeRandomActorSettings);
	rdAddMenuItemT("Set Rely On Actors","Set the 'Rely On Actor' to point to the previous actor in the selection","rdInst.SetRelyOnActors",&FrdInstEditorModule::ChangeRelyOnActorsForSelectedActors);
	rdAddMenuItemT("Remove Randomize Settings...","Remove the Randomize Settings set in the selected Actors","rdInst.RemoveRandomSettings",&FrdInstEditorModule::RemoveRandomSettingsForSelectedActors);
	rdAddMenuDivT();
	rdAddMenuItemT("Proxy Settings...","Set the Proxy options for use with the Create SpawnActor tool","rdInst.ProxySettings",&FrdInstEditorModule::ProxySettingsFromSelection);
	rdAddMenuDivT();
	rdAddMenuItemT("Move Prefab data to Arrays","Moves the Prefab build data from the Fast Tables to the customizable Instance Arrays","rdInst.ProxySettings",&FrdInstEditorModule::ConvertSelectedActorPrefabsToArrays);
	rdAddMenuItemT("Move Prefab data to Fast Tables","Moves the Prefab build data from the customizable Instance Arrays to the Fast Tables","rdInst.ProxySettings",&FrdInstEditorModule::ConvertSelectedActorPrefabsToTables);
	rdAddMenuDivT();
	rdAddMenuItemT("Create rdSpawnActor from Selection...","Creates a new rdSpawnActor from the selection","rdInst.RandomSettings",&FrdInstEditorModule::CreateSpawnActorFromSelection);
	rdAddMenuItemT("Update rdSpawnActor from Selection...","Updates an existing rdSpawnActor from the selection","rdInst.RandomSettings",&FrdInstEditorModule::UpdateSpawnActorFromSelection);
	rdAddMenuItemT("Copy rdSpawnActor/s to Level...","Copies the selected rdSpawnActors to the level, optionally replacing","rdInst.ProxySettings",&FrdInstEditorModule::CopySpawnActorsToLevel);

}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
