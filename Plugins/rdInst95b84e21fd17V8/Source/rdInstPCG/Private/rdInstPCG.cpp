//
// rdInstPCG.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 1st October 2022
// Last Modified: 1th May 2025
//
// This handles the Startup and Shutdown of the rdInstPCG Module.
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//

#include "rdInstPCG.h"

#define LOCTEXT_NAMESPACE "FrdInstPCGModule"

//.............................................................................
//  StartupModule
//.............................................................................
void FrdInstPCGModule::StartupModule() {
}

//.............................................................................
//  ShutdownModule
//.............................................................................
void FrdInstPCGModule::ShutdownModule() {
}

//.............................................................................
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FrdInstPCGModule, rdInstPCG)