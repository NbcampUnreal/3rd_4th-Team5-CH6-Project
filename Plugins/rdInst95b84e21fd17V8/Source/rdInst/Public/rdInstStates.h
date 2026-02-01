// rdInstStates.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "Engine/DataAsset.h"
#include "rdInstStates.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

UCLASS(BlueprintType)
class UrdStateDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()
public:

	// When set to false, the swap between proxies and actors does not invoke the save and load data functions
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdInstStateData)
	bool bUseInvoke=true;

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("rdInstStateAssets",GetFName()); }
};

