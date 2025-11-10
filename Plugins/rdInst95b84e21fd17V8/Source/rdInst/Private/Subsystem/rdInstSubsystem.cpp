//
// rdInstSubsystem.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 24th September 2023
// Last Modified: 6th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstSubsystem.h"
#include "rdInst.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

//.............................................................................
// rdSetUpInst
//.............................................................................
void UrdInstSubsystem::rdSetUpInst(bool useHISM,bool useISMforNanite,bool recycleInstances,bool autoInst,bool autoFromTags,bool autoMeshActors,bool autoBPs) {
	if(rdGetBaseActor()) {
		rdBaseActor->rdSetUpInst(useHISM,useISMforNanite,recycleInstances,autoInst,autoFromTags,autoMeshActors,autoBPs); 
	}
}

//.............................................................................
// rdGetBaseActor
//.............................................................................
inline bool UrdInstSubsystem::rdGetBaseActor() {

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world?world->World():GetWorld();
	if(!World) return false;
	if(World!=lastWorld) {
		rdBaseActor=nullptr;
	}

	if(rdBaseActor && IsValid(rdBaseActor)) return true;

	TArray<AActor*> existingBaseActors;
	UGameplayStatics::GetAllActorsOfClass(World,ArdInstBaseActor::StaticClass(),existingBaseActors);
	if(existingBaseActors.Num()>0) {
		rdBaseActor=Cast<ArdInstBaseActor>(existingBaseActors[0]);
		if(rdBaseActor && IsValid(rdBaseActor)) {
			lastWorld=World;
			return true;
		}
	}

	// haven't found one, lets spawn it
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags=RF_Transactional;//RF_Public
	SpawnParams.bAllowDuringConstructionScript=true;
	SpawnParams.Owner=nullptr;

	rdBaseActor=(ArdInstBaseActor*)World->SpawnActor<ArdInstBaseActor>(SpawnParams);
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
	rdBaseActor->Tags.Add(TEXT("rdBaseActor"));
	lastWorld=World;
	return true;
}

//.............................................................................
// rdGetBase
//.............................................................................
ArdInstBaseActor* UrdInstSubsystem::rdGetBase() {
	rdGetBaseActor();
	return rdBaseActor;
}

//.............................................................................
// rdResetBase
//.............................................................................
void UrdInstSubsystem::rdResetBase() {
	rdBaseActor=nullptr;
}

//.............................................................................
// Clone methods of rdInstBaseActor
//.............................................................................
UInstancedStaticMeshComponent* UrdInstSubsystem::rdGetInstanceGen(const UStaticMesh* mesh,bool create) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetInstanceGen(mesh,create); }
UInstancedStaticMeshComponent* UrdInstSubsystem::rdGetInstanceGenX(const FName sid,bool create) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetInstanceGenX(sid,create); }
UHierarchicalInstancedStaticMeshComponent* UrdInstSubsystem::rdGetHInstanceGen(const UStaticMesh* mesh,bool create) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetHInstanceGen(mesh,create); }
UHierarchicalInstancedStaticMeshComponent* UrdInstSubsystem::rdGetHInstanceGenX(const FName sid,bool create) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetHInstanceGenX(sid,create); }

FName UrdInstSubsystem::rdGetSMsid(const TSoftObjectPtr<UStaticMesh> m,ErdSpawnType type,const FName groupName) { if(!rdGetBaseActor()) return TEXT(""); return rdBaseActor->rdGetSMsid(m,type,groupName); }
FName UrdInstSubsystem::rdGetSMXsid(ErdSpawnType type,const TSoftObjectPtr<UStaticMesh> m,TArray<TSoftObjectPtr<UMaterialInterface>> mats,bool bReverseCulling,ErdCollision collision,float startCull,float endCull,int32 id,const FName groupName) { if(!rdGetBaseActor()) return TEXT(""); return rdBaseActor->rdGetSMXsid(type,m,mats,bReverseCulling,collision,startCull,endCull,id,groupName); }
FName UrdInstSubsystem::rdGetSMCsid(const UStaticMeshComponent* smc,ErdSpawnType type,float sc,float ec,int32 id,const FName groupName) { if(!rdGetBaseActor()) return TEXT(""); return rdBaseActor->rdGetSMCsid(smc,type,sc,ec,id,groupName); }
UStaticMesh* UrdInstSubsystem::rdGetMeshFromSid(const FName sid) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetMeshFromSid(sid); }
TSoftObjectPtr<UStaticMesh> UrdInstSubsystem::rdGetSoftMeshFromSid(const FName sid) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetSoftMeshFromSid(sid); }
TArray<TSoftObjectPtr<UMaterialInterface>> UrdInstSubsystem::rdGetMaterialsFromSid(const FName sid) { static TArray<TSoftObjectPtr<UMaterialInterface>> mil; if(!rdGetBaseActor()) return mil; return rdBaseActor->rdGetMaterialsFromSid(sid); }
FString UrdInstSubsystem::rdGetMeshNameFromSid(const FName sid) { if(!rdGetBaseActor()) return TEXT(""); return rdBaseActor->rdGetMeshNameFromSid(sid); }
ErdSpawnType UrdInstSubsystem::rdGetSpawnTypeFromSid(const FName sid) { if(!rdGetBaseActor()) return ErdSpawnType::UseDefaultSpawn; return rdBaseActor->rdGetSpawnTypeFromSid(sid); }
void UrdInstSubsystem::rdGetSidDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,TSoftObjectPtr<UStaticMesh>& mesh,TArray<TSoftObjectPtr<UMaterialInterface>>& materials,bool& bReverseCulling,TEnumAsByte<ErdCollision>& collision,float& startCull,float& endCull,int32& id,FName& groupName) { if(!rdGetBaseActor()) return; rdBaseActor->rdGetSidDetails(sid,ver,type,mesh,materials,bReverseCulling,collision,startCull,endCull,id,groupName); }

// Add Instances ----------------------------------------------------------------------------------

int32 UrdInstSubsystem::rdAddInstance(const AActor* instOwner,UStaticMesh* mesh,const FTransform& transform) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddInstance(instOwner,mesh,transform); }
int32 UrdInstSubsystem::rdAddInstanceX(const AActor* instOwner,const FName sid,const FTransform& transform) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddInstanceX(instOwner,sid,transform); }
void UrdInstSubsystem::rdAddInstancesX(const FName sid,const TArray<FTransform>& transforms) { if(rdGetBaseActor()) rdBaseActor->rdAddInstancesX(sid,transforms); }
int32 UrdInstSubsystem::rdAddInstanceFast(UInstancedStaticMeshComponent* instGen,const FTransform& transform) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddInstanceFast(instGen,transform); }
void UrdInstSubsystem::rdAddInstancesFast(UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms) { if(rdGetBaseActor()) rdBaseActor->rdAddInstancesFast(instGen,transforms); }
void  UrdInstSubsystem::rdAddInstancesFastWithIndexes(UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms,TArray<int32>& indexList) { if(!rdGetBaseActor()) return; rdBaseActor->rdAddInstancesFastWithIndexes(instGen,transforms,indexList); }
void  UrdInstSubsystem::rdAddInstanceArrayFastWithIndexes(UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bso) { if(rdGetBaseActor()) rdBaseActor->rdAddInstanceArrayFastWithIndexes(instGen,bso); }
void  UrdInstSubsystem::rdAddInstanceArrayFastWithIndexesRel(const FTransform& t,UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bso) { if(rdGetBaseActor()) rdBaseActor->rdAddInstanceArrayFastWithIndexesRel(t,instGen,bso); }
int32 UrdInstSubsystem::rdAddInstanceRaw(UInstancedStaticMeshComponent* instGen,const FTransform& transform) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddInstanceRaw(instGen,transform); }
int32 UrdInstSubsystem::rdAddInstancesRaw(UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddInstancesRaw(instGen,transforms); }
int32 UrdInstSubsystem::rdAddInstancesRawWithIndexes(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<FTransform>& transforms,TArray<int32>& indexList) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddInstancesRawWithIndexes(instGen,transforms,indexList); }
int32 UrdInstSubsystem::rdAddOwnedInstance(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,const FTransform& transform) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddOwnedInstance(instOwner,instGen,transform); }
int32 UrdInstSubsystem::rdAddOwnedInstances(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdAddOwnedInstances(instOwner,instGen,transforms); }

