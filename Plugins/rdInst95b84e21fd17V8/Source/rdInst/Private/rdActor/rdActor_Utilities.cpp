//
// rdActor_Utilities.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 16th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/Package.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

TArray<ArdActor*> rdDirtyList;
TArray<ArdActor*> rdTickHookList;

//.............................................................................
//  GetFunctionCycles
//.............................................................................
void ArdActor::rdGetFunctionCycles() {
	
	{ 
#if WITH_EDITOR
		SCOPE_CYCLE_COUNTER(STAT_rdFunctionCyclesFunction); 
#endif

		rdFunctionCyclesFunction();
	}
}

//.............................................................................
//  FellOutOfWorld
//.............................................................................
void ArdActor::FellOutOfWorld(const UDamageType& dmgType) {

	if(rdGetBaseActor() && rdBaseActor->rdIsActorInPool(this)) {

		rdBaseActor->rdReturnActorToPool(this);

	} else {

		Super::FellOutOfWorld(dmgType);
	}
}

//.............................................................................
//  LifeSpanExpired
//.............................................................................
void ArdActor::LifeSpanExpired() {

	if(rdGetBaseActor() && rdBaseActor->rdIsActorInPool(this)) {

		rdBaseActor->rdReturnActorToPool(this);

	} else {

		Super::LifeSpanExpired();
	}
}

//.............................................................................
//  rdIsRealWorld
//.............................................................................
bool ArdActor::rdIsRealWorld() {
#if WITH_EDITOR
	UWorld* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	return GetWorld()==world;
	// I'm going to revisit this later, but it appears like WorldType does not always reflect the correct type
	// probably just something I did though...
	// GetWorld()->WorldType==;
	// EWorldType::Editor or None or EditorPreview or Inactive
#else
	return true;
#endif
}

//.............................................................................
//  SetActorHiddenInGame
//.............................................................................
void ArdActor::SetActorHiddenInGame(bool hidden) {
	Super::SetActorHiddenInGame(hidden);
	if(hidden) {
		rdBuildAsHidden();
		rdOnHidePrefab();
	} else {
		rdBuildAsVisible();
		rdOnShowPrefab();
	}
}

//.............................................................................
//  rdGetBaseActor
//
// This routine gets the rdInstBaseActor in the scene:
//
// If the rdBaseActor has been filled, just return that. (fast)
// If this Actor has an ArdActor as a Parent, it tries to find the BaseActor from there. (not slow)
// Look through the World to see if one exists, if it does - use that (not fast)
// If there isn't one, it spawns one and returns that.  (slowest, but only happens once)
//
//.............................................................................
bool ArdActor::rdGetBaseActor() {

	if(rdBaseActor) return true;
	if(!GetWorld()) return false;

	if(GetOwner()) {
		ArdActor* parActor=Cast<ArdActor>(GetOwner());
		if(parActor) {
			parActor->rdGetBaseActor(); // majority of gets will just get the owners one, much faster
			rdBaseActor=parActor->rdBaseActor;
			if(rdBaseActor) return true;
		}
	}

	TArray<AActor*> existingBaseActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ArdInstBaseActor::StaticClass(),existingBaseActors);
	if(existingBaseActors.Num()>0) {
		rdBaseActor=Cast<ArdInstBaseActor>(existingBaseActors[0]);
		return true;
	}

	// haven't found one, lets spawn it
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags=RF_Transactional;
	SpawnParams.bAllowDuringConstructionScript=true;
	SpawnParams.Owner=nullptr;

	rdBaseActor=(ArdInstBaseActor*)GetWorld()->SpawnActor<ArdInstBaseActor>(SpawnParams);

	if(!rdBaseActor) {
		return false;
	}
	rdBaseActor->Tags.Add("NoHarvest");
	rdBaseActor->Tags.Add("NoInstancing");

	rdBaseActor->ClearFlags(RF_HasExternalPackage);
