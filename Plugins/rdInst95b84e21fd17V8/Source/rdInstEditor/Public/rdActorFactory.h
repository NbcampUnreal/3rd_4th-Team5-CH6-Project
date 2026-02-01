// rdActorFactory.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once
#include "ActorFactories/ActorFactory.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "rdActorFactory.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

// These classes are used so you can drag DataAssets into the level, and they spawn either a SpawnActor or a ProceduralActor.

UCLASS()
class RDINST_PLUGIN_API UrdSpawnActorFactory : public UActorFactory {
	GENERATED_BODY()
public:
	virtual bool CanCreateActorFrom(const FAssetData& a, FText& msg) override;
#if ENGINE_MAJOR_VERSION>4
	virtual UClass* GetDefaultActorClass(const FAssetData& a) override;
	virtual bool CanPlaceElementsFromAssetData(const FAssetData& a) override;
#endif
};

UCLASS()
class RDINST_PLUGIN_API UrdProceduralActorFactory : public UActorFactory {
	GENERATED_BODY()
public:
	virtual bool CanCreateActorFrom(const FAssetData& a, FText& msg) override;
#if ENGINE_MAJOR_VERSION>4
	virtual UClass* GetDefaultActorClass(const FAssetData& a) override;
	virtual bool CanPlaceElementsFromAssetData(const FAssetData& a) override;
#endif
};

UCLASS()
class RDINST_PLUGIN_API UrdMultiProceduralActorFactory : public UActorFactory {
	GENERATED_BODY()
public:
	virtual bool CanCreateActorFrom(const FAssetData& a, FText& msg) override;
#if ENGINE_MAJOR_VERSION>4
	virtual UClass* GetDefaultActorClass(const FAssetData& a) override;
	virtual bool CanPlaceElementsFromAssetData(const FAssetData& a) override;
#endif
};
