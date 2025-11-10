//
// rdInstBaseActor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 1st October 2022
// Last Modified: 17th May 2025
//
// This is the Instance Base Actor, only one resides in the scene (added automatically)
// and handles managing the Instanced Static meshes. The Components are added to the Root Component of this actor.
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
// Note: You can choose between ISMs and HISMs.
// The recommended method is ISM for Nanite and HISM for non-nanite.
//
// When you remove an instance with the UInstancedStaticMeshComponent all the indexs of the instances after it are
// decreased by 1. This can mess up our dependancies, and it's not very efficient to go through them all and update them.
// With the UHierarchicalInstancedStaticMeshComponent (5.3 brings it to ISM), when you remove an Instance, rather than decreasing
// the indexes of Instances after it, it swaps out the index we are removing with the last index in the Instance List - then just
// deletes that last one.
// This means there is only one index to update, and as we reverse-enumerate those Arrays, it's painless to do.
// 
// The other way (which can be set with rdSetUpInst() is to recycle them, move them down below the level to hide)
//
// Handling ISMs isn't too bad - rather than remove the instance, you shift it out of the world and mark it as ready for re-use.
//
#include "rdInstBaseActor.h"
#include "rdInst.h"
#if ENGINE_MAJOR_VERSION>4
#include "LevelInstance/LevelInstanceLevelStreaming.h"
//#include "Streaming/LevelStreamingDelegates.h"
#endif
#include "Engine/Level.h"
#include "Engine/LevelStreaming.h"
#include "rdActor.h"

bool ArdInstBaseActor::bIsPlaying=false;

//.............................................................................
//  ArdInstBaseActor Constructor
//.............................................................................
ArdInstBaseActor::ArdInstBaseActor() : poolListener(nullptr) {
	bAllowTickBeforeBeginPlay=true;
	PrimaryActorTick.bCanEverTick=true;
	SetTickableWhenPaused(true);
	SetActorTickInterval(0.0f);
	SetActorTickEnabled(true);

	//if(GetRootComponent()) {
	//	GetRootComponent()->bNavigationRelevant=true;
	//	GetRootComponent()->SetCanEverAffectNavigation(true);
	//}

	TArray<UActorComponent*> list;
	GetComponents(UInstancedStaticMeshComponent::StaticClass(),list);
	for(auto comp:list) {
		UInstancedStaticMeshComponent* ismc=(UInstancedStaticMeshComponent*)comp;
		ismc->ClearInstances();
		ismc->DestroyComponent();
	}

//#if WITH_EDITOR
//	if(!rdNonDeterminantInstance) {
//		rdNonDeterminantInstance=NewObject<UrdMultiToken>(this,FName("Nondeterminant"),RF_Transient);//|RF_MarkAsRootSet);//RF_ClassDefaultObject|
//	}
//#endif
	//FLevelStreamingDelegates::OnLevelStreamingStateChanged.AddUObject(this,&ArdInstBaseActor::OnLevelStreamingStateChanged);
}

//.............................................................................
//  ArdInstBaseActor Destructor
//.............................................................................
ArdInstBaseActor::~ArdInstBaseActor() {
#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("ArdInstBaseActor::~ArdInstBaseActor()"));
#endif
	//FLevelStreamingDelegates::OnLevelStreamingStateChanged.RemoveAll(this);
}

//.............................................................................
//  OnConstruction (Blueprint construction)
//.............................................................................
void ArdInstBaseActor::OnConstruction(const FTransform &Transform) {

	rdRecreateInstances();
}

//.............................................................................
//  BeginPlay
//.............................................................................
void ArdInstBaseActor::BeginPlay() {

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("ArdInstBaseActor::BeginPlay()"));
#endif

#if !UE_BUILD_SHIPPING
	FrdInstModule::RemoveDebugHUD();
