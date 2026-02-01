// rdProceduralActor.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "rdSpawnActor.h"
#include "rdProceduralActor.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

// The AActor class used for Procedural Population
UCLASS()
class RDINST_PLUGIN_API ArdProceduralActor : public ArdSpawnActor {

	GENERATED_BODY()
public:

	ArdProceduralActor();
	ArdProceduralActor(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION<5
	virtual void PreSave(const class ITargetPlatform* TargetPlatform);
#else
	virtual void PreSave(FObjectPreSaveContext context);
#endif
#endif

	// Spawns the Objects described in the list of items
	virtual void rdSpawn() override;

	// Bakes the procedural items into lists ready to be spawned very quickly
	virtual void rdBakeSpawnData(const FrdSpawnData& bakeData) override;

	// Bakes the spawn items into lists ready to be spawned very quickly
	virtual void rdBakeSpawnedData() override;

	// Clears the Baked Data
	virtual void rdClearBakedData() override;
};