#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION>4
	rdBaseActor->SetIsSpatiallyLoaded(false);
#endif
	rdBaseActor->SetActorLabel("rdInst Settings");
#if ENGINE_MAJOR_VERSION>4
	rdBaseActor->SetLockLocation(true);
#endif
	rdBaseActor->bRunConstructionScriptOnDrag=false;
#endif

	rdBaseActor->SetActorLocation(FVector(0,0,0));
	rdBaseActor->Tags.Add("rdBaseActor");

	return true;
}

//.............................................................................
// rdGetBase
//.............................................................................
ArdInstBaseActor* ArdActor::rdGetBase() {
	rdGetBaseActor();
	return rdBaseActor;
}

//.............................................................................
// rdTestForActorChange
//.............................................................................
void ArdActor::rdTestForActorChange(bool testTransform,bool testVisibility) {

	bool isVis=GetRootComponent()->IsVisible();
	if(isVis!=lastIsVis) {

		rdBuildFolderVisibility(GetRootComponent(),TEXT(""),isVis);
		lastIsVis=isVis;
	}

	if(InstanceDataX.Num()==0) return;

	FTransform tran((FTransform&)GetActorTransform());
	if(!tran.Equals(lastActorTransform)) {

		rdUpdateISMTransforms();
		lastActorTransform=tran;
	}
}

//.............................................................................
// rdDirty
//.............................................................................
void ArdActor::rdDirty(int32 countdown) {
	dirtyCountDown=countdown;
	rdDirtyList.AddUnique(this);
}

//.............................................................................
//  rdGetDirtyList - Static method to return the list of dirty Actors
//.............................................................................
TArray<ArdActor*>&  ArdActor::rdGetDirtyList() {
	return rdDirtyList;
}

//.............................................................................
//  rdGetHookList - Static method to return the list of Actors with Tick Hooks
//.............................................................................
TArray<ArdActor*>&  ArdActor::rdGetTickHookList() {
	return rdTickHookList;
}

//.............................................................................
// rdAddToTickHookList
//.............................................................................
void ArdActor::rdAddToTickHookList() {

	rdTickHookList.AddUnique(this);
}

//.............................................................................
// rdRemFromTickHookList
//.............................................................................
void ArdActor::rdRemFromTickHookList() {

	rdDirtyList.AddUnique(this);
}

//.............................................................................
// rdGetFolderName
//.............................................................................
FString ArdActor::rdGetFolderName(USceneComponent* comp) {

	FString cm=comp->GetName();
	int32 pos=cm.Find(TEXT("_GEN_VARIABLE"));
	if(pos>0) cm=cm.Left(pos);

	return cm;
}

//.............................................................................
// rdAddStaticMesh
//.............................................................................
UStaticMeshComponent* ArdActor::rdAddStaticMesh(UStaticMesh* mesh,const FTransform& transform,const FString& label) {
	return rdAddStaticMeshX(rdGetSMsid(mesh),transform,label);
}
UStaticMeshComponent* ArdActor::rdAddStaticMeshX(const FName sid,const FTransform& transform,const FString& label) {

	UStaticMeshComponent* smc=(UStaticMeshComponent*)ArdActor::rdAddComponent(UStaticMeshComponent::StaticClass());
	if(smc) {
		AddInstanceComponent(smc);
		smc->SetMobility(EComponentMobility::Movable);
		smc->bSelectable=true;
		smc->SetStaticMesh(rdGetMeshFromSid(sid));
		TArray<TSoftObjectPtr<UMaterialInterface>> smats=rdGetMaterialsFromSid(sid);
		if(smats.Num()>0) {
			TArray<UMaterialInterface*> mats;
			for(auto m:smats) {
				mats.Add(m.Get());
			}
			smc->OverrideMaterials=mats;
		}
		smc->SetRelativeTransform(transform);
		if(label.Len()>0) {
			TArray<UStaticMeshComponent*> meshes;
			GetComponents<UStaticMeshComponent>(meshes);
			FString nm=FString::Printf(TEXT("%s_%d"),*label,meshes.Num());
			smc->Rename(*nm);
		}
		smc->RecreatePhysicsState();
	}
	return smc;
}

