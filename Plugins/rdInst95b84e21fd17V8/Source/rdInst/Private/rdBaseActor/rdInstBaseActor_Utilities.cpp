//
// rdInstBaseActor_Utilities.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 30th April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
#include "AssetCompilingManager.h"
#include "Materials/MaterialInterface.h"
#else
#include "ShaderCompiler.h"
#endif
#include "Landscape.h"
#if ENGINE_MAJOR_VERSION>4
#include "LandscapeStreamingProxy.h"
#include "WorldPartition/HLOD/HLODActor.h"
#endif
#include "Engine/CullDistanceVolume.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Containers/BitArray.h"
#include "GameFramework/Pawn.h"
#include "UObject/Package.h"
#include "rdActor.h"
#include "DrawDebugHelpers.h"
#include "rdActorStrings.h"

//----------------------------------------------------------------------------------------------------------------
// UrdInstData::FromString
//----------------------------------------------------------------------------------------------------------------
void UrdInstData::FromString(const FString& str) {

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT(","),false);
	if(num<11) {
		return;
	}

	int32 version=FCString::Atoi(*vals[0]);
	if(version>6 || (version==1 && num!=9) || (version==2 && num!=10) || (version==3 && num!=11) || (version==4 && num!=12)
			     || (version==5 && num!=13) || (version==6 && num!=15) || (version==7 && num!=15) ) {
		return;
	}

	int i=1;

	startCull=FCString::Atof(*vals[i++]);
	endCull=FCString::Atof(*vals[i++]);
	mobility=FCString::Atoi(*vals[i++]);
	collision=FCString::Atoi(*vals[i++]);
	minLOD=FCString::Atoi(*vals[i++]);
	lightChannel=FCString::Atoi(*vals[i++]);
	randomSeed=FCString::Atoi(*vals[i++]);
	instFlags=FCString::Atoi(*vals[i++]);
	if(version<7) {
		instFlags|=rdInst_RenderInMainPass|rdInst_RenderInDepthPass|rdInst_EvaluateWPO;
	}

	if(version>1) {
		numPerInstCustData=FCString::Atoi(*vals[i++]);
		if(version>2) {
			WPODisableDistance=FCString::Atoi(*vals[i++]);
			if(version>3) {
				shadowInvalidation=FCString::Atoi(*vals[i++]);
				if(version>4) {
					collisionProfile=vals[i++];
					if(version>5) {
						minDrawDistance=FCString::Atof(*vals[i++]);
						naniteProgDistance=FCString::Atof(*vals[i++]);
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
// UrdInstData::ToString()
//----------------------------------------------------------------------------------------------------------------
FString UrdInstData::ToString() {

	FString str="7"; // version

	str+=FString::Printf(TEXT(",%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%f,%f"),
						startCull,
						endCull,
						mobility,
						collision,
						minLOD,
						lightChannel,
						randomSeed,
						instFlags,
						numPerInstCustData,
						WPODisableDistance,
						shadowInvalidation,
						*collisionProfile,
						minDrawDistance,
						naniteProgDistance
						);

	return str;
}

//.............................................................................
// rdGetNumAssetsCompiling
//.............................................................................
int32 ArdInstBaseActor::rdGetNumAssetsCompiling() {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return FAssetCompilingManager::Get().GetNumRemainingAssets();
#else
	return GShaderCompilingManager->GetNumRemainingJobs();
#endif
}

//.............................................................................
// rdWaitForCompiling
//.............................................................................
void ArdInstBaseActor::rdWaitForCompiling() {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	FAssetCompilingManager::Get().FinishAllCompilation();
#else
	GShaderCompilingManager->FinishAllCompilation();
#endif
}

//.............................................................................
// rdSubmitMaterialsForCompile
//.............................................................................
void ArdInstBaseActor::rdSubmitMaterialsForCompile() {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	UMaterialInterface::SubmitRemainingJobsForWorld(GetWorld());
#endif
}

//.............................................................................
// rdIsEditor
//.............................................................................
bool ArdInstBaseActor::rdIsEditor() {
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

//.............................................................................
// rdIsPlaying
//.............................................................................
bool ArdInstBaseActor::rdIsPlaying() {
	return bIsPlaying;
}

//.............................................................................
// rdGetLandscapeBounds
//.............................................................................
FBox ArdInstBaseActor::rdGetLandscapeBounds(ALandscapeProxy* landscape) {

#if WITH_EDITOR 
#if ENGINE_MAJOR_VERSION>4

	ALandscape* mainLandscape=landscape->GetLandscapeActor();
	
	FBox box;
	if(mainLandscape!=landscape) {

		FIntRect rct=landscape->GetBoundingRect();

		FVector loc=landscape->GetActorLocation();

		box.Min.X=rct.Min.X*100;
		box.Min.Y=rct.Min.Y*100;
		box.Min.Z=loc.Z;
		box.Max.X=rct.Max.X*100;
		box.Max.Y=rct.Max.Y*100;
		box.Max.Z=loc.Z+(landscape->GetActorScale().Z*100);

	} else {

		box=mainLandscape->GetCompleteBounds();
		box.Max.Z*=2.0f;
	}
#else
	// comes out at zero
	FIntRect rct=landscape->GetBoundingRect();//+landscape->LandscapeSectionOffset;

	FBox box;
	box.Min.X=rct.Min.X;
	box.Min.Y=rct.Min.Y;
	box.Min.Z=0.0f;//rct.Min.Z;
	box.Max.X=rct.Max.X;
	box.Max.Y=rct.Max.Y;
	box.Max.Z=0.0f;//rct.Max.Z;
#endif
#else
	FBox box;
#endif
	return box;
}

//.............................................................................
// GetCurrentScalability
//.............................................................................
rdScalabilityScale ArdInstBaseActor::rdGetCurrentScalability() {
#if ENGINE_MAJOR_VERSION>4
	Scalability::FQualityLevels levels=Scalability::GetQualityLevels();

	int32 avg=0;

	avg+=levels.ViewDistanceQuality;
	avg+=levels.AntiAliasingQuality;
	avg+=levels.ShadowQuality;
	avg+=levels.GlobalIlluminationQuality;
	avg+=levels.ReflectionQuality;
	avg+=levels.PostProcessQuality;
	avg+=levels.TextureQuality;
	avg+=levels.EffectsQuality;
	avg+=levels.FoliageQuality;
	avg+=levels.ShadingQuality;
	avg/=10;

	return (rdScalabilityScale)avg;
#else
	return (rdScalabilityScale)0;
#endif
}

//.............................................................................
// GetScalabilityBenchmarks
//.............................................................................
bool ArdInstBaseActor::rdGetScalabilityBenchmarks(float& bmCpu,float& bmGpu) {

#if ENGINE_MAJOR_VERSION>4
	Scalability::FQualityLevels levels=Scalability::GetQualityLevels();

	bmCpu=levels.CPUBenchmarkResults;
	bmGpu=levels.GPUBenchmarkResults;

	return true;
#else
	return false;
#endif
}

//.............................................................................
// rdResolutionScale
//.............................................................................
float ArdInstBaseActor::rdResolutionScale() {
#if ENGINE_MAJOR_VERSION>4
	Scalability::FQualityLevels levels=Scalability::GetQualityLevels();
	return levels.ResolutionQuality;
#else
	return 0.0f;
#endif
}

//.............................................................................
// rdGetCurrentScalabilityFor
//.............................................................................
rdScalabilityScale ArdInstBaseActor::rdGetCurrentScalabilityFor(rdScalabilityType stype) {

#if ENGINE_MAJOR_VERSION>4
	Scalability::FQualityLevels levels=Scalability::GetQualityLevels();

	switch(stype) {
		case RDSCALETYPE_VIEWDISTANCE: 
			return (rdScalabilityScale)levels.ViewDistanceQuality;
		case RDSCALETYPE_AA: 
			return (rdScalabilityScale)levels.AntiAliasingQuality;
		case RDSCALETYPE_SHADOW: 
			return (rdScalabilityScale)levels.ShadowQuality;
		case RDSCALETYPE_GI: 
			return (rdScalabilityScale)levels.GlobalIlluminationQuality;
		case RDSCALETYPE_REFLECTION: 
			return (rdScalabilityScale)levels.ReflectionQuality;
		case RDSCALETYPE_POSTPROCESS: 
			return (rdScalabilityScale)levels.PostProcessQuality;
		case RDSCALETYPE_TEXTURE: 
			return (rdScalabilityScale)levels.TextureQuality;
		case RDSCALETYPE_EFFECTS: 
			return (rdScalabilityScale)levels.EffectsQuality;
		case RDSCALETYPE_FOLIAGE: 
			return (rdScalabilityScale)levels.FoliageQuality;
		case RDSCALETYPE_SHADING: 
			return (rdScalabilityScale)levels.ShadingQuality;
	}
#endif
	return RDSCALE_NONE;
}

//.............................................................................
// rdSphereTrace
//.............................................................................
void ArdInstBaseActor::rdSphereTrace(const FVector loc,float radius,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors) {

	TArray<FHitResult> hits;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindProxies),false,nullptr);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	QueryParams.bTraceIntoSubComponents=false;
#endif
	const ECollisionChannel CollisionChannel=ECC_Visibility;
	FCollisionResponseParams ResponseParams(ECR_Overlap);
	
	bool ret=GetWorld()->SweepMultiByChannel(hits,loc,loc,FQuat::Identity,CollisionChannel,FCollisionShape::MakeSphere(radius),QueryParams);

	if(ret && hits.Num()>0) {

		TMap<UInstancedStaticMeshComponent*,FrdInstanceItems> instanceMap;
		for(auto& h:hits) {
			AActor* hitActor=h.GetActor();
			USceneComponent* hitComponent=h.GetComponent(); 
			UInstancedStaticMeshComponent* hitISM=Cast<UInstancedStaticMeshComponent>(hitComponent);

			if(hitISM) {
				int32 instanceIndex=h.Item;
				FTransform t;
				hitISM->GetInstanceTransform(instanceIndex,t,true);

				FrdInstanceItems& pii=instanceMap.FindOrAdd(hitISM);
				if(!pii.instances.Contains(instanceIndex)) {
					pii.sid=rdGetSMCsid(hitISM);
					pii.instances.Add(instanceIndex);
					pii.transforms.Add(t);
				}

			} else if(hitActor) {

				actors.Add(hitActor);
			}
		}
		for(auto& it:instanceMap) {
			instances.Add(it.Value);
		}
	}
}

//.............................................................................
// rdBoxTrace
//.............................................................................
void ArdInstBaseActor::rdBoxTrace(const FVector loc,FVector halfSize,FRotator rot,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors) {

	TArray<FHitResult> hits;
	bool ret=UKismetSystemLibrary::BoxTraceMulti(GetWorld(),loc,loc,halfSize,rot,ETraceTypeQuery::TraceTypeQuery1,false,ignoreActors,EDrawDebugTrace::None,hits,true);

	if(ret && hits.Num()>0) {

		TMap<UStaticMesh*,FrdInstanceItems> instanceMap;
		for(auto& h:hits) {
			AActor* hitActor=h.GetActor();
			USceneComponent* hitComponent=h.GetComponent(); 
			UInstancedStaticMeshComponent* hitISM=Cast<UInstancedStaticMeshComponent>(hitComponent);
			if(hitISM) {
				UStaticMesh* hitMesh=hitISM->GetStaticMesh();
				int32 instanceIndex=h.Item;
				FrdInstanceItems* pii=instanceMap.Find(hitMesh);
				if(pii) {
					pii->instances.Add(instanceIndex);
					pii->transforms.Add(hitActor->GetActorTransform());
				} else {
					FrdInstanceItems ii;
					ii.sid=rdGetSMCsid(hitISM);
					ii.instances.Add(instanceIndex);
					ii.transforms.Add(hitActor->GetActorTransform());
					instanceMap.Add(hitMesh,ii);
				}

			} else if(hitActor) {

				ArdActor* hitRdActor=Cast<ArdActor>(hitActor);
				if(hitRdActor) {
					actors.Add(hitActor);
				}
			}
		}

		for(auto& it:instanceMap) {
			instances.Add(it.Value);
		}

	}
}

//.............................................................................
// rdGetInstancesAtLocation
//.............................................................................
bool ArdInstBaseActor::rdGetInstancesAtLocation(UInstancedStaticMeshComponent* ismc,const FVector loc,TArray<int32>& indexes,float tol) {

	if(!ismc) return false;
	
	TArray<FInstancedStaticMeshInstanceData>& instData=ismc->PerInstanceSMData;
	FVector iloc;
	int32 num=ismc->GetInstanceCount();

	indexes.Empty();

	for(int32 i=0;i<num;i++) {

		FMatrix& t=instData[i].Transform;
		iloc.X=t.M[3][0];
		iloc.Y=t.M[3][1];
		iloc.Z=t.M[3][2];

		if(FVector::Distance(loc,iloc)<=tol) {
			indexes.Add(i);
		}
	}

	return indexes.Num()>0;
}

//.............................................................................
// rdGetFirstInstanceAtLocation
//.............................................................................
int32 ArdInstBaseActor::rdGetFirstInstanceAtLocation(UInstancedStaticMeshComponent* ismc,const FVector loc,float tol) {

	if(!ismc) return false;
	
	TArray<FInstancedStaticMeshInstanceData>& instData=ismc->PerInstanceSMData;
	FVector iloc;
	int32 num=ismc->GetInstanceCount();

	for(int32 i=0;i<num;i++) {

		FMatrix& t=instData[i].Transform;
		iloc.X=t.M[3][0];
		iloc.Y=t.M[3][1];
		iloc.Z=t.M[3][2];

		if(FVector::Distance(loc,iloc)<=tol) {
			return i;
		}
	}

	return -1;
}

//.............................................................................
// rdSetActorLabel
//.............................................................................
void ArdInstBaseActor::rdSetActorLabel(AActor* actor,const FName label) {
#if WITH_EDITOR
	if(actor) actor->SetActorLabel(label.ToString());
#endif
}

//.............................................................................
// rdSpawnActor
//.............................................................................
AActor* ArdInstBaseActor::rdSpawnActor(TSubclassOf<class AActor> actorClass,const FTransform& transform,AActor* actTemplate,bool temp,bool attach,const FName label,AActor* parent,bool deferConstruction,const FString& strProps) {

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("rdSpawnActor(%s)"),(!actorClass->GetName().IsEmpty())?*actorClass->GetName():TEXT("No Class Name"));
#endif

#if WITH_EDITOR
	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world?world->World():GetWorld();
#else
	UWorld* World=GetWorld();
#endif
	// UK2Node_custructobjectfromclass 
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner=parent;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.NameMode=FActorSpawnParameters::ESpawnActorNameMode::Requested;
	SpawnParams.bDeferConstruction=deferConstruction;
//	SpawnParams.ObjectFlags=RF_Public|RF_Transactional|(temp?RF_Transient:RF_NoFlags);
//	SpawnParams.ObjectFlags=temp?RF_Transient|RF_Public:RF_Public|RF_Transactional;
	SpawnParams.ObjectFlags=temp?RF_Transient:RF_Transactional;
	SpawnParams.OverrideLevel=World->GetCurrentLevel();
//	SpawnParams.OverrideParentComponent=parent;
	SpawnParams.bAllowDuringConstructionScript=true;
#if WITH_EDITOR
	SpawnParams.bTemporaryEditorActor=temp;
	SpawnParams.bHideFromSceneOutliner=temp;
#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	SpawnParams.bCreateActorPackage=false;//true;
	SpawnParams.OverridePackage=(parent&&!(SpawnParams.ObjectFlags&RF_Transient))?parent->GetExternalPackage():nullptr;
//	SpawnParams.OverrideActorGuid=CachedInstanceData?CachedInstanceData->ChildActorGUID:FGuid();
#endif
	SpawnParams.Template=actTemplate;
#endif
	AActor* actor=World->SpawnActor<AActor>(actorClass,transform,SpawnParams);
	if(actor && attach) {

		if(actor->GetRootComponent()==nullptr) {
			for(auto c:actor->GetComponents()) {
				USceneComponent* sc=Cast<USceneComponent>(c);
				if(sc && (sc->GetName().StartsWith(TEXT("Default")) || sc->GetName().StartsWith(TEXT("shared")))) {
					actor->SetRootComponent(sc);
					break;
				}
			}
		}
		if(parent) {
			actor->AttachToActor(parent,FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	if(actor) {
		actor->SetActorScale3D(transform.GetScale3D());

		if(!strProps.IsEmpty()) {
			int32 ver=FCString::Atoi(*FrdString::UpToFirst(strProps,'|'));
			FString str=FrdString::AfterFirst(strProps,'|');
			rdApplyStringProps(actor,str,ver);
		}
#if WITH_EDITOR
		if(label.GetStringLength()>0 && label!=TEXT("None")) actor->SetActorLabel(label.ToString());
#endif
	}

	return actor;
}

//.............................................................................
// rdSpawnVFX
//.............................................................................
UNiagaraComponent* ArdInstBaseActor::rdSpawnVFX(UNiagaraSystem* fx,const FTransform& transform,AActor* parent) {

	UNiagaraComponent* vfx=UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),fx,transform.GetTranslation(),transform.Rotator(),transform.GetScale3D(),false,true,ENCPoolMethod::AutoRelease,true);
	if(vfx) {
		vfx->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		vfx->SetCullDistance(10000.0f);
#if ENGINE_MAJOR_VERSION>5 && ENGINE_MINOR_VERSION>2
		vfx->ShadowCacheInvalidationBehavior=EShadowCacheInvalidationBehavior::Static;
#endif
	}
	return vfx;
}

//.............................................................................
// rdRemoveVFX
//.............................................................................
void ArdInstBaseActor::rdRemoveVFX(UNiagaraComponent* fxc) {

	if(fxc) {
		fxc->SetPaused(true);
		fxc->DestroyInstance();
	}
}

//.............................................................................
// getLandscapeZ
//.............................................................................
double ArdInstBaseActor::getLandscapeZ(double x,double y,float radius,FHitResult& hit,bool fixSlopes,bool hitSM) {

	float z=0;
	FCollisionQueryParams queryParams;
	queryParams.bReturnPhysicalMaterial=true;
	FCollisionObjectQueryParams objectQueryParams(FCollisionObjectQueryParams::AllStaticObjects);
	//objectQueryParams.IgnoreMask=;//FMaskFilter
	objectQueryParams.ObjectTypesToQuery=ECollisionChannel::ECC_WorldStatic;

	bool ret=GetWorld()->LineTraceSingleByObjectType(hit,FVector(x,y,pgLandscapeZ1),FVector(x,y,pgLandscapeZ2),objectQueryParams,queryParams);

#if ENGINE_MAJOR_VERSION>4
	ALandscape* la=hit.HitObjectHandle.FetchActor<ALandscape>();
	ALandscapeStreamingProxy* lasp=hit.HitObjectHandle.FetchActor<ALandscapeStreamingProxy>();
#else
	ALandscape* la=Cast<ALandscape>(hit.Actor);
	ALandscape* lasp=nullptr;
#endif
	if(hit.bBlockingHit && (la || lasp || hitSM)) {
		
		z=hit.ImpactPoint.Z;

		if(fixSlopes) {
			FHitResult hit2;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(x-radius,y-radius,pgLandscapeZ1),FVector(x,y,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z1=hit2.ImpactPoint.Z;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(x+radius,y-radius,pgLandscapeZ1),FVector(x,y,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z2=hit2.ImpactPoint.Z;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(x-radius,y+radius,pgLandscapeZ1),FVector(x,y,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z3=hit2.ImpactPoint.Z;
			GetWorld()->LineTraceSingleByChannel(hit2,FVector(x+radius,y+radius,pgLandscapeZ1),FVector(x,y,pgLandscapeZ2),ECollisionChannel::ECC_Visibility,queryParams);
			float z4=hit2.ImpactPoint.Z;
			z1=fmin(z1,z2);
			z2=fmin(z3,z4);
			z=fmin(z1,z2);
		}
	} else {
		//UE_LOG(LogTemp,Display,TEXT("rdInst: Failed to trace to landscape at %f,%f -> %f-%f"),x,y,pgLandscapeZ1,pgLandscapeZ2);//@@
		hit.bBlockingHit=false;
	}

	return z;
}

//.............................................................................
// GetUsedISMCs
//.............................................................................
TArray<UInstancedStaticMeshComponent*> ArdInstBaseActor::GetUsedISMCs() {

	TArray<UInstancedStaticMeshComponent*> comps;
	GetComponents(comps);
	return comps;
}

//.............................................................................
// GetActorsWithSelectedISMS
//.............................................................................
int32 ArdInstBaseActor::GetActorsWithSelectedISMS(TArray<ArdActor*>& actors,bool& beingEdited,bool filterOutSelected) {

	int32 cnt=0;
#if WITH_EDITOR
	TArray<UInstancedStaticMeshComponent*> ismcs;
	GetComponents(ismcs);

	for(auto& ismc:ismcs) {

#if ENGINE_MAJOR_VERSION>4 
		TBitArray ba=ismc->SelectedInstances;
#else
		TBitArray<> ba=ismc->SelectedInstances;
#endif
		if(ba.Num()>0) {
			for(int32 i=0;i<ba.Num();i++) {
				if(ba[i]==1) {

					AActor* selActor=rdGetActorFromInstanceIndex(ismc,i);
					if(selActor) {
						ArdActor* rdA=Cast<ArdActor>(selActor);
						if(rdA) {
							actors.AddUnique(rdA);
							cnt++;
						}
					} else {
						// Edit Instance mode
						ArdActor* rdA=Cast<ArdActor>(selActor);
						if(rdA) {
							actors.Empty();
							actors.Add(rdA);
							cnt=1;
							return cnt;
						}
					}
				}
			}
		}
	}
#endif
	return cnt;
}

//.............................................................................
// rdGetEditorCameraPos
//.............................................................................
void ArdInstBaseActor::rdGetEditorCameraPos(FVector& loc,FRotator& rot) {
#if WITH_EDITOR
	FViewport* vp=GEditor->GetActiveViewport();
	FEditorViewportClient* vc=vp?(FEditorViewportClient*)vp->GetClient():nullptr;
	if(vc) {
		loc=vc->GetViewLocation();
		rot=vc->GetViewRotation();
	}
#endif
}

//.............................................................................
// rdGetControlledPawn
//.............................................................................
APawn* ArdInstBaseActor::rdGetControlledPawn() {

	APlayerController* controller=GEngine->GetFirstLocalPlayerController(GetWorld());
	if(!controller) return nullptr;
	if(controller->bPlayerIsWaiting) return nullptr ; // waiting for match to begin

	APawn* pawn=controller->AcknowledgedPawn;// multi player
	if(!pawn) {
		pawn=Cast<APawn>(UGameplayStatics::GetPlayerPawn(GetWorld(),0)); // single player
	}

	return pawn;
}

//.............................................................................
// rdDrawLine
//.............................................................................
void ArdInstBaseActor::rdDrawLine(const FVector& start,const FVector& end,const FColor& col,float duration) {
	DrawDebugLine(GetWorld(),start,end,col,true,-1,0,0.2f);
}

//.............................................................................
// rdMoveBaseActor
//.............................................................................
void ArdInstBaseActor::rdMoveBaseActor(const FVector& location,const FRotator& rot) {
#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION>4
	SetLockLocation(false);
#endif
#endif
	SetActorLocation(location);
	SetActorRotation(rot);
#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION>4
	SetLockLocation(true);
#endif
#endif
}

//.............................................................................
// rdReseatBaseActor
//.............................................................................
void ArdInstBaseActor::rdReseatBaseActor(const FVector& location,const FRotator& rot) {

	FVector currentLocation=GetActorLocation();
	FVector diff=currentLocation-location;
	FTransform transform(FRotator(0,0,0),diff,FVector(1,1,1));

	SetActorLocation(location);

	TArray<UActorComponent*> list;
	GetComponents(UInstancedStaticMeshComponent::StaticClass(),list);
	for(auto comp:list) {
		UInstancedStaticMeshComponent* ismc=(UInstancedStaticMeshComponent*)comp;
		rdIncrementAllTransformsFast(ismc,transform);
	}

	SetActorRotation(rot);
}

//.............................................................................
// rdMeshIsNanite
//.............................................................................
bool ArdInstBaseActor::rdMeshIsNaniteX(const FName sid) {
	return rdMeshIsNanite(rdGetMeshFromSid(sid));
}
bool ArdInstBaseActor::rdMeshIsNanite(const UStaticMesh* mesh) {
#if ENGINE_MAJOR_VERSION>4

#endif
	return false;
}

//.............................................................................
// rdUpdateNav
//.............................................................................
void ArdInstBaseActor::rdUpdateNav() {

	TArray<UInstancedStaticMeshComponent*> list;
	GetComponents(UInstancedStaticMeshComponent::StaticClass(),(TArray<UActorComponent*>&)list);
	for(auto comp:list) {
		comp->UpdateBounds();
	}
}

//.............................................................................
// rdQueueProxySwapInDelegate
//.............................................................................
void ArdInstBaseActor::rdQueueProxySwapInDelegate(AActor* proxyActor,const FName sid,int32 index,UInstancedStaticMeshComponent* ismc,UrdStateDataAsset* state) {

	delegateQueue.Add(rdDelegateQueueItem(0,proxyActor,sid,index,ismc,state));
}

//.............................................................................
// rdQueueProxySwapOutDelegate
//.............................................................................
void ArdInstBaseActor::rdQueueProxySwapOutDelegate(AActor* proxyActor,const FName sid,int32 index,UInstancedStaticMeshComponent* ismc,UrdStateDataAsset* state) {

	delegateQueue.Add(rdDelegateQueueItem(1,proxyActor,sid,index,ismc,state));
}

//.............................................................................
// rdQueueProxySpawnOnServerDelegate
//.............................................................................
void ArdInstBaseActor::rdQueueProxySpawnOnServerDelegate(UClass* actorClass,const FTransform& transform,const FName sid,int32 index,UrdStateDataAsset* state) {

	delegateQueue.Add(rdDelegateQueueItem(2,actorClass,transform,sid,index,state));
}

//.............................................................................
// rdQueuePickupFocusedDelegate
//.............................................................................
void ArdInstBaseActor::rdQueuePickupFocusedDelegate(int32 pickupID,const FTransform& transform,const FName sid,int32 index) {

	delegateQueue.Add(rdDelegateQueueItem(3,pickupID,transform,sid,index));
}

//.............................................................................
// rdQueuePickupUnfocusedDelegate
//.............................................................................
void ArdInstBaseActor::rdQueuePickupUnfocusedDelegate(int32 pickupID,const FTransform& transform,const FName sid,int32 index) {

	delegateQueue.Add(rdDelegateQueueItem(4,pickupID,transform,sid,index));
}

//.............................................................................
// rdQueuePickedUpDelegate
//.............................................................................
void ArdInstBaseActor::rdQueuePickedUpDelegate(int32 pickupID,const FTransform& transform,const FName sid,int32 index) {

	delegateQueue.Add(rdDelegateQueueItem(5,pickupID,transform,sid,index));
}

void ArdInstBaseActor::rdQueueProxyDamage(UPrimitiveComponent* ocomp,AActor* actor,FVector& loc,FVector& impulse,FHitResult& hit) {

	delegateQueue.Add(rdDelegateQueueItem(6,ocomp,actor,loc,impulse,hit));
}
	
//.............................................................................
// rdProcessBeforeDelegateQueue
//
//.............................................................................
void ArdInstBaseActor::rdProcessBeforeDelegateQueue() {

	for(rdDelegateQueueItem& dqi:delegateQueue) {
		switch(dqi.type) {
			case 1: { // ProxySwapOut
				if(OnProxySwapOutDelegate.IsBound()) {
					FEditorScriptExecutionGuard ScriptGuard;
					OnProxySwapOutDelegate.Broadcast(dqi.proxyActor,dqi.sid,dqi.index,dqi.ismc,dqi.state);
				}
				break; }
			case 4: { // PickupUnfocused
				if(OnPickupUnfocusedDelegate.IsBound()) {
					FEditorScriptExecutionGuard ScriptGuard;
					OnPickupUnfocusedDelegate.Broadcast(dqi.pickupID,dqi.transform,dqi.sid,dqi.index);
				}
				break; }
		}
	}
}

//.............................................................................
// rdProcessAfterDelegateQueue
//
// The Proxy Swapping queues these events when objects are swapped or highlighted
// in other threads, these events get broadcast here in the main thread
//.............................................................................
void ArdInstBaseActor::rdProcessAfterDelegateQueue() {
/*
	// Go through and process the UnFocus Delegates first so any Focus delegate can be generically handled
	// in Blueprint - not exactly an elegant solution, but for now it works and not exactly slow or anything.
	for(rdDelegateQueueItem& dqi:delegateQueue) {
		if(dqi.type==4) {
			if(OnPickupUnfocusedDelegate.IsBound()) {
				FEditorScriptExecutionGuard ScriptGuard;
				OnPickupUnfocusedDelegate.Broadcast(dqi.pickupID,dqi.transform,dqi.sid,dqi.index);
			}
		}
	}
*/
	for(rdDelegateQueueItem& dqi:delegateQueue) {

		switch(dqi.type) {
			case 0: { // ProxySwapIn
				if(OnProxySwapInDelegate.IsBound()) {
					FEditorScriptExecutionGuard ScriptGuard;
					OnProxySwapInDelegate.Broadcast(dqi.proxyActor,dqi.sid,dqi.index,dqi.ismc,dqi.state);
				}
				break; }
//			case 1: { // ProxySwapOut
//				if(OnProxySwapOutDelegate.IsBound()) {
//					FEditorScriptExecutionGuard ScriptGuard;
//					OnProxySwapOutDelegate.Broadcast(dqi.proxyActor,dqi.sid,dqi.index,nullptr,dqi.state);
//				}
//				break; }
			case 2: { // ProxySpawnOnServer
				if(OnSpawnOnServerDelegate.IsBound()) {
					FEditorScriptExecutionGuard ScriptGuard;
					OnSpawnOnServerDelegate.Broadcast(dqi.actorClass,dqi.transform,dqi.state,dqi.sid,dqi.index);
				}
				break; }
			case 3: { // PickupFocused
				if(OnPickupFocusedDelegate.IsBound()) {
					FEditorScriptExecutionGuard ScriptGuard;
					OnPickupFocusedDelegate.Broadcast(dqi.pickupID,dqi.transform,dqi.sid,dqi.index);
				}
				break; }
			case 4: { // PickupUnfocused
				//if(OnPickupUnfocusedDelegate.IsBound()) {
				//	FEditorScriptExecutionGuard ScriptGuard;
				//	OnPickupUnfocusedDelegate.Broadcast(dqi.pickupID,dqi.transform,dqi.sid,dqi.index);
				//}
				break; }
			case 5: { // PickedUp
				if(OnPickedUpDelegate.IsBound()) {
					FEditorScriptExecutionGuard ScriptGuard;
					OnPickedUpDelegate.Broadcast(dqi.pickupID,dqi.transform,dqi.sid,dqi.index);
				}
				break; }
			case 6: { // Damage Event
				if(OnDamageDelegate.IsBound()) {
					FEditorScriptExecutionGuard ScriptGuard;
					OnDamageDelegate.Broadcast(dqi.comp,dqi.actor,dqi.location,dqi.impulse,dqi.hit);
				}
				break; }
		}
	}

	delegateQueue.Empty();
}

//.............................................................................
// rdIsMeshNanite
//.............................................................................
bool ArdInstBaseActor::rdIsMeshNanite(const UStaticMesh* mesh) {

#if ENGINE_MAJOR_VERSION<5
	return false;
#else

#if ENGINE_MINOR_VERSION<3
	return false;//(mesh->GetRenderData() && mesh->GetRenderData()->HasValidNaniteData());
#else
#if WITH_EDITOR
	return mesh->IsNaniteEnabled();
#else
	return (mesh->GetRenderData() && mesh->GetRenderData()->HasValidNaniteData());
#endif
#endif
#endif
}

#if WITH_EDITOR
//.............................................................................
// AddProxyToReferencedAssets
//.............................................................................
void ArdInstBaseActor::AddProxyToReferencedAssets(const FrdProxySetup& proxy) {

	switch(proxy.proxyType) {
		case rdProxyType::RDPROXYTYPE_SHORTDISTANCE:
		case rdProxyType::RDPROXYTYPE_LONGDISTANCE:
		case rdProxyType::RDPROXYTYPE_DESTRUCTION: {
			if(!proxy.proxyActor.IsNull()) {
				LevelReferencedActorClasses.AddUnique(proxy.proxyActor);
			}
			if(!proxy.proxyDataLayer.IsNull()) {
				TSoftClassPtr<UDataLayerAsset> dl=TSoftClassPtr<UDataLayerAsset>(proxy.proxyDataLayer.ToSoftObjectPath());
				LevelReferencedDataLayers.AddUnique(dl);
			}
			if(!proxy.proxyStaticMesh.IsNull()) {
				LevelReferencedMeshes.AddUnique(proxy.proxyStaticMesh);
			}
			for(auto& m:proxy.proxyMeshMaterials) {
				if(!m.IsNull()) LevelReferencedMaterials.AddUnique(m);
			}
			if(!proxy.destroyedMesh.IsNull()) {
				LevelReferencedMeshes.AddUnique(proxy.destroyedMesh);
			}
			for(auto& m:proxy.destroyedMeshMaterials) {
				if(!m.IsNull()) LevelReferencedMaterials.AddUnique(m);
			}
			if(!proxy.destroyedPrefab.IsNull()) {
				LevelReferencedActorClasses.AddUnique(proxy.destroyedPrefab);
			}
			break; }
		case rdProxyType::RDPROXYTYPE_PICKUP: {
			TSoftObjectPtr<UMaterialInterface> smat=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/rdInst/Materials/M_rdHighlight.M_rdHighlight")));
			LevelReferencedMaterials.AddUnique(smat);
			break; }
	}
}
#endif

//.............................................................................
