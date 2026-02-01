// rdInstPCG.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once
#include "Modules/ModuleManager.h"

#define RDINST_PLUGIN_API DLLEXPORT

class FrdInstPCGModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
