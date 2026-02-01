//
// rdInstBaseActor_Proxies.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 28th Novmber 2023
// Last Modified: 26th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Landscape.h"
#include "rdSpawnActor.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
#include "Components/LightComponent.h"
#include "Engine/Light.h"
#endif

TArray<TSoftObjectPtr<UMaterialInterface>> EmptyMatList;

//.............................................................................
// rdSetupProxies
//.............................................................................
void ArdInstBaseActor::rdSetupProxies(int32 processInterval,double swapDistance,double longDistanceSwapDistance,int32 idleTicks,int32 idleDestructionTicks,float velocityThreshold,bool scanOnlyBaked) {

	proxyProcessInterval=processInterval;
	proxySwapDistance=swapDistance;
	proxyLongDistanceSwapDistance=longDistanceSwapDistance;
	proxyIdleTicks=idleTicks;
	proxyDestructionIdleTicks=idleDestructionTicks;
	proxyCutOffPhysicsVelocity=velocityThreshold;
	bProxyOnlyScanBaked=scanOnlyBaked;

	proxyScanIgnoreActorList.Empty();
	for(TObjectIterator<ALandscape> it;it;++it) {
		proxyScanIgnoreActorList.Add((AActor*)(*it)); // ignore landscape by default
	}
	for(auto& a:proxyScanIgnoreActors) {
		proxyScanIgnoreActorList.Add(a);
	}

	proxyProcessCount=0;
	instancedMeshToProxyMap.Empty();
}

//.............................................................................
// rdRegisterSpawnActor
//.............................................................................
void ArdInstBaseActor::rdRegisterSpawnActor(ArdSpawnActor* spawnActor) {
	spawnActorRegister.AddUnique(spawnActor);
}

//.............................................................................
// rdUnregisterSpawnActor
//.............................................................................
void ArdInstBaseActor::rdUnregisterSpawnActor(ArdSpawnActor* spawnActor) {
	spawnActorRegister.Remove(spawnActor);
}

#ifdef sdkjfhsdf
//.............................................................................
// rdAddProxyToLevel
//.............................................................................
void ArdInstBaseActor::rdAddProxyToLevel(FrdProxyItem& item,int32 index) {

	// Actor Proxy
	if(item.actorClass) {

			if(item.proxyActor && item.savedState && item.savedState->bUseInvoke) {

				UInstancedStaticMeshComponent* ismc=item.instanceVolume?FindISMCforMeshInVolumeX(item.instanceVolume,item.sid):FindISMCforMesh(item.sid);

				rdQueueProxySwapInDelegate(item.proxyActor,item.sid,index,item.ismc,item.savedState);

				if(rdActor) {
					rdActor->rdLoadProxyState(item.savedState,item.proxyActor,ismc->GetStaticMesh(),index);
				} else {
					UFunction* func=item.proxyActor->FindFunction(FName(TEXT("rdLoadProxyState")));
					if(func) {
						struct fParms {
							UrdStateDataAsset* state;
							AActor* proxy;
							UStaticMesh* mesh;
							int32 index;
							ArdActor* prefab;
							UInstancedStaticMeshComponent* ismc;
						};
						fParms parms;
						parms.state=item.savedState;
						parms.proxy=item.proxyActor;
						parms.mesh=ismc->GetStaticMesh();
						parms.index=index;
						parms.prefab=item.prefab;
						parms.ismc=ismc;
						item.proxyActor->ProcessEvent(func,(void*)&parms);
					}
				}
			} else if(item.bCallSwapEvent) {

				UInstancedStaticMeshComponent* ismc=item.instanceVolume?FindISMCforMeshInVolumeX(item.instanceVolume,item.sid):FindISMCforMesh(item.sid);

				rdQueueProxySwapInDelegate(item.proxyActor,item.sid,index,ismc,item.savedState);
				UFunction* func=item.proxyActor->FindFunction(FName(TEXT("rdLoadProxyState")));
				if(func) {
					struct fParms {
						UrdStateDataAsset* state;
						AActor* proxy;
						UStaticMesh* mesh;
						int32 index;
						ArdActor* prefab;
						UInstancedStaticMeshComponent* ismc;
					};
					fParms parms;
					parms.state=item.savedState;
					parms.proxy=item.proxyActor;
					parms.mesh=ismc->GetStaticMesh();
					parms.index=index;
					parms.prefab=item.prefab;
					parms.ismc=ismc;
					item.proxyActor->ProcessEvent(func,(void*)&parms);
				}
			}
		} 
	}
}

//.............................................................................
// rdRemoveProxyFromLevel
//.............................................................................
void ArdInstBaseActor::rdRemoveProxyFromLevel(FrdProxyItem& item,FrdProxyActorItem* pai) {

	AActor* proxyActor=pai?pai->proxyActor:item.proxyActor;
	int32 index=pai?pai->instanceIndex:item.instanceIndex;

	// Proxy Actor
	if(proxyActor && (!bSpawnProxysOnServer || HasAuthority())) {

		UInstancedStaticMeshComponent* ismc=item.instanceVolume?FindISMCforMeshInVolumeX(item.instanceVolume,item.sid):FindISMCforMesh(item.sid);
		if(ismc) {
			if(item.savedState && item.savedState->bUseInvoke) {

				rdQueueProxySwapOutDelegate(item.proxyActor,item.sid,index,ismc,item.savedState);

				ArdActor* rdActor=Cast<ArdActor>(proxyActor);
				if(rdActor) {
					rdActor->rdSaveProxyState(item.savedState,proxyActor,ismc->GetStaticMesh(),index);
				} else {
					UFunction* func=proxyActor->FindFunction(FName(TEXT("rdSaveProxyState")));
					if(func) {
						struct fParms {
							UrdStateDataAsset* state;
							AActor* proxy;
							UStaticMesh* mesh;
							int32 index;
							ArdActor* prefab;
							UInstancedStaticMeshComponent* ismc;
						};
						fParms parms;
						parms.state=item.savedState;
						parms.proxy=proxyActor;
						parms.mesh=ismc->GetStaticMesh();
						parms.index=index;
						parms.prefab=nullptr;
						parms.ismc=ismc;
						item.proxyActor->ProcessEvent(func,(void*)&parms);
					}
				}
			} else if(item.bCallSwapEvent) {

				rdQueueProxySwapOutDelegate(item.proxyActor,item.sid,index,ismc,item.savedState);

				UFunction* func=proxyActor->FindFunction(FName(TEXT("rdSaveProxyState")));
				if(func) {
					struct fParms {
						UrdStateDataAsset* state;
						AActor* proxy;
						UStaticMesh* mesh;
						int32 index;
						ArdActor* prefab;
						UInstancedStaticMeshComponent* ismc;
					};
					fParms parms;
					parms.state=item.savedState;
					parms.proxy=proxyActor;
					parms.mesh=ismc->GetStaticMesh();
					parms.index=index;
					parms.prefab=nullptr;
					parms.ismc=ismc;
					proxyActor->ProcessEvent(func,(void*)&parms);
				}
			}
		}

		item.transform=proxyActor->GetActorTransform();
		if(pai) pai->proxyItem.transform=item.transform;

		if(item.bPooled) {
			rdReturnActorToPool(proxyActor);
		} else {
			proxyActor->Destroy();
		}

		item.proxyActor=nullptr;
		if(pai) pai->proxyActor=nullptr;
	}

	// StaticMesh Instance Proxy
	if(pai && !item.proxyStaticMeshSid.IsNone() && pai->proxyInstanceIndex>=0) { 

		UInstancedStaticMeshComponent* ismc=FindISMCforMesh(item.proxyStaticMeshSid);
		if(ismc) rdRemoveInstanceFast(ismc,pai->proxyInstanceIndex);

	} else if(!item.proxyStaticMeshSid.IsNone() && item.proxyInstanceIndex>=0) { 

		UInstancedStaticMeshComponent* ismc=FindISMCforMesh(item.proxyStaticMeshSid);
		if(ismc) rdRemoveInstanceFast(ismc,item.proxyInstanceIndex);
	}

	item.bHasProxy=false;
	if(pai) pai->proxyItem.bHasProxy=false;
}
#endif