// Remove Instances -------------------------------------------------------------------------------

void UrdInstSubsystem::rdRemoveInstanceFast(UInstancedStaticMeshComponent* instGen,int32 index) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveInstanceFast(instGen,index); }
void UrdInstSubsystem::rdRemoveInstancesFast(UInstancedStaticMeshComponent* instGen,const TArray<int32>& indexes) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveInstancesFast(instGen,indexes); }
void UrdInstSubsystem::rdRemoveInstanceRaw(UInstancedStaticMeshComponent* instGen,int32 index) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveInstanceRaw(instGen,index); }
void UrdInstSubsystem::rdRemoveInstancesX(const FName sid,UPARAM(ref) const TArray<int32>& indexes) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveInstancesX(sid,indexes); }
void UrdInstSubsystem::rdRemoveInstancesRaw(UInstancedStaticMeshComponent* instGen,const TArray<int32>& indexes) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveInstancesRaw(instGen,indexes); }
void UrdInstSubsystem::rdRemoveOwnedInstance(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,int32 index) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveOwnedInstance(instOwner,instGen,index); }
void UrdInstSubsystem::rdRemoveInstance(UStaticMesh* mesh,int32 index) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveInstance(mesh,index); }
void UrdInstSubsystem::rdRemoveInstanceX(const FName sid,int32 index) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveInstanceX(sid,index); }
void UrdInstSubsystem::rdRemInstancesForOwner(const AActor* instOwner) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemInstancesForOwner(instOwner); }
void UrdInstSubsystem::rdRemAllInstances() { if(!rdGetBaseActor()) return; rdBaseActor->rdRemAllInstances(); }
void UrdInstSubsystem::rdRemAllHISMCs() { if(!rdGetBaseActor()) return; rdBaseActor->rdRemAllHISMCs(); }

// Transform Instances ----------------------------------------------------------------------------

void UrdInstSubsystem::rdChangeInstanceTransformsForOwner(const AActor* instOwner,const FTransform& transform,bool worldSpace,bool markDirty,bool teleport) { if(!rdGetBaseActor()) return; rdBaseActor->rdChangeInstanceTransformsForOwner(instOwner,transform,worldSpace,markDirty,teleport); }
AActor* UrdInstSubsystem::rdGetActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetActorFromInstanceIndex(comp,index); }
ArdActor* UrdInstSubsystem::rdGetrdActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetrdActorFromInstanceIndex(comp,index); }
void UrdInstSubsystem::rdUpdateTransformFast(UInstancedStaticMeshComponent* ismc,int32 index,const FTransform& transform) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateTransformFast(ismc,index,transform); }
void UrdInstSubsystem::rdUpdateTransform(UStaticMesh* mesh,int32 index,const FTransform& transform) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateTransform(mesh,index,transform); }
void UrdInstSubsystem::rdUpdateTransformX(const FName sid,int32 index,const FTransform& transform) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateTransformX(sid,index,transform); }
void UrdInstSubsystem::rdUpdateTransformsFast(UInstancedStaticMeshComponent* ismc,int32 startIndex,const TArray<FTransform>& transforms,int32 arrayoffset) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateTransformsFast(ismc,startIndex,transforms,arrayoffset); }
void UrdInstSubsystem::rdUpdateTransforms(UStaticMesh* mesh,int32 startIndex,const TArray<FTransform>& transforms) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateTransforms(mesh,startIndex,transforms); }
void UrdInstSubsystem::rdUpdateTransformsX(const FName sid,int32 startIndex,const TArray<FTransform>& transforms) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateTransformsX(sid,startIndex,transforms); }
void UrdInstSubsystem::rdUpdateTransformArrayX(const FName sid,const TArray<int32> indices,const TArray<FTransform>& transforms) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateTransformArrayX(sid,indices,transforms); }
void UrdInstSubsystem::rdIncrementTransforms(UStaticMesh* mesh,const TArray<int32>& indexes,const FTransform& transform) { if(!rdGetBaseActor()) return; rdBaseActor->rdIncrementTransforms(mesh,indexes,transform); }
void UrdInstSubsystem::rdIncrementTransformsX(const FName sid,const TArray<int32>& indexes,const FTransform& transform) { if(!rdGetBaseActor()) return; rdBaseActor->rdIncrementTransformsX(sid,indexes,transform); }
void UrdInstSubsystem::rdIncrementTransformsFast(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indexes,const FTransform& transform) { if(!rdGetBaseActor()) return; rdBaseActor->rdIncrementTransformsFast(ismc,indexes,transform); }
void UrdInstSubsystem::rdIncrementAllTransformsFast(UInstancedStaticMeshComponent* ismc,const FTransform& transform) { if(!rdGetBaseActor()) return; rdBaseActor->rdIncrementAllTransformsFast(ismc,transform); }
TArray<FMatrix>& UrdInstSubsystem::rdGetTransformsPtr(UStaticMesh* mesh,int32& numTransforms) { static TArray<FMatrix> empty; if(!rdGetBaseActor()) return empty; return rdBaseActor->rdGetTransformsPtr(mesh,numTransforms); }
TArray<FMatrix>& UrdInstSubsystem::rdGetTransformsPtrX(const FName sid,int32& numTransforms) { static TArray<FMatrix> empty; if(!rdGetBaseActor()) return empty; return rdBaseActor->rdGetTransformsPtrX(sid,numTransforms); }
TArray<FMatrix>& UrdInstSubsystem::rdGetTransformsPtrFast(UInstancedStaticMeshComponent* ismc,int32& numTransforms) { static TArray<FMatrix> empty; if(!rdGetBaseActor()) return empty; return rdBaseActor->rdGetTransformsPtrFast(ismc,numTransforms); }

// Utilities --------------------------------------------------------------------------------------

