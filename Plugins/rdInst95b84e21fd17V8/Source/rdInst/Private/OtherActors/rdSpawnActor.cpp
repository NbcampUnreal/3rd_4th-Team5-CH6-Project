//
// rdSpawnActor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 8th February 2024 (moved from rdInstBaseActor.cpp)
// Last Modified: 30th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdSpawnActor.h"
#include "rdInstBaseActor.h"
#include "Landscape.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"
#include "Async/Async.h"
#if ENGINE_MAJOR_VERSION>4
#include "LandscapeStreamingProxy.h"
#endif
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#else
#if ENGINE_MAJOR_VERSION>4
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"
#endif
#endif
#include "NavigationSystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

//.............................................................................
// Constructor
//.............................................................................
ArdSpawnActor::ArdSpawnActor() {

	popRootComponent=CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(popRootComponent);
	volumeBox=CreateDefaultSubobject<UBoxComponent>(FName("VolumeBox"));
	volumeBox->AttachToComponent(popRootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	volumeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

//.............................................................................
// Constructor with ObjectInitializer
//.............................................................................
ArdSpawnActor::ArdSpawnActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	popRootComponent=CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(popRootComponent);
	volumeBox=CreateDefaultSubobject<UBoxComponent>(FName("VolumeBox"));
	volumeBox->AttachToComponent(popRootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	volumeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

//.............................................................................
// ConstructionScript
//.............................................................................
void ArdSpawnActor::OnConstruction(const FTransform &Transform) {

	Super::OnConstruction(Transform);

	if(!rdGetBaseActor()) return;

	actorMode=RDACTORMODE_NONE;
	if(spawnData.Num()==0) spawnData.Add(FrdSpawnData());
	if(currentSpawnDataIndex>=spawnData.Num()) currentSpawnDataIndex=spawnData.Num()-1;

#if WITH_EDITOR
	// When a BakedSpawnData DataAsset is dragged into the level
	if(rdBaseActor->lastDraggedSpawnActorDataAsset) {
		existingDataAsset=Cast<UrdBakedDataAsset>(rdBaseActor->lastDraggedSpawnActorDataAsset->GetAsset());
		rdBaseActor->lastDraggedSpawnActorDataAsset=nullptr;
	}

	// When a single or multi Placement DataAsset is dragged into the level
	if(rdBaseActor->lastDraggedProceduralActorDataAsset) {
		dragInActorsAndDataAssets.Add(Cast<UObject>(rdBaseActor->lastDraggedProceduralActorDataAsset->GetAsset()));
		rdBaseActor->lastDraggedProceduralActorDataAsset=nullptr;
	}
	
	if(existingDataAsset) {
		SetBakedDataAsset(existingDataAsset);
		existingDataAsset=nullptr;
	}
#endif

	FBoxSphereBounds& bnds=spawnData[currentSpawnDataIndex].bounds;

	if(!landscape) {
		ALandscape* ls=(ALandscape*)UGameplayStatics::GetActorOfClass(GetWorld(),ALandscape::StaticClass());
		if(ls) {
			landscape=ls->GetLandscapeActor();
		}
	}

#if WITH_EDITOR
	if(landscape) {

		// set landscape scale and offset for splatmaps that cover the entire landscape
		// is only set when in the editor, it doesn't seem possible to get the landscape bounds at runtime

		FVector aLoc=GetActorLocation();
		FVector cLoc=volumeBox->GetComponentLocation();
		FVector loc=cLoc+aLoc;
		FVector sz=volumeBox->GetScaledBoxExtent()*2.0f;
		FBox lbx=rdBaseActor->rdGetLandscapeBounds(landscape);
		FVector lsLoc=landscape->GetActorLocation();
		float lsW=lbx.Max.X-lbx.Min.X;
		float lsH=lbx.Max.Y-lbx.Min.Y;
		float saW=sz.X;
		float saH=sz.Y;
		float offX=aLoc.X-lsLoc.X;
		float offY=aLoc.Y-lsLoc.Y;

		landscapeScale=FVector4(offX/lsW,offY/lsH,saW/lsW,saH/lsH);
	}

	for(auto m:dragInMeshes) {
		if(m) {
			FrdObjectPlacementData pd;
			pd.mesh.mesh=m;
			pd.sid=rdBaseActor->rdGetSMsid(m);
			pd.ismc=rdBaseActor->rdGetInstanceGenX(pd.sid);
			spawnData[currentSpawnDataIndex].items.Add(pd);
		}
	}

	for(auto c:dragInActorsAndDataAssets) {
		if(c) {
			UrdPlacementDataAsset* pda=Cast<UrdPlacementDataAsset>(c);
			UrdMultiPlacementDataAsset* mpda=Cast<UrdMultiPlacementDataAsset>(c);
			if(mpda) { 
				for(auto mp:mpda->PlacementDataAssets) {
					mp.LoadSynchronous();
					for(auto p:mp.Get()->Placements) {
						FrdObjectPlacementData pd(p);
						pd.MultiPlacementDataAsset=mpda->GetPathName();
						spawnData[currentSpawnDataIndex].items.Add(pd);
					}
				}
			} else if(pda) {
				for(auto p:pda->Placements) {
					spawnData[currentSpawnDataIndex].items.Add(p);
				}
			} else {
				FrdObjectPlacementData pd;
				pd.actorClass=c->GetClass();
				spawnData[currentSpawnDataIndex].items.Add(pd);
			}
		}
	}

	for(auto v:dragInVFX) {
		if(v) {
			FrdObjectPlacementData pd;
			pd.vfx=v;
			spawnData[currentSpawnDataIndex].items.Add(pd);
		}
	}

	dragInMeshes.Empty();
	dragInActorsAndDataAssets.Empty();
	dragInVFX.Empty();

	if(editorLoadState!=lastEditorLoadState) {

		if(editorLoadState==rdLoadState::RDLOADSTATE_UNLOADED) {
			rdUnloadAllAssets();
			rdUnloadData();
		}
		lastEditorLoadState=editorLoadState;
	}

	if(editorLoadState!=rdLoadState::RDLOADSTATE_UNLOADED && bDrawInEditor && !bCleared && !rdBaseActor->rdIsPlaying() ) {
		rdDirty();
	}
#endif
}

#if WITH_EDITOR
//.............................................................................
//  PreSave
//.............................................................................
#if ENGINE_MAJOR_VERSION<5
void ArdSpawnActor::PreSave(const class ITargetPlatform* context) {
#else
void ArdSpawnActor::PreSave(FObjectPreSaveContext context) {
#endif

	if(!rdGetBaseActor()) return;

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	UrdBakedDataAsset* bda=GetBakedDataAsset();
	if(bda) {

		TArray<FrdBakedSpawnObjects>& bd=bda->bakedData;
		for(auto& dat:bd) {
			AddSidToReferencedAssets(dat.sid);
			rdBaseActor->AddProxyToReferencedAssets(dat.proxy);
		}

	} else {

		for(auto& o:spawnDat.items) {
			if(o.mesh.mesh.Get()) {
				AddSidToReferencedAssets(o.sid);
				rdBaseActor->AddProxyToReferencedAssets(o.proxy);
			}
		}
	}

	ArdActor::PreSave(context);
}
#endif

//.............................................................................
// GetBakedDataAsset
//.............................................................................
UrdBakedDataAsset* ArdSpawnActor::GetBakedDataAsset(bool bLoad) {

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	if(spawnDat.bakedData) {
		return spawnDat.bakedData;
	}

	UrdBakedDataAsset* bda=spawnDat.bakedDataAsset.Get();

	if(!bda && bLoad && rdGetBaseActor()) {
		bda=rdBaseActor->rdLoadBakedDataAsset(spawnDat.bakedDataAsset);
	}
	
	if(bSharedDataAsset) {

		spawnDat.bakedData=bda;

	} else {

		spawnDat.bakedData=DuplicateObject(bda,nullptr);

	}

	return bda;
}

//.............................................................................
// SetBakedDataAsset
//.............................................................................
void ArdSpawnActor::SetBakedDataAsset(UrdBakedDataAsset* dataAsset) {

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	spawnDat.bakedDataAsset=dataAsset;

	spawnDat.bakedData=nullptr;
	GetBakedDataAsset();

	TArray<FrdBakedSpawnObjects>& bakedData=spawnDat.bakedData->bakedData;

	spawnDat.items.SetNum(bakedData.Num());

	// Set the SpawnActors internal usage data
	for(FrdBakedSpawnObjects& bso:bakedData) {
		FrdObjectPlacementData& pd=spawnDat.items[bso.itemIndex];
		pd.actorClass=bso.actorClass;
		pd.strProps=bso.strProps;
		pd.vfx=bso.vfx;
		pd.pooled=bso.pooled;
		pd.sid=bso.sid;
	}

#if WITH_EDITOR
	volumeBox->SetBoxExtent(dataAsset->bounds.Origin);
	volumeBox->SetRelativeLocation(dataAsset->bounds.Origin);
#endif

}

//.............................................................................
// BeginPlay
//.............................................................................
void ArdSpawnActor::BeginPlay() {

	Super::BeginPlay();

	if(!rdGetBaseActor()) return;

	currentSpawnDataIndex=rdBaseActor->rdGetCurrentScalability();
	if(currentSpawnDataIndex>spawnData.Num()-1) currentSpawnDataIndex=spawnData.Num()-1;
	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	// Clear Baked Instance Indices and precalc the scan settings
	scanType=0;
	if(bHasMassMovement) {
		scanType|=8;
	}
	if(bHasMassPhysics) {
		scanType|=64;
	}
	for(FrdSpawnData& bd:spawnData) {
		UrdBakedDataAsset* bda=GetBakedDataAsset();
		if(bda) {

			TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;
			for(FrdBakedSpawnObjects& bso:baked) {

				if(bso.streamDistance>0) {
					bso.loadState=rdLoadState::RDLOADSTATE_UNLOADED;
				}

				if(bso.proxy.proxyType!=rdProxyType::RDPROXYTYPE_NONE) {
					scanType|=1;
				}

				if(bso.spawnMode==rdSpawnMode::RDSPAWNMODE_SDP || bso.spawnMode==rdSpawnMode::RDSPAWNMODE_FDP) {
					scanType|=32;
				}

				bso.transformMap.ForEach([this](FrdInstanceTransformItem& t) { t.index=-1; });
				bso.transformMap.Precalc();
			}
		}
	}

	rdClearSpawn();

	spawnActorCenter=rdGetSpawnerCenter();
	spawnActorRadius=rdGetSpawnerRadius();

	if(spawnDat.distance<=0.0f) {
		rdDirty();
	}
	lastLocation.Z=-500000.0f;
	rdBaseActor->rdRegisterSpawnActor(this);
}

//.............................................................................
//  rdBlockingLoad
//.............................................................................
void ArdSpawnActor::rdBlockingLoad() {

	if(!rdGetBaseActor()) return;

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	if(spawnDat.bakedDataAsset.ToString().Len()>0) {
		UrdBakedDataAsset* bda=GetBakedDataAsset();
		if(!bda) {
			return;
		}
	}

	loadState=rdLoadState::RDLOADSTATE_PARTIALLYLOADED;

	rdBlockingLoadAssets();
}

//.............................................................................
//  rdBlockingLoadAssets
//.............................................................................
void ArdSpawnActor::rdBlockingLoadAssets() {

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	UrdBakedDataAsset* da=GetBakedDataAsset();
	if(da) {

		TArray<FrdBakedSpawnObjects>& baked=da->bakedData;
		for(FrdBakedSpawnObjects& bd:baked) {

			UStaticMesh* mesh=rdGetMeshFromSid(bd.sid); // does the blocking load

		}
	} else {

		for(auto& pd:spawnDat.items) {

			FSoftObjectPath sop(pd.mesh.mesh.ToString());
			UStaticMesh* mesh=Cast<UStaticMesh>(sop.ResolveObject());
			if(!mesh) {
				mesh=Cast<UStaticMesh>(sop.TryLoad());
			}
		}
	}
	loadState=rdLoadState::RDLOADSTATE_FULLYLOADED;
}

//.............................................................................
//  rdAsyncLoad
//
// Queues the baked data asset to be loaded if it isn't, otherwise it
// queues all assets used in the SpawnActor to be loaded.
//
//.............................................................................
void ArdSpawnActor::rdAsyncLoad() {

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	if(spawnDat.bakedDataAsset.IsValid()) {
		TArray<FSoftObjectPath> items={spawnDat.bakedDataAsset.ToSoftObjectPath()};
		FStreamableManager& streamMan=UAssetManager::GetStreamableManager();
		streamMan.RequestAsyncLoad(items,FStreamableDelegate::CreateUObject(this,&ArdSpawnActor::rdAsyncLoadAllAssets));
	} else {
		// Not baked, just load the assets
		rdAsyncLoadAllAssets();
	}
}

//.............................................................................
//  rdAsyncLoadAllBakedAssets
//
// Used for baked SpawnActors, queues the load of assets within their stream 
//  distance
//
//.............................................................................
void ArdSpawnActor::rdAsyncLoadAllBakedAssets() {

	UrdBakedDataAsset* da=GetBakedDataAsset(false);
	if(!da) {
		return;
	}

	TArray<FSoftObjectPath> items;
	FStreamableManager& streamMan=UAssetManager::GetStreamableManager();

	TArray<FrdBakedSpawnObjects>& baked=da->bakedData;
	for(FrdBakedSpawnObjects& bd:baked) {

		if(bd.loadState!=rdLoadState::RDLOADSTATE_UNLOADED && bd.loadState!=rdLoadState::RDLOADSTATE_TOLOAD) continue;

		TSoftObjectPtr<UStaticMesh> mesh=rdBaseActor->rdGetSoftMeshFromSid(bd.sid);
		items.Add(mesh.ToSoftObjectPath());
		bd.loadState=rdLoadState::RDLOADSTATE_PARTIALLYLOADED;
	}

	if(items.Num()>0) {
		streamMan.RequestAsyncLoad(items,FStreamableDelegate::CreateUObject(this,&ArdSpawnActor::rdAsyncLoadAllBakedAssetsFinished));
	}
}

void ArdSpawnActor::rdAsyncLoadAllBakedAssetsFinished() { 

	UrdBakedDataAsset* da=GetBakedDataAsset();
	if(!da) {
		return;
	}
	TArray<FrdBakedSpawnObjects>& baked=da->bakedData;
	for(FrdBakedSpawnObjects& bd:baked) {
		if(bd.loadState!=rdLoadState::RDLOADSTATE_PARTIALLYLOADED) continue;
		bd.loadState=rdLoadState::RDLOADSTATE_FULLYLOADED;
	}
}

//.............................................................................
//  rdAsyncLoadMarkedBakedAssets
//
// Used for baked SpawnActors, queues the load of assets within their stream 
//  distance
//
//.............................................................................
void ArdSpawnActor::rdAsyncLoadMarkedBakedAssets() {

	UrdBakedDataAsset* da=GetBakedDataAsset();
	if(!da) {
		return;
	}

	TArray<FSoftObjectPath> items;

	TArray<FrdBakedSpawnObjects>& baked=da->bakedData;
	for(FrdBakedSpawnObjects& bd:baked) {

		if(bd.loadState!=rdLoadState::RDLOADSTATE_TOLOAD) continue;

		TSoftObjectPtr<UStaticMesh> mesh=rdBaseActor->rdGetSoftMeshFromSid(bd.sid);
		items.Add(mesh.ToSoftObjectPath());
		bd.loadState=rdLoadState::RDLOADSTATE_PARTIALLYLOADED;
	}

	if(items.Num()>0) {
		AsyncTask(ENamedThreads::GameThread,[this,titems=MoveTemp(items)] {
			FStreamableManager& streamMan=UAssetManager::GetStreamableManager();
			streamMan.RequestAsyncLoad(titems,FStreamableDelegate::CreateUObject(this,&ArdSpawnActor::rdAsyncLoadMarkedBakedAssetsFinished));
		});
	}
}

void ArdSpawnActor::rdAsyncLoadMarkedBakedAssetsFinished() { 

	if(!rdGetBaseActor()) {
		return;
	}

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	UrdBakedDataAsset* da=GetBakedDataAsset();
	if(!da) {
		return;
	}

	rdBaseActor->rdSpawnData(this,&spawnDat,false,true);

	TArray<FrdBakedSpawnObjects>& baked=da->bakedData;
	for(FrdBakedSpawnObjects& bd:baked) {
		if(bd.loadState!=rdLoadState::RDLOADSTATE_PARTIALLYLOADED) continue;
		bd.loadState=rdLoadState::RDLOADSTATE_FULLYLOADED;
	}
}

//.............................................................................
//  rdAsyncLoadAllAssets
//
// Used for Unbaked SpawnActors, just queues the load of all assets used.
//
//.............................................................................
void ArdSpawnActor::rdAsyncLoadAllAssets() {

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	TArray<FSoftObjectPath> items;

	UrdBakedDataAsset* da=GetBakedDataAsset();
	if(!da) {

		for(auto& pd:spawnDat.items) {
			
			if(pd.mesh.mesh.IsValid()) {
				items.Add(pd.mesh.mesh.ToSoftObjectPath());
			}
		}
	}

	if(items.Num()>0) {
		FStreamableManager& streamMan=UAssetManager::GetStreamableManager();
		TSharedPtr<FStreamableHandle> handle=streamMan.RequestAsyncLoad(items,FStreamableDelegate::CreateUObject(this,&ArdSpawnActor::rdAsyncLoadAllAssetsFinished));
		handle=nullptr;
	}
}

//.............................................................................
//  rdAsyncLoadAssetsFinished
//.............................................................................
void ArdSpawnActor::rdAsyncLoadAllAssetsFinished() {

	if(!rdGetBaseActor()) {
		return;
	}
	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	rdBaseActor->rdSpawnData(this,&spawnDat,false,false);
	loadState=rdLoadState::RDLOADSTATE_FULLYLOADED;
}

//.............................................................................
//  rdUnloadData
//.............................................................................
void ArdSpawnActor::rdUnloadData() {

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	if(spawnDat.bakedDataAsset.IsValid()) {
		FStreamableManager& streamMan=UAssetManager::GetStreamableManager();
		streamMan.Unload(spawnDat.bakedDataAsset.ToSoftObjectPath());
	}
	spawnDat.bakedDataAsset=nullptr;//TODO:
	spawnDat.bakedData=nullptr;

	loadState=rdLoadState::RDLOADSTATE_UNLOADED;
}

//.............................................................................
//  rdUnloadAllAssets
//.............................................................................
void ArdSpawnActor::rdUnloadAllAssets() {

	rdClearSpawn();

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	FStreamableManager& streamMan=UAssetManager::GetStreamableManager();
	UrdBakedDataAsset* da=GetBakedDataAsset(false);
	if(da) {

		for(auto& pd:spawnDat.items) {

			if(pd.mesh.mesh.IsValid()) {
				streamMan.Unload(pd.mesh.mesh.ToSoftObjectPath());
			}
		}
	}

	loadState=rdLoadState::RDLOADSTATE_PARTIALLYLOADED;
}

//.............................................................................
//  rdUnloadMarkedAssets
//.............................................................................
void ArdSpawnActor::rdUnloadMarkedAssets() {

	rdClearSpawn(true);

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	FStreamableManager& streamMan=UAssetManager::GetStreamableManager();
	UrdBakedDataAsset* da=GetBakedDataAsset(false);
	if(da) {

		TArray<FrdBakedSpawnObjects>& baked=da->bakedData;
		for(FrdBakedSpawnObjects& bd:baked) {
			if(bd.loadState!=rdLoadState::RDLOADSTATE_TOUNLOAD) continue;
			TSoftObjectPtr<UStaticMesh> mesh=rdGetSoftMeshFromSid(bd.sid);
			if(mesh) {
				streamMan.Unload(mesh.ToSoftObjectPath());
			}
			bd.loadState=rdLoadState::RDLOADSTATE_UNLOADED;
		}
	}

	loadState=rdLoadState::RDLOADSTATE_PARTIALLYLOADED;
}

//.............................................................................
//  rdFullyload
//.............................................................................
void ArdSpawnActor::rdFullyLoad() {

	rdBlockingLoad();
	rdSpawn();
}

//.............................................................................
//  rdFullyUnload
//.............................................................................
void ArdSpawnActor::rdFullyUnload() {

	rdUnloadAllAssets();
	rdUnloadData();
}

//.............................................................................
//  rdSaveDataAsset
//.............................................................................
bool ArdSpawnActor::rdSaveDataAsset() {

	return false;
}

//.............................................................................
//  rdSaveAsDataAsset
//.............................................................................
bool ArdSpawnActor::rdSaveAsDataAsset(const FString& name) {

	return false;
}

//.............................................................................
//  Serialize
//.............................................................................
void ArdSpawnActor::Serialize(FArchive& Ar) {

	Super::Serialize(Ar);
}

//.............................................................................
//  rdBuild
//.............................................................................
void ArdSpawnActor::rdBuild() {

	if(!rdGetBaseActor()) {
		return;
	}
	if(rdBaseActor->rdIsPlaying()) {
		if(spawnData[currentSpawnDataIndex].distance<=0.0f) {
			rdSpawn(); 
		}
#if WITH_EDITOR
	} else if(editorLoadState!=rdLoadState::RDLOADSTATE_UNLOADED && bDrawInEditor) {
#else
	} else {
#endif
		rdSpawn(); 
	}
}

//.............................................................................
//  EndPlay
//.............................................................................
void ArdSpawnActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {

	if(rdGetBaseActor()) {
		rdBaseActor->rdUnregisterSpawnActor(this);
	}
	Super::EndPlay(EndPlayReason);
}

//.............................................................................
//  BeginDestroy
//.............................................................................
void ArdSpawnActor::BeginDestroy() {

	if(!IsEngineExitRequested()) {
		if(rdGetBaseActor()) {
			rdBaseActor->rdUnregisterSpawnActor(this);
		}
		rdClearSpawn();
	}
	Super::BeginDestroy();
}

//.............................................................................
// For Editor Events
//.............................................................................
void ArdSpawnActor::Destroyed() {

	if(rdGetBaseActor()) {
		rdBaseActor->rdUnregisterSpawnActor(this);
	}
	rdClearSpawn();
	Super::Destroyed();
}

//.............................................................................
//  rdGetSpawnDistance
//.............................................................................
double ArdSpawnActor::rdGetSpawnDistance() {
	return spawnData[currentSpawnDataIndex].distance;
}

//.............................................................................
// rdClearAlteredList
//.............................................................................
void ArdSpawnActor::rdClearAlteredList() {
	alterations.Empty();
}

//.............................................................................
// rdBuildAlteredList
//.............................................................................
FString ArdSpawnActor::rdBuildAlteredList() {

	FString str;

	UrdBakedDataAsset* bda=GetBakedDataAsset(false);
	if(bda) {
		for(auto a:alterations) {
			str+=a+TEXT("~");
		}
	}

	return str;
}

//.............................................................................
// rdApplyAlteredList
//.............................................................................
int32 ArdSpawnActor::rdApplyAlteredList(const FString& str) {

	UrdBakedDataAsset* bda=GetBakedDataAsset(false);
	if(!bda) {
		return 0;
	}

	TArray<FString> alts;
	int32 num=str.ParseIntoArray(alts,TEXT("~"));
	for(auto a:alts) {
		alterations.Add(a);
	}

	return num;
}

//.............................................................................
// rdGetSpawnOrigin
//.............................................................................
FVector ArdSpawnActor::rdGetSpawnOrigin() const {

	FVector org=GetActorLocation();
	FVector scale=GetActorScale3D();
	
	org-=GetActorRotation().RotateVector(scale*30.0f);
	
	return org;
}

//.............................................................................
// rdGetSpawnOriginTransform
//.............................................................................
FTransform ArdSpawnActor::rdGetSpawnOriginTransform() const {

	FVector org=GetActorLocation();
	FVector scale=GetActorScale3D();
	
//	org-=(scale*0.5);
	org-=GetActorRotation().RotateVector(scale*30.0f);
	
//	FTransform t(GetActorRotation(),org,scale);
	FTransform t(FRotator(),org,scale);
	return t;
}

//.............................................................................
// rdSpawn
//.............................................................................
void ArdSpawnActor::rdSpawn() {

	if(!rdGetBaseActor()) return;
/*
	if(nmesh) {
		UNiagaraComponent* niagara=UNiagaraFunctionLibrary::SpawnSystemAttached(rdBaseActor->niagaraSpawner.LoadSynchronous(),popRootComponent,NAME_None,FVector(0.f),FRotator(0.f),EAttachLocation::Type::KeepRelativeOffset,false);
		UStaticMeshComponent* smc=(UStaticMeshComponent*)AddComponentByClass(UStaticMeshComponent::StaticClass(),false,FTransform(),false);
		if(smc) {
			AddInstanceComponent(smc);
			smc->SetMobility(EComponentMobility::Movable);
			smc->SetStaticMesh(nmesh);
		}
		UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(niagara,TEXT("rdMesh"),smc);

	}
*/
#if WITH_EDITOR
	if(editorLoadState==rdLoadState::RDLOADSTATE_UNLOADED) return;
	if(!bDrawInEditor && !rdBaseActor->rdIsPlaying()) return;
#endif

	if(hasSpawned) {
		rdClearSpawn();
	}

	rdUpdateFromDataAssets();

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	UrdBakedDataAsset* bda=GetBakedDataAsset();
	if(bda) {

		TArray<FrdBakedSpawnObjects>& bd=bda->bakedData;
		for(auto& dat:bd) {
			spawnDat.items[dat.itemIndex].sid=dat.sid;
			spawnDat.items[dat.itemIndex].ismc=rdBaseActor->rdGetInstanceGenX(dat.sid);
		}

	} else {

		for(auto& o:spawnDat.items) {
			if(o.mesh.mesh.Get()) {
				if(o.sid.IsNone()) {
					o.sid=rdBaseActor->rdGetInstSid(o.mesh);
				}
				o.ismc=rdBaseActor->rdGetInstanceGenX(o.sid);
			}
		}
	}

	bCleared=false;

	if(rdBaseActor->OnPreSpawnDelegate.IsBound()) {
		FEditorScriptExecutionGuard ScriptGuard;
		rdBaseActor->OnPreSpawnDelegate.Broadcast(this);
	}

	rdBaseActor->rdSpawn(this);
	hasSpawned=true;

	if(rdBaseActor->OnPostSpawnDelegate.IsBound()) {
		FEditorScriptExecutionGuard ScriptGuard;
		rdBaseActor->OnPostSpawnDelegate.Broadcast(this);
	}
}

//.............................................................................
// rdSpawnNewlyStreamed
//.............................................................................
void ArdSpawnActor::rdSpawnNewlyStreamed() {

	if(!rdGetBaseActor()) return;

	if(rdBaseActor->OnPreSpawnDelegate.IsBound()) {
		FEditorScriptExecutionGuard ScriptGuard;
		rdBaseActor->OnPreSpawnDelegate.Broadcast(this);
	}

	rdBaseActor->rdSpawn(this);

	if(rdBaseActor->OnPostSpawnDelegate.IsBound()) {
		FEditorScriptExecutionGuard ScriptGuard;
		rdBaseActor->OnPostSpawnDelegate.Broadcast(this);
	}
}

//.............................................................................
// rdMarkForSpawn
//.............................................................................
void ArdSpawnActor::ProcessStreamingObjects(float dist) {

	if(!rdGetBaseActor()) return;
	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	UrdBakedDataAsset* bda=spawnDat.bakedData;
	if(bda) {

		TArray<FrdBakedSpawnObjects>& bd=bda->bakedData;
		for(FrdBakedSpawnObjects& dat:bd) {
			float d=defaultStreamDistance;
			if(dat.streamDistance>=0) {
				d=dat.streamDistance;
			}
			if(dist<d) {
				if(dat.loadState==rdLoadState::RDLOADSTATE_UNLOADED) {
					rdMarkForStreamIn(dat);
				}
			} else {
				if(dat.loadState==rdLoadState::RDLOADSTATE_FULLYLOADED) {
					rdMarkForStreamOut(dat);
				}
			}
		}
	}
}

//.............................................................................
// rdMarkForStreamIn
//.............................................................................
void ArdSpawnActor::rdMarkForStreamIn(FrdBakedSpawnObjects& bso) {

	if(bso.loadState!=rdLoadState::RDLOADSTATE_UNLOADED) {
		return;
	}

	bso.loadState=rdLoadState::RDLOADSTATE_TOLOAD;
}

//.............................................................................
// rdMarkForStreamOut
//.............................................................................
void ArdSpawnActor::rdMarkForStreamOut(FrdBakedSpawnObjects& bso) {

	if(bso.loadState!=rdLoadState::RDLOADSTATE_FULLYLOADED && bso.loadState!=rdLoadState::RDLOADSTATE_TOLOAD) {
		return;
	}

	bso.loadState=rdLoadState::RDLOADSTATE_TOUNLOAD;
}

//.............................................................................
// rdSpawnEvent - implement this in Blueprint (rdSpawnEvent) when Subclassing
//.............................................................................
void ArdSpawnActor::rdSpawnEvent_Implementation() {

}

//.............................................................................
// rdBakeEvent - implement this in Blueprint (rdBakeEvent) when Subclassing
//.............................................................................
void ArdSpawnActor::rdBakeEvent_Implementation() {

}

//.............................................................................
// rdBakeSpawnedData
//.............................................................................
void ArdSpawnActor::rdBakeSpawnedData() {

	if(!rdGetBaseActor()) return;

	rdClearBakedData();

	int32 savedScalabilityLevel=currentSpawnDataIndex;
	int32 numSpawnDatum=spawnData.Num();

	for(int32 i=0;i<numSpawnDatum;i++) {
		currentSpawnDataIndex=i;
		rdBaseActor->rdSpawn(this,true);
	}
	
	currentSpawnDataIndex=savedScalabilityLevel;

	rdSpawn();
}

//.............................................................................
// rdAddSpawnedInstance
//.............................................................................
void ArdSpawnActor::rdAddSpawnedInstance(const FrdObjectPlacementData& opd,int32 instance) {

	TArray<int32>& il=ismIndexList.FindOrAdd(opd.sid);
	il.Add(instance);
}

//.............................................................................
// rdAddSpawnedActor
//.............................................................................
void ArdSpawnActor::rdAddSpawnedActor(const FrdObjectPlacementData& opd,AActor* actor) {

	((FrdObjectPlacementData&)opd).actorList.Add(actor);
}

//.............................................................................
// rdAddSpawnedVFX
//.............................................................................
void ArdSpawnActor::rdAddSpawnedVFX(const FrdObjectPlacementData& opd,UNiagaraComponent* vfx) {

	((FrdObjectPlacementData&)opd).vfxList.Add(vfx);
}

//.............................................................................
// rdSpawnObject
//.............................................................................
void ArdSpawnActor::rdSpawnObject(FrdObjectPlacementData& opd,const FTransform& t) {

	if(!rdGetBaseActor()) return;

	FrdSpawnData& sd=(FrdSpawnData&)spawnData[currentSpawnDataIndex];

	if(opd.mesh.mesh.Get()) {
		FName sid=rdBaseActor->rdGetInstSid(opd.mesh);
		int32 instIndex=rdAddInstanceX(sid,t);
		if(instIndex>=0) {
			TArray<int32>& il=ismIndexList.FindOrAdd(sid);
			il.Add(instIndex);
			if(opd.proxy.proxyType!=RDPROXYTYPE_NONE) {
				rdBaseActor->AddProxyForInstance(opd.sid,opd.proxy,instIndex);
			}
		}
	}

	if(opd.actorClass) {
		AActor* actor=nullptr;
		if(opd.pooled) {
			actor=rdBaseActor->rdGetActorFromPool(opd.actorClass,t);
		} else {
			actor=rdBaseActor->rdSpawnActor(opd.actorClass,t,nullptr,true,false,FName(),nullptr,false,opd.strProps);
		}
		if(actor) {
			((FrdObjectPlacementData&)opd).actorList.Add(actor);
			ArdActor* rdActor=Cast<ArdActor>(actor);
			if(rdActor && opd.proxy.proxyType!=RDPROXYTYPE_NONE) {
				rdBaseActor->AddProxyForActor(rdActor,opd.proxy);
			}
		}
	}

	if(opd.vfx) {
		UNiagaraComponent* fxc=rdBaseActor->rdSpawnVFX(opd.vfx,t);
		if(fxc) {
			((FrdObjectPlacementData&)opd).vfxList.Add(fxc);
		}
	}
}

//.............................................................................
// rdAddBakedObject
//.............................................................................
void ArdSpawnActor::rdAddBakedObject(const FrdObjectPlacementData& opd,const FTransform& t) {

	if(!rdGetBaseActor()) return;

	if(opd.mesh.mesh.Get()) {
		FName sid=rdBaseActor->rdGetInstSid(opd.mesh);
		rdAddBakedInstanceX(opd,sid,t);
	}
	if(opd.actorClass) rdAddBakedActor(opd,opd.actorClass,t);
	if(opd.vfx) rdAddBakedVFX(opd,opd.vfx,t);
}

//.............................................................................
// rdAddBakedInstance
//.............................................................................
void ArdSpawnActor::rdAddBakedInstance(const FrdObjectPlacementData& opd,UStaticMesh* mesh,const FTransform& t) {
	rdAddBakedInstanceX(opd,opd.sid,t);
}
void ArdSpawnActor::rdAddBakedInstanceX(const FrdObjectPlacementData& opd,const FName sid,const FTransform& t) {

	if(sid.IsNone()) return;
	if(!rdGetBaseActor()) return;

	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];

	UrdBakedDataAsset* bda=rdBaseActor->rdLoadBakedDataAsset(sd.bakedDataAsset,true);
	if(!bda) {
		return;
	}
	bda=GetBakedDataAsset();
	TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;

	for(auto& b:baked) {
		if(b.sid==sid) {
			b.transformMap.Add(t);
			sd.numBakedInstances++;
			return;
		}
	}

	int32 iInd=0;
	for(auto& i:sd.items) {
		if(i.sid==sid) break;
		iInd++;
	}

	FrdBakedSpawnObjects bs;
	bs.sid=sid;
	bs.transformMap.Add(t);
	bs.proxy=opd.proxy;
	bs.itemIndex=iInd;
	baked.Add(bs);

	sd.numBakedMeshes++;
	sd.numBakedInstances++;

}

//.............................................................................
// rdAddBakedInstances
//.............................................................................
void ArdSpawnActor::rdAddBakedInstances(const FName sid,const TArray<FTransform>& t) {

	if(sid.IsNone()) return;
	if(!rdGetBaseActor()) return;

	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];

	UrdBakedDataAsset* bda=rdBaseActor->rdLoadBakedDataAsset(sd.bakedDataAsset,true);
	if(!bda) {
		return;
	}
	bda=GetBakedDataAsset();
	TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;

	for(auto& b:baked) {
		if(b.sid==sid) {
			FrdObjectPlacementData& pd=(FrdObjectPlacementData&)sd.items[b.itemIndex];
			b.transformMap.Add(t);
			sd.numBakedInstances+=t.Num();
			return;
		}
	}

	int32 iInd=0;
	for(auto& i:sd.items) {
		if(i.sid==sid) break;
		iInd++;
	}
	if(iInd==sd.items.Num()) {
		int32 ver=0;
		TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn;
		TSoftObjectPtr<UStaticMesh> mesh;
		TArray<TSoftObjectPtr<UMaterialInterface>> materials;
		bool bReverseCulling;
		TEnumAsByte<ErdCollision> collision;
		float startCull,endCull;
		int32 id;
		FName groupID;
		rdBaseActor->rdGetSidDetails(sid,ver,type,mesh,materials,bReverseCulling,collision,startCull,endCull,id,groupID);
		FrdObjectPlacementData opd(type,sid,mesh.Get(),materials,collision,startCull,endCull,0,id,FName());
		opd.sid=sid;
		opd.ismc=rdBaseActor->rdGetInstanceGenX(sid);
		sd.items.Add(opd);
	}

	FrdBakedSpawnObjects bs;
	bs.sid=sid;
	bs.transformMap.Add(t);
	bs.itemIndex=iInd;
	FrdObjectPlacementData& pd=(FrdObjectPlacementData&)sd.items[bs.itemIndex];
	baked.Add(bs);

	sd.numBakedMeshes++;
	sd.numBakedInstances+=t.Num();
}

//.............................................................................
// rdAddBakedActor
//.............................................................................
void ArdSpawnActor::rdAddBakedActor(const FrdObjectPlacementData& opd,UClass* actorClass,const FTransform& t) {

	if(!actorClass) return;
	if(!rdGetBaseActor()) return;

	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];
	UrdBakedDataAsset* bda=rdBaseActor->rdLoadBakedDataAsset(sd.bakedDataAsset,true);
	if(!bda) {
		return;
	}
	bda=GetBakedDataAsset();
	TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;

	for(auto& b:baked) {
		if(b.actorClass==actorClass) {
			b.transformMap.Add(t);
			return;
		}
	}

	int32 iInd=0;
	for(auto& i:sd.items) {
		if(i.actorClass==actorClass) break;
		iInd++;
	}

	FrdBakedSpawnObjects bs;
	bs.actorClass=opd.actorClass;
	bs.strProps=opd.strProps;
	bs.transformMap.Add(t);
	bs.proxy=opd.proxy;
	bs.itemIndex=iInd;
	baked.Add(bs);
}

//.............................................................................
// rdAddBakedVFX
//.............................................................................
void ArdSpawnActor::rdAddBakedVFX(const FrdObjectPlacementData& opd,UNiagaraSystem* vfx,const FTransform& t) {

	if(!vfx) return;
	if(!rdGetBaseActor()) return;

	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];
	UrdBakedDataAsset* bda=rdBaseActor->rdLoadBakedDataAsset(sd.bakedDataAsset,true);
	if(!bda) {
		return;
	}
	bda=GetBakedDataAsset();
	TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;

	for(auto& b:baked) {
		if(b.vfx==vfx) {
			b.transformMap.Add(t);
			return;
		}
	}

	int32 iInd=0;
	for(auto& i:sd.items) {
		if(i.vfx==vfx) break;
		iInd++;
	}

	FrdBakedSpawnObjects bs;
	bs.vfx=vfx;
	bs.transformMap.Add(t);
	bs.proxy=opd.proxy;
	bs.itemIndex=iInd;
	baked.Add(bs);
}

