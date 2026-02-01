//
// rdProceduralActor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2024 (moved from rdInstBaseActor.cpp)
// Last Modified: 27th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdProceduralActor.h"
#include "rdInstBaseActor.h"
#include "rdSpawnActor.h"

//.............................................................................
// Constructor
//.............................................................................
ArdProceduralActor::ArdProceduralActor() {
}

//.............................................................................
// Constructor with ObjectInitializer
//.............................................................................
ArdProceduralActor::ArdProceduralActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

}

#if WITH_EDITOR
//.............................................................................
//  PreSave
//.............................................................................
#if ENGINE_MAJOR_VERSION<5
void ArdProceduralActor::PreSave(const class ITargetPlatform* context) {
#else
void ArdProceduralActor::PreSave(FObjectPreSaveContext context) {
#endif

	if(!rdGetBaseActor()) return;

	ArdSpawnActor::PreSave(context);
}
#endif

//.............................................................................
// rdSpawn
//.............................................................................
void ArdProceduralActor::rdSpawn() {

	if(!rdGetBaseActor()) return;

#if WITH_EDITOR
	if(editorLoadState==rdLoadState::RDLOADSTATE_UNLOADED) return;
	if(!bDrawInEditor && !rdBaseActor->rdIsPlaying()) return;
#endif

	rdBlockingLoad();

	FrdSpawnData& spawnDat=spawnData[currentSpawnDataIndex];

	if(GetBakedDataAsset()) {

		spawnDat.items.Empty();
/*
		int32 ind=0;
		for(FrdBakedSpawnObjects& bd:spawnDat.bakedData) {
			int32 ver=0;
			UStaticMesh* mesh=nullptr;
			TArray<TSoftObjectPtr<UMaterialInterface>> materials;
			bool bReverseCulling=false;
			TEnumAsByte<ErdCollision> collision;
			float startCull=-1.0f,endCull=-1.0f;
			int32 id=0;
			rdBaseActor->rdGetSidDetails(bd.sid,ver,type,mesh,materials,bReverseCulling,collision,startCull,endCull,id);
			FrdObjectPlacementData opd(mesh,materials,collision,startCull,endCull,id);
			opd.sid=bd.sid;
			opd.ismc=rdBaseActor->rdGetInstanceGenX(bd.sid);
			spawnDat.items.Add(opd);
			bd.itemIndex=ind++;
		}
*/	
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

	if(hasSpawned) {
		rdClearSpawn();
	}

	rdUpdateFromDataAssets();

	if(rdBaseActor->OnPreSpawnDelegate.IsBound()) {
		FEditorScriptExecutionGuard ScriptGuard;
		rdBaseActor->OnPreSpawnDelegate.Broadcast(this);
	}

	rdBaseActor->ignorePMaps=ignorePhysicalMatsList;
	UrdBakedDataAsset* bda=GetBakedDataAsset();
	if(bda && bda->bakedData.Num()>0) {
		rdBaseActor->rdSpawnData(this,&spawnData[currentSpawnDataIndex]);
	} else {
		rdBaseActor->rdProceduralGenerateBake(this,nullptr);
	}
	hasSpawned=true;

	if(rdBaseActor->OnPostSpawnDelegate.IsBound()) {
		FEditorScriptExecutionGuard ScriptGuard;
		rdBaseActor->OnPostSpawnDelegate.Broadcast(this);
	}

}

//.............................................................................
// rdBakeSpawnData
//.............................................................................
void ArdProceduralActor::rdBakeSpawnData(const FrdSpawnData& bakeData) {

	if(!rdGetBaseActor()) return;

	rdBaseActor->ignorePMaps=ignorePhysicalMatsList;
	rdBaseActor->rdProceduralGenerateBake(this,(FrdSpawnData*)&bakeData);
	hasSpawned=true;
}

//.............................................................................
// rdBakeSpawnedData
//.............................................................................
void ArdProceduralActor::rdBakeSpawnedData() {

	rdBakeSpawnData(spawnData[currentSpawnDataIndex]);
	MarkComponentsRenderStateDirty();
}

//.............................................................................
// rdClearBakedData
//.............................................................................
void ArdProceduralActor::rdClearBakedData() {

	spawnData[currentSpawnDataIndex].bakedData=nullptr;
	spawnData[currentSpawnDataIndex].bakedDataAsset=nullptr;
	spawnData[currentSpawnDataIndex].numBakedMeshes=0;
	spawnData[currentSpawnDataIndex].numBakedInstances=0;
	rdClearSpawn();
	MarkComponentsRenderStateDirty();
}

//.............................................................................
