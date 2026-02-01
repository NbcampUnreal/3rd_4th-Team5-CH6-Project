//
// rdSpawn.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 8th February 2024 (moved from rdInstBaseActor.cpp)
// Last Modified: 29th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "rdSpawnActor.h"
#include "Landscape.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ChildActorComponent.h"
#include "ComponentInstanceDataCache.h"
#if ENGINE_MAJOR_VERSION>4
#include "LandscapeStreamingProxy.h"
#endif
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "rdActorStrings.h"
#include "NiagaraRendererProperties.h"
#include "NiagaraEmitter.h"
#include "NiagaraSystem.h"
#if WITH_EDITOR
#include "FileHelpers.h"
#endif
//.............................................................................
// rdSpawn (from rdInstBaseActor and rdInstSubSystem)
//.............................................................................
void ArdInstBaseActor::rdSpawn(const ArdSpawnActor* spawner,bool bake) {

	if(!spawner) return;
#if WITH_EDITOR
	if(!spawner->bDrawInEditor && !rdIsPlaying()) return;
#endif

	FrdSpawnData* spawnData=(FrdSpawnData*)&spawner->spawnData[spawner->currentSpawnDataIndex];
	if(spawnData) {

		if(bake) {
			((ArdSpawnActor*)spawner)->rdBakeEvent();
		} else {
			UrdBakedDataAsset* bda=((ArdSpawnActor*)spawner)->GetBakedDataAsset(false);
			if(!bda) {
				((ArdSpawnActor*)spawner)->rdSpawnEvent();
			}
		}
	}

	rdSpawnData((ArdSpawnActor*)spawner,spawnData,bake);
}

//.............................................................................
// SpawnInstances
//.............................................................................
void ArdInstBaseActor::SpawnInstances(ArdSpawnActor* sa,UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData) {

	if(!instGen) {
		return;
	}
	
	FrdObjectPlacementData& pd=(FrdObjectPlacementData&)spawnData->items[bd.itemIndex];

	if(sa && (sa->spawnMode==rdSpawnMode::RDSPAWNMODE_SDP || sa->spawnMode==rdSpawnMode::RDSPAWNMODE_FDP)) {

	} else {

		if(spawnData->distributeFrames>1) {
			TArray<int32> iList;
			rdAddDistributedInstancesX(bd.sid,bd.transformMap.GetAll(),bd.transformMap.Num()/spawnData->distributeFrames,iList,bd.proxy);

		} else if(sa) {

			if(bd.proxy.proxyType!=RDPROXYTYPE_NONE) {

				if(sa->bRelativeSpawning) {
					rdAddInstanceArrayFastWithIndexesRel(sa->GetActorTransform(),instGen,bd);
				} else {
					rdAddInstanceArrayFastWithIndexes(instGen,bd);
				}

				TArray<int32> instIndexList;
				AddProxiesForInstances(bd.sid,bd.proxy,instIndexList);

				if(bd.proxy.scanDistance>sa->maxProxyScanDistance) {
					sa->maxProxyScanDistance=bd.proxy.scanDistance;
				}

			} else {

				if(sa->bRelativeSpawning) {
					rdAddInstanceArrayFastWithIndexesRel(sa->GetActorTransform(),instGen,bd);
				} else {
					rdAddInstanceArrayFastWithIndexes(instGen,bd);
				}
			}
		}
	}
}