#endif

	// Niagara mesh spawner
	niagaraSpawner=TSoftObjectPtr<UNiagaraSystem>(FSoftObjectPath(TEXT("/Game/rdTools/System/Niagara/NS_rdMeshArray.NS_rdMeshArray")));

	// Pickup Highlight material for the Inverse Hull highlighting
	TSoftObjectPtr<UMaterialInterface> smat=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/rdInst/Materials/M_rdHighlight.M_rdHighlight")));
	pickupHighlightMat=smat.LoadSynchronous();

	// Pickup Highlight material for the StencilBuffer highlighting
	TSoftObjectPtr<UMaterialInterface> smat2=TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/rdInst/Materials/M_rdHighlightStencil.M_rdHighlightStencil")));
	pickupHighlightStencilMat=smat2.LoadSynchronous();

	bIsPlaying=true;

	rdRemAllInstances();
	pickupIDtoSidMap.Empty();

#if !UE_SERVER
	if(ConvertToISMAtPlay) {
		rdConvertAutoISMs();
	}
#endif

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	rdBuildMegaLightList();
#endif

	maxLongDistance=0.0f;

	Super::BeginPlay();
}

//.............................................................................
//  EndPlay
//.............................................................................
void ArdInstBaseActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {

#if !UE_BUILD_SHIPPING
	FrdInstModule::RemoveDebugHUD();
#endif

	bIsPlaying=false;
#if !UE_SERVER
	if(ConvertToISMAtPlay) rdRevertFromAutoISMs();
#endif
	Super::EndPlay(EndPlayReason);
}

//.............................................................................
//  BeginDestroy
//.............................................................................
void ArdInstBaseActor::BeginDestroy() {

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("ArdInstBaseActor::BeginDestroy()"));
#endif

	Super::BeginDestroy();
}

//----------------------------------------------------------------------------------------------------------------
// OnLevelStreamingStateChanged
//----------------------------------------------------------------------------------------------------------------
/*
void ArdInstBaseActor::OnLevelStreamingStateChanged(UWorld* world,const ULevelStreaming* stream,ULevel* level,ELevelStreamingState prevState,ELevelStreamingState state) {

	if(state!=prevState && state!=ELevelStreamingState::LoadedVisible) return;
	 
	const ULevelStreamingLevelInstance* levelInstance=(ULevelStreamingLevelInstance*)Cast<ULevelStreamingLevelInstance>(stream);
	if(!levelInstance) return;

	ELevelStreamingState newSate=state;
}
*/
#if WITH_EDITOR
//.............................................................................
// PostEditUndo
//.............................................................................
void ArdInstBaseActor::PostEditUndo() {
	Super::PostEditUndo();
	rdRecreateInstances();
}

void ArdInstBaseActor::PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) {
	Super::PostEditUndo(TransactionAnnotation);
	rdRecreateInstances();
}

//.............................................................................
// Modify
//.............................................................................
bool ArdInstBaseActor::Modify(bool bAlwaysMarkDirty) {

	if(dontDoModify>0) { // Set this to true before running ISMC functions like UpdateTransform from the Editor
		return true;
	}
	return Super::Modify(bAlwaysMarkDirty);
}
#endif

//.............................................................................
// Tick
//.............................................................................
void ArdInstBaseActor::Tick(float DeltaTime) {

#if !WITH_EDITOR
	// Dirty List
	TArray<ArdActor*> removeList;

	TArray<ArdActor*> tempList=ArdActor::rdGetDirtyList();
	for(auto i:tempList) {
		if(IsValid(i)) {
			if(i->dirtyCountDown<=0) {
				i->rdBuild();
				removeList.Add(i);
			} else {
				i->dirtyCountDown--;
			}
		}
	}
	for(auto i:removeList) ArdActor::rdGetDirtyList().Remove(i);
#endif

	ProcessEntities();
}

//.............................................................................
// rdSetUpInst
//.............................................................................
void ArdInstBaseActor::rdSetUpInst(bool useHISM,bool useISMforNanite,bool recycleInstances,bool autoInst,bool autoFromTags,bool autoMeshActors,bool autoBPs) {

	bUseHISMs=useHISM;
	bRecycleInstances=recycleInstances;
	ConvertToISMAtPlay=autoInst;
	ConvertToISM_FromTags=autoFromTags;
	ConvertToISM_IncMeshActors=autoMeshActors;
	ConvertToISM_IncBPs=autoBPs;
}

//.............................................................................
// rdGetrdInstVersion
//.............................................................................
float ArdInstBaseActor::rdGetrdInstVersion() { 
	return (float)RDINST_MAJOR_VERSION+(float)RDINST_MINOR_VERSION/100.0f; 
}

//.............................................................................
