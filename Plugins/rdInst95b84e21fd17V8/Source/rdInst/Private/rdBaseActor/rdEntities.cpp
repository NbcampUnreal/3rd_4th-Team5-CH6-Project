//
// rdEntities.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 1st February 2025 (moved from rdInstBaseActor.cpp)
// Last Modified: 27th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdEntities.h"
#include "rdInstBaseActor.h"
#include "rdSpawnActor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Async/Async.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
#include "Components/LightComponent.h"
#include "Engine/Light.h"
#endif

//.............................................................................
// ProcessEntities
//
// This is the main routine that scans Entities (Instances, Actors, Decals etc)
//  for Proxy Changes, Pickups, Streaming SpawnActors in and out on distance,
//  Populating levels with spawn actor entities around player or frustum based 
//  (kind of like landscape grass) and Entity movement.
//
// These are all done in the same set of threaded loops to minimize CPU time.
//  The results are arrays of commands that can be executed in batch on the 
//  render thread - things like move instances, spawn actors etc.
//  These result arrays are collated by entity type, with lists of object pointers
//  or instance indexes.
//
// The routine is pretty monolythic for now, more features may be added, so 
//  it's kept fairly straight-forward. This will be optimized once I'm happy
//  that it does what it needs to.
//
// 1. The first part of the routine adds any distributed transaction packets
//     to the spawn queues.
//
// 2. The second part of the routine processes the Add, Move and Remove queues.
//     These get populated from the processing threads below - they are never
//     alive when this is run, so no mutexing is needed.
//
// 2. The non-SpawnActor based Proxy scanning is run now, queuing any proxy 
//     swaps ready to be processed by the thread below.
//
// 3. Preparation (in main thread) - sets up the current player position and
//     prepares the results tables used by the threads.
//
// 4. First Thread - first this processes the Proxy Administration (Adding,
//     Removing etc) - Calling the Add/Remove Proxy functions queues them to
//     be processed here. This way the tables used by the proxies are only
//     ever read here and the threads below so don't need to be mutexed.
//
//    Then this routine goes through and spawns threads for each needed SpawnActor
//     (distance based, or when mass movement needed, time sliced by distance).
//     These threads are very fast, the transform data they use is spatially 
//     optimized so only close range items are scanned - most threads are finished
//     before many others are spawned, keeping the number of threads low.
//     See the comment for the ProcessEntity() method.
//
//       Caveats:
//           * The scan area is large and the SpawnActors are very dense
//			 * The mass movement is intricate and slow
//
//       Currently there is a timeout - this may need to be tweaked and/or reworked.
//
// 5. Wait for the SpawnActor Scan Threads to finish 
//
// 6. Process the Proxies for items found during the scan
//
// 7. Exit the processing thread and mark it ready to be called again.
//
//.............................................................................

void ArdInstBaseActor::ProcessEntities() {

	static FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));
	
	if(processingEntities) return;

	// First thing to do is add any distributed spawn packets to our spawn queues
//	processEntities_processDistributedTransactions();

	// Process the Entity Changes and Delegates before any threads run
	//
	// note: this will be optimized after a period of live testing
	//       (it's not inherently slow, just could do things like the recycling insitu etc)
	// 
	// Things like adding instances and spawning actors could well benefit from having these
	//  lists getting passed directly to the game thread at the time they're currently added to the list.
	//  I'll test that once happy that it's all tickidy boo here.
	// 

	processEntities_processRenderQueues();

	// Get the Player location
	APawn* pawn=rdGetControlledPawn();
	hasPlayer=!!pawn;
	if(pawn) {
		FTransform tran=pawn->GetActorTransform();
		playerLoc=tran.GetLocation();
		playerRot=tran.Rotator();
	}

	// Thread Preparation
	processingEntities=true;
	TArray<ArdSpawnActor*> spawnActors=spawnActorRegister;
	scannedActors.Empty();
	scannedInstances.Empty();
	threadCount=0;
	threadID=0;

	// Scanning for Proxies and Pickups that aren't attached to SpawnActors with a SphereTrace (must be done in the main thread)
	if(proxyProcessInterval>=0 && playerLoc!=lastPawnLocation[0] && !bProxyOnlyScanBaked) {

		proxyProcessCount++;
		if(proxyProcessCount>=proxyProcessInterval) {

			rdSphereTrace(playerLoc,proxySwapDistance,scannedActors,scannedInstances,proxyScanIgnoreActorList);
			proxyProcessCount=0;
		}
	}

	// Fire off all the Async Loading and Unloading
	for(ArdSpawnActor* sa:spawnActors) {

		if(!sa || !IsValid(sa)) continue;
		sa->rdAsyncLoadMarkedBakedAssets();
		sa->rdUnloadMarkedAssets();
	}

	// Swap and MegaLights (UE5.5 up)
	processEntities_SwapMegaLights();

	// First Thread, used to process Proxy Creation/Removal/Swapping, and fire off the SpawnActor scan threads.
	AsyncTask(ENamedThreads::AnyThread,[this,spawnActors]() {

		// First thing to do in this thread is add any distributed spawn packets to our spawn queues
		processEntities_processDistributedTransactions();

		processEntities_processProxyQueues();

		// -----------------------------------------------------------------------------------------------
		// Spawn the SpawnActor Scan Threads

		threadResults.SetNum(spawnActors.Num()); // More than needed, but no checks needed in tight loops

		for(ArdSpawnActor* sa:spawnActors) {

			if(!sa || !IsValid(sa)) continue;
			
			float dist=FVector::Distance(sa->spawnActorCenter+sa->spawnActorRadius,playerLoc);

			// Load in or Destroy Streaming SpawnActors - either with the default value in the SpawnActor 
			// or overridden in any of the object placement instances
			sa->ProcessStreamingObjects(dist);

			// Only scan SpawnActors that need to be scanned - i.e. within distance or have movment.
			if(sa->scanType==0) continue;

			if((sa->scanType&6)==0) { // always process for collision and movement processing (2=global collision scanning, 4=global movement processing)
				bool b1=false,b2=false;
				if(sa->scanType&1 && dist>sa->maxProxyScanDistance) b1=true; // 1=proxy scanning
				if(sa->scanType&56 && dist>(sa->spawnActorRadius)) b2=true; // 8=local collision,16=local movement processing,32=local or frustrum updating
				if(b1 && b2) continue;
			}

			if(!ShouldScanFromDistance(sa,dist)) continue; // the further away, the less often its scanned - distance + staggering

			// ok, lets scan it...
			if(critProcessEntities.TryLock()) {
				++threadCount;
				++threadID;
				critProcessEntities.Unlock();

				rdThreadResult* tr=&threadResults[threadID-1];
				tr->addMap.Empty();
				tr->moveMap.Empty();
				tr->destroyActorList.Empty();
				tr->scannedInstances.Empty();
				tr->scannedActors.Empty();
				tr->damagedProxies.queueMap.Empty();

				AsyncTask(ENamedThreads::AnyThread,[this,sa,tr]() {

					sa->ProcessEntity(tr);

					if(critProcessEntities.TryLock()) {
						--threadCount;
						critProcessEntities.Unlock();
					}
				});
			}
		}

		// -----------------------------------------------------------------------------------------------
		// Wait for those threads to finish

		int32 timeout=3000;
		while(timeout) {
			if(critProcessEntities.TryLock()) {
				if(!threadCount) {
					critProcessEntities.Unlock();
					break;
				}
				critProcessEntities.Unlock();
			}
			FPlatformProcess::Sleep(0.0f);
			--timeout;
		}

		if(!timeout) {
			// the threads took too long, lets just exit out now and process next time
			processingEntities=false;
			return;
		}

		// Collate the Results of the SpawnActor Scans
		for(int32 i=0;i<threadID;i++) {
			rdThreadResult& tr=threadResults[i];
			addMap.Append(MoveTemp(tr.addMap));
			moveMap.Append(MoveTemp(tr.moveMap));
			destroyActorList.Append(MoveTemp(tr.destroyActorList));
			for(auto& it:tr.damagedProxies.queueMap) { 
				DamageQueueArray& dqa=damageProxyActorQueue.queueMap.FindOrAdd(it.Key);
				dqa.items.Append(it.Value.items);
			}
			//damageProxyActorQueue.queueMap.Append(MoveTemp(tr.damagedProxies));
			scannedInstances.Append(MoveTemp(tr.scannedInstances));
			scannedActors.Append(MoveTemp(tr.scannedActors));
		}

		//if(critProcessEntities.TryLock()) {
		//	critProcessEntities.Unlock();
		//}

		// Now process all the Proxies found from the scans
		processEntities_processScannedEntities();

		// Swap any Proxies outside our radii back to their instances
		processEntities_SwapOutInactiveProxies();

		lastPawnLocation[0]=playerLoc;
		lastPawnRotation[0]=playerRot;

		// threads have finished
		processingEntities=false;
	});
}