//.............................................................................
// SpawnActors
//.............................................................................
void ArdInstBaseActor::SpawnActors(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData) {

	const FrdObjectPlacementData& pd=spawnData->items[bd.itemIndex];
	TArray<AActor*>& aList=(TArray<AActor*>&)pd.actorList;
				
	if(spawnData->distributeFrames>1) {

		rdSpawnDistributedActors(bd.actorClass,nullptr,bd.transformMap.GetAll(),bd.transformMap.Num()/spawnData->distributeFrames,bd.pooled,aList,bd.proxy,bd.strProps);

	} else {
		if(bd.pooled) {

			bd.transformMap.ForEach([this,&bd,&aList](FrdInstanceTransformItem& t) {
				AActor* a=rdGetActorFromPool(bd.actorClass,t.transform);
				aList.Add(a);
				switch(bd.proxy.proxyType) {
					case RDPROXYTYPE_SHORTDISTANCE:
						rdAddActorProxy(a,bd.proxy);
						if(bd.proxy.bUseWithDestruction) {
							rdAddDestructionActorProxy(a,bd.proxy);
						}
						break;
					case RDPROXYTYPE_LONGDISTANCE:
						rdAddLongDistanceActorProxy(a,proxyLongDistanceSwapDistance,bd.proxy);
						if(bd.proxy.bUseWithDestruction) {
							rdAddDestructionActorProxy(a,bd.proxy);
						}
						break;
					case RDPROXYTYPE_DESTRUCTION:
						rdAddDestructionActorProxy(a,bd.proxy);
						break;
					//case RDPROXYTYPE_PICKUP:
					//	rdAddPickupProxy(a,bd.proxy);
					//	break;
				}
			});

		} else {

			//TODO: test timings with ActorTemplate vs DuplicateObject vs simple Spawn
			bd.transformMap.ForEach([this,&bd,&aList,&sa,spawnData](FrdInstanceTransformItem& t) {
				FTransform nt=sa->bRelativeSpawning?rdRelativeToWorld(t.transform,sa->GetActorTransform()):t.transform;
				AActor* a=rdSpawnActor(bd.actorClass,nt,nullptr,false,false,FName(),nullptr,true,bd.strProps);
				if(!a) return;
				a->FinishSpawning(nt,false,nullptr);
				a->SetActorTransform(nt);
				a->MarkComponentsRenderStateDirty();
#if WITH_EDITOR
				if(!rdIsPlaying()) {
					a->SetFolderPath_Recursively(FName(spawnData->bakedDataAsset.GetAssetName()+TEXT("_Spawned")));
				}
#endif
				aList.Add(a);
				if(bd.proxy.proxyType!=RDPROXYTYPE_NONE) {
					switch(bd.proxy.proxyType) {
						case RDPROXYTYPE_SHORTDISTANCE:
							rdAddActorProxy(a,bd.proxy);
							if(bd.proxy.bUseWithDestruction) {
								rdAddDestructionActorProxy(a,bd.proxy);
							}
							break;
						case RDPROXYTYPE_LONGDISTANCE:
							rdAddLongDistanceActorProxy(a,proxyLongDistanceSwapDistance,bd.proxy);
							if(bd.proxy.bUseWithDestruction) {
								rdAddDestructionActorProxy(a,bd.proxy);
							}
							break;
						case RDPROXYTYPE_DESTRUCTION:
							rdAddDestructionActorProxy(a,bd.proxy);
							break;
						//case RDPROXYTYPE_PICKUP:
						//	rdAddPickupProxy(rdActor,bd.proxy);
						//	break;
					}
				}
			});
		}
	}
}