int32 UrdInstSubsystem::rdGetLastInstanceIndex(const AActor* instOwner,UStaticMesh* mesh) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetLastInstanceIndex(instOwner,mesh); }
int32 UrdInstSubsystem::rdGetLastInstanceIndexX(const AActor* instOwner,const FName sid) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetLastInstanceIndexX(instOwner,sid); }
bool UrdInstSubsystem::rdGetInstanceTransform(const UStaticMesh* mesh,int32 index,FTransform& transform) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetInstanceTransform(mesh,index,transform); }
bool UrdInstSubsystem::rdGetInstanceTransformX(const FName sid,int32 index,FTransform& transform) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetInstanceTransformX(sid,index,transform); }
bool UrdInstSubsystem::rdGetInstanceLocation(UStaticMesh* sm,int32 i,FVector& loc) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetInstanceLocation(sm,i,loc); }
bool UrdInstSubsystem::rdGetInstanceLocationX(const FName sid,int32 i,FVector& loc) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetInstanceLocationX(sid,i,loc); }
bool UrdInstSubsystem::rdGetInstanceTransformFast(UInstancedStaticMeshComponent* ismc,int32 index,FTransform& transform) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetInstanceTransformFast(ismc,index,transform); }
bool UrdInstSubsystem::rdGetInstanceLocationFast(UInstancedStaticMeshComponent* ismc,int32 i,FVector& loc) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetInstanceLocationFast(ismc,i,loc); }
void UrdInstSubsystem::rdCalcInstanceCounts() { if(!rdGetBaseActor()) return; rdBaseActor->rdCalcInstanceCounts(); }
void UrdInstSubsystem::rdRecreateInstances() { if(!rdGetBaseActor()) return; rdBaseActor->rdRecreateInstances(); }
void UrdInstSubsystem::SetHISMCdata(UStaticMesh* mesh,UInstancedStaticMeshComponent* hismc) { if(!rdGetBaseActor()) return; rdBaseActor->SetHISMCdata(mesh,hismc); }
void UrdInstSubsystem::SetHISMCdataX(const FName sid,UInstancedStaticMeshComponent* hismc) { if(!rdGetBaseActor()) return; rdBaseActor->SetHISMCdataX(sid,hismc); }
UInstancedStaticMeshComponent* UrdInstSubsystem::FindISMCforMesh(const FName& sid) { if(!rdGetBaseActor()) return 0; return rdBaseActor->FindISMCforMesh(sid); }
UHierarchicalInstancedStaticMeshComponent* UrdInstSubsystem::FindHISMCforMesh(const FName& sid) { if(!rdGetBaseActor()) return 0; return rdBaseActor->FindHISMCforMesh(sid); }
void UrdInstSubsystem::rdSetActorLabel(AActor* actor,const FName label) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetActorLabel(actor,label); }
TArray<UInstancedStaticMeshComponent*> UrdInstSubsystem::GetUsedISMCs() { TArray<UInstancedStaticMeshComponent*> ary; if(!rdGetBaseActor()) return ary; return rdBaseActor->GetUsedISMCs(); }

int32 UrdInstSubsystem::rdGetTotalNumberOfInstancesInRecycleCache() { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetTotalNumberOfInstancesInRecycleCache(); }
int32 UrdInstSubsystem::rdGetNumberOfInstancesInRecycleCache(UStaticMesh* mesh) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetNumberOfInstancesInRecycleCache(mesh); }
int32 UrdInstSubsystem::rdGetNumberOfInstancesInRecycleCacheX(const FName sid) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetNumberOfInstancesInRecycleCacheX(sid); }

ArdSpawnActor* UrdInstSubsystem::rdCreateSpawnActorFromVolume(AActor* volume,double distance,int32 distFrames,bool spatial,TEnumAsByte<rdSpawnMode> spawnMode) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdCreateSpawnActorFromVolume(volume,distance,distFrames,spatial,spawnMode); }
ArdSpawnActor* UrdInstSubsystem::rdCreateSpawnActorFromObjectList(UrdSetObjectsList* instances,FTransform& transform,const FString& filename,double distance,int32 distFrames,bool spatial,TEnumAsByte<rdSpawnMode> spawnMode,bool harvestInstances,int32 gridX,int32 gridY) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdCreateSpawnActorFromObjectList(instances,transform,filename,distance,distFrames,spatial,spawnMode,harvestInstances,gridX,gridY); }

bool UrdInstSubsystem::rdSetInstanceVisibility(UStaticMesh* mesh,int32 index,bool vis) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdSetInstanceVisibility(mesh,index,vis); }
bool UrdInstSubsystem::rdSetInstanceVisibilityX(const FName sid,int32 index,bool vis) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdSetInstanceVisibilityX(sid,index,vis); }
bool UrdInstSubsystem::rdSetInstanceVisibilityFast(UInstancedStaticMeshComponent* ismc,int32 index,bool vis) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdSetInstanceVisibilityFast(ismc,index,vis); }
void UrdInstSubsystem::rdSetInstancesVisibility(UStaticMesh* mesh,const TArray<int32>& indexes,bool vis) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetInstancesVisibility(mesh,indexes,vis); }
void UrdInstSubsystem::rdSetInstancesVisibilityX(const FName sid,const TArray<int32>& indexes,bool vis) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetInstancesVisibilityX(sid,indexes,vis); }
void UrdInstSubsystem::rdSetInstancesVisibilityFast(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indexes,bool vis) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetInstancesVisibilityFast(ismc,indexes,vis); }

// Custom Data ------------------------------------------------------------------------------------