//.............................................................................
// rdAutoSetupProxies
//.............................................................................
void ArdInstBaseActor::rdAutoSetupProxies(UStaticMesh* mesh) {
	rdAutoSetupProxiesX(rdGetSMsid(mesh));
}
void ArdInstBaseActor::rdAutoSetupProxiesX(const FName sid) {

	if(proxyProcessInterval<0) {
		proxyProcessInterval=3;

		proxyProcessCount=0;
		instancedMeshToProxyMap.Empty();
	}
}

//.............................................................................
// rdAddInstanceProxy
//.............................................................................
void ArdInstBaseActor::rdAddInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup& proxy) {
	rdAddInstanceProxyX(rdGetSMsid(mesh),index,proxy);
}
void ArdInstBaseActor::rdAddInstanceProxyX(const FName sid,int32 index,const FrdProxySetup& proxy) {

	if(sid.IsNone()) return;
	FrdProxyItem item(proxy,sid,rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.proxyStaticMesh.Get(),proxy.proxyMeshMaterials,false,proxy.proxyMeshCollision,-1.0f,-1.0f,0),
								 rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.destroyedMesh.Get(),proxy.destroyedMeshMaterials,false,proxy.destroyedMeshCollision,-1.0f,-1.0f,0));

	item.instanceIndex=index;
	item.proxyType=proxy.proxyType;
	item.savedState=proxy.savedState?DuplicateObject(proxy.savedState,this):nullptr;

	FTransform transform;
	item.ismc=FindISMCforMesh(sid);
	if(item.ismc) {
		UStaticMesh* mesh=item.ismc->GetStaticMesh();
		if(mesh) {
			FBoxSphereBounds bnds=mesh->GetBounds();
			FVector& orig=bnds.Origin;
			//FVector& ext=bnds.BoxExtent;
			item.centerOffset=orig;
		}
	}

	if(index>=0) {
		if(item.ismc) {
			item.ismc->GetInstanceTransform(index,transform,true);
		}
	}

	if(proxy.bPooled) {
		rdPoolActor(proxy.proxyActor.LoadSynchronous(),proxy.pooledAmount,true,true,false,true,proxy.bSimplePool);
	}

	if(critProcessEntities.TryLock()) {
		proxySetupQueue.Add(FrdProxySetupQueueItem(proxy.proxyType,sid,nullptr,index,0.0f,item,transform,item.ismc));
		critProcessEntities.Unlock();
	}
}

//.............................................................................
// rdAddLongDistanceInstanceProxy
//.............................................................................
void ArdInstBaseActor::rdAddLongDistanceInstanceProxy(UStaticMesh* mesh,int32 index,double distance,const FrdProxySetup& proxy) {
	rdAddLongDistanceInstanceProxyX(rdGetSMsid(mesh),index,distance,proxy);
}
void ArdInstBaseActor::rdAddLongDistanceInstanceProxyX(const FName sid,int32 index,double distance,const FrdProxySetup& proxy) {

	if(sid.IsNone() || index<0) return;

	FrdProxyItem item(proxy,sid,rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.proxyStaticMesh.Get(),proxy.proxyMeshMaterials,false,proxy.proxyMeshCollision,-1.0f,-1.0f,0),
								 rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.destroyedMesh.Get(),proxy.destroyedMeshMaterials,false,proxy.destroyedMeshCollision,-1.0f,-1.0f,0));

	item.instanceIndex=index;
	item.distance=distance>0.0f?distance:proxyLongDistanceSwapDistance;
	item.proxyType=RDPROXYTYPE_LONGDISTANCE;
	item.savedState=proxy.savedState?DuplicateObject(proxy.savedState,this):nullptr;

	FTransform transform;
	item.ismc=FindISMCforMesh(sid);
	if(item.ismc) {
		item.ismc->GetInstanceTransform(index,transform,true);
	}

	if(proxy.bPooled) {
		rdPoolActor(proxy.proxyActor.LoadSynchronous(),proxy.pooledAmount,true,true,false,true,proxy.bSimplePool);
	}

	if(critProcessEntities.TryLock()) {
		proxySetupQueue.Add(FrdProxySetupQueueItem(proxy.proxyType,sid,nullptr,index,item.distance,item,transform,item.ismc));
		critProcessEntities.Unlock();
	}
}

//.............................................................................
// rdAddLongDistanceActorProxy
//.............................................................................
void ArdInstBaseActor::rdAddLongDistanceActorProxy(AActor* actor,double distance,const FrdProxySetup& proxy) {

	if(!actor) return;

	FrdProxyItem item(proxy,FName(),rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.proxyStaticMesh.Get(),proxy.proxyMeshMaterials,false,proxy.proxyMeshCollision,-1.0f,-1.0f,0),
								 rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.destroyedMesh.Get(),proxy.destroyedMeshMaterials,false,proxy.destroyedMeshCollision,-1.0f,-1.0f,0));

	item.prefab=actor;
	item.proxyType=RDPROXYTYPE_LONGDISTANCE;
	item.distance=(proxy.scanDistance>0.0f)?proxy.scanDistance:proxyLongDistanceSwapDistance;
	item.transform=actor->GetTransform();
	item.savedState=proxy.savedState?DuplicateObject(proxy.savedState,this):nullptr;

	if(proxy.bPooled) {
		rdPoolActor(proxy.proxyActor.LoadSynchronous(),proxy.pooledAmount,true,true,false,true,proxy.bSimplePool);
	}

	if(critProcessEntities.TryLock()) {
		proxySetupQueue.Add(FrdProxySetupQueueItem(proxy.proxyType,NAME_None,actor,-1,distance,item,item.transform,nullptr));
		critProcessEntities.Unlock();
	}
}

//.............................................................................
// rdAddActorProxy
//.............................................................................
void ArdInstBaseActor::rdAddActorProxy(AActor* actor,const FrdProxySetup& proxy) {

	if(!actor) return;

	FrdProxyItem item(proxy,FName(),rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.proxyStaticMesh.Get(),proxy.proxyMeshMaterials,false,proxy.proxyMeshCollision,-1.0f,-1.0f,0),
								 rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.destroyedMesh.Get(),proxy.destroyedMeshMaterials,false,proxy.destroyedMeshCollision,-1.0f,-1.0f,0));

	item.prefab=actor;
	item.transform=actor->GetTransform();
	item.savedState=proxy.savedState?DuplicateObject(proxy.savedState,this):nullptr;

	if(proxy.bPooled) {
		rdPoolActor(proxy.proxyActor.LoadSynchronous(),proxy.pooledAmount,true,true,false,true,proxy.bSimplePool);
	}

	if(critProcessEntities.TryLock()) {
		proxySetupQueue.Add(FrdProxySetupQueueItem(proxy.proxyType,NAME_None,actor,-1,0.0f,item,item.transform,nullptr));
		critProcessEntities.Unlock();
	}
}

//.............................................................................
// rdAddDestructionInstanceProxy
//.............................................................................
void ArdInstBaseActor::rdAddDestructionInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup& proxy) {
	rdAddDestructionInstanceProxyX(rdGetSMsid(mesh),index,proxy);
}
void ArdInstBaseActor::rdAddDestructionInstanceProxyX(const FName sid,int32 index,const FrdProxySetup& proxy) {

	if(sid.IsNone()) return;

	FrdProxyItem item(proxy,sid,rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.proxyStaticMesh.Get(),proxy.proxyMeshMaterials,false,proxy.proxyMeshCollision,-1.0f,-1.0f,0),
								 rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.destroyedMesh.Get(),proxy.destroyedMeshMaterials,false,proxy.destroyedMeshCollision,-1.0f,-1.0f,0));
	item.instanceIndex=index;
	item.bUseWithDestruction=true;
	item.savedState=proxy.savedState?DuplicateObject(proxy.savedState,this):nullptr;

	item.ismc=FindISMCforMesh(sid);
	if(item.ismc && index>=0) {
		item.ismc->GetInstanceTransform(index,item.transform,true);
	}

	if(proxy.bPooled) {
		rdPoolActor(proxy.proxyActor.LoadSynchronous(),proxy.pooledAmount,true,true,false,true,proxy.bSimplePool);
	}

	if(critProcessEntities.TryLock()) {
		proxySetupQueue.Add(FrdProxySetupQueueItem(3,sid,nullptr,index,0.0f,item,item.transform,item.ismc));
		critProcessEntities.Unlock();
	}
}