//.............................................................................
// rdClearBakedData
//.............................................................................
void ArdSpawnActor::rdClearBakedData() {

	spawnData[currentSpawnDataIndex].bakedData=nullptr;
	rdClearAlteredList();
	rdClearSpawn();
	Modify();
	MarkComponentsRenderStateDirty();
}

//.............................................................................
// rdClearSpawn
//.............................................................................
void ArdSpawnActor::rdClearSpawn(bool justMarked) {

	if(!rdGetBaseActor()) return;

	FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));
	
#if WITH_EDITOR
	rdBaseActor->dontDoModify++;
#endif

	if(!justMarked) {

		// Spawned Instances
		for(auto& it:ismIndexList) {
			FName sid=it.Key;
		
			TArray<UActorComponent*> list=rdBaseActor->GetComponentsByTag(UInstancedStaticMeshComponent::StaticClass(),sid);
			if(list.Num()==0) {
				continue;
			}
			UInstancedStaticMeshComponent* instGen=(UInstancedStaticMeshComponent*)list[0];
			if(!instGen) continue;
		
			if(rdBaseActor->bRecycleInstances) {
				for(int32 i:it.Value) {
					instGen->UpdateInstanceTransform(i,ztran,rdBaseActor->bWorldSpaceInstancing,false,true);
				}
				TArray<int32>* indexes=rdBaseActor->freeInstanceMap.Find(sid);
				if(!indexes) {
					rdBaseActor->freeInstanceMap.Add(sid,it.Value);
				} else {
					indexes->Append(it.Value);
				}
			} else {
				for(int32 i:it.Value) {
					rdBaseActor->rdRemoveInstanceFast(instGen,i);
				}
			}
		}
		ismIndexList.Empty();
	}

	// Baked Instances
	for(FrdSpawnData& bd:spawnData) {

		UrdBakedDataAsset* bda=bd.bakedData;
		if(bda) {
			TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;
			for(FrdBakedSpawnObjects& bso:baked) {

				if(justMarked && bso.loadState!=rdLoadState::RDLOADSTATE_TOUNLOAD) continue;

				FName sid=bso.sid;
				UInstancedStaticMeshComponent* instGen=rdGetInstanceGenX(sid);
				if(!instGen) continue;
				bso.transformMap.ForEach([this,&ztran,&instGen,sid](FrdInstanceTransformItem& t) {
					if(t.index>=0) {
						if(rdBaseActor->bRecycleInstances) {
							instGen->UpdateInstanceTransform(t.index,ztran,true,false,true);
							TArray<int32>& indexes=rdBaseActor->freeInstanceMap.FindOrAdd(sid);
							indexes.Add(t.index);
						} else {
							rdBaseActor->rdRemoveInstanceFast(instGen,t.index);
						}
						t.index=-1;
					}
				});
			}
		}
	}

	// Other Objects
	if(!justMarked) {

		for(auto& o:spawnData[currentSpawnDataIndex].items) {

			if(o.actorClass) {
				for(auto& a:o.actorList) {
					if(!a) continue;
					if(o.pooled) {
						rdBaseActor->rdReturnActorToPool(a);
					} else {
						a->Destroy();
					}
				}
				o.actorList.Empty();
			}

			if(o.vfx) {
				for(auto& v:o.vfxList) {
					rdBaseActor->rdRemoveVFX(v);
				}
				o.vfxList.Empty();
			}
		}
	}