void UrdInstSubsystem::rdSetNumInstCustomData(UStaticMesh* mesh,int32 numData) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetNumInstCustomData(mesh,numData); }
void UrdInstSubsystem::rdSetNumInstCustomDataX(const FName sid,int32 numData) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetNumInstCustomDataX(sid,numData); }
void UrdInstSubsystem::rdSetNumInstCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 numData) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetNumInstCustomDataFast(ismc,numData); }
TArray<float>& UrdInstSubsystem::rdGetCustomDataPtr(UStaticMesh* mesh,int32& numData) { static TArray<float> empty; if(!rdGetBaseActor()) return empty; return rdBaseActor->rdGetCustomDataPtr(mesh,numData); }
TArray<float>& UrdInstSubsystem::rdGetCustomDataPtrX(const FName sid,int32& numData) { static TArray<float> empty; if(!rdGetBaseActor()) return empty; return rdBaseActor->rdGetCustomDataPtrX(sid,numData); }
TArray<float>& UrdInstSubsystem::rdGetCustomDataPtrFast(UInstancedStaticMeshComponent* ismc,int32& numData) { static TArray<float> empty; if(!rdGetBaseActor()) return empty; return rdBaseActor->rdGetCustomDataPtrFast(ismc,numData); }
FLinearColor UrdInstSubsystem::rdGetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex) { static FLinearColor col; if(!rdGetBaseActor()) return col; return rdBaseActor->rdGetCustomCol3Data(mesh,instanceIndex,dataIndex); }
FLinearColor UrdInstSubsystem::rdGetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex) { static FLinearColor col; if(!rdGetBaseActor()) return col; return rdBaseActor->rdGetCustomCol3DataX(sid,instanceIndex,dataIndex);  }
FLinearColor UrdInstSubsystem::rdGetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex) { static FLinearColor col; if(!rdGetBaseActor()) return col; return rdBaseActor->rdGetCustomCol3DataFast(ismc,instanceIndex,dataIndex);  }
FLinearColor UrdInstSubsystem::rdGetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex) { static FLinearColor col; if(!rdGetBaseActor()) return col; return rdBaseActor->rdGetCustomCol4Data(mesh,instanceIndex,dataIndex);  }
FLinearColor UrdInstSubsystem::rdGetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex) { static FLinearColor col; if(!rdGetBaseActor()) return col; return rdBaseActor->rdGetCustomCol4DataX(sid,instanceIndex,dataIndex);  }
FLinearColor UrdInstSubsystem::rdGetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex) { static FLinearColor col; if(!rdGetBaseActor()) return col; return rdBaseActor->rdGetCustomCol4DataFast(ismc,instanceIndex,dataIndex);  }
void UrdInstSubsystem::rdSetCustomData(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,float value,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomData(mesh,instanceIndex,dataIndex,value,batch); }
void UrdInstSubsystem::rdSetCustomDataX(const FName sid,int32 instanceIndex,int32 dataIndex,float value,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomDataX(sid,instanceIndex,dataIndex,value,batch); }
void UrdInstSubsystem::rdSetCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,float value,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomDataFast(ismc,instanceIndex,dataIndex,value,batch); }
void UrdInstSubsystem::rdSetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomCol3Data(mesh,instanceIndex,dataIndex,col,batch); }
void UrdInstSubsystem::rdSetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomCol3DataX(sid,instanceIndex,dataIndex,col,batch); }
void UrdInstSubsystem::rdSetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomCol3DataFast(ismc,instanceIndex,dataIndex,col,batch); }
void UrdInstSubsystem::rdSetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomCol4Data(mesh,instanceIndex,dataIndex,col,batch); }
void UrdInstSubsystem::rdSetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomCol4DataX(sid,instanceIndex,dataIndex,col,batch); }
void UrdInstSubsystem::rdSetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetCustomCol4DataFast(ismc,instanceIndex,dataIndex,col,batch); }
void UrdInstSubsystem::rdSetAllCustomData(UStaticMesh* mesh,UPARAM(ref) TArray<float>& data,bool update) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetAllCustomData(mesh,data,update); }
void UrdInstSubsystem::rdSetAllCustomDataX(const FName sid,UPARAM(ref) TArray<float>& data,bool update) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetAllCustomDataX(sid,data,update); }
void UrdInstSubsystem::rdSetAllCustomDataFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) TArray<float>& data,bool update) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetAllCustomDataFast(ismc,data,update); }
void UrdInstSubsystem::rdUpdateCustomData(UStaticMesh* mesh) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateCustomData(mesh); }
void UrdInstSubsystem::rdUpdateCustomDataX(const FName sid) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateCustomDataX(sid); }
void UrdInstSubsystem::rdUpdateCustomDataFast(UInstancedStaticMeshComponent* ismc) { if(!rdGetBaseActor()) return; rdBaseActor->rdUpdateCustomDataFast(ismc); }
void UrdInstSubsystem::rdFillCustomData(UStaticMesh* mesh,int32 offset,int32 stride,float baseValue,float randomVariance,int32 granularity,float inc,float incRandomVariance) { if(!rdGetBaseActor()) return; rdBaseActor->rdFillCustomData(mesh,offset,stride,baseValue,randomVariance,granularity,inc,incRandomVariance); }
void UrdInstSubsystem::rdFillCustomDataX(const FName sid,int32 offset,int32 stride,float baseValue,float randomVariance,int32 granularity,float inc,float incRandomVariance) { if(!rdGetBaseActor()) return; rdBaseActor->rdFillCustomDataX(sid,offset,stride,baseValue,randomVariance,granularity,inc,incRandomVariance); }
void UrdInstSubsystem::rdFillCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 offset,int32 stride,float baseValue,float randomVariance,int32 granularity,float inc,float incRandomVariance) { if(!rdGetBaseActor()) return; rdBaseActor->rdFillCustomDataFast(ismc,offset,stride,baseValue,randomVariance,granularity,inc,incRandomVariance); }
int32 UrdInstSubsystem::rdFillCustomDataInArea(const TArray<UInstancedStaticMeshComponent*> ismcs,const FVector& loc,float radius,const FVector& box,int32 index,float value) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdFillCustomDataInArea(ismcs,loc,radius,box,index,value); }
int32 UrdInstSubsystem::rdFillCustomDataInAreaMulti(const TArray<UInstancedStaticMeshComponent*> ismcs,const FVector& loc,float radius,const FVector& box,int32 index,TArray<float> values)  { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdFillCustomDataInAreaMulti(ismcs,loc,radius,box,index,values); }

// Spawning ---------------------------------------------------------------------------------------

AActor* UrdInstSubsystem::UrdInstSubsystem::rdSpawnActor(TSubclassOf<class AActor> actorClass,const FTransform& transform,AActor* actTemplate,bool temp,bool attach,const FName label,AActor* parent,bool deferConstruction,const FString& strProps) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdSpawnActor(actorClass,transform,actTemplate,temp,attach,label,parent,deferConstruction,strProps); }
UNiagaraComponent* UrdInstSubsystem::rdSpawnVFX(UNiagaraSystem* fx,const FTransform& transform,AActor* parent) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdSpawnVFX(fx,transform,parent); }
void UrdInstSubsystem::rdRemoveVFX(UNiagaraComponent* fxc) { if(rdGetBaseActor()) rdBaseActor->rdRemoveVFX(fxc); }

// Distributed Spawning and Instancing ------------------------------------------------------------

void UrdInstSubsystem::rdAddDistributedInstances(UStaticMesh* mesh,const TArray<FTransform>& transforms,int32 transactionsPerTick,TArray<int32>& fillList,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddDistributedInstances(mesh,transforms,transactionsPerTick,fillList,proxy); }
void UrdInstSubsystem::rdAddDistributedInstancesX(const FName sid,const TArray<FTransform>& transforms,int32 transactionsPerTick,TArray<int32>& fillList,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddDistributedInstancesX(sid,transforms,transactionsPerTick,fillList,proxy); }

// Actor Pooling ----------------------------------------------------------------------------------

void UrdInstSubsystem::rdPoolActor(UClass* actorClass,int32 numToPool,bool premake,bool doTick,bool startHidden,bool reuse,bool simplePool,int32 growBy) { if(rdGetBaseActor()) rdBaseActor->rdPoolActor(actorClass,numToPool,premake,doTick,startHidden,reuse,simplePool,growBy); }
bool UrdInstSubsystem::rdSetActorPoolSize(UClass* actorClass,int32 numToPool,bool premake,bool doTick,bool startHidden,bool reuse,bool simplePool,int32 growBy) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdSetActorPoolSize(actorClass,numToPool,premake,doTick,startHidden,reuse,simplePool,growBy); }
void UrdInstSubsystem::rdRemoveActorPool(UClass* actorClass) { if(rdGetBaseActor()) rdBaseActor->rdRemoveActorPool(actorClass); }
AActor* UrdInstSubsystem::rdGetActorFromPool(TSubclassOf<class AActor> actorClass,const FTransform& tran) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetActorFromPool(actorClass,tran); }
void UrdInstSubsystem::rdRemoveActorFromPool(AActor* actor) { if(rdGetBaseActor()) rdBaseActor->rdRemoveActorFromPool(actor); }
void UrdInstSubsystem::rdReturnActorToPool(AActor* actor) { if(rdGetBaseActor()) rdBaseActor->rdReturnActorToPool(actor); }
void UrdInstSubsystem::rdSetPoolListener(ArdActor* actor) { if(rdGetBaseActor()) rdBaseActor->rdSetPoolListener(actor); }
bool UrdInstSubsystem::rdIsActorInPool(ArdActor* actor) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdIsActorInPool(actor); }