//.............................................................................
// rdRemoveAllStaticMeshes
//.............................................................................
void ArdActor::rdRemoveAllStaticMeshes() {

	TArray<UStaticMeshComponent*> meshes;
    GetComponents<UStaticMeshComponent>(meshes);

	for(auto sm:meshes) {
		RemoveInstanceComponent(sm);
		sm->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		sm->UnregisterComponent();
		sm->Rename(nullptr,nullptr,REN_DontCreateRedirectors|REN_NonTransactional);
		sm->DestroyComponent(false);
	}
}

//.............................................................................
// rdSetActorLabel
//.............................................................................
void ArdActor::rdSetActorLabel(const FName label) {
	rdGetBaseActor();
	rdBaseActor->rdSetActorLabel(this,label);
}

//.............................................................................
// rdConvertISMsToSMs
//.............................................................................
void ArdActor::rdConvertISMsToSMs() {

	if(bStaticMeshMode) return;

	if(!rdGetBaseActor()) return;

	for(auto& it:InstanceDataX) {
		FName sid=it.Key;

		FrdInstanceSettingsArray& i=it.Value;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);

		for(FrdInstanceFastSettings& j:i.settingsFast) {
			if(!j.bNoVis) {
				UStaticMeshComponent* smc=rdAddStaticMeshX(sid,j.transform,j.name);
				smc->ComponentTags.Add(FName(FString::Printf(TEXT("rdName=%s"),*j.name)));
				smc->ComponentTags.Add(FName(FString::Printf(TEXT("rdFolder=%s"),*j.folder)));
			}
		}

		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
			if(!j.bNoVis) {
				UStaticMeshComponent* smc=rdAddStaticMeshX(sid,j.actualTransform,j.name);
				rdCreateTagsFromRandom(smc->ComponentTags,j.randomSettings);
				smc->ComponentTags.Add(FName(FString::Printf(TEXT("rdName=%s"),*j.name)));
				smc->ComponentTags.Add(FName(FString::Printf(TEXT("rdFolder=%s"),*j.folder)));
			}
		}
	}

	rdRemoveInstances();
	InstanceDataX.Empty();

	bStaticMeshMode=true;
}

