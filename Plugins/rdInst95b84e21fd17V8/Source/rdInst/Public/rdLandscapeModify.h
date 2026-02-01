// rdLandscapeModify.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
//#include "rdLandscapeModify.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

// Modes of rendering to the Landscape heightmap
UENUM()
enum rdLandscapeShape {
	RDLANDSCAPESHAPE_RECTANGLE	UMETA(DisplayName="Rectangle"),		
	RDLANDSCAPESHAPE_CIRCLE		UMETA(DisplayName="Circle"),	
	RDLANDSCAPESHAPE_SPLINE		UMETA(DisplayName="Spline")
};