//.............................................................................
// SpawnComponentMeshes
//.............................................................................
void ArdInstBaseActor::SpawnComponentMeshes(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData) {

	const FrdObjectPlacementData& pd=spawnData->items[bd.itemIndex];
	TArray<UStaticMeshComponent*>& smcList=(TArray<UStaticMeshComponent*>&)pd.smcList;
				
	if(spawnData->distributeFrames>1) {

		//rdSpawnDistributedSMCs(bd.sid,nullptr,bd.GetAllTransforms(),bd.GetNumberOfTransforms()/spawnData->distributeFrames,bd.pooled,smcList);

	} else {

			bd.transformMap.ForEach([this,&bd,&smcList,&sa,spawnData](FrdInstanceTransformItem& t) {
				FTransform nt=sa->bRelativeSpawning?rdRelativeToWorld(t.transform,sa->GetActorTransform()):t.transform;

				//	UStaticMeshComponent* smc=rdAddStaticMesh(pd.sid,t);
/*
struct FData
{
    FCustomPrimitiveData CustomData;
    FStaticMeshSceneProxyDesc ProxyDesc;
    FPrimitiveSceneInfoData SceneInfoData;
    FPrimitiveSceneDesc SceneDesc;
};

	Data = MakeUnique<FData>();

    Data->ProxyDesc.StaticMesh = StaticMesh;
    Data->ProxyDesc.CustomPrimitiveData = &Data->CustomData;
    Data->ProxyDesc.NaniteResources = StaticMesh->GetRenderData()->NaniteResourcesPtr.Get();
    Data->ProxyDesc.World = WorldIn;
    Data->ProxyDesc.Owner = Owner;
    Data->ProxyDesc.Scene = WorldIn->Scene;
    Data->ProxyDesc.ComponentId = Data->SceneInfoData.PrimitiveSceneId;

    Nanite::FMaterialAudit NaniteMaterials{};
    if(Data->ProxyDesc.ShouldCreateNaniteProxy(&NaniteMaterials))
    {
        Proxy = new Nanite::FSceneProxy(NaniteMaterials, Data->ProxyDesc);
    }
    else
    {
        Proxy = new FStaticMeshSceneProxy(Data->ProxyDesc, false);
    }
    
    Data->SceneInfoData.SceneProxy = Proxy; 
    
    Data->SceneDesc.World = WorldIn;
    Data->SceneDesc.PrimitiveUObject = Owner;
    Data->SceneDesc.ProxyDesc = &Data->ProxyDesc;
    Data->SceneDesc.PrimitiveSceneData = &Data->SceneInfoData;
    Data->SceneDesc.LocalBounds = StaticMesh->GetBounds();

    SetTransform_Internal(Transform);

    WorldIn->Scene->AddPrimitive(&Data->SceneDesc);
-----

// Set transform internal
{
    if(!IsValid()) return;
    
    Data->SceneDesc.RenderMatrix = NewTransform.ToMatrixWithScale();
    Data->SceneDesc.AttachmentRootPosition = NewTransform.GetLocation();
    Data->SceneDesc.Bounds = CalculateBounds(NewTransform);
}


// function for bounds
{
    FBoxSphereBounds Bounds;
    if(IsValid())
    {
        Bounds = Data->SceneDesc.LocalBounds.TransformBy(NewTransform);
    }

    return Bounds;
}

----
// render thread will continue to use the data for a bit
// don't delete the proxy, the proxy will be deleted by the render thread eventually

	if(!IsValid()) return;

    Data->SceneDesc.GetWorld()->Scene->RemovePrimitive(&Data->SceneDesc);
    Proxy = nullptr;

    if(bDestroyData)
    {
        ENQUEUE_RENDER_COMMAND(FLightWeightMeshContainer_Destroy)(
            [DataToDelete = std::move(Data)](FRHICommandListImmediate& RHICmdList) mutable 
            {
                DataToDelete.Reset();
            }
        );
    }

*/
				});
	}
}