//.............................................................................
// rdConvertSMsToISMs
//.............................................................................
void ArdActor::rdConvertSMsToISMs() {

	if(!bStaticMeshMode) return;
	if(!rdGetBaseActor()) return;

	TArray<UStaticMeshComponent*> meshes;
    GetComponents<UStaticMeshComponent>(meshes);

	InstanceDataX.Empty();
	InstanceFastArray.Empty();
	InstanceRandomizedArray.Empty();

	if(bCreateFromArrays) {

		for(auto sm:meshes) {
			FrdRandomSettings settings;
			FString name,folder;
			for(auto& t:sm->ComponentTags) {
				FString str=t.ToString();
				if(str.StartsWith("rdName=")) {
					name=str.RightChop(7);
				} else if(str.StartsWith("rdFolder=")) {
					folder=str.RightChop(9);
				}
			}
			if(rdSetRandomFromTags(sm->ComponentTags,&settings)) {
				FrdInstanceRandomSettings isettings;
				isettings.origTransform=isettings.actualTransform=sm->GetRelativeTransform();
				isettings.name=name;
				isettings.folder=folder;
				isettings.randomSettings=settings;
				TArray<FrdInstanceRandomSettings> ary={isettings};
				InstanceRandomizedArray.Add(FrdAddInstanceRandomizedArray(FrdInstanceSetup(ErdSpawnType::UseDefaultSpawn,NAME_None,(UStaticMesh*)sm->GetStaticMesh(),sm->OverrideMaterials,(ErdCollision)(sm->BodyInstance.GetCollisionEnabled()+1),-1,-1,0,FName()),ary));
			} else {
				TArray<FTransform> ary={sm->GetRelativeTransform()};
				InstanceFastArray.Add(FrdAddInstanceFastArray(FrdInstanceSetup(ErdSpawnType::UseDefaultSpawn,NAME_None,(UStaticMesh*)sm->GetStaticMesh(),sm->OverrideMaterials,(ErdCollision)(sm->BodyInstance.GetCollisionEnabled()+1),-1,-1,0,FName()),ary));
			}
		}

	} else {

		for(auto sm:meshes) {

			FString name,folder;
			for(auto& t:sm->ComponentTags) {
				FString str=t.ToString();
				if(str.StartsWith("rdName=")) {
					name=str.RightChop(7);
				} else if(str.StartsWith("rdFolder=")) {
					folder=str.RightChop(9);
				}
			}

			UStaticMesh* mesh=sm->GetStaticMesh();
			if(!mesh) continue;
			FName csid=rdGetSMCsid(sm);

			FrdInstanceSettingsArray& i=InstanceDataX.FindOrAdd(csid);
			FrdRandomSettings settings;
			if(rdSetRandomFromTags(sm->ComponentTags,&settings)) {

				FrdInstanceRandomSettings irs;
				irs.origTransform=sm->GetRelativeTransform();
				irs.name=name;
				irs.folder=folder;
				irs.randomSettings=settings;
				i.settingsRandom.Add(MoveTemp(irs));

			} else {
				FrdInstanceFastSettings ifs;
				ifs.transform=sm->GetRelativeTransform();
				ifs.name=name;
				ifs.folder=folder;
				i.settingsFast.Add(MoveTemp(ifs));
			}
		}
	}

	rdRemoveAllStaticMeshes();
	bStaticMeshMode=false;
	rdBuild();
}

//.............................................................................
// rdUpdateInstanceTransforms
//.............................................................................
void ArdActor::rdUpdateInstanceTransforms() {

	if(!rdGetBaseActor()) return;
	
//	Modify();
	FTransform t;
	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdBaseActor->FindISMCforMesh(sid);
		if(!ismc) continue;

		FrdInstanceSettingsArray& isa=it.Value;
		TArray<FrdInstanceRandomSettings>& settingsRandom=isa.settingsRandom;
		TArray<FrdInstanceFastSettings>& settingsFast=isa.settingsFast;

		// Prefab stored Transform
		for(auto& sr:settingsRandom) {

			if(sr.index<0 || sr.index>=ismc->PerInstanceSMData.Num()) {
				continue;
			}
			FInstancedStaticMeshInstanceData& iData=ismc->PerInstanceSMData[sr.index];
			t.SetFromMatrix(iData.Transform);
			t=rdWorldToInst(t);

			sr.origTransform=t; // The original Tranform of this Instance (The Randomization uses it as the origin)
			sr.actualTransform=t; // The Transform of this Instance with the Randomization applied
		}
	
		for(auto& sf:settingsFast) {

			if(sf.index<0 || sf.index>=ismc->PerInstanceSMData.Num()) {
				continue;
			}
			FInstancedStaticMeshInstanceData& iData=ismc->PerInstanceSMData[sf.index];
			t.SetFromMatrix(iData.Transform);
			t=rdWorldToInst(t);

			sf.transform=t;
		}
	}
}

