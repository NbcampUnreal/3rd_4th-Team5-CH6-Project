//
// rdActor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 1st October 2022
// Last Modified: 30th June 2025
//
// This is the Class that you SubClass from to get the ISM and Randomization functionality.
//
// The code in this file should be UE version agnostic until a time they remove HISMCs (which would make this somewhat redundant anyway).
// If you're here to port to a newer version you'll probably not need to do anything here unless a new way of creating child components
// is introduced, then it would simply be adding it to the rdSpawnActor() method.
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

//.............................................................................
//  ArdActor Constructor
//.............................................................................
ArdActor::ArdActor() :	
	bStartHidden(false),bCanBeAssimilated(true),actorMode(RDACTORMODE_RANDOMIZE),
	bDestroyAttachedActors(true),bRecurseVisibilityChanges(true),bStaticMeshMode(false),bEditInstances(false),
	bTickHookOneShot(true),bCreateFromArrays(false),bScanForProxy(false),proxyInstanceIndex(-1),proxyInstanceComponent(nullptr),instanceVault(nullptr),
	locMultiplier(1,1,1),rotMultiplier(1,1,1),scaleMultiplier(1,1,1),showMultiplier(1),
	bRandomStart(false),randomSeed(0),bOverrideRandom(false),bOverrideTheme(false),rdBaseActor(nullptr)
{
	SetActorTickEnabled(false);
}

//.............................................................................
//  ArdActor Constructor with ObjectInitializer
//.............................................................................
ArdActor::ArdActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	bStartHidden(false),bCanBeAssimilated(true),actorMode(RDACTORMODE_RANDOMIZE),
	bDestroyAttachedActors(true),bRecurseVisibilityChanges(true),bStaticMeshMode(false),bEditInstances(false),
	bTickHookOneShot(true),bCreateFromArrays(false),bScanForProxy(false),proxyInstanceIndex(-1),proxyInstanceComponent(nullptr),instanceVault(nullptr),
	locMultiplier(1,1,1),rotMultiplier(1,1,1),scaleMultiplier(1,1,1),showMultiplier(1),
	bRandomStart(false),randomSeed(0),bOverrideRandom(false),bOverrideTheme(false),rdBaseActor(nullptr) 
{
	SetActorTickEnabled(false);

}

//.............................................................................
//  ArdActor Destructor
//.............................................................................
ArdActor::~ArdActor() {
#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("ArdActor::~ArdActor()"));
#endif
}

#if WITH_EDITOR
//.............................................................................
//  PostEditMove
//.............................................................................
void ArdActor::PostEditMove(bool bFinished) {
/*
	if(InstanceDataX.Num()) {
		if(!rdBaseActor && GetWorld()) {
			for(TActorIterator<AActor> it(GetWorld());it;++it) {
				rdBaseActor=Cast<ArdInstBaseActor>(*it);
				if(rdBaseActor) break;
			}
		}
		if(rdBaseActor && IsValid(rdBaseActor)) {
			rdBaseActor->rdRemInstancesForOwner(this);
		}
	}
	
	ArdActor::rdGetDirtyList().Remove(this);
*/
	Super::PostEditMove(bFinished);
}

//.............................................................................
//  AddSidToReferencedAssets
//.............................................................................
void ArdActor::AddSidToReferencedAssets(const FName sid) {

	if(!rdGetBaseActor()) return;

	ErdSpawnType type=rdBaseActor->rdGetSpawnTypeFromSid(sid);

	switch(type) {
		case ErdSpawnType::Actor: { // Actor Classes
			TSoftClassPtr<UObject> sc=rdBaseActor->rdGetActorClassFromSid(sid);
			rdBaseActor->LevelReferencedActorClasses.AddUnique(sc);
			break; }
		case ErdSpawnType::DataLayer: { // DataLayers
			TSoftClassPtr<UDataLayerAsset> dl=rdBaseActor->rdGetDataLayerFromSid(sid);
			rdBaseActor->LevelReferencedDataLayers.AddUnique(dl);
			break; }
		case ErdSpawnType::VFX: { // VFX
			//TODO:
			//	TArray<TSoftClassPtr<UNiagaraSystem>>		
			//rdBaseActor->LevelReferencedVFX.AddUnique(vfx)
			break; }
		default: { // StaticMeshes
			TSoftObjectPtr<UStaticMesh> softMesh=rdBaseActor->rdGetSoftMeshFromSid(sid);
			rdBaseActor->LevelReferencedMeshes.AddUnique(softMesh);

			TArray<TSoftObjectPtr<UMaterialInterface>> softMats=rdBaseActor->rdGetMaterialsFromSid(sid);
			for(auto& m:softMats) {
				if(!m.IsNull()) rdBaseActor->LevelReferencedMaterials.AddUnique(m);
			}
			break; }
	}
}

