//
// rdInstBaseActor_Conversions.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 6th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "Engine/StaticMeshActor.h"
#include "rdActor.h"
#include "Engine/World.h"
#if WITH_EDITOR
#include "FileHelpers.h"
#include "ObjectTools.h"
#endif
#include "GenericPlatform/GenericPlatformFile.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "rdSpawnActor.h"
#if WITH_EDITOR
bool fileExists(const FString& file) {

	FString path=file;
	if(path.StartsWith(TEXT("/All/"))) path.RemoveAt(0,4);
	else if(path.StartsWith(TEXT("/Game/"))) path.RemoveAt(0,6);
	else if(path.StartsWith(TEXT("Game/"))) path.RemoveAt(0,5);
	FString realfile=FPaths::ProjectContentDir()+path+TEXT(".uasset");
	bool exists=FPlatformFileManager::Get().GetPlatformFile().FileExists(*realfile);

	if(!exists) { // check to see if it resides only in memory
		UPackage* package=FindPackage(NULL,*file);
		if(package) {
			exists=true;
		}
	}
	return exists;
}
#endif

//.............................................................................
// rdConvertInstanceToChildActor
//
// Converts the Instance to a ChildActorComponent
//
//.............................................................................
UChildActorComponent* ArdInstBaseActor::rdConvertInstanceToChildActor(ArdActor* actor,UStaticMesh* mesh,int32 index) {
	return rdConvertInstanceToChildActorX(actor,rdGetSMsid(mesh),index);
}
UChildActorComponent* ArdInstBaseActor::rdConvertInstanceToChildActorX(ArdActor* actor,const FName sid,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {
		UChildActorComponent* cac=actor->rdAddChildComponent(AStaticMeshActor::StaticClass(),transform);
		if(cac) {
			AStaticMeshActor* meshActor=(AStaticMeshActor*)cac->GetChildActor();
			if(!meshActor) meshActor=(AStaticMeshActor*)cac->GetChildActorTemplate();
			if(meshActor) {
				meshActor->GetStaticMeshComponent()->SetStaticMesh(rdGetMeshFromSid(sid));
				rdRemoveInstanceX(sid,index);
				return cac;
			}
		}
	}

	return nullptr;
}