//.............................................................................
// rdUpdateInstanceTransformsFromList
//.............................................................................
void ArdActor::rdUpdateInstanceTransformsFromList(const FrdInstancesMap& selISMCmap) {

	if(!rdGetBaseActor()) return;
	bool update=false;
	FTransform t;

	for(auto& it:selISMCmap.instances) {

		UInstancedStaticMeshComponent* ismc=it.Key;
		if(!ismc) continue;

		FrdInstanceSettingsArray* isa=InstanceDataX.Find(rdGetSMCsid(ismc));
		if(!isa) continue;

		TArray<FrdInstanceRandomSettings>& settingsRandom=isa->settingsRandom;
		TArray<FrdInstanceFastSettings>& settingsFast=isa->settingsFast;

		for(auto i:it.Value.indexes) {

			// Instance Transform
			FInstancedStaticMeshInstanceData& iData=ismc->PerInstanceSMData[i];
			t.SetFromMatrix(iData.Transform);
			t=rdWorldToInst(t);

			// Prefab stored Transform
			bool found=false;
			for(auto& sr:settingsRandom) {
				if(sr.index==i) {
					found=true;
					sr.origTransform=t; // The original Tranform of this Instance (The Randomization uses it as the origin)
					sr.actualTransform=t; // The Transform of this Instance with the Randomization applied
					update=true;
					break;
				}
			}
	
			if(!found) {
				for(auto& sf:settingsFast) {
					if(sf.index==i) {
						sf.transform=t;
						update=true;
						break;
					}
				}
			}
		}
	}
//	if(update) {
//		Modify();
//	}
}

//.............................................................................
// rdReseatMeshInstances
//.............................................................................
int32 ArdActor::rdReseatMeshInstances(UStaticMesh* mesh,const FVector& shift) {
	return rdReseatMeshInstancesX(rdGetSMsid(mesh),shift);
}
int32 ArdActor::rdReseatMeshInstancesX(const FName sid,const FVector& shift) {

	UE_LOG(LogTemp, Display, TEXT("rdActor::rdReseatMeshInstances(%s,%s)"),sid.IsNone()?*rdGetMeshNameFromSid(sid):TEXT("All Objects"),*shift.ToString());

	int32 cnt=0;

	if(!rdGetBaseActor()) return cnt;

	// Instance data
	for(auto& it:InstanceDataX) {
		//UStaticMesh* sm=it.Key;
		FName isid=it.Key;
		if(sid.IsNone() || isid==sid) {
			UInstancedStaticMeshComponent* imsc=rdBaseActor->FindISMCforMesh(sid);
			if(imsc) {
				FrdInstanceSettingsArray& i=InstanceDataX[sid];
				for(FrdInstanceFastSettings& j:i.settingsFast) {

					FTransform at=j.transform;
					j.transform.SetTranslation(at.GetTranslation()+at.Rotator().RotateVector(shift*at.GetScale3D()));
					cnt++;
				}

				for(FrdInstanceRandomSettings& j:i.settingsRandom) {
					FTransform at=j.actualTransform;
					j.actualTransform.SetTranslation(at.GetTranslation()+at.Rotator().RotateVector(shift*at.GetScale3D()));
					cnt++;
				}
			}
		}
	}

	// Instance Arrays
	if(bCreateFromArrays && (InstanceFastArray.Num()>0 || InstanceRandomizedArray.Num()>0)) {

		for(auto& i:InstanceFastArray) {
			FName isid=rdBaseActor->rdGetInstSid(i.meshSetup);
			if(sid.IsNone() || isid==sid) {
				for(FTransform& tt:i.transforms) {
					tt.SetTranslation(tt.GetTranslation()+tt.Rotator().RotateVector(shift*tt.GetScale3D()));
				}
			}
		}

		for(auto& i:InstanceRandomizedArray) {
			FName isid=rdBaseActor->rdGetInstSid(i.meshSetup);
			if(sid.IsNone() || isid==sid) {
				for(FrdInstanceRandomSettings& tt:i.settings) {
					FTransform at=tt.origTransform;
					tt.origTransform.SetTranslation(at.GetTranslation()+at.Rotator().RotateVector(shift*at.GetScale3D()));
					at=tt.actualTransform;
					tt.actualTransform.SetTranslation(at.GetTranslation()+at.Rotator().RotateVector(shift*at.GetScale3D()));
				}
			}
		}
	}

	// StaticMesh and ChildActor Components
	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(false,comps);
	for(auto c:comps) {

		UStaticMeshComponent* smComp=Cast<UStaticMeshComponent>(c);
		if(smComp) {
			if(sid.IsNone() || rdGetSMCsid(smComp)==sid) {

				FTransform at=smComp->GetComponentTransform();
				smComp->SetWorldLocation(at.GetTranslation()+at.Rotator().RotateVector(shift*at.GetScale3D()));
				cnt++;
			}

		} else {

			UChildActorComponent* caComp=Cast<UChildActorComponent>(c);
			if(caComp) {
				if(sid.IsNone() || caComp->GetChildActorClass()==AStaticMeshActor::StaticClass()) {
					AStaticMeshActor* sma=Cast<AStaticMeshActor>(caComp->GetChildActor());
					FName csid=rdGetSMCsid(sma->GetStaticMeshComponent());
					if(sid.IsNone() || csid==sid) { 
						TArray<FName>& tags=caComp->ComponentTags;
						FVector loc(0,0,0);
						for(auto& tag:tags) {
							FString st=tag.ToString();
							if(st.StartsWith(TEXT("loc="))) {
								getOneVector(st.RightChop(4),loc);
								loc+=shift;
								tags.Remove(tag);
								tags.Add(FName(*FString::Printf(TEXT("loc=%f,%f,%f"),loc.X,loc.Y,loc.Z)));
								break;
							}
						}

						FTransform ct=smComp->GetRelativeTransform();
						caComp->SetRelativeLocation(ct.GetTranslation()+ct.Rotator().RotateVector(shift*ct.GetScale3D()));
						cnt++;
					}
				}
			}
		}
	}
	
	rdBuild();

	return cnt;
}