#if WITH_EDITOR
	rdBaseActor->dontDoModify--;
#endif

	hasSpawned=false;
	bCleared=true;
}

//.............................................................................
// rdUpdateFromDataAssets
//.............................................................................
void ArdSpawnActor::rdUpdateFromDataAssets() {
#if WITH_EDITOR
	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	TMap<FString,TArray<FString>> localList;
	TArray<int32> removeList;
	TArray<UrdMultiPlacementDataAsset*> multiList;
	int32 ind=0;
	for(auto& pd:spawnDat.items) {

		if(pd.PlacementDataAsset.Len()>0) {
			
			if(pd.MultiPlacementDataAsset.Len()>0) {

				TSoftObjectPtr<UrdMultiPlacementDataAsset> smpda(FSoftObjectPath(pd.MultiPlacementDataAsset));
				TSoftObjectPtr<UrdPlacementDataAsset> spda(FSoftObjectPath(pd.PlacementDataAsset));
				UrdMultiPlacementDataAsset* mdpa=smpda.Get();
				if(!mdpa->PlacementDataAssets.Contains(spda)) {
					removeList.Add(ind);
				} else {
					for(auto p:spda->Placements) {
						if(p.PlacementDataAssetID==pd.PlacementDataAssetID) {
							pd=p;
							pd.MultiPlacementDataAsset=mdpa->GetPathName();
							TArray<FString>& ll=localList.FindOrAdd(mdpa->GetPathName());
							ll.Add(pd.PlacementDataAsset);
						}
					}
				}
				multiList.AddUnique(mdpa);

			} else {

				TSoftObjectPtr<UrdPlacementDataAsset> spda(FSoftObjectPath(pd.PlacementDataAsset));
				for(auto p:spda->Placements) {
					if(p.PlacementDataAssetID==pd.PlacementDataAssetID) {
						pd=p;
						TArray<FString>& ll=localList.FindOrAdd(pd.PlacementDataAsset);
						ll.Add(pd.PlacementDataAsset);
					}
				}
			}
		}
		ind++;
	}

	for(auto i:removeList) {
		spawnDat.items.RemoveAt(i);
	}

	for(auto mp:multiList) {

		TArray<FString>* ll=localList.Find(mp->GetPathName());
		if(ll) {
			for(auto pda:mp->PlacementDataAssets) {
				if(!ll->Contains(pda.Get()->GetPathName())) {
					for(auto& p:pda->Placements) {
						spawnData[currentSpawnDataIndex].items.Add(p);
						FrdObjectPlacementData* pd=&spawnData[currentSpawnDataIndex].items[spawnData[currentSpawnDataIndex].items.Num()-1];
						pd->MultiPlacementDataAsset=mp->GetPathName();
					}
				}
			}
		}
	}

#endif
}