// Proxies ----------------------------------------------------------------------------------------

void UrdInstSubsystem::rdSetupProxies(int32 processInterval,double swapDistance,double longDistanceSwapDistance,int32 idleTicks,int32 idleDestructionTicks,float velocityThreshold,bool scanOnlyBaked) { if(rdGetBaseActor()) rdBaseActor->rdSetupProxies(processInterval,swapDistance,longDistanceSwapDistance,idleTicks,idleDestructionTicks,velocityThreshold,scanOnlyBaked); }
void UrdInstSubsystem::rdAddInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddInstanceProxy(mesh,index,proxy); }
void UrdInstSubsystem::rdAddInstanceProxyX(const FName sid,int32 index,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddInstanceProxyX(sid,index,proxy); }
void UrdInstSubsystem::rdAddActorProxy(AActor* actor,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddActorProxy(actor,proxy); }
void UrdInstSubsystem::rdAddPrefabProxy(ArdActor* prefab,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddPrefabProxy(prefab,proxy); }
void UrdInstSubsystem::rdAddLongDistanceInstanceProxy(UStaticMesh* mesh,int32 index,double distance,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddLongDistanceInstanceProxy(mesh,index,distance,proxy); }
void UrdInstSubsystem::rdAddLongDistanceInstanceProxyX(const FName sid,int32 index,double distance,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddLongDistanceInstanceProxyX(sid,index,distance,proxy); }
void UrdInstSubsystem::rdAddLongDistanceActorProxy(AActor* actor,double distance,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddLongDistanceActorProxy(actor,distance,proxy); }
void UrdInstSubsystem::rdAddLongDistancePrefabProxy(ArdActor* prefab,double distance,const FrdProxySetup proxy) { if(rdGetBaseActor()) rdBaseActor->rdAddLongDistancePrefabProxy(prefab,distance,proxy); }
void UrdInstSubsystem::rdAddDestructionInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup proxy)  { if(rdGetBaseActor()) rdBaseActor->rdAddDestructionInstanceProxy(mesh,index,proxy); }
void UrdInstSubsystem::rdAddDestructionInstanceProxyX(const FName sid,int32 index,const FrdProxySetup proxy)  { if(rdGetBaseActor()) rdBaseActor->rdAddDestructionInstanceProxyX(sid,index,proxy); }
void UrdInstSubsystem::rdAddDestructionActorProxy(AActor* actor,const FrdProxySetup proxy)  { if(rdGetBaseActor()) rdBaseActor->rdAddDestructionActorProxy(actor,proxy); }
void UrdInstSubsystem::rdAddDestructionPrefabProxy(ArdActor* prefab,const FrdProxySetup proxy)  { if(rdGetBaseActor()) rdBaseActor->rdAddDestructionPrefabProxy(prefab,proxy); }
void UrdInstSubsystem::rdAddPickupProxy(UStaticMesh* mesh,int32 id,FrdPickup pickup) { if(rdGetBaseActor()) rdBaseActor->rdAddPickupProxy(mesh,id,pickup); }
void UrdInstSubsystem::rdAddPickupProxyX(const FName sid,int32 id,FrdPickup pickup) { if(rdGetBaseActor()) rdBaseActor->rdAddPickupProxyX(sid,id,pickup); }

void UrdInstSubsystem::rdMarkProxyAsDestroyed(AActor* proxy,const FTransform& offset) { if(rdGetBaseActor()) rdBaseActor->rdMarkProxyAsDestroyed(proxy,offset); }
void UrdInstSubsystem::rdSetProxyToDestroyed(AActor* proxy,const FTransform& offset) { if(rdGetBaseActor()) rdBaseActor->rdSetProxyToDestroyed(proxy,offset); }
void UrdInstSubsystem::rdRemoveProxyByInstance(UStaticMesh* mesh,int32 index) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByInstance(mesh,index); }
void UrdInstSubsystem::rdRemoveProxyByInstanceX(const FName sid,int32 index) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByInstanceX(sid,index); }
void UrdInstSubsystem::rdRemoveProxyByProxy(AActor* proxy,bool bKeepPinned) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByProxy(proxy,bKeepPinned); }
void UrdInstSubsystem::rdRemoveProxyByDataLayer(TSoftObjectPtr<UDataLayerAsset> dla) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByDataLayer(dla); }
void UrdInstSubsystem::rdRemoveProxyByProxyISM(UStaticMesh* mesh,int32 index) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByProxyISM(mesh,index); }
void UrdInstSubsystem::rdRemoveProxyByProxyISMX(const FName sid,int32 index) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByProxyISMX(sid,index); }
void UrdInstSubsystem::rdRemoveProxyByActor(AActor* actor) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByActor(actor); }
void UrdInstSubsystem::rdRemoveProxyByPrefab(ArdActor* prefab) { if(rdGetBaseActor()) rdBaseActor->rdRemoveProxyByPrefab(prefab); }
void UrdInstSubsystem::rdRemoveLongDistanceProxyByProxy(AActor* proxy) { if(rdGetBaseActor()) rdBaseActor->rdRemoveLongDistanceProxyByProxy(proxy); }
void UrdInstSubsystem::rdRemovePickupProxies(int32 id) { if(rdGetBaseActor()) rdBaseActor->rdRemovePickupProxies(id); }
int32 UrdInstSubsystem::rdSpawnPickup(int32 id,const FTransform& transform) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdSpawnPickup(id,transform); }
void UrdInstSubsystem::rdPickupPickup(int32 id,int32 instance) { if(rdGetBaseActor()) rdBaseActor->rdPickupPickup(id,instance); }
void UrdInstSubsystem::rdPickupCurrent() { if(rdGetBaseActor()) rdBaseActor->rdPickupCurrent(); }
void UrdInstSubsystem::rdRemovePickup(int32 id,int32 instance) {if(rdGetBaseActor()) rdBaseActor->rdRemovePickup(id,instance); }
void UrdInstSubsystem::rdRemoveAllProxies()  { if(rdGetBaseActor()) rdBaseActor->rdRemoveAllProxies(); }
void UrdInstSubsystem::rdSwapInstanceToProxyActor(UStaticMesh* mesh,int32 index,const FTransform& transform,bool forImpact,bool forLongDistance) { if(rdGetBaseActor()) rdBaseActor->rdSwapInstanceToProxyActor(mesh,index,transform,forImpact,forLongDistance); }
void UrdInstSubsystem::rdSwapInstanceToProxyActorX(const FName sid,int32 index,const FTransform& transform,bool forImpact,bool forLongDistance) { if(rdGetBaseActor()) rdBaseActor->rdSwapInstanceToProxyActorX(sid,index,transform,forImpact,forLongDistance); }
void UrdInstSubsystem::rdSwapActorToProxyActor(AActor* actor,int32 index,bool forImpact,bool forLongDistance) { if(rdGetBaseActor()) rdBaseActor->rdSwapActorToProxyActor(actor,index,forImpact,forLongDistance); }
void UrdInstSubsystem::rdSwapPrefabToProxyActor(ArdActor* prefab,int32 index,bool forImpact,bool forLongDistance) { if(rdGetBaseActor()) rdBaseActor->rdSwapPrefabToProxyActor(prefab,index,forImpact,forLongDistance); }
void UrdInstSubsystem::rdSwapProxyActorToInstance(AActor* actor)  { if(!rdGetBaseActor()) return; rdBaseActor->rdSwapProxyActorToInstance(actor); }
void UrdInstSubsystem::rdSwapProxyActorToActor(AActor* actor)  { if(!rdGetBaseActor()) return; rdBaseActor->rdSwapProxyActorToActor(actor); }
void UrdInstSubsystem::rdSwapProxyActorToPrefab(AActor* actor)  { if(!rdGetBaseActor()) return; rdBaseActor->rdSwapProxyActorToPrefab(actor); }
void UrdInstSubsystem::rdProcessImpactProxies(const AActor* caller,const FVector& loc,const FHitResult& hit,float distance,FVector impulse,USceneComponent* comp,int32 index) { if(rdGetBaseActor()) rdBaseActor->rdProcessImpactProxies(caller,loc,hit,distance,impulse,comp,index); }
void UrdInstSubsystem::rdSpawn(const ArdSpawnActor* spawner,bool bake) { if(rdGetBaseActor()) rdBaseActor->rdSpawn(spawner,bake); }