//.............................................................................
// GetSplineInstanceData
//.............................................................................
TArray<FrdSplineInstanceData>& ArdActor::GetSplineInstanceData() {
	return splineInstanceData;
}

//.............................................................................
// SetSplineInstanceData
//.............................................................................
void ArdActor::SetSplineInstanceData(const TArray<FrdSplineInstanceData>& instData) {
	splineInstanceData=instData;
}

//.............................................................................
// rdSetInstanceEditMode
//.............................................................................
void ArdActor::rdSetInstanceEditMode(bool editMode) {
	bEditInstances=editMode;
}

//.............................................................................
// rdIsEditInstanceMode (filters out rdInst prefabs from rdBPtools prefabs)
//.............................................................................
bool ArdActor::rdIsEditInstanceMode() {
	return false;
}

//.............................................................................
// rdUpdatePrefabBP
//.............................................................................
bool ArdActor::rdUpdatePrefabBP(const AActor* src) {

	const ArdActor* rdActor=Cast<ArdActor>(src);
	if(!rdActor) return false;

	InstanceDataX=rdActor->InstanceDataX;
	InstanceFastArray=rdActor->InstanceFastArray;
	InstanceRandomizedArray=rdActor->InstanceRandomizedArray;

	return true;
}

//.............................................................................
// rdUpdatePrefabWithActorList
//.............................................................................
bool ArdActor::rdUpdatePrefabWithActorList(AActor* src,TArray<FrdInstItemX> meshList,TArray<FrdActorItemX> list,bool updateTransforms) {

	return false;
}

//.............................................................................
// rdSetGeometryCollection
//.............................................................................
void ArdActor::rdSetGeometryCollection(UGeometryCollectionComponent* gcComp,UGeometryCollection* gc) {
	if(!gcComp) {
		return;
	}
	if(gc) {
		gcComp->SetRestCollection(gc);
	}
	gcComp->RecreatePhysicsState();
}

//.............................................................................