//.............................................................................
// rdBakeSpawnData
//.............................................................................
void ArdSpawnActor::rdBakeSpawnData(const FrdSpawnData& bakeData) {

	if(!rdGetBaseActor()) return;

	rdBaseActor->rdSpawnData(this,&bakeData,true);
}

//.............................................................................
// rdSphereTrace
//.............................................................................
void ArdSpawnActor::rdSphereTrace(const FVector location,float radius,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors) {

	FrdSpawnData& sDat=spawnData[currentSpawnDataIndex];
	if(!sDat.bakedData) return; // no baked data
	if(!rdGetBaseActor()) return;

	UrdBakedDataAsset* bda=sDat.bakedData;
	if(!bda) {
		return;
	}
	TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;

	FVector aloc=rdGetSpawnerCenter();
	double dist=FVector::Distance(location,aloc);

	if(sDat.distance>0.0f && dist>sDat.distance) return; // we haven't spawned yet

	if(dist>(sDat.bounds.SphereRadius+radius)) return; // we're out of range

	for(auto& bd:baked) {
		FrdObjectPlacementData& pd=sDat.items[bd.itemIndex];
		if(bd.sid.IsNone() || (bd.proxy.proxyType!=RDPROXYTYPE_SHORTDISTANCE && bd.proxy.proxyType!=RDPROXYTYPE_PICKUP)) continue;
		FrdInstanceItems ii;
		ii.sid=bd.sid;
		bd.transformMap.ForEach([this,&location,&pd,&ii,&radius](FrdInstanceTransformItem& t) {
			double d=FVector::Distance(t.transform.GetTranslation(),location);
			if(d<radius && t.index>=0) {
				ii.instances.Add(t.index);
				ii.transforms.Add(t.transform);
			}
		});
		if(ii.instances.Num()>0) {
			instances.Add(ii);
		}
	}
}