void UrdInstSubsystem::rdSphereTrace(const FVector location,float radius,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors) { if(!rdGetBaseActor()) return; rdBaseActor->rdSphereTrace(location,radius,actors,instances,ignoreActors); }
void UrdInstSubsystem::rdBoxTrace(const FVector location,FVector halfSize,FRotator rot,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors) { if(!rdGetBaseActor()) return; rdBaseActor->rdBoxTrace(location,halfSize,rot,actors,instances,ignoreActors); }
bool UrdInstSubsystem::rdGetInstancesAtLocation(UInstancedStaticMeshComponent* ismc,const FVector loc,TArray<int32>& indexes,float tol) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetInstancesAtLocation(ismc,loc,indexes,tol); }
int32 UrdInstSubsystem::rdGetFirstInstanceAtLocation(UInstancedStaticMeshComponent* ismc,const FVector loc,float tol) { if(!rdGetBaseActor()) return -1; return rdBaseActor->rdGetFirstInstanceAtLocation(ismc,loc,tol); }

TMap<FName,FrdProxyItem>& UrdInstSubsystem::rdGetInstancedMeshToProxyMap() { static TMap<FName,FrdProxyItem> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetInstancedMeshToProxyMap(); }
TMap<FName,FrdProxyInstanceItems>& UrdInstSubsystem::rdGetInstancesToProxyMap() { static TMap<FName,FrdProxyInstanceItems> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetInstancesToProxyMap(); }
TMap<AActor*,FrdProxyItem>& UrdInstSubsystem::rdGetActorsToProxyMap() { static TMap<AActor*,FrdProxyItem> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetActorsToProxyMap(); }
TMap<ArdActor*,FrdProxyItem>& UrdInstSubsystem::rdGetPrefabsToProxyMap() { static TMap<ArdActor*,FrdProxyItem> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetPrefabsToProxyMap(); }
TMap<FName,FrdProxyGridArray>& UrdInstSubsystem::rdGetLongDistanceInstancesToProxyMap() { static TMap<FName,FrdProxyGridArray> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetLongDistanceInstancesToProxyMap(); }
TMap<AActor*,FrdProxyItem>& UrdInstSubsystem::rdGetLongDistanceActorsToProxyMap() { static TMap<AActor*,FrdProxyItem> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetLongDistanceActorsToProxyMap(); }
TMap<ArdActor*,FrdProxyGridArray>& UrdInstSubsystem::rdGetLongDistancePrefabsToProxyMap() { static TMap<ArdActor*,FrdProxyGridArray> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetLongDistancePrefabsToProxyMap(); }
TMap<AActor*,FrdProxyActorItem>&  UrdInstSubsystem::rdGetProxyActorMap() { static TMap<AActor*,FrdProxyActorItem> i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdGetProxyActorMap(); }

FrdProxySetup UrdInstSubsystem::rdMakeActorProxySetup(UClass* proxyActor,bool bDontHide,bool bDontRemove,float proxyPhysicsTimeout,bool bPooled,bool bSimplePool,int32 pooledAmount,UrdStateDataAsset* savedState,bool bCallSwapEvent) { static FrdProxySetup i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdMakeActorProxySetup(proxyActor,bDontHide,bDontRemove,proxyPhysicsTimeout,bPooled,bSimplePool,pooledAmount,savedState,bCallSwapEvent); }
FrdProxySetup UrdInstSubsystem::rdMakeDataLayerProxySetup(TSoftObjectPtr<UDataLayerAsset> proxyDataLayer,bool bDontHide,bool bDontRemove,UrdStateDataAsset* savedState,bool bCallSwapEvent) { static FrdProxySetup i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdMakeDataLayerProxySetup(proxyDataLayer,bDontHide,bDontRemove,savedState,bCallSwapEvent); }
FrdProxySetup UrdInstSubsystem::rdMakeStaticMeshProxySetup(UStaticMesh* proxyMesh,TArray<TSoftObjectPtr<UMaterialInterface>> materials,bool reverseCulling,TEnumAsByte<ErdCollision> collision,bool bDontHide,bool bDontRemove,UrdStateDataAsset* savedState,bool bCallSwapEvent) { static FrdProxySetup i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdMakeStaticMeshProxySetup(proxyMesh,materials,reverseCulling,collision,bDontHide,bDontRemove,savedState,bCallSwapEvent); }
FrdProxySetup UrdInstSubsystem::rdAddDestructionToProxySetup(const FrdProxySetup& inProxySetup,UStaticMesh* destroyedMesh,UClass* destroyedPrefab,const FTransform& destroyedOffset) { static FrdProxySetup i; if(!rdGetBaseActor()) return i; return rdBaseActor->rdAddDestructionToProxySetup(inProxySetup,destroyedMesh,destroyedPrefab,destroyedOffset); }

// Conversion -------------------------------------------------------------------------------------

UChildActorComponent* UrdInstSubsystem::rdConvertInstanceToChildActor(ArdActor* actor,UStaticMesh* mesh,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToChildActor(actor,mesh,index); }
UChildActorComponent* UrdInstSubsystem::rdConvertInstanceToChildActorX(ArdActor* actor,const FName sid,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToChildActorX(actor,sid,index); }
AActor* UrdInstSubsystem::rdConvertInstanceToLevelActor(UStaticMesh* mesh,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToLevelActor(mesh,index); }
AActor* UrdInstSubsystem::rdConvertInstanceToLevelActorX(const FName sid,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToLevelActorX(sid,index); }
AActor* UrdInstSubsystem::rdConvertInstanceToActor(UStaticMesh* mesh,UClass* actorClass,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToActor(mesh,actorClass,index); }
AActor* UrdInstSubsystem::rdConvertInstanceToActorX(const FName sid,UClass* actorClass,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToActorX(sid,actorClass,index); }
AActor* UrdInstSubsystem::rdConvertInstanceToActorFromPool(UStaticMesh* mesh,UClass* actorClass,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToActorFromPool(mesh,actorClass,index); }
AActor* UrdInstSubsystem::rdConvertInstanceToActorFromPoolX(const FName sid,UClass* actorClass,int32 index) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdConvertInstanceToActorFromPoolX(sid,actorClass,index); }
int32 UrdInstSubsystem::rdConvertInstancesToActorsFromPool(UStaticMesh* mesh,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdConvertInstancesToActorsFromPool(mesh,actorClass,mode,radius,start,finish,actors); }
int32 UrdInstSubsystem::rdConvertInstancesToActorsFromPoolX(const FName sid,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdConvertInstancesToActorsFromPoolX(sid,actorClass,mode,radius,start,finish,actors); }