//.............................................................................
// SpawnNiagaraMeshes
//.............................................................................
void ArdInstBaseActor::SpawnNiagaraMeshes(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData) {

	UStaticMesh* mesh=rdGetMeshFromSid(bd.sid);

	UNiagaraComponent* niagara=UNiagaraFunctionLibrary::SpawnSystemAttached(niagaraSpawner.LoadSynchronous(),sa->popRootComponent,NAME_None,FVector(0.f),FRotator(0.f),EAttachLocation::Type::SnapToTarget,false);

	UStaticMeshComponent* smc=nullptr;//rdAddStaticMesh(mesh,FTransform());
/*
	if(smc) {
		sa->AddInstanceComponent(smc);
		smc->SetMobility(EComponentMobility::Movable);
		smc->SetStaticMesh(mesh);
	}
*/
	UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(niagara,TEXT("rdMesh"),smc);

//	UNiagaraSystem* sys=DuplicateObject(niagaraSpawner.LoadSynchronous(),GetTransientPackage());
//	UNiagaraComponent* niagara=UNiagaraFunctionLibrary::SpawnSystemAttached(niagaraSpawner.LoadSynchronous(),nullptr,NAME_None,FVector(0.f),FRotator(0.f),EAttachLocation::Type::KeepRelativeOffset,true);
//	UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(niagara,TEXT("rdMesh"),mesh);
//	UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(niagara,TEXT("rdMesh"),mesh);

	//UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayColor(UNiagaraComponent* NiagaraSystem, FName OverrideName, const TArray& ArrayData)

	//UNiagaraComponent* niagara=UNiagaraFunctionLibrary::SpawnSystemAttached(FireEffectMuzzle,WeaponMuzzle,NAME_None,FVector(0.f),FRotator(0.f),EAttachLocation::Type::KeepRelativeOffset,true);
	//niagara->SetNiagaraVariableFloat(FString("mesh"),mesh);
	/*
	UNiagaraSystem* sys=DuplicateObject(niagaraSpawner.LoadSynchronous(),GetTransientPackage());

//	for(const FNiagaraEmitterHandle& emitterHandle:sys->GetEmitterHandles()) {
	for(FVersionedNiagaraEmitterHandle& emitterHandle:sys->GetEmitterHandles()) {

		const UNiagaraEmitter* emitter=emitterHandle.GetInstance();
		for(UNiagaraRendererProperties* renderer:emitter->GetRenderers()) {

			if(UNiagaraMeshRendererProperties* meshRenderer=Cast<UNiagaraMeshRendererProperties>(renderer)) {

				for(FNiagaraMeshRendererMeshProperties& nMesh:meshRenderer->Meshes) {
					nMesh.Mesh=mesh;
				}
			}
		}
	}
	*/
}

//.............................................................................
// ReplaceNiagaraMesh
//.............................................................................
UNiagaraSystem* ArdInstBaseActor::ReplaceNiagaraMesh(UNiagaraSystem* existingSys,UStaticMesh* mesh) {

/*
	UNiagaraSystem* sys=DuplicateObject(existingSys,GetTransientPackage());
	for(const FNiagaraEmitterHandle& emitterHandle:sys->GetEmitterHandles()) {
		const UNiagaraEmitter* emitter=emitterHandle.GetInstance();
		for(UNiagaraRendererProperties* renderer:emitter->GetRenderers()) {
			if(UNiagaraMeshRendererProperties* meshRenderer=Cast<UNiagaraMeshRendererProperties>(renderer)) {
				for(FNiagaraMeshRendererMeshProperties& nMesh:meshRenderer->Meshes) {
					nMesh.Mesh=mesh;
				}
			}
		}
	}
*/
	return nullptr;
}

//.............................................................................
// SpawnRdMeshlets
//.............................................................................
void ArdInstBaseActor::SpawnRdMeshlets(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData) {

}

//.............................................................................
// SpawnCompless
//.............................................................................
void ArdInstBaseActor::SpawnCompless(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData) {

}