//.............................................................................
// GetSpawnerBounds
//.............................................................................
const FBoxSphereBounds ArdSpawnActor::rdGetSpawnerBounds() const {

	return volumeBox->Bounds;
}

//.............................................................................
// GetSpawnerBoundingBox
//.............................................................................
const FBox ArdSpawnActor::rdGetSpawnerBoundingBox() const {

	return volumeBox->Bounds.GetBox();
}

//.............................................................................
// GetSpawnerCenter
//.............................................................................
const FVector ArdSpawnActor::rdGetSpawnerCenter() const {

	FVector aloc=GetActorLocation()+volumeBox->GetScaledBoxExtent();
	return aloc;
}

//.............................................................................
// GetSpawnerSize
//.............................................................................
const FVector ArdSpawnActor::rdGetSpawnerSize() const {
	if(!volumeBox) {
		return FVector(1,1,1);
	}
	return volumeBox->GetScaledBoxExtent()*2.0f;
}

//.............................................................................
// GetSpawnerRadius
//.............................................................................
const float ArdSpawnActor::rdGetSpawnerRadius() const {

	return FVector::Distance(FVector(0,0,0),volumeBox->GetScaledBoxExtent());
}

//.............................................................................
// rdSnapToLandscape
//.............................................................................
void ArdSpawnActor::rdSnapToLandscape() {
#if WITH_EDITOR	
	if(!landscape) {
		ALandscape* ls=(ALandscape*)UGameplayStatics::GetActorOfClass(GetWorld(),ALandscape::StaticClass());
		if(ls) {
			landscape=ls->GetLandscapeActor();
		}
	}
	if(!landscape || !rdGetBaseActor()) return;

	FVector vsz=volumeBox->GetUnscaledBoxExtent()*FVector(2.0f,2.0f,4.0f);
	FBox lbx=rdBaseActor->rdGetLandscapeBounds(landscape);
	FVector lsl=lbx.Max-lbx.Min;

	FTransform t(landscape->GetActorTransform());
	t.SetScale3D(lsl/vsz);
	SetActorTransform(t);

	Modify();
#endif
}