//.............................................................................
// rdConvertInstanceToLevelActor
//
// Converts the Instance to a new Actor in the level (spawn)
//
//.............................................................................
AActor* ArdInstBaseActor::rdConvertInstanceToLevelActor(UStaticMesh* mesh,int32 index) {
	return rdConvertInstanceToLevelActorX(rdGetSMsid(mesh),index);
}
AActor* ArdInstBaseActor::rdConvertInstanceToLevelActorX(const FName sid,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AStaticMeshActor* actor=(AStaticMeshActor*)rdSpawnActor(AStaticMeshActor::StaticClass(),transform);
		if(actor) {
			actor->GetStaticMeshComponent()->SetStaticMesh(rdGetMeshFromSid(sid));
			rdRemoveInstanceX(sid,index);
			return (AActor*)actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstanceToActor
//.............................................................................
AActor* ArdInstBaseActor::rdConvertInstanceToActor(UStaticMesh* mesh,UClass* actorClass,int32 index) {
	return rdConvertInstanceToActorX(rdGetSMsid(mesh),actorClass,index);
}
AActor* ArdInstBaseActor::rdConvertInstanceToActorX(const FName sid,UClass* actorClass,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AActor* actor=rdSpawnActor(actorClass,transform);
		if(actor) {
			rdRemoveInstanceX(sid,index);
			return actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstanceToActorFromPool
//.............................................................................
AActor* ArdInstBaseActor::rdConvertInstanceToActorFromPool(UStaticMesh* mesh,UClass* actorClass,int32 index) {
	return rdConvertInstanceToActorFromPoolX(rdGetSMsid(mesh),actorClass,index);
}
AActor* ArdInstBaseActor::rdConvertInstanceToActorFromPoolX(const FName sid,UClass* actorClass,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AActor* actor=rdGetActorFromPool(actorClass,transform);
		if(actor) {
			rdRemoveInstanceX(sid,index);
			return (AActor*)actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstancesToActorsFromPool
//
// Not used now, the Proxy system takes care of all this
//.............................................................................
int32 ArdInstBaseActor::rdConvertInstancesToActorsFromPool(UStaticMesh* mesh,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors) {
	return rdConvertInstancesToActorsFromPoolX(rdGetSMsid(mesh),actorClass,mode,radius,start,finish,actors);
}
int32 ArdInstBaseActor::rdConvertInstancesToActorsFromPoolX(const FName sid,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors) {

	UWorld* world=GetWorld();
	TArray<FHitResult> hits;

	switch(mode) {
//.........................................................
		case rdTraceMode::RDTRACEMODE_LINE: {

			FCollisionShape shape=FCollisionShape::MakeSphere(radius);
			static const FName sphereTraceMultiName(TEXT("SphereTraceMultiForObjects"));
			TArray<AActor*> ignoreActors;

			TEnumAsByte<EObjectTypeQuery> objectToTrace=EObjectTypeQuery::ObjectTypeQuery12;
			TArray<TEnumAsByte<EObjectTypeQuery>> objectsToTraceAsByte;
			objectsToTraceAsByte.Add(objectToTrace);

			world->SweepMultiByObjectType(hits,start,finish,FQuat::Identity,FCollisionObjectQueryParams(objectsToTraceAsByte),shape);
//			bool ret=LineTraceMulti(world,start,finish,ETraceTypeQuery TraceChannel,bool bTraceComplex,const TArray< AActor * > & ActorsToIgnore,EDrawDebugTrace::Type DrawDebugType,TArray< FHitResult > & OutHits,bool bIgnoreSelf,FLinearColor TraceColor,FLinearColor TraceHitColor,float DrawTime
			break; }
//.........................................................
		case rdTraceMode::RDTRACEMODE_BOX:


			break;
//.........................................................
		case rdTraceMode::RDTRACEMODE_SPHERE:


			break;
//.........................................................
	}

	for(const FHitResult& hit:hits) {
		UPrimitiveComponent* hitComponent=hit.GetComponent();
		check(hitComponent);

		//...
	}

	return 0;
}

//.............................................................................
// rdCreateTempBakedDataAsset
//.............................................................................
UrdBakedDataAsset* ArdInstBaseActor::rdCreateTempBakedDataAsset() {

	return NewObject<UrdBakedDataAsset>();
}

//.............................................................................
// rdLoadBakedDataAsset
//.............................................................................
UrdBakedDataAsset* ArdInstBaseActor::rdLoadBakedDataAsset(TSoftObjectPtr<UrdBakedDataAsset>& data,bool create) {

	UrdBakedDataAsset* bda=data.Get();

	if(bda) {
		return bda;
	}

	bda=data.LoadSynchronous();
	if(bda) {
		return bda;
	}

#if WITH_EDITOR
	//data.Reset();
	//data.IsPending();
	if(!create) {
		return nullptr;
	}

	FString filename=data.GetLongPackageName();

	//} else 
	if(filename.IsEmpty()) {

		FString bdr=baseBakedAssetFolder+TEXT("/");
		int32 cnt=1;
		filename=FString::Printf(TEXT("%sBakedDataAsset_1"),*bdr);
		while(fileExists(filename)) {
			filename=FString::Printf(TEXT("%sBakedDataAsset_%d"),*bdr,cnt++);
		}
	}

	UPackage* package=CreatePackage(*filename);
	if(package) {
		FString name=filename.RightChop(filename.Find(TEXT("/"),ESearchCase::CaseSensitive,ESearchDir::FromEnd)+1);
		package->FullyLoad();
		bda=NewObject<UrdBakedDataAsset>(package,UrdBakedDataAsset::StaticClass(),FName(*name),RF_Public|RF_Standalone);
		bda->AddToRoot();
		package->SetDirtyFlag(true);
		FEditorFileUtils::PromptForCheckoutAndSave({package},false,false);
		FAssetRegistryModule::AssetCreated(bda);
		data=bda;
	}
#endif
	return bda;
}

//.............................................................................
// rdBakeVolumeTransformsForProxies
//.............................................................................
int32 ArdInstBaseActor::rdBakeVolumeTransformsForProxies(AActor* volume,TArray<FrdSpawnData>& bakedObjectList,int32 rows,int32 cols,bool storeTransforms) {
	static TArray<FName> sids;
	return rdBakeVolumeTransformsForProxiesX(volume,sids,bakedObjectList,rows,cols,storeTransforms);
}
int32 ArdInstBaseActor::rdBakeVolumeTransformsForProxiesX(AActor* volume,const TArray<FName>& sids,TArray<FrdSpawnData>& bakedObjectList,int32 rows,int32 cols,bool storeTransforms) {

	rdRemoveVolumeTransformsForProxies(volume);

	TArray<UInstancedStaticMeshComponent*> ismComps;
	TArray<UActorComponent*> comps;
	volume->GetComponents(comps,true);

	FBox box(FVector(99999,99999,99999),FVector(-99999,-99999,-99999));
	for(auto c:comps) {
		UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
		if(ismc) {
			UStaticMesh* mesh=ismc->GetStaticMesh();
			if(!mesh) continue;

			FrdProxySetup proxy;
			rdGetProxySettings(nullptr,mesh,proxy);
			if(proxy.proxyType==rdProxyType::RDPROXYTYPE_NONE) continue;

			FName csid=rdGetSMCsid(ismc);
			if(sids.Num()>0 && sids.Find(csid)==INDEX_NONE) continue;

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

	float totVolumeWidth=box.Max.X-box.Min.X;
	float totVolumeDepth=box.Max.Y-box.Min.Y;
	float subVolumeWidth=totVolumeWidth/(float)rows;
	float subVolumeHeight=totVolumeDepth/(float)cols;

	int32 totalSubVolumes=FMath::Max(1,rows*cols);

	bakedObjectList.SetNum(totalSubVolumes);

	for(float y=0;y<cols;y++) {
		for(float x=0;x<rows;x++) {
			FrdSpawnData& sd=bakedObjectList[(y*cols)+x];
			sd.center=FVector(box.Min.X+(x*subVolumeWidth)+(subVolumeWidth/2.0f),box.Min.Y+(y*subVolumeHeight)+(subVolumeHeight/2.0f),box.Min.Z);
			sd.distance=sqrt(((subVolumeWidth/2.0f)*(subVolumeWidth/2.0f))+((subVolumeHeight/2.0f)*(subVolumeHeight/2.0f)));
		}
	}

	int32 numInst=0;
	for(auto ismc:ismComps) {

		UStaticMesh* mesh=ismc->GetStaticMesh();
		FName csid=rdGetSMCsid(ismc);
		FrdProxySetup proxy;
		rdGetProxySettings(nullptr,mesh,proxy);
		int32 cnt=ismc->GetInstanceCount();

		TArray<FrdBakedSpawnObjects*> MeshToVolList_Ref;
		MeshToVolList_Ref.SetNum(totalSubVolumes);

		for(int32 i=0;i<totalSubVolumes;i++) {

			FrdSpawnData& sd=bakedObjectList[i];
			UrdBakedDataAsset* bda=rdCreateTempBakedDataAsset();
			if(!bda) continue;
			int32 n=bda->bakedData.Num(),j=0;
			for(;j<n;j++) if(bda->bakedData[j].sid==csid) break;
			if(j==n) {
				FrdBakedSpawnObjects newSO=FrdBakedSpawnObjects(csid);
				newSO.proxy=proxy;
				if(proxy.proxyType==RDPROXYTYPE_SHORTDISTANCE) {
					FrdProxyItem* pi=instancedMeshToProxyMap.Find(csid);
					if(!pi) {
						FrdProxyItem sitem;
						sitem.sid=csid;
						sitem.proxyType=RDPROXYTYPE_SHORTDISTANCE;
						sitem.actorClass=proxy.proxyActor.LoadSynchronous();
						sitem.bPooled=proxy.bPooled;
						sitem.bDontHide=proxy.bDontHide;
						sitem.bDontRemove=proxy.bDontRemove;
						sitem.instanceVolume=volume;
						sitem.proxyPhysicsTimeout=proxy.proxyPhysicsTimeout;
						sitem.bCallSwapEvent=proxy.bCallSwapEvent;
						sitem.proxyDataLayer=proxy.proxyDataLayer;
						sitem.proxyStaticMeshSid=rdGetSMXsid(ErdSpawnType::UseDefaultSpawn,proxy.proxyStaticMesh.Get(),proxy.proxyMeshMaterials,false,proxy.proxyMeshCollision);
						instancedMeshToProxyMap.Add(csid,sitem);
					}
				}
				FrdObjectPlacementData pd;
				sd.items.Add(pd);
				newSO.itemIndex=sd.items.Num()-1;
				bda->bakedData.Add(newSO);
			}
			MeshToVolList_Ref[i]=&bda->bakedData[j];
		}

		for(int i=0;i<cnt;i++) {

			FTransform tran;
			ismc->GetInstanceTransform(i,tran,true);
			FVector loc=tran.GetTranslation();

			int32 irow=fmin((loc.X-box.Min.X)/subVolumeWidth,rows-1);
			int32 icol=fmin((loc.Y-box.Min.Y)/subVolumeHeight,cols-1);

			int32 index=irow+(icol*rows);
			FrdSpawnData& sd=bakedObjectList[index];
			FrdBakedSpawnObjects* bso=MeshToVolList_Ref[index];
			if(bso) {
				if(storeTransforms) bso->transformMap.Add(tran);
				bso->transformMap.Precalc();
			}
		}
		numInst+=cnt;
	}

	spawnVolumeProxyScanList.Add(FrdSpawnDataScanItem(volume,bakedObjectList));

	return numInst;
}

//.............................................................................
// rdRemoveVolumeTransformsForProxies
//.............................................................................
void ArdInstBaseActor::rdRemoveVolumeTransformsForProxies(AActor* volume) {

	for(int32 i=0;i<spawnVolumeProxyScanList.Num();i++) {
		if(spawnVolumeProxyScanList[i].volume==volume) {
			spawnVolumeProxyScanList.RemoveAt(i);
			return;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
// rdHarvestInstancesFromActorList
//----------------------------------------------------------------------------------------------------------------
void ArdInstBaseActor::rdHarvestInstancesFromActorList(TArray<AActor*> actorList,bool justInstances,TArray<AActor*>& addedList,TMap<FName,FrdInstItemX>& scrapeMap) {

	// Add to the level
	AActor* firstActor=nullptr;
	TArray<UInstancedStaticMeshComponent*> ismComps;
	TArray<UStaticMeshComponent*> smComps;
	TArray<UChildActorComponent*> caComps;
	FBox box(FVector(99999,99999,99999),FVector(-99999,-99999,-99999));
	for(auto a:actorList) {
		if(!firstActor) firstActor=a;
		bool harvestedAll=false;

		if(a->Tags.Contains("NoInstancing")) {
			addedList.Add(a);
			continue;
		}

		TArray<UActorComponent*> comps;
		a->GetComponents(comps,true);
		bool compNoInst=false;
		for(auto c:comps) {
			if(c->ComponentTags.Contains("NoInstancing")) {
				compNoInst=true;
				break;
			}
		}
		if(compNoInst) {
			addedList.Add(a);
			continue;
		}

		for(auto c:comps) {
			UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
			if(ismc && ismc->GetStaticMesh()) {
				ismComps.Add(ismc);
				FBox bnds=ismc->Bounds.GetBox();
				if(bnds.Min.X<box.Min.X) box.Min.X=bnds.Min.X;
				if(bnds.Min.Y<box.Min.Y) box.Min.Y=bnds.Min.Y;
				if(bnds.Min.Z<box.Min.Z) box.Min.Z=bnds.Min.Z;
				if(bnds.Max.X>box.Max.X) box.Max.X=bnds.Max.X;
				if(bnds.Max.Y>box.Max.Y) box.Max.Y=bnds.Max.Y;
				if(bnds.Max.Z>box.Max.Z) box.Max.Z=bnds.Max.Z;
			}
			UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(c);
			if(smc && smc->GetStaticMesh()) {
				smComps.Add(smc);
				FBox bnds=smc->Bounds.GetBox();
				if(bnds.Min.X<box.Min.X) box.Min.X=bnds.Min.X;
				if(bnds.Min.Y<box.Min.Y) box.Min.Y=bnds.Min.Y;
				if(bnds.Min.Z<box.Min.Z) box.Min.Z=bnds.Min.Z;
				if(bnds.Max.X>box.Max.X) box.Max.X=bnds.Max.X;
				if(bnds.Max.Y>box.Max.Y) box.Max.Y=bnds.Max.Y;
				if(bnds.Max.Z>box.Max.Z) box.Max.Z=bnds.Max.Z;
			}
			UChildActorComponent* cac=Cast<UChildActorComponent>(c);
			if(cac && cac->GetChildActorClass()==AStaticMeshActor::StaticClass()) {
				AStaticMeshActor* ca=Cast<AStaticMeshActor>(cac->GetChildActor());
				if(ca && ca->GetStaticMeshComponent() && ca->GetStaticMeshComponent()->GetStaticMesh()) {
					smComps.Add(ca->GetStaticMeshComponent());
					FBox bnds=smc->Bounds.GetBox();
					if(bnds.Min.X<box.Min.X) box.Min.X=bnds.Min.X;
					if(bnds.Min.Y<box.Min.Y) box.Min.Y=bnds.Min.Y;
					if(bnds.Min.Z<box.Min.Z) box.Min.Z=bnds.Min.Z;
					if(bnds.Max.X>box.Max.X) box.Max.X=bnds.Max.X;
					if(bnds.Max.Y>box.Max.Y) box.Max.Y=bnds.Max.Y;
					if(bnds.Max.Z>box.Max.Z) box.Max.Z=bnds.Max.Z;
				}
			}
		}
		AStaticMeshActor* sma=Cast<AStaticMeshActor>(a);
		if(false) {//sma) {
			smComps.Add(sma->GetStaticMeshComponent());
			FVector origin,extent;
			sma->GetActorBounds(false,origin,extent,false);
			FBox bnds;
			bnds.Min=origin-extent;
			bnds.Max=origin+extent;
			if(bnds.Min.X<box.Min.X) box.Min.X=bnds.Min.X;
			if(bnds.Min.Y<box.Min.Y) box.Min.Y=bnds.Min.Y;
			if(bnds.Min.Z<box.Min.Z) box.Min.Z=bnds.Min.Z;
			if(bnds.Max.X>box.Max.X) box.Max.X=bnds.Max.X;
			if(bnds.Max.Y>box.Max.Y) box.Max.Y=bnds.Max.Y;
			if(bnds.Max.Z>box.Max.Z) box.Max.Z=bnds.Max.Z;
		}
	}

	int32 totalInstances=smComps.Num();
	for(auto ismc:ismComps) {
		totalInstances+=ismc->GetInstanceCount();
	}

	if(totalInstances==0) {
		return;
	}
/*
	FName baseFolder;
	GetUniqueWorldFolder(FName(*(firstActor->GetActorLabel()+TEXT("_Harvested"))),baseFolder);
	FName folder=baseFolder;

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world->World();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner=nullptr;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.NameMode=FActorSpawnParameters::ESpawnActorNameMode::Requested;
	SpawnParams.ObjectFlags=RF_Transactional;//|RF_Public
	SpawnParams.OverrideLevel=World->GetCurrentLevel();
	SpawnParams.bTemporaryEditorActor=false;
	SpawnParams.bHideFromSceneOutliner=false;
*/
	TArray<AActor*> meshActorList;
	for(auto ismc:ismComps) {
		UStaticMesh* mesh=ismc->GetStaticMesh();
		FName sid=rdGetSMCsid(ismc);
		int32 cnt=ismc->GetInstanceCount();
		for(int i=0;i<cnt;i++) {
			FTransform tran;
			ismc->GetInstanceTransform(i,tran,true);
			FrdInstItemX& ix=scrapeMap.FindOrAdd(sid);
			ix.transforms.Add(tran);
			ix.sid=sid;
		}
	}

	for(auto smc:smComps) {
		UStaticMesh* mesh=smc->GetStaticMesh();
		FTransform tran=smc->GetComponentTransform();
		FName sid=rdGetSMCsid(smc,smc->ComponentTags.Contains(TEXT("UseSM"))?ErdSpawnType::SMC:ErdSpawnType::UseDefaultSpawn);
		FrdInstItemX& ix=scrapeMap.FindOrAdd(sid);
		ix.transforms.Add(tran);
		ix.sid=sid;
	}
}

//.............................................................................
