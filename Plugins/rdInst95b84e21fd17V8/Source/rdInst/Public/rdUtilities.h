// rdUtilities.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "rdUtilities.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT


// Modes that the rdActor can have:
UENUM()
enum rdScalabilityScale {
	// Low
	RDSCALE_NONE=-1		UMETA(DisplayName="None"),	
	// Low
	RDSCALE_LOW=0		UMETA(DisplayName="Low"),	
	// Medium
	RDSCALE_MEDIUM		UMETA(DisplayName="Medium"),	
	// High
	RDSCALE_HIGH		UMETA(DisplayName="High"),	
	// Epic
	RDSCALE_EPIC		UMETA(DisplayName="Epic"),	
	// Cinematic
	RDSCALE_CINEMATIC	UMETA(DisplayName="Cinematic")
};

// Modes that the rdActor can have:
UENUM()
enum rdScalabilityType {
	// ViewDistanceQuality
	RDSCALETYPE_VIEWDISTANCE	UMETA(DisplayName="ViewDistanceQuality"),	
	// AntiAliasingQuality
	RDSCALETYPE_AA				UMETA(DisplayName="AntiAliasingQuality"), 
	// ShadowQuality
	RDSCALETYPE_SHADOW			UMETA(DisplayName="ShadowQuality"),		
	// GlobalIlluminationQuality
	RDSCALETYPE_GI				UMETA(DisplayName="GlobalIlluminationQuality"),		
	// ReflectionQuality
	RDSCALETYPE_REFLECTION		UMETA(DisplayName="ReflectionQuality"),		
	// PostProcessQuality
	RDSCALETYPE_POSTPROCESS		UMETA(DisplayName="PostProcessQuality"),		
	// TextureQuality
	RDSCALETYPE_TEXTURE			UMETA(DisplayName="TextureQuality"),		
	// EffectsQuality
	RDSCALETYPE_EFFECTS			UMETA(DisplayName="EffectsQuality"),		
	// FoliageQuality
	RDSCALETYPE_FOLIAGE			UMETA(DisplayName="FoliageQuality"),		
	// ShadingQuality
	RDSCALETYPE_SHADING			UMETA(DisplayName="ShadingQuality"),		
};