//.............................................................................
// rdTraceToLandscape
//.............................................................................
bool ArdSpawnActor::rdTraceToLandscape(const FrdObjectPlacementData& opd,const FTransform& transform,FTransform& outTransform,bool addToZ) {

	outTransform=transform;

	if(!opd.bPlaceOnGround && ignorePhysicalMatsList.Num()==0 && opd.filterToPhysicalMatsList.Num()==0) {
		return true;
	}

	FHitResult hit;

	FCollisionQueryParams queryParams;
	queryParams.bReturnPhysicalMaterial=true;
	FCollisionObjectQueryParams objectQueryParams(FCollisionObjectQueryParams::AllStaticObjects);
	objectQueryParams.ObjectTypesToQuery=ECollisionChannel::ECC_WorldStatic;

	float pgLandscapeZ2=GetActorLocation().Z-200;
	float pgLandscapeZ1=pgLandscapeZ2+(64.0f*GetActorScale3D().Z)+200;
	float radius=opd.slopedRadius;
	FVector rloc=transform.GetTranslation();
	bool ret=GetWorld()->LineTraceSingleByObjectType(hit,FVector(rloc.X,rloc.Y,pgLandscapeZ1),FVector(rloc.X,rloc.Y,pgLandscapeZ2),objectQueryParams,queryParams);

#if ENGINE_MAJOR_VERSION>4
	ALandscape* la=hit.HitObjectHandle.FetchActor<ALandscape>();
	ALandscapeStreamingProxy* lasp=hit.HitObjectHandle.FetchActor<ALandscapeStreamingProxy>();
#else
	ALandscape* la=Cast<ALandscape>(hit.Actor);
	ALandscape* lasp=nullptr;
#endif
	float zpos=0.0f;
	if(hit.bBlockingHit && (la || lasp)) {
		
		zpos=hit.ImpactPoint.Z;
		if(opd.bFixSlopePlacement) {
			FHitResult hit2;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(rloc.X-radius,rloc.Y-radius,pgLandscapeZ1),FVector(rloc.X-radius,rloc.Y-radius,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z1=hit2.ImpactPoint.Z;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(rloc.X+radius,rloc.Y-radius,pgLandscapeZ1),FVector(rloc.X+radius,rloc.Y-radius,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z2=hit2.ImpactPoint.Z;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(rloc.X-radius,rloc.Y+radius,pgLandscapeZ1),FVector(rloc.X-radius,rloc.Y+radius,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z3=hit2.ImpactPoint.Z;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(rloc.X+radius,rloc.Y+radius,pgLandscapeZ1),FVector(rloc.X+radius,rloc.Y+radius,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z4=hit2.ImpactPoint.Z;
			z1=fmin(z1,z2);
			z2=fmin(z3,z4);
			zpos=fmin(z1,z2);
		}
	} else {
		return false;
	}

	if(!hit.bBlockingHit) return false;

	FVector ot=transform.GetTranslation();
	if(addToZ) ot.Z+=zpos;
	else ot.Z=zpos;
	outTransform.SetTranslation(ot);

	if(opd.filterToPhysicalMatsList.Num()>0) {

		bool cntn=true;
		for(auto pmap:opd.filterToPhysicalMatsList) {
			if(hit.PhysMaterial.Get()==pmap) {
				cntn=false;
				break;
			}
		}
		if(cntn) return false;

	} else if(ignorePhysicalMatsList.Num()>0) {
		bool cntn=false;
		for(auto pmap:ignorePhysicalMatsList) {
			if(hit.PhysMaterial.Get()==pmap) {
				cntn=true;
				break;
			}
		}
		if(cntn) return false;
	}

	if(opd.minSlope>0.0f) {
		FRotator rt=UKismetMathLibrary::MakeRotFromZY(hit.Normal*180.0f,FVector(0.0f,1.0f,0.0f));
		if(fabs(rt.Pitch)>opd.minSlope || fabs(rt.Roll)>opd.minSlope) return false;
	}

	if(opd.bAlignToNormal) {
		outTransform.SetRotation(UKismetMathLibrary::MakeRotFromZY(hit.Normal*180.0f,FVector(0.0f,1.0f,0.0f)).Quaternion());
	}

	return true;
}

//.............................................................................
// rdIsOnNavMesh
//.............................................................................
bool ArdSpawnActor::rdIsOnNavMesh(const FVector& loc,const FVector& ext) {

	UNavigationSystemV1* navSys=(UNavigationSystemV1*)GetWorld()->GetNavigationSystem();
	if(!navSys) return true;

	FNavAgentProperties ap;
	ap.AgentHeight=ext.Z;
	ap.AgentRadius=sqrt((ext.X*ext.X)*(ext.Y*ext.Y));
	ap.AgentStepHeight=-1;
	ap.NavWalkingSearchHeightScale=1.0f;
	//ap.PreferredNavData=nullptr;

	FNavLocation nloc;
	bool ret=navSys->ProjectPointToNavigation(loc,nloc,ext,&ap);

	return ret;
}

//.............................................................................
// rdGetPlacementData
//.............................................................................
TArray<FrdObjectPlacementData>& ArdSpawnActor::rdGetPlacementData() {

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];
	return spawnDat.items;
}

//.............................................................................
// rdGetProxyState
//.............................................................................
UrdStateDataAsset* ArdSpawnActor::rdGetProxyState(const FName sid,int32 index) {

	if(!rdGetBaseActor()) return nullptr;

	FrdProxyItem* pi=rdBaseActor->rdGetProxyItemFromInstance(sid,index);
	if(pi) {
		return pi->savedState;
	}

	return nullptr;
}

//.............................................................................
// rdGetAllProxyStates
//.............................................................................
TArray<FrdProxyStateItem> ArdSpawnActor::rdGetAllProxyStates() {

	TArray<FrdProxyStateItem> items;
	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];
	for(FrdObjectPlacementData& pd:sd.items) {
		if(IsValid(pd.proxy.savedState)) {
			//items.Add(FrdProxyStateItem(pd.proxy.savedState));
		}
	}

	return items;
}

//.............................................................................
// rdGetUsedISMCs
//.............................................................................
TArray<UInstancedStaticMeshComponent*> ArdSpawnActor::rdGetUsedISMCs() {

	TArray<UInstancedStaticMeshComponent*> comps;
	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];
	for(FrdObjectPlacementData& opd:sd.items) {
		if(opd.ismc.IsValid()) comps.AddUnique(opd.ismc.Get());
	}

	return comps;
}

//.............................................................................
// rdGetUsedMeshes
//.............................................................................
TArray<UStaticMesh*> ArdSpawnActor::rdGetUsedMeshes() {

	TArray<UStaticMesh*> meshes;
	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];
	for(FrdObjectPlacementData& opd:sd.items) {
		if(opd.ismc.IsValid()) meshes.AddUnique(opd.ismc.Get()->GetStaticMesh());
	}

	return meshes;
}