//.............................................................................
// ShouldScanFromDistance
//.............................................................................
bool ArdInstBaseActor::ShouldScanFromDistance(ArdSpawnActor* sa,float dist) {

	int32 modulo=dist/10000.0f; // every 100 meters drops a frame for now

	return true;
}

//.............................................................................
// processEntities_processDistributedTransactions
//
// Takes the first array of transforms for each type of object in the DT queue
//  and adds them to the Add and/or AddProxy queues to be added in the following
//  frame.
//
//.............................................................................
void ArdInstBaseActor::processEntities_processDistributedTransactions() {

	for(auto& it:dtMap) {

		rdDistributedObjectData& dod=it.Value;

		if(dod.transforms.Num()==0) continue;

		if(dod.proxy.proxyType!=RDPROXYTYPE_NONE) {

			if(dod.proxy.bEntireMeshProxy) {

				// Just add a Proxy setup for the first one, then add the rest in the normal queue

				TArray<FAddProxyQueueItem>& pitems=addProxyMap.FindOrAdd(dod.sid);
				TArray<FTransform>& iitems=addMap.FindOrAdd(dod.sid);
				FrdProxyItem pi(dod.proxy,dod.sid,NAME_None,NAME_None);
				FrdProxyActorItem pai(dod.sid,-1,pi,false,0);
				bool first=true;
				for(auto& t:dod.transforms[0]) {
					if(first) {
						pitems.Add(FAddProxyQueueItem(pai,t));
						first=false;
					} else {
						iitems.Add(t);
					}
				}

			} else {

				// Add all items with the Proxy setup

				TArray<FAddProxyQueueItem>& items=addProxyMap.FindOrAdd(dod.sid);
				FrdProxyItem pi(dod.proxy,dod.sid,NAME_None,NAME_None);
				FrdProxyActorItem pai(dod.sid,-1,pi,false,0);

				for(auto& t:dod.transforms[0]) {
					items.Add(FAddProxyQueueItem(pai,t));
				}
			}

			dod.transforms.RemoveAt(0);

		} else {

			// Add all items in the normal queue

			TArray<FTransform>& items=addMap.FindOrAdd(dod.sid);
			items.Append(MoveTemp(dod.transforms[0]));
		}
	}
}

//.............................................................................
// processEntities_processRenderQueues
//.............................................................................
void ArdInstBaseActor::processEntities_processRenderQueues() {

	// Before Delegates
	rdProcessBeforeDelegateQueue();

	if(addMap.Num()>0 || addProxyMap.Num()>0 || remProxyMap.Num()>0 || moveMap.Num()>0 || showActorList.Num()>0 || hideActorList.Num()>0 || destroyActorList.Num()>0) {
		lastPawnLocation[0]=FVector(-9999999,-9999999,-9999999);
	}

	// Add Entities
	for(auto& it:addMap) {
		if(it.Value.Num()>0) {
			FName sid=it.Key;
			TEnumAsByte<ErdSpawnType> type=rdGetSpawnTypeFromSid(sid);

			if(type==ErdSpawnType::Actor) {

				TArray<AActor*> outActors;
				rdSpawnActorsFromSid(sid,it.Value,outActors);

			} else if(type==ErdSpawnType::VFX) {

			} else {

				rdAddInstancesX(sid,it.Value);
				//for(auto& t:it.Value) {
				//	int32 ind=rdAddInstanceX(nullptr,sid,t);
				//}
			}
		}
	}
	addMap.Empty();

	// Add with proxy Entities
	for(auto& it:addProxyMap) {
		if(it.Value.Num()>0) {
			FName sid=it.Key;

/*
		if(bSpawnProxysOnServer) {

			// Broadcast event to the server
			rdQueueProxySpawnOnServerDelegate(item.actorClass,const item.transform,item.savedState,item.sid,index);

			// Send an Event (Function marked as RunInEditor) to the controlled pawn so it can send it to the server
			AActor* pawn=rdGetControlledPawn();
			if(pawn) {
				UFunction* func=pawn->FindFunction(FName(TEXT("rdSpawnOnServer")));
				if(func) {
					struct fParms {
						UClass* spawnClass;
						FTransform transform;
						UrdStateDataAsset* state;
						UStaticMesh* mesh;
						FName sid;
						int32 index;
					};
					fParms parms;
					parms.spawnClass=item.actorClass;
					parms.transform=item.transform;
					parms.state=item.savedState;
					parms.sid=item.sid;
					parms.index=index;
					pawn->ProcessEvent(func,(void*)&parms);
				}
			}

		} else {
*/

			TEnumAsByte<ErdSpawnType> type=rdGetSpawnTypeFromSid(sid);
			if(type==ErdSpawnType::DataLayer) {

				for(FAddProxyQueueItem& pq:it.Value) {

					// Spawning Proxy StaticMesh Instances
					FrdProxyActorItem& item=pq.actorItem;

					// DataLayer Proxy (World Partition enabled levels)
					if(item.proxyItem.proxyDataLayer.Get() && !proxyDataLayerMap.Contains(item.proxyItem.proxyDataLayer)) { 
#if ENGINE_MAJOR_VERSION>4
#if ENGINE_MINOR_VERSION>2
						UDataLayerManager* dlMan=UDataLayerManager::GetDataLayerManager(GetWorld());
						//FSoftObjectPath dlAssetPath(*FString::Printf(TEXT("/Script/Engine.DataLayerAsset'/Game/%s'"),*item.proxyDataLayerName));
						//UDataLayerAsset* alAsset=Cast<UDataLayerAsset>(dlAssetPath.TryLoad());
						UDataLayerAsset* alAsset=item.proxyItem.proxyDataLayer.Get();
						const UDataLayerInstance* dli=dlMan->GetDataLayerInstance(alAsset);
						if(dli) {
							dlMan->SetDataLayerInstanceRuntimeState(dli,EDataLayerRuntimeState::Activated);
						}
#else							
#if ENGINE_MINOR_VERSION>0
						UDataLayerSubsystem* dlSubsys=UWorld::GetSubsystem<UDataLayerSubsystem>(GetWorld());
						if(dlSubsys) {
							dlSubsys->SetDataLayerInstanceRuntimeState(item.proxyItem.proxyDataLayer.Get(),EDataLayerRuntimeState::Activated,true);
						}
#else
						UDataLayerSubsystem* dlSubsys=UWorld::GetSubsystem<UDataLayerSubsystem>(GetWorld());
						if(dlSubsys) {
							dlSubsys->SetDataLayerRuntimeStateByLabel(FName(*item.proxyItem.proxyDataLayer.Get()->Name),EDataLayerRuntimeState::Activated,true);
						}
#endif
#endif
#endif
						item.proxyItem.bHasProxy=true;
						proxyDataLayerMap.Add(item.proxyItem.proxyDataLayer,item);
					}
				}

			} else if(type==ErdSpawnType::Actor) {

				// Spawning Proxy Actors when swapped in by the thread below
				//TODO: respect Pooling
				//TODO: auto re-use
				rdSpawnActorsAndProxiesFromSid(sid,it.Value);

			} else { //TODO: add physics spawning StaticMeshComponents as well as the instances

				for(FAddProxyQueueItem& pq:it.Value) {

					// Spawning Proxy StaticMesh Instances
					FrdProxyActorItem& item=pq.actorItem;
					FTransform& t=pq.transform;
					FHitResult& hit=pq.hit;

					item.proxyInstanceIndex=rdAddInstanceX(nullptr,sid,t);
					item.proxyItem.bHasProxy=true;
					TMap<int32,FrdProxyActorItem>& instMap=proxyISMMap.FindOrAdd(item.sid);
					instMap.Add(item.instanceIndex,item);
				}
			}
		}
	}
	addProxyMap.Empty();

	// Remove Proxy Actors (from swapping out when further than the radii)
	for(auto& it:remProxyMap) {
		if(it.Value.Num()>0) {
			FName sid=it.Key;

			for(FrdProxyActorItem& item:it.Value) {

				if(item.proxyItem.proxyDataLayer.Get()) {

#if ENGINE_MAJOR_VERSION>4
#if ENGINE_MINOR_VERSION>2
					UDataLayerManager* dlMan=UDataLayerManager::GetDataLayerManager(GetWorld());
					UDataLayerAsset* alAsset=item.proxyItem.proxyDataLayer.Get();
					const UDataLayerInstance* dli=dlMan->GetDataLayerInstance(alAsset);
					if(dli) {
						dlMan->SetDataLayerInstanceRuntimeState(dli,EDataLayerRuntimeState::Loaded);
					}
#else							
#if ENGINE_MINOR_VERSION>0
					UDataLayerSubsystem* dlSubsys=UWorld::GetSubsystem<UDataLayerSubsystem>(GetWorld());
					if(dlSubsys) {
						dlSubsys->SetDataLayerInstanceRuntimeState(item.proxyItem.proxyDataLayer.Get(),EDataLayerRuntimeState::Loaded,true);
					}
#else
					UDataLayerSubsystem* dlSubsys=UWorld::GetSubsystem<UDataLayerSubsystem>(GetWorld());
					if(dlSubsys) {
						dlSubsys->SetDataLayerRuntimeStateByLabel(FName(*item.proxyItem.proxyDataLayer.Get()->Name),EDataLayerRuntimeState::Loaded,true);
					}
#endif
#endif
#endif
				}

				if(item.proxyActor) {
					item.proxyItem.transform=item.proxyActor->GetActorTransform();

					if(item.proxyItem.bPooled) {
						rdReturnActorToPool(item.proxyActor);
					} else {
						item.proxyActor->Destroy();
					}

					item.proxyActor=nullptr;
				}
			}
		}
	}
	remProxyMap.Empty();

	// Move Instances
	for(auto& it:moveMap) {
		if(it.Value.Num()>0) {
			rdUpdateTransformTuplesX(it.Key,it.Value);
		}
	}
	moveMap.Empty();

	// Show Actors
	for(auto tp:showActorList) {
		if(IsValid(tp.Key)) {
			tp.Key->SetActorTransform(tp.Value);
			tp.Key->SetActorHiddenInGame(false);
		}
	}
	showActorList.Empty();

	// Hide Actors
	for(auto a:hideActorList) {
		if(IsValid(a)) {
			a->SetActorHiddenInGame(true);
		}
	}
	hideActorList.Empty();

	// Destroy Actors
	for(auto a:destroyActorList) {
		if(IsValid(a)) {
			a->Destroy();
		}
	}
	destroyActorList.Empty();

	// After Delegates
	rdProcessAfterDelegateQueue();
}

//.............................................................................
// processEntities_processProxyQueues
//.............................................................................
void ArdInstBaseActor::processEntities_processProxyQueues() {

	static FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));

	// Proxy Administration Queues
	if(!critProcessEntities.TryLock()) {
		return;
	}

	TArray<FrdProxySetupQueueItem> queue=MoveTemp(proxySetupQueue);
	TArray<FrdProxyActorItem> swapQueue=MoveTemp(swapProxyActorQueue);
	TArray<FrdProxyActorItem> swapOutQueue=MoveTemp(swapOutProxyActorQueue);
	TArray<ProxyRemoveQueueItem> removeQueue=MoveTemp(removeProxyQueue);

	critProcessEntities.Unlock();

	// Add Proxy Queue
	for(FrdProxySetupQueueItem& qitem:queue) {

		switch(qitem.type) {

			case 1: // ShortDistance Proxy

				if(qitem.prefab) {

					FrdProxyItem& i=actorsToProxyMap.Add(qitem.prefab,qitem.proxy);

					i.prefab=qitem.prefab;
					i.transform=qitem.prefab->GetTransform();
					i.distance=qitem.distance>0.0f?qitem.distance:proxySwapDistance;
					i.savedState=qitem.proxy.savedState?DuplicateObject(qitem.proxy.savedState,this):nullptr;

					if(qitem.proxy.bPooled) {
						//rdPoolActor(proxy.proxyActor,proxy.pooledAmount,true,true,false,true,proxy.bSimplePool);
					}

				} else {

					FrdProxyItem* item=nullptr;
					if(qitem.index<0) {
						item=&instancedMeshToProxyMap.FindOrAdd(qitem.sid);
					} else {
						FrdProxyInstanceItems& pii=instancesToProxyMap.FindOrAdd(qitem.sid);
						item=&pii.proxyInstanceIndexList.FindOrAdd(qitem.index);
					}
					if(item) {
						*item=FrdProxyItem(qitem.proxy);
						item->ismc=FindISMCforMesh(qitem.sid);
						item->instanceIndex=qitem.index;
						item->transform=qitem.transform;
						item->distance=qitem.distance>0.0f?qitem.distance:proxySwapDistance;
						item->savedState=qitem.proxy.savedState;
					}
				}
				break;

			case 2: { // LongDistance Proxy

				FrdProxyGridArray& pga=longDistanceInstancesToProxyMap.FindOrAdd(qitem.sid);
				FrdProxyInstanceGridItem* pgi=pga.Add(qitem.transform,qitem.index,qitem.proxy);
				if(qitem.proxy.distance>maxLongDistance) {
					maxLongDistance=qitem.proxy.distance;
				}
				FrdProxyItem& i=pgi->proxy;
				i.sid=qitem.sid;
				i.instanceIndex=qitem.index;
				i.transform=qitem.transform;
				i.distance=qitem.distance>0.0f?qitem.distance:proxyLongDistanceSwapDistance;
				i.proxyType=RDPROXYTYPE_LONGDISTANCE;
				i.savedState=qitem.proxy.savedState;
				i.ismc=FindISMCforMesh(qitem.sid);
				pga.Precalc();
				break; }

			case 3: { // Destruction Proxy

				FrdProxyItem* item=nullptr;
				if(qitem.index<0) {
					item=instancedMeshToProxyMap.Find(qitem.sid);
					if(!item) {
						item=&instancedMeshToProxyMap.Add(qitem.sid,qitem.proxy);
					}
				} else {
					FrdProxyInstanceItems* pii=instancesToProxyMap.Find(qitem.sid);
					if(pii) {
						FrdProxyInstanceItems& items=*pii;
						item=&items.proxyInstanceIndexList.Add(qitem.index,qitem.proxy); // overwrites any existing proxy with this mesh/index
					} else {
						FrdProxyInstanceItems items;
						item=&items.proxyInstanceIndexList.Add(qitem.index,qitem.proxy);
						instancesToProxyMap.Add(qitem.sid,items);
					}
				}

				FrdProxyItem& i=*item;
				i.sid=qitem.sid;
				i.instanceIndex=qitem.index;
				i.bUseWithDestruction=true;
				i.savedState=qitem.proxy.savedState;
				i.ismc=FindISMCforMesh(qitem.sid);
				break; }

			case 4: { // Pickup

				FrdProxyItem* item=nullptr;
				if(qitem.index<0) {
					item=instancedMeshToProxyMap.Find(qitem.sid);
					if(!item) {
						item=&instancedMeshToProxyMap.Add(qitem.sid,qitem.proxy);
					}
				} else {
					FrdProxyInstanceItems& pii=instancesToProxyMap.FindOrAdd(qitem.sid);
					item=pii.proxyInstanceIndexList.Find(qitem.index);
					if(!item) {
						item=&pii.proxyInstanceIndexList.Add(qitem.index,qitem.proxy);
					}
				}

				if(item) {

					FrdPickup& pickup=qitem.proxy.pickup;
					pickupIDtoSidMap.Add(pickup.id,qitem.sid);

					FName hsid;
					if(pickup.highlightType>RDPICKUPHIGHLIGHT_NONE) {

						UStaticMesh* mesh=rdGetMeshFromSid(qitem.sid);
						int32 numMats=mesh->GetStaticMaterials().Num();
						TArray<TSoftObjectPtr<UMaterialInterface>> mats;

						for(int32 i=0;i<numMats;i++) mats.Add(pickup.useStencilBuffer?pickupHighlightStencilMat:pickupHighlightMat);

						// sid for the highlight mesh with highlight material overrides
						hsid=rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,mesh,mats,false,ErdCollision::NoCollision);

						AsyncTask(ENamedThreads::GameThread,[this,item,hsid,pickup] {

							UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(hsid);
							if(ismc) {

								UMaterialInstanceDynamic* mi=UMaterialInstanceDynamic::Create(pickup.useStencilBuffer?pickupHighlightStencilMat:pickupHighlightMat,nullptr);
								if(mi) {

									mi->SetVectorParameterValue(TEXT("HighlightColor1"),item->pickup.color1);
									mi->SetVectorParameterValue(TEXT("HighlightColor2"),item->pickup.color2);
									mi->SetScalarParameterValue(TEXT("HighlightThickness"),item->pickup.thickness);
									mi->SetScalarParameterValue(TEXT("HighlightDistance"),item->pickup.highlightDistance);
									mi->SetScalarParameterValue(TEXT("HighlightFadeToGround"),item->pickup.fadeToGround);
									mi->SetScalarParameterValue(TEXT("HighlightDistanceFade"),item->pickup.fadeInDistance);
									mi->SetScalarParameterValue(TEXT("HighlightStrobeSpeed"),item->pickup.highlightStyle>RDHIGHLIGHTSTYLE_SOLID?item->pickup.strobeSpeed:0.0f);
									mi->SetScalarParameterValue(TEXT("HighlightEbbSpeed"),item->pickup.highlightStyle>RDHIGHLIGHTSTYLE_SOLID?item->pickup.ebbSpeed:0.0f);
									mi->SetScalarParameterValue(TEXT("HighlightInterpolate"),item->pickup.highlightStyle==RDHIGHLIGHTSSTYLE_PULSE?1.0f:0.0f);

									int32 nMats=ismc->GetNumMaterials();
									for(int32 i=0;i<nMats;i++) {
										ismc->SetMaterial(i,mi);
									}
								}
							}
						});
					}

					FrdProxyItem& i=*item;
					i.proxyType=RDPROXYTYPE_PICKUP;
					i.sid=qitem.sid;
					i.instanceIndex=qitem.index;
					i.transform=qitem.transform;
					i.bDontHide=true;
					i.proxyStaticMeshSid=hsid;
					i.savedState=nullptr;
					i.pickup=qitem.proxy.pickup;
					i.ismc=qitem.ismc;
				}
				break; }
		}
	}

	// SwapOutProxyActor queue
	for(FrdProxyActorItem& pai:swapOutQueue) {
		processSwapOutProxyActor(pai);
	}

	// SwapProxyActor queue
	for(FrdProxyActorItem& pai:swapQueue) {
		processSwapToProxyActor(pai);
	}

	// Remove Proxy Queue
	for(ProxyRemoveQueueItem& qitem:removeQueue) {

		switch(qitem.type) {
			case 1: // ShortDistance Proxy
			case 3: // Destruction Proxy
			case 4: // Pickup

				if(qitem.item.prefab) {

					FrdProxyItem* item=actorsToProxyMap.Find(qitem.item.prefab);
					if(item) {

						if(item->prefab) {
							actorsToProxyMap.Remove(item->prefab);
							item->prefab->Destroy();
						}

						if(item->proxyActor) {
							proxyActorMap.Remove(item->proxyActor);
							TArray<FrdProxyActorItem>& remArray=remProxyMap.FindOrAdd(qitem.item.sid);
							FrdProxyActorItem pai(qitem.item);
							remArray.Add(pai);
						}
					}

				} else {

					FrdProxyItem* item=nullptr;
					item=instancedMeshToProxyMap.Find(qitem.item.sid);
					if(!item) {
						FrdProxyInstanceItems& pii=instancesToProxyMap.FindOrAdd(qitem.item.sid);
						item=pii.proxyInstanceIndexList.Find(qitem.item.instanceIndex);
					}

					if(item) {

						FrdProxyActorItem* pai=proxyActorMap.Find(item->proxyActor);
						if(!pai) pai=proxyDataLayerMap.Find(item->proxyDataLayer);
						bool proxyInst=false;
						if(!pai && qitem.pai->proxyInstanceIndex>=0) {
							// check the ProxyISM array - this is Keyed on the main Instances Index
							TMap<int32,FrdProxyActorItem>* instMap=proxyISMMap.Find(item->proxyStaticMeshSid);
							if(instMap) pai=instMap->Find(qitem.pai->instanceIndex);
							if(pai) {
								instMap->Remove(qitem.pai->instanceIndex);
								proxyInst=true;
							}
						}
						if(item->prefab) {
							actorsToProxyMap.Remove(item->prefab);
							item->prefab->Destroy();
						}

						if(item->proxyActor) {
							proxyActorMap.Remove(item->proxyActor);
							TArray<FrdProxyActorItem>& remArray=remProxyMap.FindOrAdd(qitem.item.sid);
							remArray.Add(*pai);
						}
						item->swappedInstances.Remove(qitem.pai->instanceIndex);

						// Hide the Instance
						TArray<TTuple<int32,FTransform>>& moveArray=moveMap.FindOrAdd(qitem.pai->sid);
						moveArray.Add(TTuple<int32,FTransform>(proxyInst?qitem.pai->proxyInstanceIndex:qitem.pai->instanceIndex,ztran));
						//UInstancedStaticMeshComponent* ismc=pi->instanceVolume?FindISMCforMeshInVolumeX(pi->instanceVolume,sid):FindISMCforMesh(sid);
					}
				}
				break; 
			case 2: { // LongDistance Proxy
				FrdProxyGridArray* pii=longDistanceInstancesToProxyMap.Find(qitem.item.sid);
				if(pii) {
					FrdProxyInstanceGridItem* pgi=pii->FindByInstanceIndex(qitem.pai->instanceIndex);
					if(pgi) {
						// maybe add to the alteration list... pgi->Remove(qitem.pai->instanceIndex);
						if(pgi->proxy.prefab) {
							actorsToProxyMap.Remove(pgi->proxy.prefab);
							pgi->proxy.prefab->Destroy(); //TODO: threadsafe
						}
						if(pgi->proxy.proxyActor) {
							proxyActorMap.Remove(pgi->proxy.proxyActor);
						}

						// Remove any existing Proxy Actor
						if(pgi->proxy.proxyActor) {
							//TArray<FrdProxyActorItem>& remArray=remProxyMap.FindOrAdd(qitem.item.sid);
							//remArray.Add(qitem.);
							pgi->proxy.proxyActor->Destroy();//TODO: threadsafe
						}

						// Hide the Instance
						TArray<TTuple<int32,FTransform>>& moveArray=moveMap.FindOrAdd(qitem.item.sid);
						moveArray.Add(TTuple<int32,FTransform>(qitem.pai->instanceIndex,ztran));
						//UInstancedStaticMeshComponent* ismc=pi->instanceVolume?FindISMCforMeshInVolumeX(pi->instanceVolume,sid):FindISMCforMesh(sid);
								
					}
				}
				break; }
		}
	}
}

//.............................................................................
// processEntities_processScannedEntities
//.............................................................................
void ArdInstBaseActor::processEntities_processScannedEntities() {

	static FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));

	// Short Distance
	if(scannedInstances.Num()>0) {

		// Find Object front of us
		APlayerCameraManager* camMan=UGameplayStatics::GetPlayerCameraManager(GetWorld(),0);
		FVector loc=camMan->GetCameraLocation();
		FRotator rot=camMan->GetCameraRotation();
		FVector nm=rot.Vector();
		FHitResult hit;
		FCollisionQueryParams queryParams;
		GetWorld()->LineTraceSingleByChannel(hit,loc,loc+(nm*10000.0f),ECollisionChannel::ECC_Visibility,queryParams);
		UInstancedStaticMeshComponent* hitISMC=nullptr;
		int32 hitIndex=-1;
		if(hit.bBlockingHit) {
			hitIndex=hit.Item;
			hitISMC=Cast<UInstancedStaticMeshComponent>(hit.Component);
		}

		ArdActor* prefab=nullptr;
		for(FrdInstanceItems& ins:scannedInstances) {

			// First, check the mesh list (sid=proxy) - both the "all meshes" and single instance lists
			FrdProxyItem* pi=instancedMeshToProxyMap.Find(ins.sid);
			FrdProxyInstanceItems* pii=instancesToProxyMap.Find(ins.sid);

			if(pi || pii) {

				bool allMesh=false;
				if(pi) { // all mesh proxy
					if(pi->bDestroyed) continue;
					if(pi->instanceIndex==-1) pi->instanceVolume=ins.volume;
					allMesh=true;
				}

				TArray<TTuple<int32,FTransform>>& moveArray=moveMap.FindOrAdd(ins.sid);
				int32 ind=0;
				for(FTransform& t:ins.transforms) {

					int32 index=ins.instances[ind++];
					FVector dloc=t.GetTranslation();

					if(!allMesh) {
						pi=pii->proxyInstanceIndexList.Find(index);
						if(!pi) continue; // index not within radius
					}

					dloc+=(pi->centerOffset*t.GetScale3D());

					// Pickups
					if(pi->proxyType==RDPROXYTYPE_PICKUP && pi->pickup.type!=RDPICKUP_NONE) {

						if(pi->pickup.highlightType==RDPICKUPHIGHLIGHT_LOOKATOUTLINE) {

							float dist=FVector::Distance(playerLoc,dloc);
							if(dist<pi->pickup.highlightDistance) {

								if(pi->ismc==hitISMC && index==hitIndex) {
									if(allMesh) {
										if(!pi->swappedInstances.Contains(index)) {
											rdSwapInstanceToProxyActorX(ins.sid,index,t);
											currentPickupHighID=pi->pickup.id;
											currentPickupHighInstance=index;
											rdQueuePickupFocusedDelegate(pi->pickup.id,t,ins.sid,index);
										}
									} else if(!pi->bHasProxy) {
										rdSwapInstanceToProxyActorX(ins.sid,index,t);
										pi->bHasProxy=true;
										currentPickupHighID=pi->pickup.id;
										currentPickupHighInstance=index;
										rdQueuePickupFocusedDelegate(pi->pickup.id,t,ins.sid,index);
									}

								} else {

									if(allMesh) {
										if(pi->swappedInstances.Contains(index)) {
											rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
											if(currentPickupHighID==pi->pickup.id && currentPickupHighInstance==index) {
												currentPickupHighID=-1;
												currentPickupHighInstance=-1;
											}
											rdQueuePickupUnfocusedDelegate(pi->pickup.id,t,ins.sid,index);
										}
									} else if(pi->bHasProxy) {
										rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
										pi->bHasProxy=false;
										if(currentPickupHighID==pi->pickup.id && currentPickupHighInstance==index) {
											currentPickupHighID=-1;
											currentPickupHighInstance=-1;
										}
										rdQueuePickupUnfocusedDelegate(pi->pickup.id,t,ins.sid,index);
									}
								}

							} else {

								if(allMesh) {
									if(pi->swappedInstances.Contains(index)) {
										rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
										rdQueuePickupUnfocusedDelegate(pi->pickup.id,t,ins.sid,index);
									}
								} else if(pi->bHasProxy) {
									rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
									pi->bHasProxy=false;
									rdQueuePickupUnfocusedDelegate(pi->pickup.id,t,ins.sid,index);
								}
							}

							/*
							// routine for if collision turned off
							FVector v=(dloc-playerLoc);
							v.Normalize();
							FVector forward=FRotationMatrix(playerRot).GetScaledAxis(EAxis::X);
							float la=FVector::DotProduct(forward,v);
							float angle=FMath::Acos(la)*(180.0f/3.141592653589793238463f);
							if(angle<10.0f) { // Looking at object

							} else {

							}
							*/
						} else if(pi->pickup.highlightType==RDPICKUPHIGHLIGHT_OUTLINE) {

							float dist=FVector::Distance(playerLoc,dloc);
							if(dist<pi->pickup.highlightDistance) {

								if(allMesh) {
									if(!pi->swappedInstances.Contains(index)) {
										rdSwapInstanceToProxyActorX(ins.sid,index,t);
									}
								} else if(!pi->bHasProxy) {
									rdSwapInstanceToProxyActorX(ins.sid,index,t);
									pi->bHasProxy=true;
								}

							} else {

								if(allMesh) {
									if(pi->swappedInstances.Contains(index)) {
										rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
									}
								} else if(pi->bHasProxy) {
									rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
									pi->bHasProxy=false;
								}
							}
						}

						if(pi->pickup.type==RDPICKUP_OVERLAP) {

							float dist=FVector::Distance(playerLoc,dloc);
							if(dist<pi->pickup.pickupDistance) {
								if(allMesh) {
									if(pi->swappedInstances.Contains(index)) {
										rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
									}
								} else if(pi->bHasProxy) {
									rdRemoveProxyByProxyISMX(pi->proxyStaticMeshSid,index);
									pi->bHasProxy=false;
								}
								moveArray.Add(TTuple<int32,FTransform>(index,ztran));
								rdRemoveProxyByInstanceX(ins.sid,index);
								rdQueuePickedUpDelegate(pi->pickup.id,t,ins.sid,index);
							}
						}

					} else {

						// Normal Proxies
						rdSwapInstanceToProxyActorX(ins.sid,index,t);
					}
				}

			} else {

				UInstancedStaticMeshComponent* ismc=FindISMCforMesh(ins.sid);
				if(!ismc) continue;
				int32 ind=0;

//				for(FTransform& t:ins.transforms) {

//					int32 index=ins.instances[ind++];
//					FVector dloc=t.GetTranslation();

				for(int32 i:ins.instances) {

					// Check if the instance belongs to a prefab
					prefab=(ismc)?rdGetrdActorFromInstanceIndex(ismc,i):nullptr;
					ArdSpawnActor* spa=Cast<ArdSpawnActor>(prefab);
					if(prefab && !spa) {
						if(prefab->bScanForProxy) {
							if(!scannedActors.Contains(prefab)) {
								scannedActors.Add(prefab); 
								rdSwapActorToProxyActor(prefab,i);
								//ArdActor* rda=Cast<ArdActor>(p);
								//if(rda) {
									//rdSwapPrefabToProxyActor(rda);
								//}
							}
						}
					} else {
						//FTransform tran2;
						//if(!rdGetInstanceTransformFast(ismc,i,tran2)) tran2=ins.transforms[ind];
						//if(tran2.GetTranslation().Z>=-400000.0f) { // baked items don't store the hidden offset
						//	rdSwapInstanceToProxyActorX(ins.sid,i,tran2);
						//}
					}
					ind++;
				}
			}
		}
	}

	// Long Distance
	FVector& pLoc=playerLoc;
	for(auto& it:longDistanceInstancesToProxyMap) {
		it.Value.ForEachWithinRadius(playerLoc,maxLongDistance,[this,&pLoc](FrdProxyInstanceGridItem& p) {
			FrdProxyItem& pi=p.proxy;
			FVector l=pi.proxyActor?pi.proxyActor->GetActorLocation():pi.transform.GetTranslation();
			if(FVector::Distance(l,pLoc)<pi.distance) {
				if(!pi.bHasProxy) {
					rdSwapInstanceToProxyActorX(pi.sid,pi.instanceIndex,pi.transform,false,true);
				}
			//} else {
				//if(pi.bHasProxy) {
				//	rdSwapProxyActorToInstance(pi.proxyActor);
				//}
			}
		});
	}

	// Actors
	for(AActor* a:scannedActors) {

		FrdProxyItem* pi=actorsToProxyMap.Find(a);
		if(pi) {

			if(FVector::Distance(a->GetActorLocation(),playerLoc)<pi->distance) {
				if(!pi->bHasProxy) {
					rdSwapActorToProxyActor(a,-1,false,false);
				}
			}
		}

		pi=longDistanceActorsToProxyMap.Find(a);
		if(pi) {

			if(FVector::Distance(a->GetActorLocation(),playerLoc)<pi->distance) {
				if(!pi->bHasProxy) {
					rdSwapActorToProxyActor(a,-1,false,false);
				}
			}
		}
	}

	//for(FrdProxyItem& pi:longDistanceActorsToProxyMap) {
		/* //TODO:
		it.Value.ForEachWithinRadius(playerLoc,maxLongDistance,[this,&pLoc](FrdProxyInstanceGridItem& p) {
			FrdProxyItem& pi=p.proxy;
			FVector l=pi.proxyActor?pi.proxyActor->GetActorLocation():pi.transform.GetTranslation();
			if(FVector::Distance(l,pLoc)<pi.distance) {
				if(!pi.bHasProxy) {
					rdSwapActorToProxyActor(pi.prefab,pi.instanceIndex,false,true);
				}
			} else if(pi.bHasProxy) {
				rdSwapProxyActorToActor(pi.proxyActor);
			}
		});
		*/
	//}

	// Impacts
	if(critProcessEntities.TryLock()) {
		TMap<FName,DamageQueueArray> damageQueue=MoveTemp(damageProxyActorQueue.queueMap);
		critProcessEntities.Unlock();
		for(auto& it:damageQueue) {
			FName sid=it.Key;
			for(DamageQueueItem& dqi:it.Value.items) {
				if(dqi.prefab) {
					rdSwapActorToProxyActor(dqi.prefab,dqi.instance,true);
				} else {
					rdSwapInstanceToProxyActorWithHitX(sid,dqi.instance,dqi.transform,dqi.hit);
				}
			}
		}
	}
}

//.............................................................................
// processEntities_SwapOutInactiveProxies
//.............................................................................
void ArdInstBaseActor::processEntities_SwapOutInactiveProxies() {

	TArray<AActor*> removeProxyActorList;
	TArray<TSoftObjectPtr<UDataLayerAsset>> removeProxyDataLayerList;
	TMap<FName,TArray<int32>> removeProxyISMList;
	bool countdownDestruction=false;

	for(auto& ai:proxyActorMap) {
		FrdProxyActorItem& a=ai.Value;
		if(a.forLongDistance || a.bDontRemove) continue;
		if(a.forDestruction!=countdownDestruction) continue;
		rdSwapInactiveProxy(a,playerLoc,removeProxyActorList,removeProxyDataLayerList,removeProxyISMList);
	}

	for(auto& dl:proxyDataLayerMap) {
		FrdProxyActorItem& a=dl.Value;
		if(a.forLongDistance || a.bDontRemove) continue;
		if(a.forDestruction!=countdownDestruction) continue;
		rdSwapInactiveProxy(a,playerLoc,removeProxyActorList,removeProxyDataLayerList,removeProxyISMList);
	}

	for(auto& am:proxyISMMap) {
		TMap<int32,FrdProxyActorItem>& aim=am.Value;
		for(auto& ai:aim) {
			FrdProxyActorItem& a=ai.Value;
			if(a.forLongDistance || a.bDontRemove) continue;
			if(a.forDestruction!=countdownDestruction) continue;
			rdSwapInactiveProxy(a,playerLoc,removeProxyActorList,removeProxyDataLayerList,removeProxyISMList);
		}
	}

	for(auto& a:removeProxyActorList) proxyActorMap.Remove(a);
	for(auto& dl:removeProxyDataLayerList) proxyDataLayerMap.Remove(dl);
	for(auto& it:removeProxyISMList) {
		
		TMap<int32,FrdProxyActorItem>* map=proxyISMMap.Find(it.Key);
		if(map) {
			for(auto& index:it.Value) {
				map->Remove(index);
			}
			//if(map->IsEmpty()) {
			//	proxyISMMap.Remove(it.Key);
			//}
		}
	}
}

//.............................................................................
// processEntities_SwapMegaLights
//.............................................................................
void ArdInstBaseActor::processEntities_SwapMegaLights() {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	// MegaLights, for now, we just do this in the main thread
	if(bSwapMegaLightsToVSMinProximity && playerLoc!=lastPawnLocation[0]) {

		TArray<ALight*> lightActors;
		for(auto l:megaLightsToSwap) {
			FVector aloc=l->GetActorLocation();
			float dist=FVector::Distance(aloc,playerLoc);
			if(dist<swapMegaLightDistance) {
				if(l) lightActors.Add(l);
			}
		}

		// Swap back lights outside the radius
		TArray<ALight*> delLights;
		for(auto l:swappedLights) {
			if(!lightActors.Contains(l)) {
				ULightComponent* lc=l->GetLightComponent();
				if(lc && lc->MegaLightsShadowMethod!=EMegaLightsShadowMethod::RayTracing) {
					lc->MegaLightsShadowMethod=EMegaLightsShadowMethod::RayTracing;
					lc->InvalidateLightingCacheDetailed(false,true);
				}
				delLights.Add(l);
			}
		}
		for(auto l:delLights) {
			swappedLights.Remove(l);
		}

		// swap in lights inside the radius
		for(auto l:lightActors) {
			ULightComponent* lc=l->GetLightComponent();
			if(lc && lc->MegaLightsShadowMethod!=EMegaLightsShadowMethod::VirtualShadowMap) {
				lc->MegaLightsShadowMethod=EMegaLightsShadowMethod::VirtualShadowMap;
				lc->InvalidateLightingCacheDetailed(false,true);
				swappedLights.Add(l);
			}
		}
	}
#endif
}

//.............................................................................

//.............................................................................
// ProcessEntity (SpawnActor method)
//
// Note: this method must not write to any Actor properties as it gets called
// from another thread.
//
// All results are passed into thread safe data arrays for the entity processing
// thread to collate with all other process threads and safely pass back to the 
// render thread.
//
// This routine:
//    * Scans the spawn actor for Proxy Changes
//    * Populates new radius or frustrum exposed areas and hides hidden areas
//    * Handles EntityMovement
//
//.............................................................................
void ArdSpawnActor::ProcessEntity(rdThreadResult* tr) {
	
	currentLocation=rdBaseActor->playerLoc;
	FrdSpawnData& sDat=(FrdSpawnData&)spawnData[currentSpawnDataIndex];
	UrdBakedDataAsset* bda=sDat.bakedData; // should always be loaded at this point
	if(!bda) {
		return;
	}
	TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;

	FVector loc=currentLocation,aloc=sDat.center;
	if(bRelativeSpawning) {
		loc-=GetActorLocation();
	}

	double dist=FVector::Distance(loc,aloc);
	FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));
	bool outside=false;

	// Loop through each spawn type (based on its sid) and process the minimum of what's required

	// If this spawnActor is outside of the spawn distance, remove all objects set as playerradius or viewfrustum
	if(sDat.distance>0.0f && dist>(sDat.distance+localRadius)) {
		outside=true;
		for(auto& bd:baked) {
			FrdObjectPlacementData& pd=(FrdObjectPlacementData&)sDat.items[bd.itemIndex];
			if((bd.spawnMode==rdSpawnMode::RDSPAWNMODE_SDP || bd.spawnMode==rdSpawnMode::RDSPAWNMODE_FDP) && !bd.sid.IsNone()) {
				TArray<TTuple<int32,FTransform>>& moveArray=tr->moveMap.FindOrAdd(pd.sid);
				bd.transformMap.ForEach([this,&ztran,&moveArray](FrdInstanceTransformItem& t) {
					if(t.index>=0) {
						moveArray.Add(TTuple<int32,FTransform>(t.index,ztran));
					}
				});
				/*
				int32 num=pd.ismIndexList.Num();
				for(int32 i=0;i<num;i++) {
					int32 pi=pd.ismIndexList[i];
					if(pi>=0) {
						moveArray.Add(TTuple<int32,FTransform>(pi,ztran));
					}
				}
				*/
			}
		}
	}

	for(auto& bd:baked) {

		FrdObjectPlacementData& pd=(FrdObjectPlacementData&)sDat.items[bd.itemIndex];
		if(bd.sid.IsNone()) continue;

		// Work out the type of Population this Placement set should use, taking the rdInstBaseActor overrides into account
		rdSpawnMode sm=(bd.spawnMode==rdSpawnMode::RDSPAWNMODE_DEFAULT)?spawnMode:bd.spawnMode;
		
		rdPopulationOverride po=rdBaseActor->runPopOverride;
		if(po!=RDFORCEPOP_NOOVERRIDE) {
			switch(po) {
				case rdPopulationOverride::RDFORCEPOP_NONE:
					if(sm==rdSpawnMode::RDSPAWNMODE_FDP || sm==rdSpawnMode::RDSPAWNMODE_SDP) {
						continue;
					}
					break;
				case rdPopulationOverride::RDFORCEPOP_NOFRUSTUM:
					if(sm==rdSpawnMode::RDSPAWNMODE_FDP) {
						continue;
					}
					break;
				case rdPopulationOverride::RDFORCEPOP_NOSPHERICAL:
					if(sm==rdSpawnMode::RDSPAWNMODE_SDP) {
						continue;
					}
					break;
				case rdPopulationOverride::RDFORCEPOP_ALL:
					if(sm==rdSpawnMode::RDSPAWNMODE_FDP || sm==rdSpawnMode::RDSPAWNMODE_SDP) {
						sm=rdSpawnMode::RDSPAWNMODE_ALL;
					}
					break;
				case rdPopulationOverride::RDFORCEPOP_FRUSTUMTOSPHERICAL:
					if(sm==rdSpawnMode::RDSPAWNMODE_FDP) {
						sm=rdSpawnMode::RDSPAWNMODE_SDP;
					}
					break;
			}
		}

		bool hasMovement=(pd.movement && pd.movement->hasMovement());
		bool hasProxy=(pd.proxy.proxyType==RDPROXYTYPE_SHORTDISTANCE||pd.proxy.proxyType==RDPROXYTYPE_LONGDISTANCE||pd.proxy.proxyType==RDPROXYTYPE_PICKUP);
		if(!hasProxy && !hasMovement &&  sm!=rdSpawnMode::RDSPAWNMODE_SDP && sm!=rdSpawnMode::RDSPAWNMODE_FDP) {
			continue;
		}

		FrdInstanceItems ii;
		ii.sid=bd.sid;

		rdGridInfo gi;
		gi.moveArray=&tr->moveMap.FindOrAdd(pd.sid);
		gi.addArray=&tr->addMap.FindOrAdd(pd.sid);
		gi.destroyArray=&tr->destroyActorList;
		gi.proxyItems=&proxyItemsMap.FindOrAdd(pd.sid);
		gi.hasProxy=hasProxy;
		gi.hasMovement=hasMovement;
		gi.isISM=pd.ismc.IsValid();
		gi.loc=loc;
		gi.proxySwapDistance=pd.proxy.scanDistance>0.0f?pd.proxy.scanDistance:rdBaseActor->proxySwapDistance;
		gi.pim=proxyItemsMap.Find(pd.sid);
		
		if(sm==rdSpawnMode::RDSPAWNMODE_SDP) {

			bd.transformMap.ForEachOverlappingInSpheres(currentLocation,lastLocation,localRadius,[this,&gi,&pd,&ztran,&ii](FrdInstanceTransformItem& t,int32 state) {
				if(state==1) {
					gi.addArray->Add(t.transform);
				} else if(state==2) {
					if(gi.isISM) {
						gi.moveArray->Add(TTuple<int32,FTransform>(t.index,ztran));
					} else if(gi.pim) {
/*
struct FrdProxyInstanceItems {
	GENERATED_BODY()
public:
	UPROPERTY(Category=rdProxies,EditAnywhere)
	TMap<int32,FrdProxyItem> proxyInstanceIndexList;
	UPROPERTY(Category=rdProxies,EditAnywhere)
	TMap<AActor*,FrdProxyItem> proxyActorList;
};*/
						//AActor* actor=gi.pim->proxyActorList
						//gi.destroyArray->Add(actor);
					}
				} else {
					if(gi.hasProxy) {
						float dist=FVector::Distance(t.transform.GetTranslation(),gi.loc);
						if(dist<gi.proxySwapDistance) {
							ii.instances.Add(t.index);
							ii.transforms.Add(t.transform);
						}
					}
				}
			});

		} else if(sm==rdSpawnMode::RDSPAWNMODE_FDP) {

			bd.transformMap.ForEachOverlappingInTriangles(currentLocation,currentRotation,lastLocation,lastRotation,localRadius,viewFrustumDistance,[this,&loc,&gi,&pd,&ztran,&ii](FrdInstanceTransformItem& t,int32 state) {
				if(state==1) {
					gi.addArray->Add(t.transform);
				} else if(state==2) {
					if(gi.isISM) gi.moveArray->Add(TTuple<int32,FTransform>(t.index,ztran));
					//else gi.remArray->Add(index);//TODO:
				} else {
					if(gi.hasProxy) {
						float dist=FVector::Distance(t.transform.GetTranslation(),loc);
						if(dist<gi.proxySwapDistance) {
							ii.instances.Add(t.index);
							ii.transforms.Add(t.transform);
						}
					}
				}
			});

		} else {

			bd.transformMap.ForEachWithinRadius(currentLocation,gi.proxySwapDistance,[this,&loc,&gi,&pd,&ii](FrdInstanceTransformItem& t) {
				if(gi.hasProxy) {
					FTransform rt=rdBaseActor->rdRelativeToWorldNoScale(t.transform,GetActorTransform());
					float dist=FVector::Distance(rt.GetTranslation(),loc);
					if(dist<gi.proxySwapDistance) {
						ii.instances.Add(t.index);
						ii.transforms.Add(rt);
					}
				}
			});
		}

		if(ii.instances.Num()>0) {
			tr->scannedInstances.Add(MoveTemp(ii));
		}
	}

	lastLocation=currentLocation;
}

//.............................................................................