//.............................................................................
// rdAddDestructionActorProxy
//.............................................................................
void ArdInstBaseActor::rdAddDestructionActorProxy(AActor* actor,const FrdProxySetup& proxy) {

	if(!actor) return;

	FrdProxyItem item(proxy,FName(),rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.proxyStaticMesh.Get(),proxy.proxyMeshMaterials,false,proxy.proxyMeshCollision,-1.0f,-1.0f,0),
								 rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.destroyedMesh.Get(),proxy.destroyedMeshMaterials,false,proxy.destroyedMeshCollision,-1.0f,-1.0f,0));

	item.prefab=actor;
	item.transform=actor->GetTransform();
	item.bUseWithDestruction=true;
	item.savedState=proxy.savedState?DuplicateObject(proxy.savedState,this):nullptr;
	item.bDestroyed=false;

	if(proxy.bPooled) {
		rdPoolActor(proxy.proxyActor.LoadSynchronous(),proxy.pooledAmount,true,true,false,true,proxy.bSimplePool);
	}

	if(critProcessEntities.TryLock()) {
		proxySetupQueue.Add(FrdProxySetupQueueItem(3,NAME_None,actor,-1,0.0f,item,item.transform,nullptr));
		critProcessEntities.Unlock();
	}
}

//.............................................................................
// rdGetProxyItemFromInstance
//.............................................................................
FrdProxyItem* ArdInstBaseActor::rdGetProxyItemFromInstance(const FName sid,int32 index) {

	FrdProxyItem* item=nullptr;

	// per-instance proxy
	FrdProxyInstanceItems* items=instancesToProxyMap.Find(sid);
	if(items) {
		item=items->proxyInstanceIndexList.Find(index);
		if(item) {
			return item;
		}
	}

	// all-instance proxy
	item=instancedMeshToProxyMap.Find(sid);
	if(item) {
		return item;
	}

	// long distance instance proxy
	FrdProxyGridArray* pii=longDistanceInstancesToProxyMap.Find(sid); 
	if(pii) {
		FrdProxyInstanceGridItem* pgi=pii->FindByInstanceIndex(index);
		if(pgi) {
			item=&pgi->proxy;
			return item;
		}
	}

	return nullptr;
}

//.............................................................................
// rdGetProxyItemFromActor
//.............................................................................
FrdProxyItem* ArdInstBaseActor::rdGetProxyItemFromActor(const AActor* actor) {

	FrdProxyItem* item=nullptr;

	// prefab proxy
	item=actorsToProxyMap.Find(actor);
	if(item) {
		return item;
	}

	// long distance prefab proxy
//	FrdProxyGridArray* pii=longDistanceActorsToProxyMap.Find(actor);
	FrdProxyItem* pii=longDistanceActorsToProxyMap.Find(actor);
	if(pii) {
		//FrdProxyInstanceGridItem* pgi=pii->FindByPrefab(index);
		//if(pgi) {
		//	item=&pgi->proxy;
		//	return item;
		//}
	}

	return nullptr;
}

//.............................................................................
// rdGetProxyItem
//.............................................................................
FrdProxyItem* ArdInstBaseActor::rdGetProxyItem(FrdProxyActorItem* proxyActorItem) {

	FrdProxyItem* item=nullptr;

	switch(proxyActorItem->proxySource) {
		case 0: { // per-instance proxy
			FrdProxyInstanceItems* items=instancesToProxyMap.Find(proxyActorItem->proxyItem.sid);
			if(items) {
				item=items->proxyInstanceIndexList.Find(proxyActorItem->instanceIndex);
			}
			if(item) break; 
		}
		case 1: { // all-instance proxy
			item=instancedMeshToProxyMap.Find(proxyActorItem->proxyItem.sid);
			break; }
		case 2: { // long distance instance proxy
			FrdProxyGridArray* pii=longDistanceInstancesToProxyMap.Find(proxyActorItem->proxyItem.sid);
			if(pii) {
				FrdProxyInstanceGridItem* pgi=pii->FindByInstanceIndex(proxyActorItem->instanceIndex);
				if(pgi) {
					item=&pgi->proxy;
				}
			}
			break; }
		case 3: { // actor proxy
			item=actorsToProxyMap.Find(proxyActorItem->proxyItem.prefab);
			break; }
		case 4: { // long distance actor proxy
			//FrdProxyGridArray* pii=longDistanceActorsToProxyMap.Find(proxyActorItem->proxyItem.prefab);
			FrdProxyItem* pii=longDistanceActorsToProxyMap.Find(proxyActorItem->proxyItem.prefab);
			if(pii) {
				//FrdProxyInstanceGridItem* pgi=pii->FindByInstanceIndex(proxyActorItem->instanceIndex);
				//if(pgi) {
					item=pii;//&pgi->proxy;
				//}
			}
			break; }
	}

	return item;
}

//.............................................................................
// rdMarkProxyAsDestroyed
//.............................................................................
void ArdInstBaseActor::rdMarkProxyAsDestroyed(AActor* proxy,const FTransform& offset) {

	FrdProxyActorItem* pai=proxyActorMap.Find(proxy);
	if(pai) {
		pai->proxyItem.bDestroyed=true;
		FrdProxyItem* rpi=rdGetProxyItem(pai);
		if(rpi) rpi->bDestroyed=true;
	}
}

//.............................................................................
// rdSetProxyToDestroyed
//.............................................................................
void ArdInstBaseActor::rdSetProxyToDestroyed(AActor* proxy,const FTransform& offset) {

	FrdProxyActorItem* pai=proxyActorMap.Find(proxy);
	if(pai) {

		FTransform atran=proxy->GetActorTransform();

		if(!pai->proxyItem.destroyedSid.IsNone()) {
			FTransform t(pai->proxyItem.destroyedOffset);
			t.Accumulate(offset);
			t.Accumulate(atran);
			UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(pai->proxyItem.destroyedSid);
			if(ismc) rdAddInstanceRaw(ismc,t);
		}

		if(pai->proxyItem.destroyedPrefab) {
			FTransform t(pai->proxyItem.destroyedOffset);
			t.Accumulate(offset);
			t.Accumulate(atran);
			rdSpawnActor(pai->proxyItem.destroyedPrefab,t);
		}

		rdRemoveProxyByProxy(proxy);
	}
}

//.............................................................................
// rdRemoveProxyByProxy
//.............................................................................
void ArdInstBaseActor::rdRemoveProxyByProxy(AActor* proxy,bool bKeepPinned) {

	FrdProxyActorItem* pai=proxyActorMap.Find(proxy);
	if(pai) {
		if(critProcessEntities.TryLock()) {
			removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
			critProcessEntities.Unlock();
		}
	}
}

//.............................................................................
// rdRemoveProxyByDataLayer
//.............................................................................
void ArdInstBaseActor::rdRemoveProxyByDataLayer(TSoftObjectPtr<UDataLayerAsset> dla) {

	FrdProxyActorItem* pai=proxyDataLayerMap.Find(dla);
	if(pai) {
		if(critProcessEntities.TryLock()) {
			removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
			critProcessEntities.Unlock();
		}
	}
}

//.............................................................................
// rdRemoveProxyByProxyISM
//
// This is Keyed on the main Instances Index rather than the Proxy ISMs index
//  so it can easily be searched for by the main instance.
//
//.............................................................................
void ArdInstBaseActor::rdRemoveProxyByProxyISM(UStaticMesh* mesh,int32 index) {
	rdRemoveProxyByProxyISMX(rdGetSMsid(mesh),index);
}
void ArdInstBaseActor::rdRemoveProxyByProxyISMX(const FName sid,int32 index) {

	TMap<int32,FrdProxyActorItem>* instMap=proxyISMMap.Find(sid);
	if(!instMap) return;
	FrdProxyActorItem* pai=instMap->Find(index);
	if(pai) {
		if(critProcessEntities.TryLock()) {
			removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
			critProcessEntities.Unlock();
		}
	}
}

