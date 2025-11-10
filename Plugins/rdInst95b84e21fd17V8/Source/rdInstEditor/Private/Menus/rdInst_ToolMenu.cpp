//
// rdInst_ToolMenu.cpp
//
// Copyright Recourse Design ltd 2024, Inc. All Rights Reserved.
//
// Creation Date: 18th August 2024
// Last Modified: 27th October 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "Engine/Selection.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "rdInstOptions.h"
#include "rdUMGHelpers.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//----------------------------------------------------------------------------------------------------------------
// ToolMenuSubMenu
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ToolMenuSubMenu(UToolMenu* menu) {

	FToolMenuSection& Section=menu->AddSection("rdInstSec",LOCTEXT("rdInst_ToolMenu_Label1","rdInst"));

	rdAddMenuItemT("Settings...","","rdInst.Settings",&FrdInstEditorModule::ShowMainSettings);
	rdAddMenuItemT("About...","","rdInst.About",&FrdInstEditorModule::ShowAboutWindow);
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