//.............................................................................
// rdSpawnData (from rdInstBaseActor)
//.............................................................................
void ArdInstBaseActor::rdSpawnData(ArdSpawnActor* ssa,const FrdSpawnData* spawnData,bool bake,bool justNewlyStreamed) {

	FrdSpawnData& sd=(FrdSpawnData&)(*spawnData);

	if(bake) {
		sd.numBakedMeshes=0;
		sd.numBakedInstances=0;
	}

	UrdBakedDataAsset* bda=ssa->GetBakedDataAsset();
	if(!bake && bda) {

		int32 distributeFrames=spawnData->distributeFrames;
		for(FrdBakedSpawnObjects& bd:bda->bakedData) {

			if(justNewlyStreamed && bd.loadState!=rdLoadState::RDLOADSTATE_PARTIALLYLOADED) continue;

			if(!bd.sid.IsNone()) {

				ErdSpawnType spawnType=rdGetSpawnTypeFromSid(bd.sid);
				if(spawnType==ErdSpawnType::UseDefaultSpawn) spawnType=bUseHISMs?ErdSpawnType::HISM:ErdSpawnType::ISM;

				switch(spawnType) {
					case ErdSpawnType::ISM:
						SpawnInstances(ssa,rdGetInstanceGenX(bd.sid),bd,spawnData);
						break;
					case ErdSpawnType::HISM:
						SpawnInstances(ssa,rdGetHInstanceGenX(bd.sid),bd,spawnData);
						break;
					case ErdSpawnType::SMC:
						SpawnComponentMeshes(ssa,bd,spawnData);
						break;
					case ErdSpawnType::NiagaraMesh:
						SpawnNiagaraMeshes(ssa,bd,spawnData);
						break;
					case ErdSpawnType::rdMeshlet:
						SpawnRdMeshlets(ssa,bd,spawnData);
						break;
					case ErdSpawnType::ComponentlessMesh:
						SpawnCompless(ssa,bd,spawnData);
						break;
					case ErdSpawnType::Actor:
						SpawnActors(ssa,bd,spawnData);
						break;
				}
			}
		}
	}
}

//.............................................................................
// rdCreateSpawnActorFromVolume
//.............................................................................
ArdSpawnActor* ArdInstBaseActor::rdCreateSpawnActorFromVolume(AActor* volume,double distance,int32 distFrames,bool spatial,TEnumAsByte<rdSpawnMode> spwnMode) {

	if(!volume) {
		return nullptr;
	}

	TArray<UInstancedStaticMeshComponent*> ismComps;
	TArray<UActorComponent*> comps;
	volume->GetComponents(comps,true);

	FBox box(FVector(99999,99999,99999),FVector(-99999,-99999,-99999));
	for(auto c:comps) {
		UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
		if(ismc) {
			UStaticMesh* mesh=ismc->GetStaticMesh();
			FName csid=rdGetSMCsid(ismc);
			if(mesh) {

				ismComps.Add(ismc);

				FBox bnds=ismc->Bounds.GetBox();
				if(bnds.Min.X<box.Min.X) box.Min.X=bnds.Min.X;
				if(bnds.Min.Y<box.Min.Y) box.Min.Y=bnds.Min.Y;
				if(bnds.Min.Z<box.Min.Z) box.Min.Z=bnds.Min.Z;
				if(bnds.Max.X>box.Max.X) box.Max.X=bnds.Max.X;
				if(bnds.Max.Y>box.Max.Y) box.Max.Y=bnds.Max.Y;
				if(bnds.Max.Z>box.Max.Z) box.Max.Z=bnds.Max.Z;
			}
		}
	}

	float volumeWidth=box.Max.X-box.Min.X;
	float volumeDepth=box.Max.Y-box.Min.Y;
	FTransform volumeTransform(FRotator(0,0,0),FVector(box.Min.X+(volumeWidth/2.0f),box.Min.Y+(volumeDepth/2.0f),box.Min.Z),FVector(1,1,1));
	UrdSetObjectsList objects;

	for(auto ismc:ismComps) {

		UStaticMesh* mesh=ismc->GetStaticMesh();
		FName csid=rdGetSMCsid(ismc);
		int32 cnt=ismc->GetInstanceCount();
		if(cnt==0) continue;

		FrdSetObjectItem item(csid);
		objects.objects.Add(item);
		for(int i=0;i<cnt;i++) {

			FTransform tran;
			ismc->GetInstanceTransform(i,tran,true);
			item.transforms.Add(tran);
		}
	}
	return rdCreateSpawnActorFromObjectList(&objects,volumeTransform,TEXT(""),distance,distFrames,spatial,spwnMode);
}