//.............................................................................
// rdRemoveProxyByInstance
//.............................................................................
void ArdInstBaseActor::rdRemoveProxyByInstance(UStaticMesh* mesh,int32 index) {
	rdRemoveProxyByInstanceX(rdGetSMsid(mesh),index);
}
void ArdInstBaseActor::rdRemoveProxyByInstanceX(const FName sid,int32 index) {

	FrdProxyInstanceItems* pii=instancesToProxyMap.Find(sid);
	if(pii) {
		FrdProxyInstanceItems& items=*pii;
		FrdProxyItem* pi=items.proxyInstanceIndexList.Find(index);
		if(pi) {
			if(critProcessEntities.TryLock()) {
				removeProxyQueue.Add(ProxyRemoveQueueItem(pi->proxyType,*pi,nullptr));
				critProcessEntities.Unlock();
			}
		}
	}
}

//.............................................................................
// rdRemoveLongDistanceProxyByProxy
//.............................................................................
void ArdInstBaseActor::rdRemoveLongDistanceProxyByProxy(AActor* proxy) {

	FrdProxyActorItem* pai=proxyActorMap.Find(proxy);
	if(pai) {
		if(pai) {
			if(critProcessEntities.TryLock()) {
				removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
				critProcessEntities.Unlock();
			}
		}
	}
}

//.............................................................................
// rdRemoveProxyByActor
//.............................................................................
void ArdInstBaseActor::rdRemoveProxyByActor(AActor* actor) {

	FrdProxyItem* pi=actorsToProxyMap.Find(actor);
	if(pi) {
		FrdProxyItem& item=*pi;
		FrdProxyActorItem* pai=proxyActorMap.Find(item.proxyActor);
		if(pai) {
			if(critProcessEntities.TryLock()) {
				removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
				critProcessEntities.Unlock();
			}
		}
	}
}

//.............................................................................
// rdRemoveAllProxies
//.............................................................................
void ArdInstBaseActor::rdRemoveAllProxies() {

	for(auto& items:instancesToProxyMap) {
		for(auto& item:items.Value.proxyInstanceIndexList) {
			FrdProxyActorItem* pai=proxyActorMap.Find(item.Value.proxyActor);
			if(pai) {
				if(critProcessEntities.TryLock()) {
					removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
					critProcessEntities.Unlock();
				}
			}
		}
	}
	instancesToProxyMap.Empty();

	for(auto& it:instancedMeshToProxyMap) {
		FrdProxyItem& item=it.Value;
		FrdProxyActorItem* pai=proxyActorMap.Find(item.proxyActor);
		if(pai) {
			if(critProcessEntities.TryLock()) {
				removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
				critProcessEntities.Unlock();
			}
		}
	}
	instancedMeshToProxyMap.Empty();

	for(auto& item:actorsToProxyMap) {
		FrdProxyActorItem* pai=proxyActorMap.Find(item.Value.proxyActor);
		if(pai) {
			if(critProcessEntities.TryLock()) {
				removeProxyQueue.Add(ProxyRemoveQueueItem(pai->proxyItem.proxyType,pai->proxyItem,pai));
				critProcessEntities.Unlock();
			}
		}
	}
	actorsToProxyMap.Empty();
}

//.............................................................................
// rdSwapInstanceToProxyActor
//
// Swaps the ISM over to the Proxy it has assigned - can be an Actor, Niagara, 
//   another ISM and soon a physics enabled SM component.
//
//.............................................................................
void ArdInstBaseActor::rdSwapInstanceToProxyActor(UStaticMesh* mesh,int32 index,const FTransform& transform,bool forImpact,bool forLongDistance) {
	rdSwapInstanceToProxyActorX(rdGetSMsid(mesh),index,transform,forImpact,forLongDistance);
}
void ArdInstBaseActor::rdSwapInstanceToProxyActorX(const FName sid,int32 index,const FTransform& transform,bool forImpact,bool forLongDistance) {

	FrdProxyItem* pi=nullptr;
	bool forAllMesh=false;
	if(forLongDistance) {
		FrdProxyGridArray* pii=longDistanceInstancesToProxyMap.Find(sid);
		if(pii) {
			FrdProxyInstanceGridItem* pgi=pii->FindByInstanceIndex(index);
			if(pgi) {
				pi=&pgi->proxy;
			}
		}
	} else {
		pi=instancedMeshToProxyMap.Find(sid);
		if(pi) {
			// all mesh proxies
			if(pi->instanceIndex==-1) {
				if(pi->swappedInstances.Contains(index)) { // only contains indexes close to player, so small
					return;
				}
				pi->transform=transform;
				pi->proxyActor=nullptr; // gets reused for the "entire mesh" proxy
				pi->bHasProxy=false;
			}
			forAllMesh=true;
		} else {
			// single instance proxies
			FrdProxyInstanceItems* pii=instancesToProxyMap.Find(sid);
			if(pii) {
				FrdProxyInstanceItems& items=*pii;
				pi=items.proxyInstanceIndexList.Find(index);
			}
		}
	}

	if(pi && !pi->bDestroyed) {
		
		FrdProxyItem& item=*pi;
		
		if((forImpact && !item.bUseWithDestruction) || (!forImpact && (item.proxyType!=RDPROXYTYPE_SHORTDISTANCE && item.proxyType!=RDPROXYTYPE_PICKUP) && !forLongDistance)) {
			return;
		}

		if(item.proxyActor || item.bHasProxy) {
			if(!forImpact) {
				if(item.proxyActor) proxyActorMap[item.proxyActor].idleCountdown=proxyIdleTicks;
			}
			return;
		}

		if(!bSpawnProxysOnServer || HasAuthority()) {

			if(critProcessEntities.TryLock()) {
				swapProxyActorQueue.Add(FrdProxyActorItem(sid,index,item,forImpact,forLongDistance?2:forAllMesh?1:0));
				critProcessEntities.Unlock();
			}
		}

		//TODO: try spawning directly on the game thread like below and test difference in timing
//		AsyncTask(ENamedThreads::GameThread, [Data = MoveTemp(Data)] {
//		});
	}
}

void ArdInstBaseActor::rdSwapInstanceToProxyActorWithHitX(const FName sid,int32 index,const FTransform& transform,FHitResult& hit) {

	FrdProxyItem* pi=nullptr;
	bool forAllMesh=false;

	pi=instancedMeshToProxyMap.Find(sid);
	if(pi) {
		// all mesh proxies
		if(pi->instanceIndex==-1) {
			if(pi->swappedInstances.Contains(index)) { // only contains indexes close to player, so small
				return;
			}
			pi->transform=transform;
			pi->proxyActor=nullptr; // gets reused for the "entire mesh" proxy
			pi->bHasProxy=false;
		}
		forAllMesh=true;
	} else {
		// single instance proxies
		FrdProxyInstanceItems* pii=instancesToProxyMap.Find(sid);
		if(pii) {
			FrdProxyInstanceItems& items=*pii;
			pi=items.proxyInstanceIndexList.Find(index);
		}
	}

	if(pi && !pi->bDestroyed) {
		
		FrdProxyItem& item=*pi;
		
		if(!item.bUseWithDestruction) {
			return;
		}

		if(item.proxyActor || item.bHasProxy) {
			return;
		}

		if(!bSpawnProxysOnServer || HasAuthority()) {

			if(critProcessEntities.TryLock()) {
				swapProxyActorQueue.Add(FrdProxyActorItem(sid,index,item,true,0,&hit));
				critProcessEntities.Unlock();
			}
		}
	}
}

//.............................................................................
// processSwapToProxyActor
//.............................................................................
void ArdInstBaseActor::processSwapToProxyActor(const FrdProxyActorItem& proxy) {

	static FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));

	FrdProxyItem* pi=rdGetProxyItem((FrdProxyActorItem*)&proxy);
	if(!pi || pi->bDestroyed) {
		return;
	}

	bool forAllMesh=false;
	if(proxy.proxySource<3 && pi->instanceIndex==-1) {
		if(pi->swappedInstances.Contains(proxy.instanceIndex)) { // only contains indexes close to player, so small
			return;
		}
		pi->transform=proxy.instanceTransform;
		pi->proxyActor=nullptr; // gets reused for the "entire mesh" proxy
		pi->bHasProxy=false;
		forAllMesh=true;
	}

	FrdProxyItem& item=*pi;

	if((proxy.forImpact && !item.bUseWithDestruction) || (!proxy.forImpact && (item.proxyType!=RDPROXYTYPE_SHORTDISTANCE && item.proxyType!=RDPROXYTYPE_PICKUP) && !proxy.forLongDistance)) {
		return;
	}
	if(item.proxyActor || item.bHasProxy) {
		if(!proxy.forImpact) {
			if(item.proxyActor) proxyActorMap[item.proxyActor].idleCountdown=proxyIdleTicks;
		}
		return;
	}

	if(!item.bDontHide) {
		if(proxy.proxySource<3) {
			// ISM
			TArray<TTuple<int32,FTransform>>& moveArray=moveMap.FindOrAdd(proxy.sid);
			moveArray.Add(TTuple<int32,FTransform>(proxy.instanceIndex,ztran));
		} else {
			// prefab
			if(item.prefab)	{
				ArdActor* rdActor=Cast<ArdActor>(item.prefab);
				if(rdActor) {
					rdActor->rdBuildAsHidden();//TODO: is setvis threadsafe? doubt it
				} else {
					item.prefab->SetHidden(true);
				}
			}
		}
	} 
	if(forAllMesh) {
		item.swappedInstances.Add(proxy.instanceIndex);
	}

	if(!item.bHasProxy) {

		FrdProxyActorItem pai(item);
		pai.instanceIndex=proxy.instanceIndex;
		if(item.proxyDataLayer.IsValid()) {
			pai.sid=rdGetDataLayerSid(item.proxyDataLayer,NAME_None);
		} else if(item.actorClass) {
			pai.sid=rdGetActorSid(item.actorClass,FString(),NAME_None);
		} else if(item.proxyStaticMeshSid.IsValid()) {
			pai.sid=item.proxyStaticMeshSid;
		}
		pai.proxySource=proxy.proxySource;
		pai.idleCountdown=proxy.forImpact?proxyDestructionIdleTicks:proxyIdleTicks;
		pai.spawnMillis=FDateTime::Now().GetMillisecond();
//		pai.ismc=item.ismc;
		
		TArray<FAddProxyQueueItem>& addArray=addProxyMap.FindOrAdd(pai.sid);
		addArray.Add(FAddProxyQueueItem(pai,item.transform));

		item.bHasProxy=true;
	}
}

//.............................................................................
// processSwapOutProxyActor
//.............................................................................
void ArdInstBaseActor::processSwapOutProxyActor(const FrdProxyActorItem& proxy) {

	FrdProxyItem* pi=rdGetProxyItem((FrdProxyActorItem*)&proxy);
	if(!pi || pi->bDestroyed) {
		return;
	}

	if(proxy.instanceIndex>=0) {
		pi->swappedInstances.Remove(proxy.instanceIndex);
	}

	FrdProxyItem& item=*pi;

	if(proxy.proxySource<3) {
		// ISM
		TArray<TTuple<int32,FTransform>>& moveArray=moveMap.FindOrAdd(proxy.sid);
		moveArray.Add(TTuple<int32,FTransform>(proxy.instanceIndex,item.transform));
	} else {
		// prefab
		if(item.prefab)	{
			ArdActor* rdActor=Cast<ArdActor>(item.prefab);
			if(rdActor) {
				rdActor->rdBuildAsHidden();//TODO: is setvis threadsafe? doubt it
			} else {
				item.prefab->SetHidden(true);
			}
		}
	}

	FrdProxyActorItem pai(item);
	pai.instanceIndex=proxy.instanceIndex;
	if(item.proxyDataLayer.IsValid()) {
		pai.sid=rdGetDataLayerSid(item.proxyDataLayer,NAME_None);
	} else if(item.actorClass) {
		pai.sid=rdGetActorSid(item.actorClass,FString(),NAME_None);
	} else if(item.proxyStaticMeshSid.IsValid()) {
		pai.sid=item.proxyStaticMeshSid;
	}
	pai.proxySource=proxy.proxySource;
	pai.idleCountdown=proxy.forImpact?proxyDestructionIdleTicks:proxyIdleTicks;
	//pai.spawnMillis=FDateTime::Now().GetMillisecond();

	TArray<FrdProxyActorItem>& remArray=remProxyMap.FindOrAdd(proxy.sid);
	remArray.Add(FrdProxyActorItem(pai.sid,proxy.instanceIndex,item,false,0));

	item.bHasProxy=false;
}

//.............................................................................
// rdSwapActorToProxyActor
//.............................................................................
void ArdInstBaseActor::rdSwapActorToProxyActor(AActor* actor,int32 index,bool forImpact,bool forLongDistance) {

	FrdProxyItem* pi=nullptr;

	if(forLongDistance) {
		/* //TODO:
		FrdProxyGridArray* pii=longDistancePrefabsToProxyMap.Find(actor);
		if(pii) {
			FrdProxyInstanceGridItem* pgi=pii->FindByInstanceIndex(index);
			if(pgi) {
				pi=&pgi->proxy;
			}
		}
		*/
	} else {
		pi=actorsToProxyMap.Find(actor);
	}

	if(pi) {
		FrdProxyItem& item=*pi;
		if((forImpact && !item.bUseWithDestruction) || (!forImpact && item.proxyType!=RDPROXYTYPE_SHORTDISTANCE)) return;
		if(item.proxyActor || item.bHasProxy) {
			if(!forImpact) {
				if(item.proxyActor) proxyActorMap[item.proxyActor].idleCountdown=proxyIdleTicks;//TODO: thread writing to idleCountdown
			}
			return;
		}

		if(critProcessEntities.TryLock()) {
			swapProxyActorQueue.Add(FrdProxyActorItem(item.sid,item.instanceIndex,item,forImpact,forLongDistance?4:3));
			critProcessEntities.Unlock();
		}
	}
}

//.............................................................................
// rdSwapProxyActorToInstance
//.............................................................................
void ArdInstBaseActor::rdSwapProxyActorToInstance(AActor* proxy) {

	FrdProxyActorItem* pai=proxyActorMap.Find(proxy);
	if(pai) {
		FrdProxyItem* rpi=rdGetProxyItem(pai);
		if(rpi) {
			if(critProcessEntities.TryLock()) {
				swapOutProxyActorQueue.Add(FrdProxyActorItem(rpi->sid,rpi->instanceIndex,*rpi,false,0));
				critProcessEntities.Unlock();
			}
		}
	}
}

//.............................................................................
// rdSwapProxyActorToActor
//.............................................................................
void ArdInstBaseActor::rdSwapProxyActorToActor(AActor* proxy) {

	FrdProxyActorItem* pai=proxyActorMap.Find(proxy);
	if(pai) {

		FrdProxyItem* rpi=rdGetProxyItem(pai);
		if(rpi) {
			if(critProcessEntities.TryLock()) {
				swapOutProxyActorQueue.Add(FrdProxyActorItem(rpi->sid,rpi->instanceIndex,*rpi,false,2));
				critProcessEntities.Unlock();
			}
		}
	}
}

//.............................................................................
// rdProcessImpactProxies
//
// Finds any instances around the impact point with rdProxies and swaps them
//  to their Proxies. An rdOnProxyDamaged event is fired for the proxy associated
//  with the instance that got the collision
//.............................................................................
void ArdInstBaseActor::rdProcessImpactProxies(const AActor* caller,const FVector& loc,const FHitResult& hit,float distance,FVector impulse,USceneComponent* comp,int32 index) {

	TArray<FHitResult> hits;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindProxies),false,nullptr);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	QueryParams.bTraceIntoSubComponents=false;
#endif
	const ECollisionChannel CollisionChannel=ECC_Visibility;
	FCollisionResponseParams ResponseParams(ECR_Overlap);
	
	bool ret=GetWorld()->SweepMultiByChannel(hits,loc,loc,FQuat::Identity,CollisionChannel,FCollisionShape::MakeSphere(distance<0.001f?proxyImpactScanDistance:distance),QueryParams);

	DamageQueue dq;

	if(ret && hits.Num()>0) {


		for(auto& h:hits) {
			AActor* hitActor=h.GetActor();
			USceneComponent* hitComponent=h.GetComponent(); 
			UInstancedStaticMeshComponent* hitISM=Cast<UInstancedStaticMeshComponent>(hitComponent);

			if(hitISM) {
				int32 instanceIndex=h.Item;
				FTransform t;
				hitISM->GetInstanceTransform(instanceIndex,t,true);

				FName sid=rdGetSMCsid(hitISM);
				dq.Add((AActor*)caller,sid,instanceIndex,t,(FVector&)impulse,(FHitResult&)hit);

			} else if(hitActor) {

				ArdActor* hitRdActor=Cast<ArdActor>(hitActor);
				if(hitRdActor) {
					FTransform t=hitRdActor->GetActorTransform();
					dq.Add((AActor*)caller,NAME_None,hitRdActor,t,impulse,h); //TODO: this can potentially get called for all ISMs in a prefab within the scan distance, so could end up being quite dense - think of a way to optimize this (e.g. make prefabs only include a collision mesh or something)
				}
			}
		}

		if(dq.queueMap.Num()>0 && critProcessEntities.TryLock()) {
			damageProxyActorQueue.Add(dq);
			critProcessEntities.Unlock();
		}
	}
}

//.............................................................................
// rdSwapInactiveProxy
//.............................................................................
void ArdInstBaseActor::rdSwapInactiveProxy(FrdProxyActorItem& a,const FVector& aloc,TArray<AActor*>& removeProxyActorList,TArray<TSoftObjectPtr<UDataLayerAsset>>& removeProxyDataLayerList,TMap<FName,TArray<int32>>& removeProxyISMList) {

	if(a.idleCountdown>0) {
		if(a.proxyActor || a.proxyItem.bHasProxy) {

			bool isOutsideProxyArea=false;
			bool isPickup=a.proxyItem.proxyType==RDPROXYTYPE_PICKUP;

			if(a.proxyActor) {
				bool isSimulating=false;
				TArray<UActorComponent*> comps;
#if ENGINE_MAJOR_VERSION>4
				a.proxyActor->GetComponents(UPrimitiveComponent::StaticClass(),comps,false);
#else
				comps=a.proxyActor->GetComponentsByClass(UPrimitiveComponent::StaticClass());
#endif
				uint64 millis=FDateTime::Now().GetMillisecond();
				for(auto& c:comps) {
					float span=(((float)millis)/1000.0f)-(((float)a.spawnMillis)/1000.0f);
					if(	FVector::Distance(((UPrimitiveComponent*)c)->GetPhysicsLinearVelocity(),FVector(0,0,0))>proxyCutOffPhysicsVelocity && 
						FVector::Distance(((UPrimitiveComponent*)c)->GetPhysicsAngularVelocityInRadians(),FVector(0,0,0))>proxyCutOffPhysicsVelocity &&
						span<a.proxyItem.proxyPhysicsTimeout) { 
							isSimulating=true; 
							break; 
					}
				}
				if(!isSimulating && FVector::Distance(a.proxyActor->GetActorLocation(),aloc)>((a.proxyItem.distance>0.0f?a.proxyItem.distance:proxySwapDistance)+200.0f)) {
					isOutsideProxyArea=true;
				}
			} else {

				if(FVector::Distance(a.proxyItem.transform.GetTranslation(),aloc)>((a.proxyItem.distance>0.0f?a.proxyItem.distance:proxySwapDistance)+(isPickup?0.0f:200.0f))) {
					isOutsideProxyArea=true;
				}
			}

			if(isOutsideProxyArea) {

				a.idleCountdown--;
				if(isPickup) a.idleCountdown=0;
				if(a.idleCountdown==0) { // swap back to proxy

					if(a.proxyActor) {
						removeProxyActorList.Add(a.proxyActor);
						a.proxyItem.transform=a.instanceTransform=a.proxyActor->GetActorTransform();
					} else if(a.proxyItem.proxyDataLayer.Get()) removeProxyDataLayerList.Add(a.proxyItem.proxyDataLayer);
					else if(!a.proxyItem.proxyStaticMeshSid.IsNone()) {
						TArray<int32>& indexes=removeProxyISMList.FindOrAdd(a.proxyItem.proxyStaticMeshSid);
						indexes.Add(a.proxyItem.proxyInstanceIndex);
					}
					FrdProxyItem* rpi=rdGetProxyItem(&a);

					if(rpi) {

						if(rpi->bCallSwapEvent) {
							rdQueueProxySwapOutDelegate(a.proxyActor,rpi->sid,a.instanceIndex,a.ismc,rpi->savedState);
						}
						if(isPickup && rpi->bHasProxy && rpi->pickup.highlightType==RDPICKUPHIGHLIGHT_LOOKATOUTLINE) {
							rdQueuePickupUnfocusedDelegate(rpi->pickup.id,rpi->transform,rpi->sid,a.instanceIndex);
						}

						// Remove the Proxy from the Level
						FrdProxyItem& item=*rpi;
						TArray<FrdProxyActorItem>& remArray=remProxyMap.FindOrAdd(item.sid);
						remArray.Add(a);

						if(!a.bDontHide && !item.bDestroyed) {
							if(item.instanceVolume) {
								//TODO: 
								//UInstancedStaticMeshComponent* ismc=FindISMCforMeshInVolumeX(item.instanceVolume,item.sid);
								//if(ismc) {
								//	rdUpdateTransformFast(ismc,a.instanceIndex,a.proxyItem.transform);
								//}
							} else {
								if(item.prefab) {
									showActorList.Add(TTuple<AActor*,FTransform>(item.prefab,a.instanceTransform));
								}
								if(!item.sid.IsNone()) {
									TArray<TTuple<int32,FTransform>>& moveArray=moveMap.FindOrAdd(item.sid);
									moveArray.Add(TTuple<int32,FTransform>(a.instanceIndex,a.instanceTransform));
								}
								item.swappedInstances.Remove(a.instanceIndex);
								item.bHasProxy=false;
							}
						} else if(a.bDontHide) {
							item.swappedInstances.Remove(a.instanceIndex);
						}
					}
				}
			}
		}
	}
}

//.............................................................................
// rdMakeActorProxySetup
//.............................................................................
FrdProxySetup ArdInstBaseActor::rdMakeActorProxySetup(UClass* proxyActor,bool bDontHide,bool bDontRemove,float proxyPhysicsTimeout,bool bPooled,bool bSimplePool,int32 pooledAmount,UrdStateDataAsset* savedState,bool bCallSwapEvent) {
	TArray<TSoftObjectPtr<UMaterialInterface>> ary;
	return FrdProxySetup(RDPROXYTYPE_SHORTDISTANCE,proxyActor,nullptr,nullptr,ary,ErdCollision::UseDefault,nullptr,ary,ErdCollision::UseDefault,nullptr,FTransform(),0.0f,proxyPhysicsTimeout,false,bPooled,bSimplePool,pooledAmount,bDontRemove,bDontHide,false,savedState,bCallSwapEvent,FrdPickup());
}
//.............................................................................
// rdMakeDataLayerProxySetup
//.............................................................................
FrdProxySetup ArdInstBaseActor::rdMakeDataLayerProxySetup(TSoftObjectPtr<UDataLayerAsset> proxyDataLayer,bool bDontHide,bool bDontRemove,UrdStateDataAsset* savedState,bool bCallSwapEvent) {
	TArray<TSoftObjectPtr<UMaterialInterface>> ary;
	return FrdProxySetup(RDPROXYTYPE_SHORTDISTANCE,nullptr,proxyDataLayer,nullptr,ary,ErdCollision::UseDefault,nullptr,ary,ErdCollision::UseDefault,nullptr,FTransform(),0.0f,30.0f,false,false,false,0,bDontRemove,bDontHide,false,savedState,bCallSwapEvent,FrdPickup());

}

//.............................................................................
// rdMakeStaticMeshProxySetup
//.............................................................................
FrdProxySetup ArdInstBaseActor::rdMakeStaticMeshProxySetup(UStaticMesh* proxyMesh,TArray<TSoftObjectPtr<UMaterialInterface>> materials,bool reverseCulling,TEnumAsByte<ErdCollision> collision,bool bDontHide,bool bDontRemove,UrdStateDataAsset* savedState,bool bCallSwapEvent) {
	TArray<TSoftObjectPtr<UMaterialInterface>> ary;
	return FrdProxySetup(RDPROXYTYPE_SHORTDISTANCE,nullptr,nullptr,proxyMesh,materials,collision,nullptr,ary,ErdCollision::UseDefault,nullptr,FTransform(),0.0f,30.0f,false,false,false,0,bDontRemove,bDontHide,false,savedState,bCallSwapEvent,FrdPickup());
}

//.............................................................................
// rdAddDestructionToProxySetup
//.............................................................................
FrdProxySetup ArdInstBaseActor::rdAddDestructionToProxySetup(const FrdProxySetup& inProxySetup,UStaticMesh* destroyedMesh,UClass* destroyedPrefab,const FTransform& destroyedOffset) {
	FrdProxySetup* p=new FrdProxySetup(inProxySetup);
	p->destroyedMesh=destroyedMesh;
	p->destroyedPrefab=destroyedPrefab;
	p->destroyedOffset=destroyedOffset;
	return *p;
}

//.............................................................................
// AddProxiesForInstances
//.............................................................................
void ArdInstBaseActor::AddProxiesForInstances(const FName sid,const FrdProxySetup& proxy,const TArray<int32>& instIndexList) {

	switch(proxy.proxyType) {
		case RDPROXYTYPE_SHORTDISTANCE:
			if(proxy.bEntireMeshProxy) {
				rdAddInstanceProxyX(sid,-1,proxy);
				if(proxy.bUseWithDestruction) {
					rdAddDestructionInstanceProxyX(sid,-1,proxy);
				}
			} else {
				for(auto i:instIndexList) {
					rdAddInstanceProxyX(sid,i,proxy);
					if(proxy.bUseWithDestruction) {
						rdAddDestructionInstanceProxyX(sid,i,proxy);
					}
				}
			}
			break;
		case RDPROXYTYPE_LONGDISTANCE:
			for(auto i:instIndexList) {
				rdAddLongDistanceInstanceProxyX(sid,i,proxy.scanDistance,proxy);
				if(proxy.bUseWithDestruction) {
					rdAddDestructionInstanceProxyX(sid,i,proxy);
				}
			}
			break;
		case RDPROXYTYPE_DESTRUCTION:
			if(proxy.bEntireMeshProxy) {
				rdAddDestructionInstanceProxyX(sid,-1,proxy);
			} else {
				for(auto i:instIndexList) {
					rdAddDestructionInstanceProxyX(sid,i,proxy);
				}
			}
			break;
		case RDPROXYTYPE_PICKUP:
			rdAddPickupProxyX(sid,-1,proxy.pickup);
			break;
	}
}

//.............................................................................
// AddProxyForInstance
//.............................................................................
void ArdInstBaseActor::AddProxyForInstance(const FName sid,const FrdProxySetup& proxy,const int32 instIndex) {

	switch(proxy.proxyType) {
		case RDPROXYTYPE_SHORTDISTANCE:
			rdAddInstanceProxyX(sid,proxy.bEntireMeshProxy?-1:instIndex,proxy);
			if(proxy.bUseWithDestruction) {
				rdAddDestructionInstanceProxyX(sid,-1,proxy);
			}
			break;
		case RDPROXYTYPE_LONGDISTANCE:
			rdAddLongDistanceInstanceProxyX(sid,instIndex,proxy.scanDistance,proxy);
			if(proxy.bUseWithDestruction) {
				rdAddDestructionInstanceProxyX(sid,instIndex,proxy);
			}
			break;
		case RDPROXYTYPE_DESTRUCTION:
			rdAddDestructionInstanceProxyX(sid,instIndex,proxy);
			break;
		case RDPROXYTYPE_PICKUP:
			rdAddPickupProxyX(sid,-1,proxy.pickup);
			break;
	}
}

//.............................................................................
// AddProxyForActor
//.............................................................................
void ArdInstBaseActor::AddProxyForActor(AActor* actor,const FrdProxySetup& proxy) {
	if(!actor) return;
	switch(proxy.proxyType) {
		case RDPROXYTYPE_SHORTDISTANCE:
			rdAddActorProxy(actor,proxy);
			if(proxy.bUseWithDestruction) {
				rdAddDestructionActorProxy(actor,proxy);
			}
			break;
		case RDPROXYTYPE_LONGDISTANCE:
			rdAddLongDistanceActorProxy(actor,proxyLongDistanceSwapDistance,proxy);
			if(proxy.bUseWithDestruction) {
				rdAddDestructionActorProxy(actor,proxy);
			}
			break;
		case RDPROXYTYPE_DESTRUCTION:
			rdAddDestructionActorProxy(actor,proxy);
			break;
		case RDPROXYTYPE_PICKUP:
			//rdAddPickupProxyX(sid,-1,proxy.pickup);
			break;
	}
}

//.............................................................................
// rdBuildMegaLightList
//.............................................................................
void ArdInstBaseActor::rdBuildMegaLightList() {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	megaLightsToSwap.Empty();
	if(bOnlySwapMegaLightsWithTag) {
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(),ALight::StaticClass(),TEXT("MegaLightSwap"),(TArray<AActor*>&)megaLightsToSwap);
	} else {
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),ALight::StaticClass(),(TArray<AActor*>&)megaLightsToSwap);
	}
#endif
}

//.............................................................................
// FrdProxySetup::ToString
//.............................................................................
FString FrdProxySetup::ToString() {

	FString proxyMeshMatList;
	for(auto& m:proxyMeshMaterials) {
		if(proxyMeshMatList.Len()>0) proxyMeshMatList+=TEXT(",");
		proxyMeshMatList+=m.ToString();
	}
	FString destroyedMeshMatList;
	for(auto& m:destroyedMeshMaterials) {
		if(destroyedMeshMatList.Len()>0) destroyedMeshMatList+=TEXT(",");
		destroyedMeshMatList+=m.ToString();
	}

	FString str=FString::Printf(TEXT("1|%d|%s|%s|%s|%s|%d|%s|%s|%d|%s|%f|%f|%d|%d|%d|%d|%d|%d|%d|%f|%f|%f|%f|%f|%f|%f|%f|%f|%d|%d|%d|%d|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%d|%f|%d|%f|%f|%f|%d|%d"),
								(int32)proxyType,
								*proxyActor.ToString(),
								*proxyDataLayer.ToString(),
								*proxyStaticMesh.ToString(),
								*proxyMeshMatList,
								(int32)proxyMeshCollision,
								*destroyedMesh.ToString(),
								*destroyedMeshMatList,
								(int32)destroyedMeshCollision,
								*destroyedPrefab.ToString(),
								scanDistance,
								proxyPhysicsTimeout,
								bUseWithDestruction,
								bSimplePool,
								pooledAmount,
								bDontRemove,
								bDontHide,
								bEntireMeshProxy,
								bCallSwapEvent,
								destroyedOffset.GetTranslation().X,
								destroyedOffset.GetTranslation().Y,
								destroyedOffset.GetTranslation().Z,
								destroyedOffset.Rotator().Roll,
								destroyedOffset.Rotator().Pitch,
								destroyedOffset.Rotator().Yaw,
								destroyedOffset.GetScale3D().X,
								destroyedOffset.GetScale3D().Y,
								destroyedOffset.GetScale3D().Z,

								(int32)pickup.type,
								(int32)pickup.highlightType,
								(int32)pickup.highlightStyle,
								pickup.id,
								pickup.thickness,
								pickup.color1.R,
								pickup.color1.G,
								pickup.color1.B,
								pickup.color1.A,
								pickup.color2.R,
								pickup.color2.G,
								pickup.color2.B,
								pickup.color2.A,
								pickup.strobeSpeed,
								pickup.ebbSpeed,
								pickup.fadeToGround,
								pickup.respawnTime,
								pickup.respawnLonely,

								pickup.highlightDistance,
								pickup.pickupDistance,
								pickup.respawnDistance,
								pickup.fadeInDistance,
								pickup.useStencilBuffer
							);
	return str;
}

