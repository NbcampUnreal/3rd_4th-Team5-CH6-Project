//
// rdInst_OutlinerMenu.cpp
//
// Copyright Recourse Design ltd 2024, Inc. All Rights Reserved.
//
// Creation Date: 3rd September 2024
// Last Modified: 15th September 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "Engine/Selection.h"
#include "rdInstOptions.h"
#include "rdUMGHelpers.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//----------------------------------------------------------------------------------------------------------------
// RandomSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::RandomSubMenu(UToolMenu* menu) {

	//FToolMenuSection& Section=menu->AddSection("rdInstRandSec",LOCTEXT("rdInst_RandomMenu_Label","Randomization"));
	//rdAddMenuItemT("Randomize Settings...","Change the Randomize Settings used for the selected actors (for when used with the Container Blueprint)","rdInst.RandomSettings",&FrdInstModule::ChangeRandomActorSettings);
	//rdAddMenuItemT("Set Rely On Actors","Set the 'Rely On Actor' to point to the previous actor in the selection","rdInst.SetRelyOnActors",&FrdInstModule::ChangeRelyOnActorsForSelectedActors);
	//rdAddMenuItemT("Remove Randomize Settings...","Remove the Randomize Settings set in the selected Actors","rdInst.RemoveRandomSettings",&FrdInstModule::RemoveRandomSettingsForSelectedActors);
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE

