//
// rdInstBaseActor_Pickups.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 28th Novmber 2023 (Moved from rdInstBaseActor_Proxies.cpp)
// Last Modified: 23rd May 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "rdSpawnActor.h"
#include "UObject/Package.h"

//.............................................................................
// rdAddPickupProxy
//.............................................................................
void ArdInstBaseActor::rdAddPickupProxy(UStaticMesh* mesh,int32 index,FrdPickup pickup) {
	rdAddPickupProxyX(rdGetSMsid(mesh),index,pickup);
}
void ArdInstBaseActor::rdAddPickupProxyX(const FName sid,int32 index,FrdPickup pickup) { 
	
	if(sid.IsNone()) return;

	// Create the Highlight material
	TSoftObjectPtr<UMaterialInterface> smat=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/rdInst/Materials/M_rdHighlight.M_rdHighlight")));
	UMaterialInterface* mat=smat.Get();
	if(!mat) {
		return;
	}
	UStaticMesh* mesh=rdGetMeshFromSid(sid);
	int32 numMats=mesh->GetStaticMaterials().Num();
	TArray<TSoftObjectPtr<UMaterialInterface>> mats;
	for(int32 i=0;i<numMats;i++) mats.Add(mat);
	FName hsid=rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,mesh,mats,false,ErdCollision::NoCollision);

	FrdProxyItem item;
	item.proxyType=RDPROXYTYPE_PICKUP;
	item.sid=sid;
	item.instanceIndex=-1;
	item.bDontHide=true;
	item.proxyStaticMeshSid=hsid;
	item.savedState=nullptr;
	item.pickup=pickup;
	item.ismc=FindISMCforMesh(sid);

	FTransform transform;
	if(index>=0) {
		if(item.ismc) {
			item.ismc->GetInstanceTransform(index,transform,true);
		}
	}
	if(critProcessEntities.TryLock()) {
		proxySetupQueue.Add(FrdProxySetupQueueItem(4,sid,nullptr,index,0.0f,item,transform,item.ismc));
		critProcessEntities.Unlock();
	}
}

//.............................................................................
// rdRemovePickupProxies
//.............................................................................
void ArdInstBaseActor::rdRemovePickupProxies(int32 id) {

}

//.............................................................................
// rdRemovePickup
//.............................................................................
int32 ArdInstBaseActor::rdSpawnPickup(int32 id,const FTransform& transform) {

	return -1;
}

//.............................................................................
// rdPickupCurrent
//.............................................................................
void ArdInstBaseActor::rdPickupCurrent() {

	if(currentPickupHighID>=0 && currentPickupHighInstance>=0) {
		rdPickupPickup(currentPickupHighID,currentPickupHighInstance);
	}
}

//.............................................................................
// rdPickupPickup
//.............................................................................
void ArdInstBaseActor::rdPickupPickup(int32 id,int32 instance) {

	FName* sid=pickupIDtoSidMap.Find(id);
	if(!sid) return;

	FrdProxyItem* item=instancedMeshToProxyMap.Find(*sid);
	if(!item) {
		FrdProxyInstanceItems* pii=instancesToProxyMap.Find(*sid);
		if(pii) {
			item=pii->proxyInstanceIndexList.Find(instance);
		}
	}
	if(!item) {
		return;
	}

	rdRemoveProxyByProxyISMX(item->proxyStaticMeshSid,instance);

	static FTransform ztran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));
	TArray<TTuple<int32,FTransform>>& moveArray=moveMap.FindOrAdd(*sid);
	moveArray.Add(TTuple<int32,FTransform>(instance,ztran));

	rdRemoveProxyByInstanceX(*sid,instance);

	rdQueuePickedUpDelegate(id,item->transform,*sid,instance);
}

//.............................................................................
// rdRemovePickup
//.............................................................................
void ArdInstBaseActor::rdRemovePickup(int32 id,int32 instance) {

}

//.............................................................................