int32 UrdInstSubsystem::rdProceduralGenerate(const ArdSpawnActor* spawner) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdProceduralGenerate(spawner); }

// Utilities --------------------------------------------------------------------------------------

void UrdInstSubsystem::rdHarvestSMs(AActor* actor,bool hide,int32& numConverted,int32& numStandard,TMap<FName,int32>& sidMap) { if(!rdGetBaseActor()) return; rdBaseActor->rdHarvestSMs(actor,hide,numConverted,numStandard,sidMap); }
void UrdInstSubsystem::rdRemoveHarvestedInstances(AActor* actor) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveHarvestedInstances(actor); }
void UrdInstSubsystem::rdSetupAutoInst(bool autoInst,bool autoFromTags,bool autoMeshActors,bool autoBPs) { if(!rdGetBaseActor()) return; rdBaseActor->rdSetupAutoInst(autoInst,autoFromTags,autoMeshActors,autoBPs); }
void UrdInstSubsystem::rdConvertAutoISMs() { if(!rdGetBaseActor()) return; rdBaseActor->rdConvertAutoISMs(); }
void UrdInstSubsystem::rdRevertFromAutoISMs() { if(!rdGetBaseActor()) return; rdBaseActor->rdRevertFromAutoISMs(); }

double UrdInstSubsystem::getLandscapeZ(double x,double y,float radius,FHitResult& hit,bool fixSlopes,bool hitSM) { if(!rdGetBaseActor()) return 0; return rdBaseActor->getLandscapeZ(x,y,radius,hit,fixSlopes,hitSM); }
int32 UrdInstSubsystem::rdGetNumAssetsCompiling() { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdGetNumAssetsCompiling(); }
void UrdInstSubsystem::rdWaitForCompiling() { if(!rdGetBaseActor()) return; rdBaseActor->rdWaitForCompiling(); }
void UrdInstSubsystem::rdSubmitMaterialsForCompile() { if(!rdGetBaseActor()) return; rdBaseActor->rdSubmitMaterialsForCompile(); }
bool UrdInstSubsystem::rdIsEditor() { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdIsEditor(); }
bool UrdInstSubsystem::rdIsPlaying() { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdIsPlaying(); }
FBox UrdInstSubsystem::rdGetLandscapeBounds(ALandscapeProxy* landscape) { if(!rdGetBaseActor()) return FBox(); return rdBaseActor->rdGetLandscapeBounds(landscape); }
rdScalabilityScale UrdInstSubsystem::rdGetCurrentScalability() { if(!rdGetBaseActor()) return RDSCALE_NONE; return rdBaseActor->rdGetCurrentScalability(); }
rdScalabilityScale UrdInstSubsystem::rdGetCurrentScalabilityFor(rdScalabilityType stype) { if(!rdGetBaseActor()) return RDSCALE_NONE; return rdBaseActor->rdGetCurrentScalabilityFor(stype); }
float UrdInstSubsystem::rdResolutionScale() { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdResolutionScale(); }
bool UrdInstSubsystem::rdGetScalabilityBenchmarks(float& bmCpu,float& bmGpu) { if(!rdGetBaseActor()) return false; return rdBaseActor->rdGetScalabilityBenchmarks(bmCpu,bmGpu); }

UrdBakedDataAsset* UrdInstSubsystem::rdCreateTempBakedDataAsset() { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdCreateTempBakedDataAsset(); }
UrdBakedDataAsset* UrdInstSubsystem::rdLoadBakedDataAsset(TSoftObjectPtr<UrdBakedDataAsset>& data,bool create) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdLoadBakedDataAsset(data,create); }
int32 UrdInstSubsystem::rdBakeVolumeTransformsForProxies(AActor* volume,TArray<FrdSpawnData>& bakedObjectList,int32 rows,int32 cols,bool storeTransforms) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdBakeVolumeTransformsForProxies(volume,bakedObjectList,rows,cols,storeTransforms); }
int32 UrdInstSubsystem::rdBakeVolumeTransformsForProxiesX(AActor* volume,const TArray<FName>& sids,TArray<FrdSpawnData>& bakedObjectList,int32 rows,int32 cols,bool storeTransforms) { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdBakeVolumeTransformsForProxiesX(volume,sids,bakedObjectList,rows,cols,storeTransforms); }
void UrdInstSubsystem::rdRemoveVolumeTransformsForProxies(AActor* volume) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveVolumeTransformsForProxies(volume); }

void UrdInstSubsystem::rdGetEditorCameraPos(FVector& loc,FRotator& rot) { if(!rdGetBaseActor()) return; rdBaseActor->rdGetEditorCameraPos(loc,rot); }

APawn* UrdInstSubsystem::rdGetControlledPawn()  { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetControlledPawn(); }

void UrdInstSubsystem::rdDrawLine(const FVector& start,const FVector& end,const FColor& col,float duration)  { if(!rdGetBaseActor()) return; rdBaseActor->rdDrawLine(start,end,col,duration); }
void UrdInstSubsystem::rdMoveBaseActor(const FVector& location,const FRotator& rot) { if(!rdGetBaseActor()) return; rdBaseActor->rdMoveBaseActor(location,rot); }
void UrdInstSubsystem::rdReseatBaseActor(const FVector& location,const FRotator& rot) { if(!rdGetBaseActor()) return; rdBaseActor->rdReseatBaseActor(location,rot); }

FTransform UrdInstSubsystem::rdRelativeToWorld(const FTransform& t1,const FTransform& t2) { if(!rdGetBaseActor()) return FTransform(); return rdBaseActor->rdRelativeToWorld(t1,t2); }
FTransform UrdInstSubsystem::rdWorldToRelative(const FTransform& t1,const FTransform& t2) { if(!rdGetBaseActor()) return FTransform(); return rdBaseActor->rdWorldToRelative(t1,t2); }
FTransform UrdInstSubsystem::rdRelativeToWorldNoScale(const FTransform& t1,const FTransform& t2) { if(!rdGetBaseActor()) return FTransform(); return rdBaseActor->rdRelativeToWorldNoScale(t1,t2); }
FTransform UrdInstSubsystem::rdWorldToRelativeNoScale(const FTransform& t1,const FTransform& t2) { if(!rdGetBaseActor()) return FTransform(); return rdBaseActor->rdWorldToRelativeNoScale(t1,t2); }

// Splines ----------------------------------------------------------------------------------------