//.............................................................................
// FrdProxySetup::FromString
//.............................................................................
bool FrdProxySetup::FromString(const FString& str) {

	proxyType=RDPROXYTYPE_NONE;

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT("|"),false);
	if(num<52) {
		return false;
	}

	int32 i=1;
	proxyType=(TEnumAsByte<rdProxyType>)FCString::Atoi(*vals[i++]);
	FSoftObjectPath spa(vals[i++]);
	proxyActor=spa;
	FSoftObjectPath sdl(vals[i++]);
	proxyDataLayer=sdl;
	FSoftObjectPath ssm(vals[i++]);
	ssm.TryLoad();
	proxyStaticMesh=ssm;
	FString proxyMatList=vals[i++];
	int32 pmlLen=proxyMatList.Len();
	if(proxyStaticMesh.IsValid() && pmlLen>0) {

		TArray<FString> pvals;
		int32 pnum=proxyMatList.ParseIntoArray(pvals,TEXT(","),false);
		FString lastMat;
		for(int32 ind=0;ind<pnum;ind++) {

			FString fname;
			if(pvals[ind]==TEXT("+")) fname=lastMat;
			else fname=pvals[ind];
			lastMat=fname;
			int32 j=fname.Find(TEXT("."),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
			FString path,name;
			if(j>=0) {
				path=fname.Left(j);
				name=fname.RightChop(j+1);
			} else {
				j=fname.Find(TEXT("/"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
				if(j<0) j=fname.Find(TEXT("\\"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
				if(j>=0) {
					path=fname;
					name=fname.RightChop(j+1);
				}
			}

			FSoftObjectPath sop(path);
			UMaterialInterface* mat=Cast<UMaterialInterface>(sop.ResolveObject());
			if(!mat) {
				mat=Cast<UMaterialInterface>(sop.TryLoad());
			}
			if(mat) {
				proxyMeshMaterials.Add(MoveTemp(mat));
			} else {
				proxyMeshMaterials.Add(nullptr);
			}
		}
	}
	proxyMeshCollision=(TEnumAsByte<ErdCollision>)FCString::Atoi(*vals[i++]);
	FSoftObjectPath desM(vals[i++]);
	desM.TryLoad();
	destroyedMesh=desM;
	FString destroyedMatList=vals[i++];
	int32 dmlLen=destroyedMatList.Len();
	if(destroyedMesh.IsValid() && dmlLen>0) {

		TArray<FString> pvals;
		int32 pnum=destroyedMatList.ParseIntoArray(pvals,TEXT(","),false);
		FString lastMat;
		for(int32 ind=0;ind<pnum;ind++) {

			FString fname;
			if(pvals[ind]==TEXT("+")) fname=lastMat;
			else fname=pvals[ind];
			lastMat=fname;
			int32 j=fname.Find(TEXT("."),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
			FString path,name;
			if(j>=0) {
				path=fname.Left(j);
				name=fname.RightChop(j+1);
			} else {
				j=fname.Find(TEXT("/"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
				if(j<0) j=fname.Find(TEXT("\\"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
				if(j>=0) {
					path=fname;
					name=fname.RightChop(j+1);
				}
			}

			FSoftObjectPath sop(path);
			UMaterialInterface* mat=Cast<UMaterialInterface>(sop.ResolveObject());
			if(!mat) {
				mat=Cast<UMaterialInterface>(sop.TryLoad());
			}
			if(mat) {
				destroyedMeshMaterials.Add(MoveTemp(mat));
			} else {
				destroyedMeshMaterials.Add(nullptr);
			}
		}
	}
	destroyedMeshCollision=(TEnumAsByte<ErdCollision>)FCString::Atoi(*vals[i++]);
	FSoftObjectPath dpf(vals[i++]);
	destroyedPrefab=dpf;
	scanDistance=FCString::Atof(*vals[i++]);
	proxyPhysicsTimeout=FCString::Atof(*vals[i++]);
	bUseWithDestruction=vals[i++].ToBool();
	bSimplePool=vals[i++].ToBool();
	pooledAmount=FCString::Atoi(*vals[i++]);
	bPooled=pooledAmount>0;
	bDontRemove=vals[i++].ToBool();
	bDontHide=vals[i++].ToBool();
	bEntireMeshProxy=vals[i++].ToBool();
	bCallSwapEvent=vals[i++].ToBool();
	FVector loc;
	loc.X=FCString::Atof(*vals[i++]);
	loc.Y=FCString::Atof(*vals[i++]);
	loc.Z=FCString::Atof(*vals[i++]);
	FRotator rot;
	rot.Roll=FCString::Atof(*vals[i++]);
	rot.Pitch=FCString::Atof(*vals[i++]);
	rot.Yaw=FCString::Atof(*vals[i++]);
	FVector scl;
	scl.X=FCString::Atof(*vals[i++]);
	scl.Y=FCString::Atof(*vals[i++]);
	scl.Z=FCString::Atof(*vals[i++]);
	destroyedOffset=FTransform(rot,loc,scl);

	pickup.type=(TEnumAsByte<rdPickupType>)FCString::Atoi(*vals[i++]);
	pickup.highlightType=(TEnumAsByte<rdPickupHighlightType>)FCString::Atoi(*vals[i++]);
	pickup.highlightStyle=(TEnumAsByte<rdPickupHighlightStyle>)FCString::Atoi(*vals[i++]);
	pickup.id=FCString::Atoi(*vals[i++]);
	pickup.thickness=FCString::Atof(*vals[i++]);
	pickup.color1.R=FCString::Atof(*vals[i++]);
	pickup.color1.G=FCString::Atof(*vals[i++]);
	pickup.color1.B=FCString::Atof(*vals[i++]);
	pickup.color1.A=FCString::Atof(*vals[i++]);
	pickup.color2.R=FCString::Atof(*vals[i++]);
	pickup.color2.G=FCString::Atof(*vals[i++]);
	pickup.color2.B=FCString::Atof(*vals[i++]);
	pickup.color2.A=FCString::Atof(*vals[i++]);
	pickup.strobeSpeed=FCString::Atof(*vals[i++]);
	pickup.ebbSpeed=FCString::Atof(*vals[i++]);
	pickup.fadeToGround=vals[i++].ToBool();
	pickup.respawnTime=FCString::Atof(*vals[i++]);
	pickup.respawnLonely=vals[i++].ToBool();

	pickup.highlightDistance=FCString::Atof(*vals[i++]);
	pickup.pickupDistance=FCString::Atof(*vals[i++]);
	pickup.respawnDistance=FCString::Atof(*vals[i++]);
	pickup.fadeInDistance=vals[i++].ToBool();
	pickup.useStencilBuffer=vals[i++].ToBool();

	return true;
}

//.............................................................................
// rdGetProxySettings
//.............................................................................
void ArdInstBaseActor::rdGetProxySettings(AActor* meshActor,UStaticMesh* mesh,FrdProxySetup& proxy) {

	if(mesh) {
		UrdInstProxyAssetUserData* pad=(UrdInstProxyAssetUserData*)mesh->GetAssetUserDataOfClass(UrdInstProxyAssetUserData::StaticClass());
		if(pad) {
			proxy=pad->proxy;
		}
		if(meshActor) {
			for(FName tag:meshActor->Tags) {
				FString stag=tag.ToString();
				if(stag.StartsWith(TEXT("rdProxy="))) {
					proxy.FromString(stag.RightChop(8));
					break;
				}
			}
		}
	}
}

//.............................................................................
