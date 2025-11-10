//
// rdInstBaseActor_Distributed.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th February 2024 (moved from rdInstBaseActor.cpp)
// Last Modified: 6th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "LandscapeProxy.h"

//.............................................................................
// rdAddDistributedInstances
//.............................................................................
void ArdInstBaseActor::rdAddDistributedInstances(UStaticMesh* mesh,const TArray<FTransform>& transforms,int32 transactionsPerTick,TArray<int32>& fillList,const FrdProxySetup proxy) {
	rdAddDistributedInstancesX(rdGetSMsid(mesh),transforms,transactionsPerTick,fillList,proxy);
}
void ArdInstBaseActor::rdAddDistributedInstancesX(const FName sid,const TArray<FTransform>& transforms,int32 transactionsPerTick,TArray<int32>& fillList,const FrdProxySetup proxy) {

	if(transforms.Num()==0) return;

	rdDistributedObjectData& dod=dtMap.FindOrAdd(sid);
	dod.sid=sid;

	int32 sz=transforms.Num()/transactionsPerTick;
	int32 rem=transforms.Num()-(sz*transactionsPerTick);
	int32 ind=0;

	for(int32 i=0;i<sz-1;i++) {
		TArray<FTransform> ary;
		for(int32 j=0;j<transactionsPerTick;j++) {
			ary.Add(transforms[ind++]);
		}
		dod.transforms.Add(MoveTemp(ary));
	}
	TArray<FTransform> ary;
	for(int32 j=0;j<rem;j++) {
		ary.Add(transforms[ind++]);
	}
	dod.transforms.Add(MoveTemp(ary));

/*
struct rdDistributedObjectData {
	FName						sid;
	bool						pooled=false;
	TArray<TArray<FTransform>>	transforms;
	FrdProxySetup				proxy;
}
*/
}

//.............................................................................
// rdSpawnDistributedActors
//.............................................................................
void ArdInstBaseActor::rdSpawnDistributedActors(UClass* actorClass,AActor* actorTemplate,const TArray<FTransform>& transforms,int32 transactionsPerTick,bool pooled,TArray<AActor*>& fillList,const FrdProxySetup proxy,const FString& strProps) {

}

//.............................................................................