//.............................................................................
// rdGetUsedSIDs
//.............................................................................
TArray<FName> ArdSpawnActor::rdGetUsedSIDs() {

	TArray<FName> sids;

	FrdSpawnData& sd=spawnData[currentSpawnDataIndex];
	for(FrdObjectPlacementData& opd:sd.items) {
		sids.AddUnique(opd.sid);
	}

	return sids;
}

//.............................................................................
// rdGetInstanceIndices
//.............................................................................
TArray<int32>& ArdSpawnActor::rdGetInstanceIndices(UStaticMesh* mesh) {
	return rdGetInstanceIndicesX(rdGetSMsid(mesh));
}
TArray<int32>& ArdSpawnActor::rdGetInstanceIndicesX(const FName sid) {
	static TArray<int32> emptyList;
	TArray<int32>* ls=ismIndexList.Find(sid);
	if(!ls) {
		return emptyList;
	}
	return *ls;
}

//.............................................................................
// UpdateLocalArea
//.............................................................................
//void ArdSpawnActor::UpdateLocalArea(rdThreadResult* tr) {
/*
	currentLocation=rdBaseActor->playerLoc;
	FrdSpawnData& sDat=(FrdSpawnData&)spawnData[currentSpawnDataIndex];
	UrdBakedDataAsset* bda=sDat.bakedData;
	if(!bda) {
		return;
	}
	TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;

	FVector aloc=sDat.center;
	double dist=FVector::Distance(currentLocation,aloc);
	FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));

	if(sDat.distance>0.0f&&dist>(sDat.distance+localRadius)) {
		for(auto& bd:baked) {
			FrdObjectPlacementData& pd=(FrdObjectPlacementData&)sDat.items[bd.itemIndex];
			if((bd.spawnMode==rdSpawnMode::RDSPAWNMODE_SDP || bd.spawnMode==rdSpawnMode::RDSPAWNMODE_FDP) && !bd.sid.IsNone()) {
				TArray<TTuple<int32,FTransform>>& moveArray=tr->moveMap.FindOrAdd(pd.sid);
				int32 num=pd.ismIndexList.Num();
				for(int32 i=0;i<num;i++) {
					int32 pi=pd.ismIndexList[i];
					if(pi>=0) {
						moveArray.Add(TTuple<int32,FTransform>(pi,ztran));
					}
				}
			}
		}
		lastLocation=currentLocation;
		return; // we're out of range
	}

	for(auto& bd:baked) {

		FrdObjectPlacementData& pd=(FrdObjectPlacementData&)sDat.items[bd.itemIndex];
		if(bd.sid.IsNone()) continue;

		bool hasProxy=(pd.proxy.proxyType==RDPROXYTYPE_SHORTDISTANCE||pd.proxy.proxyType==RDPROXYTYPE_LONGDISTANCE||pd.proxy.proxyType==RDPROXYTYPE_PICKUP);
		if(!hasProxy && bd.spawnMode!=rdSpawnMode::RDSPAWNMODE_SDP && bd.spawnMode!=rdSpawnMode::RDSPAWNMODE_FDP) {// || bd.sid.IsNone() || pd.ismIndexList.Num()!=bd.GetNumberOfTransforms()) {
			continue;
		}

		TArray<TTuple<int32,FTransform>>& moveArray=tr->moveMap.FindOrAdd(pd.sid);
		TArray<FTransform>& addArray=tr->addMap.FindOrAdd(pd.sid);
		TArray<int32>& remArray=tr->remMap.FindOrAdd(pd.sid);

		if(bd.spawnMode==rdSpawnMode::RDSPAWNMODE_SDP) {

			bd.ForEachOverlappingInSpheres(currentLocation,lastLocation,localRadius,[&moveArray,&addArray,&ztran](FTransform& t,int32 index,int32 state) {
				if(state==1) {
					addArray.Add(t);
				} else if(state==2) {
					moveArray.Add(TTuple<int32,FTransform>(index,ztran));
				} else {
					remArray.Add(index);
				}
			});

		} else if(bd.spawnMode==rdSpawnMode::RDSPAWNMODE_FDP) {

			bd.ForEachOverlappingInCones(currentLocation,currentRotation,lastLocation,lastRotation,localRadius,viewFrustumDistance,[&moveArray,&addArray,&ztran](FTransform& t,int32 index,int32 state) {
				if(state==1) {
					addArray.Add(t);
				} else if(state==2) {
					moveArray.Add(TTuple<int32,FTransform>(index,ztran));
				} else {
					remArray.Add(index);
				}
			});

		} else {

			bd.ForEachOverlappingInSpheres(currentLocation,lastLocation,localRadius,[&moveArray,&addArray,&ztran](FTransform& t,int32 index,int32 state) {
				if(state==1) {
					addArray.Add(t);
				} else if(state==2) {
					moveArray.Add(TTuple<int32,FTransform>(index,ztran));
				} else {
					remArray.Add(index);
				}
			});
		}
	}

	lastLocation=currentLocation;
*/
//}

