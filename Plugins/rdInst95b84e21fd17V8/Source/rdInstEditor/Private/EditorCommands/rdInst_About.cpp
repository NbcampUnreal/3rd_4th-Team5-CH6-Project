//
// rdInst_About.cpp
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 18th August 2024
// Last Modified: 18th August 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "Interfaces/IMainFrameModule.h"

//----------------------------------------------------------------------------------------------------------------
// ShowAboutWindow
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ShowAboutWindow() {

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp,Display,TEXT("Opening rdAbout Window..."));
#endif

	ShowAboutSettings();

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp,Display,TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
