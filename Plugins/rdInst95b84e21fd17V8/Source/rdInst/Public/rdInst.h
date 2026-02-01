// rdInst.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once
#include "Modules/ModuleManager.h"
#include "rdDebugHUD.h"

#define RDINST_MAJOR_VERSION 1
#define RDINST_MINOR_VERSION 51
#define RDINST_RELEASEDATE "30th June 2025"

#define rdLog(m) UE_LOG(LogTemp,Display,TEXT(m))
#define rdLog1(m,v) UE_LOG(LogTemp,Display,TEXT(m),v)
#define rdLog2(m,v1,v2) UE_LOG(LogTemp,Display,TEXT(m),v1,v2)
#define rdLog3(m,v1,v2,v3) UE_LOG(LogTemp,Display,TEXT(m),v1,v2,v3)

#define RDINST_PLUGIN_API DLLEXPORT

class RDINST_PLUGIN_API FrdInstModule : public IModuleInterface {
public:
	virtual void		StartupModule() override;
	virtual void		ShutdownModule() override;

#if WITH_EDITOR
	static void			rdTick(float deltaSeconds);
#endif

#if !UE_BUILD_SHIPPING
	static void			SetDebugHUDType(int32 hudLevel);
	static void			RemoveDebugHUD();
	static void			UpdateDebugHUD();
	static void			LogInstanceStats();
	static void			LogInstanceStatsVerbose();

	static int32		lastDebugLevel;
	static float		debugScanLapse;
	static int32		debugScanSize;
	static TObjectPtr<UrdDebugHUD> debugHUD;
#endif
};
