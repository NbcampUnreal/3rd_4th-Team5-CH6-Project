// rdInstBPLibrary.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/AssetUserData.h"
#include "rdInstances.h"

#define RDINST_PLUGIN_API DLLEXPORT

class UrdRandomizeAssetUserData;
class UrdTransformAssetUserData;

// These static methods returns and/or creates UserAssetData contained in the meshes or Components
//  These Structs contain the Randomization and Transform information used Randomize the rdInst Actors.
//
// The information in these Structs is also stored in the objects as Tags. This allows fallback settings
// to be created if that is all that is found. 
//
// The Blueprint Only Blueprints for Randomization are purely based on Tags, so this allows both sets of
// Blueprints to work with each other.
//
class RDINST_PLUGIN_API rdInstLib {
public:
	static UrdRandomizeAssetUserData* GetRandomizeUserDataFromTags(TArray<FName>& tags);
	static UrdRandomizeAssetUserData* GetRandomizeUserData(AActor* actor,bool make);
	static UrdRandomizeAssetUserData* GetRandomizeUserData(UActorComponent* actor,bool make);
	static UrdTransformAssetUserData* GetTransformUserData(UActorComponent* actor,bool make);

	static void RemoveRandomizeUserData(AActor* actor);
	static void RemoveRandomizeUserData(UActorComponent* actor);
	static void RemoveTransformUserData(UActorComponent* actor);
};