//.............................................................................
// rdRemoveInstanceByIndex
//.............................................................................
bool ArdSpawnActor::rdRemoveInstanceByIndex(const FName sid,int32 index) {

	int32 bind=0;
	alterations.Add(FString::Printf(TEXT("RI:%s:%d"),*sid.ToString(),bind));
	return false;
}

//.............................................................................
// rdRemoveInstanceByLocation
//.............................................................................
bool ArdSpawnActor::rdRemoveInstanceByLocation(const FName sid,const FVector& location) {
	int32 bind=0;
	alterations.Add(FString::Printf(TEXT("RI:%s:%d"),*sid.ToString(),bind));
	return false;
}

//.............................................................................
// rdRemoveActor
//.............................................................................
bool ArdSpawnActor::rdRemoveActor(const AActor* actor) {
	alterations.Add(FString::Printf(TEXT("RA:%s"),*actor->GetName()));
	return false;
}

//.............................................................................
// rdRemoveVFX
//.............................................................................
bool ArdSpawnActor::rdRemoveVFX(const UNiagaraComponent* vfx) {
	alterations.Add(FString::Printf(TEXT("RV:%s"),*vfx->GetName()));
	return false;
}

//.............................................................................
// rdRemoveInstancesWithinRadius
//.............................................................................
int32 ArdSpawnActor::rdRemoveInstancesWithinRadius(const FName sid,const FVector& location,const float radius) {
	alterations.Add(FString::Printf(TEXT("RIR:%s:%f:%f:%f:%f"),*sid.ToString(),location.X,location.Y,location.Z,radius));
	return 0;
}

//.............................................................................
// rdRemoveAllInstancesWithinRadius
//.............................................................................
int32 ArdSpawnActor::rdRemoveAllInstancesWithinRadius(const FVector& location,const float radius) {
	alterations.Add(FString::Printf(TEXT("RAIR:%f:%f:%f:%f"),location.X,location.Y,location.Z,radius));
	return 0;
}

//.............................................................................
// rdRemoveAllObjectsWithinRadius
//.............................................................................
int32 ArdSpawnActor::rdRemoveAllObjectsWithinRadius(const FVector& location,const float radius) {
	alterations.Add(FString::Printf(TEXT("RAOR:%f:%f:%f:%f"),location.X,location.Y,location.Z,radius));
	return 0;
}

//.............................................................................
// rdChangeInstancesWithinRadius
//.............................................................................
int32 ArdSpawnActor::rdChangeInstancesWithinRadius(const FName sid1,const FName sid2,const FVector& location,const float radius) {
	alterations.Add(FString::Printf(TEXT("CIR:%s:%s:%f:%f:%f:%f"),*sid1.ToString(),*sid2.ToString(),location.X,location.Y,location.Z,radius));
	return 0;
}

//.............................................................................
// rdChangeInstanceListWithinRadius
//.............................................................................
int32 ArdSpawnActor::rdChangeInstanceListWithinRadius(const TArray<FName>& sidList1,const TArray<FName> sidList2,const FVector& location,const float radius) {
	if(sidList1.Num()!=sidList2.Num()) {
		return 0;
	}
	int cnt=0;
	for(int32 i=0;i<sidList1.Num();i++) {
		cnt+=rdChangeInstancesWithinRadius(sidList1[i],sidList2[i],location,radius);
	}
	return cnt;
}

//.............................................................................