TArray<AActor*> UrdInstSubsystem::rdSplitSplineInto(AActor* actor,USplineComponent* spline,int32 num,bool callBPfunctions) { TArray<AActor*> ea; if(!rdGetBaseActor()) return ea; return rdBaseActor->rdSplitSplineInto(actor,spline,num,callBPfunctions); }
void UrdInstSubsystem::rdGetSplinePointPosition(int32 point,FVector& loc,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace) { if(!rdGetBaseActor()) return; rdBaseActor->rdGetSplinePointPosition(point,loc,subsplines,splineActor,cordSpace); }
TArray<AActor*> UrdInstSubsystem::rdSetSplinePointPosition(int32 point,const FVector& loc,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace,bool updateSpline) { TArray<AActor*> ary; if(!rdGetBaseActor()) return ary; return rdBaseActor->rdSetSplinePointPosition(point,loc,subsplines,splineActor,cordSpace,updateSpline); }
void UrdInstSubsystem::rdGetSplinePointRotation(int32 point,FRotator& rot,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace) { if(!rdGetBaseActor()) return; rdBaseActor->rdGetSplinePointRotation(point,rot,subsplines,splineActor,cordSpace); }
TArray<AActor*> UrdInstSubsystem::rdSetSplinePointRotation(int32 point,const FRotator& rot,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace,bool updateSpline) { TArray<AActor*> ary; if(!rdGetBaseActor()) return ary; return rdBaseActor->rdSetSplinePointRotation(point,rot,subsplines,splineActor,cordSpace,updateSpline); }
void UrdInstSubsystem::rdGetSplinePointScale(int32 point,FVector& scale,const TArray<AActor*>& subsplines,const AActor* splineActor) { if(!rdGetBaseActor()) return; rdBaseActor->rdGetSplinePointScale(point,scale,subsplines,splineActor); }
TArray<AActor*> UrdInstSubsystem::rdSetSplinePointScale(int32 point,const FVector& scale,const TArray<AActor*>& subsplines,const AActor* splineActor,bool updateSpline) { TArray<AActor*> ary; if(!rdGetBaseActor()) return ary; return rdBaseActor->rdSetSplinePointScale(point,scale,subsplines,splineActor,updateSpline); }
void UrdInstSubsystem::rdGetSplinePointTangent(int32 point,FVector& leaveTangent,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace) { if(!rdGetBaseActor()) return; rdBaseActor->rdGetSplinePointTangent(point,leaveTangent,subsplines,splineActor,cordSpace); }
TArray<AActor*> UrdInstSubsystem::rdSetSplinePointTangent(int32 point,const FVector& leaveTangent,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace,bool updateSpline) { TArray<AActor*> ary; if(!rdGetBaseActor()) return ary; return rdBaseActor->rdSetSplinePointTangent(point,leaveTangent,subsplines,splineActor,cordSpace,updateSpline); }
AActor* UrdInstSubsystem::rdGetSubSplineAtPoint(int32 point,const TArray<AActor*>& subsplines,int32& subPoint) {  if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdGetSubSplineAtPoint(point,subsplines,subPoint); }
AActor* UrdInstSubsystem::rdFindClosestSplinePoint(const FVector& loc,const TArray<AActor*>& subsplines,int32& localPoint,int32& overallPoint,float& distance,bool& found) {  if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdFindClosestSplinePoint(loc,subsplines,localPoint,overallPoint,distance,found); }
void UrdInstSubsystem::rdCopySpline(USplineComponent* spline1,USplineComponent* spline2,int32 start,int32 end) { if(!rdGetBaseActor()) return; rdBaseActor->rdCopySpline(spline1,spline2,start,end); }
void UrdInstSubsystem::rdTrimSpline(USplineComponent* spline,int32 start,int32 len) { if(!rdGetBaseActor()) return; rdBaseActor->rdTrimSpline(spline,start,len); }
void UrdInstSubsystem::rdJoinSplines(AActor* actor,bool callBPfunctions) { if(!rdGetBaseActor()) return; rdBaseActor->rdJoinSplines(actor,callBPfunctions); }
void UrdInstSubsystem::rdMergeActorsSubSplines(AActor* actor,bool callBPfunctions) { if(!rdGetBaseActor()) return; rdBaseActor->rdMergeActorsSubSplines(actor,callBPfunctions); }
int32 UrdInstSubsystem::rdPopulateSplineFast(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,UStaticMesh* mesh,const FTransform offset,float gap,UMaterialInterface* mat,const TEnumAsByte<ESplineMeshAxis::Type> axis,bool useCollision,bool useRoll)  { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdPopulateSplineFast(actor,spline,startDistance,endDistance,mesh,offset,gap,mat,axis,useCollision,useRoll); }
int32 UrdInstSubsystem::rdPopulateSpline(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,const TArray<FrdSplinePopulateData>& data,TArray<FrdSplineInstanceData>& instData,bool random,bool useCollision,bool useRoll)  { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdPopulateSpline(actor,spline,startDistance,endDistance,data,instData,random,useCollision,useRoll); }
void UrdInstSubsystem::rdRemoveSplineSMCs(AActor* actor,USplineComponent* spline) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveSplineSMCs(actor,spline); }
#ifdef includeProceduralMeshStuff
int32 UrdInstSubsystem::rdPopulateSplinePMCFast(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,UStaticMesh* mesh,UMaterialInterface* mat,const FVector scale,const TEnumAsByte<ESplineMeshAxis::Type> axis)  { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdPopulateSplinePMCFast(actor,spline,startDistance,endDistance,mesh,mat,scale,axis); }
int32 UrdInstSubsystem::rdPopulateSplinePMC(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,const TArray<FrdSplinePopulateData>& data,TArray<FrdSplineInstanceData>& instData,bool random)  { if(!rdGetBaseActor()) return 0; return rdBaseActor->rdPopulateSplinePMC(actor,spline,startDistance,endDistance,data,instData,random); }
void UrdInstSubsystem::rdRemoveSplinePMCs(AActor* actor,USplineComponent* spline) { if(!rdGetBaseActor()) return; rdBaseActor->rdRemoveSplinePMCs(actor,spline); }
#endif

// Landscape Tools --------------------------------------------------------------------------------

double UrdInstSubsystem::rdGetLandscapeZ(double x,double y) { if(!rdGetBaseActor()) return 0.0f; return rdBaseActor->rdGetLandscapeZ(x,y); }
void UrdInstSubsystem::rdPaintLandscape(ULandscapeLayerInfoObject* paintLayer,TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle,float strength,float falloff,USplineComponent* spline,const FString& layer)  { if(!rdGetBaseActor()) return; rdBaseActor->rdPaintLandscape(paintLayer,shape,x,y,width,height,angle,strength,falloff,spline,layer); }
void UrdInstSubsystem::rdFlattenLandscapeArea(TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle,float strength,float falloff,USplineComponent* spline,const FString& layer) { if(!rdGetBaseActor()) return; rdBaseActor->rdFlattenLandscapeArea(shape,x,y,width,height,angle,strength,falloff,spline,layer); }
void UrdInstSubsystem::rdStampLandscapeArea(UTexture2D* stamp,float x,float y,float width,float height,float strength,const FString& layer) { if(!rdGetBaseActor()) return; rdBaseActor->rdStampLandscapeArea(stamp,x,y,width,height,strength,layer); }
UTexture2D* UrdInstSubsystem::rdCreateTextureForShape(TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle,float strength,float falloff,USplineComponent* spline) { if(!rdGetBaseActor()) return nullptr; return rdBaseActor->rdCreateTextureForShape(shape,x,y,width,height,angle,strength,falloff,spline); }

//.............................................................................