//.............................................................................
// AddNewPlacementData
//.............................................................................
FrdBakedSpawnObjects* ArdInstBaseActor::AddNewPlacementData(FrdSpawnData& sd,UrdBakedDataAsset* bda,const FName sid,FrdProxySetup* proxy,int32 gridW,int32 gridH) {

	FrdBakedSpawnObjects* bakedDat=new FrdBakedSpawnObjects(sid);
	bakedDat->transformMap.SetGridSize(gridW,gridH);
	TEnumAsByte<ErdSpawnType> type=rdGetSpawnTypeFromSid(sid);
	int32 ver=0;
	if(type==ErdSpawnType::Actor) {

		UClass* uclass=nullptr;
		FString propStr;
		FName groupName;
		rdGetSidActorDetails(sid,ver,type,uclass,propStr,groupName);

		FrdObjectPlacementData pd(uclass,propStr);
		pd.sid=sid;
		sd.items.Add(pd);
		bakedDat->itemIndex=sd.items.Num()-1;
		bakedDat->actorClass=uclass;
		bakedDat->strProps=FString::Printf(TEXT("%d|%s"),RDSTRING_VERSION,*propStr);
		bakedDat=&bda->bakedData.Add_GetRef(*bakedDat);

	} else {

		TSoftObjectPtr<UStaticMesh> mesh;
		TArray<TSoftObjectPtr<UMaterialInterface>> materials;
		bool bReverseCulling;
		TEnumAsByte<ErdCollision> collision;
		float startCull,endCull;
		int32 id=0;
		FName groupName;
		rdGetSidDetails(sid,ver,type,mesh,materials,bReverseCulling,collision,startCull,endCull,id,groupName);
		if(startCull<=0.0f && endCull<=0.0f) {
			startCull=endCull=-1.0f;
		}
		FrdObjectPlacementData pd(type,sid,mesh.Get(),materials,collision,startCull,endCull,0,id,groupName);
		pd.sid=sid;
		pd.ismc=rdGetInstanceGenX(sid);
		if(proxy) pd.proxy=*proxy;
		sd.items.Add(pd);
		bakedDat->itemIndex=sd.items.Num()-1;
		bakedDat=&bda->bakedData.Add_GetRef(*bakedDat);
	}

	return bakedDat;
}

//.............................................................................
// rdAddSpawnSidTransform
//.............................................................................
void ArdInstBaseActor::rdAddSpawnSidTransform(const FName sid,const FTransform& stransform,FrdSpawnData& sd,TArray<FrdBakedSpawnObjects>& bd,UrdBakedDataAsset* bda,const FTransform& transform,FrdProxySetup* proxy,int32 gridW,int32 gridH) {

	FrdBakedSpawnObjects* bakedDat=nullptr;
	for(auto& bdi:bd) {
		if(bdi.sid==sid) {
			bakedDat=&bdi;
			break;
		}
	}

	if(!bakedDat) {
		bakedDat=AddNewPlacementData(sd,bda,sid,nullptr,gridW,gridH);
	}
	if(proxy) bakedDat->proxy=*proxy;

	bakedDat->transformMap.Add(rdWorldToRelativeNoScale(stransform,transform));
}

//.............................................................................
// rdAddSpawnSidTransforms
//.............................................................................
void ArdInstBaseActor::rdAddSpawnSidTransforms(const FName sid,TArray<FTransform>& transforms,FrdSpawnData& sd,TArray<FrdBakedSpawnObjects>& bd,UrdBakedDataAsset* bda,const FTransform& transform,FrdProxySetup* proxy,int32 gridW,int32 gridH) {

	FrdBakedSpawnObjects* bakedDat=nullptr;
	for(auto& bdi:bd) {
		if(bdi.sid==sid) {
			bakedDat=&bdi;
			break;
		}
	}

	if(!bakedDat) {
		bakedDat=AddNewPlacementData(sd,bda,sid,proxy,gridW,gridH);
	}
	if(proxy) {
		bakedDat->proxy=*proxy;
	}

	for(auto& t:transforms) {
		bakedDat->transformMap.Add(rdWorldToRelativeNoScale(t,transform));
	}
}