//.............................................................................
//  PreSave
//.............................................................................
#if ENGINE_MAJOR_VERSION<5
void ArdActor::PreSave(const class ITargetPlatform* context) {
#else
void ArdActor::PreSave(FObjectPreSaveContext context) {
#endif

	if(!rdGetBaseActor()) return;

	for(auto& it:InstanceDataX) {
		AddSidToReferencedAssets(it.Key);
	}

	for(FrdAddInstanceFastArray& i:InstanceFastArray) {
		AddSidToReferencedAssets(i.meshSetup.sid);
	}

	for(FrdAddInstanceRandomizedArray& i:InstanceRandomizedArray) {
		AddSidToReferencedAssets(i.meshSetup.sid);
	}

	Super::PreSave(context);
}
#endif

//.............................................................................
//  OnConstruction (Blueprint construction)
//.............................................................................
void ArdActor::OnConstruction(const FTransform &Transform) {

	Super::OnConstruction(Transform);

	if(!rdGetBaseActor()) return;

	if(InstanceData.Num()>0) {
		InstanceDataX.Empty();
		for(auto& it:InstanceData) {
			FName sid=rdGetSMsid(it.Key);
			FrdInstanceSettingsArray& isa=it.Value;
			InstanceDataX.Add(sid,isa);
		}
	}

	// When dragging an rdActor into the level from the content browser, it's hit detection routines
// finds the rdInstBaseActor (owner of the IMSCs) and messes up the positioning of the dragged actor.
#if WITH_EDITOR
	if(bIsEditorPreviewActor) {
		return;
	}

	if(bHiddenEd) {
		return;
	}

	if(bEditInstances) {
		HidePrefabOutline();
	}

#endif
	splineInstanceData.Empty();

	if(bRandomStart) {
		randomStream.GenerateNewSeed(); //FDateTime::Now().GetTicks();
	} else {
		randomStream.Initialize(randomSeed);
	}

	rdBaseActor=nullptr;
	//AActor::OnConstruction(Transform);

	rdDirty();
}

//.............................................................................
//  BeginPlay
//.............................................................................
void ArdActor::BeginPlay() {

	splineInstanceData.Empty();

	if(bRandomStart) {
		randomStream.GenerateNewSeed();//randomSeed=FDateTime::Now().GetTicks();
	} else {
		randomStream.Initialize(randomSeed);
	}

	//rdRecreateBPInstances();
	rdDirty();

	Super::BeginPlay();
}

//.............................................................................
//  EndPlay
//.............................................................................
void ArdActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {

	splineInstanceData.Empty();
	rdRemoveInstances();
	Super::EndPlay(EndPlayReason);
}

//.............................................................................
//  BeginDestroy
//.............................................................................
void ArdActor::BeginDestroy() {

	if(InstanceData.Num()) {// && GetWorld()) {
		if(!rdBaseActor && GetWorld()) {
			for(TActorIterator<AActor> it(GetWorld());it;++it) {
				rdBaseActor=Cast<ArdInstBaseActor>(*it);
				if(rdBaseActor) break;
			}
		}
		if(rdBaseActor && IsValid(rdBaseActor)) {
			rdBaseActor->rdRemInstancesForOwner(this);
		}
	}
	
	ArdActor::rdGetDirtyList().Remove(this);
	ArdActor::rdGetTickHookList().Remove(this);

	Super::BeginDestroy();
}

// For Editor Events
void ArdActor::Destroyed() {

	rdDestroyed();

	if(bDestroyAttachedActors) {

		TArray<AActor*> actors;
#if ENGINE_MAJOR_VERSION>4
		GetAttachedActors(actors,true,false);
#else
		GetAttachedActors(actors,true);
#endif

		for(auto a:actors) {
			if(!a->ActorHasTag(TEXT("rdSticky"))) {
				a->Destroy();
			}
		}
	}

	ArdActor::rdGetDirtyList().Remove(this);
	ArdActor::rdGetTickHookList().Remove(this);
	Super::Destroyed();
}

//.............................................................................
//  Serialize
//.............................................................................
void ArdActor::Serialize(FArchive& Ar) {

	Super::Serialize(Ar);
}

//.............................................................................
// SetIsTemporarilyHiddenInEditor
//.............................................................................
#if WITH_EDITOR
void ArdActor::SetIsTemporarilyHiddenInEditor(bool bIsHidden) {

	Super::SetIsTemporarilyHiddenInEditor(bIsHidden);

	if(bIsEditorPreviewActor) {
		return;
	}

	if(bIsHidden) {
		rdRemoveInstances();
	} else {
		rdBuild();
	}
}

//.............................................................................
// HidePrefabOutline
//.............................................................................
void ArdActor::HidePrefabOutline() {

	UE_LOG(LogTemp,Display,TEXT("HidePrefabOutline"));
	if(!rdGetBaseActor()) return;

	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdBaseActor->FindISMCforMesh(sid);
		if(ismc && IsValid(ismc)) {
			int32 num=ismc->GetInstanceCount();
			FrdInstanceSettingsArray& i=it.Value;
			for(FrdInstanceFastSettings& j:i.settingsFast) {
				if(j.index>=0 && j.index<num) ismc->SelectInstance(false,j.index);
			}
			for(FrdInstanceRandomSettings& j:i.settingsRandom) {
				if(j.index>=0 && j.index<num) ismc->SelectInstance(false,j.index);
			}
		}
	}
}

//.............................................................................
// ShowPrefabOutline
//.............................................................................
void ArdActor::ShowPrefabOutline() {

	UE_LOG(LogTemp,Display,TEXT("ShowPrefabOutline"));
	if(!rdGetBaseActor()) return;

	for(auto& it:InstanceDataX) {

		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdBaseActor->FindISMCforMesh(sid);
		if(ismc && IsValid(ismc)) {
			int32 num=ismc->GetInstanceCount();
			FrdInstanceSettingsArray& i=it.Value;
			for(FrdInstanceFastSettings& j:i.settingsFast) {
				if(j.index>=0 && j.index<num) ismc->SelectInstance(true,j.index);
			}
			for(FrdInstanceRandomSettings& j:i.settingsRandom) {
				if(j.index>=0 && j.index<num) ismc->SelectInstance(true,j.index);
			}
		}
	}
}

#endif
//.............................................................................