//.............................................................................
// rdCreateSpawnActorFromObjectList
//.............................................................................
ArdSpawnActor* ArdInstBaseActor::rdCreateSpawnActorFromObjectList(UrdSetObjectsList* objects,FTransform& transform,const FString& filename,double distance,int32 distFrames,bool spatial,TEnumAsByte<rdSpawnMode> spwnMode,bool harvestInstances,int32 gridX,int32 gridY) {

	if(!objects) return nullptr;

	UE_LOG(LogTemp,Display,TEXT("CreateSpawnActor1"));//@@

	FVector scale=transform.GetScale3D();
	transform.SetScale3D(FVector(1,1,1));

	ArdSpawnActor* ssa=(ArdSpawnActor*)rdSpawnActor(ArdSpawnActor::StaticClass(),transform);
	if(!ssa || !ssa->spawnData.Num()) {
		return nullptr;
	}
	ssa->volumeBox->SetBoxExtent(scale/2.0);
	ssa->volumeBox->SetRelativeLocation(scale*0.5f);
	ssa->bRelativeSpawning=true;
	FrdSpawnData& sd=ssa->spawnData[0];

	sd.bounds=ssa->volumeBox->CalcBounds(ssa->volumeBox->GetRelativeTransform());
	sd.distance=distance;
	sd.distributeFrames=distFrames;
	sd.bakedDataAsset=FSoftObjectPath(filename);
	UrdBakedDataAsset* bda=rdLoadBakedDataAsset(sd.bakedDataAsset,true);
	if(!bda) {
		return nullptr;
	}

	UE_LOG(LogTemp,Display,TEXT("CreateSpawnActor2"));//@@

	bda->bounds=sd.bounds;
	TArray<FrdBakedSpawnObjects>& bd=bda->bakedData;

	UE_LOG(LogTemp,Display,TEXT("CreateSpawnActor3"));//@@

	for(FrdSetObjectItem& i:objects->objects) {
		
		if(!i.sid.IsNone()) {
			rdAddSpawnSidTransforms(i.sid,i.transforms,sd,bd,bda,transform,&i.proxy,gridX,gridY);
			if(i.proxy.proxyType==RDPROXYTYPE_SHORTDISTANCE && i.proxy.bEntireMeshProxy) {
				ssa->scanType|=0x01;
			}
		}
		if(i.actorClass) {
			bool isAllNonISM=true;
			bool containsNonISM=false;
			bool harvested=false;
			if(harvestInstances && i.actorTemplate && !i.actorTemplate->Tags.Contains("NoInstancing") && !i.actorTemplate->Tags.Contains(TEXT("NoHarvest"))) {
				AActor* a=i.actorTemplate;
				TArray<UActorComponent*> comps;
				a->GetComponents(comps,false);
				for(auto c:comps) {
					UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
					if(ismc && ismc->GetStaticMesh()) {
						isAllNonISM=false;
					} else {
						USplineMeshComponent* spc=Cast<USplineMeshComponent>(c);
						UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(c);
						if(smc || spc) {
							isAllNonISM=false;
						} else {

							UChildActorComponent* cac=Cast<UChildActorComponent>(c);
							if(cac && cac->GetChildActorClass()==AStaticMeshActor::StaticClass()) {
								AStaticMeshActor* ca=Cast<AStaticMeshActor>(cac->GetChildActor());
								if(ca && ca->GetStaticMeshComponent()) {
									isAllNonISM=false;
								} else {
									containsNonISM=true;
								}
							} else {
								if(c->ComponentTags.Contains("NoInstancing")) {
									containsNonISM=true;
								}
							}
						}
					}
				}

				if(!isAllNonISM) {
					for(auto c:comps) {
						UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
						UStaticMeshComponent* meshComp=nullptr;
						TArray<FTransform> transforms;
						bool revCull=false;
						if(ismc && ismc->GetStaticMesh()) {
							meshComp=ismc;
							revCull=ismc->bReverseCulling;
							int32 num=ismc->GetInstanceCount();
							FTransform t;
							for(int32 v=0;v<num;v++) {
								ismc->GetInstanceTransform(v,t,true);
								transforms.Add(t);
							}
						} else {
							UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(c);
							if(smc && smc->GetStaticMesh()) {
								meshComp=smc;
								revCull=meshComp->GetComponentTransform().ToMatrixWithScale().Determinant()<0.0f;
								transforms.Add(smc->GetComponentTransform());
							} else {
								UChildActorComponent* cac=Cast<UChildActorComponent>(c);
								if(cac) {
									if(cac->GetChildActorClass()==AStaticMeshActor::StaticClass()) {
										AStaticMeshActor* ca=Cast<AStaticMeshActor>(cac->GetChildActor());
										if(ca && ca->GetStaticMeshComponent() && ca->GetStaticMeshComponent()->GetStaticMesh()) {
											meshComp=ca->GetStaticMeshComponent();
											revCull=meshComp->GetComponentTransform().ToMatrixWithScale().Determinant()<0.0f;
											transforms.Add(meshComp->GetComponentTransform());
										}
									} else {
										FName sid=rdGetActorSid(cac->GetChildActor());
										FTransform t=cac->GetComponentTransform();
										rdAddSpawnSidTransform(sid,t,sd,bd,bda,transform);
									}
								}
							}
						}
						if(meshComp) {

							FName sid=rdGetAutoInstSidIncludingCullVolumes(meshComp);
							rdAddSpawnSidTransforms(sid,transforms,sd,bd,bda,transform,nullptr,gridX,gridY);
							harvested=true;
						}
					}
				}
			} 
			
			if(!harvested || isAllNonISM) {

				FName sid=rdGetActorSid(i.actorTemplate);
				rdAddSpawnSidTransforms(sid,i.transforms,sd,bd,bda,transform,&i.proxy,gridX,gridY);
//				newSO.pooled=i.pool;
			}
		}
		if(i.vfx) {
			FrdObjectPlacementData pd(i.vfx);
			sd.items.Add(MoveTemp(pd));
			FrdBakedSpawnObjects newSO;
			newSO.vfx=i.vfx;
			for(auto& t:i.transforms) {
				newSO.transformMap.Add(rdWorldToRelativeNoScale(t,transform));
			}
			newSO.itemIndex=sd.items.Num()-1;
			bd.Add(MoveTemp(newSO));
		}
	}

//	newSO.transformMap.Precalc();

	UE_LOG(LogTemp,Display,TEXT("CreateSpawnActor4"));//@@

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION>4
	ssa->SetIsSpatiallyLoaded(spatial);
#endif

	bda->Modify();
	UPackage* package=CastChecked<UPackage>(bda->GetOuter());
	if(package) {
		package->FullyLoad();
		package->SetDirtyFlag(true);
		FEditorFileUtils::PromptForCheckoutAndSave({package},false,false);
	}
#endif

	UE_LOG(LogTemp,Display,TEXT("CreateSpawnActor5"));//@@

	ssa->rdSpawn();

	UE_LOG(LogTemp,Display,TEXT("CreateSpawnActor6"));//@@

	return ssa;
}

//.............................................................................
// rdAsyncLoadEntityAssets
//.............................................................................
void FrdBakedSpawnObjects::rdAsyncLoadEntityAssets() {
}

//.............................................................................
// rdAsyncLoadEntityAssetsFinished
//.............................................................................
void FrdBakedSpawnObjects::rdAsyncLoadEntityAssetsFinished() { 
	loadState=rdLoadState::RDLOADSTATE_FULLYLOADED; 
}

//.............................................................................
