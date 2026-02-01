// rdInstBaseActor.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#define RD_SID_VER 2

#include "rdInst.h"
#include "rdInstBPLibrary.h"
#include "rdProxies.h"
#include "rdSpawn.h"
#include "rdPools.h"
#include "rdSplines.h"
#include "rdPlacement.h"
#include "rdUtilities.h"
#include "rdInstances.h"
#include "rdInstStates.h"
#include "rdLandscapeModify.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "rdInstBaseActor.generated.h"

class UrdInstData;
class rdPixelWrapper;
struct FrdSpawnData;
class ULandscapeLayerInfoObject;
class ALight;
class ADecalActor;
class ALightActor;
class ASpotLight;
class ARectLight;
class APointLight;
#if ENGINE_MAJOR_VERSION>4
class ALocalFogVolume;
#endif
class APostProcessVolume;
class ANiagaraActor;
class ATextRenderActor;

#define RDINST_PLUGIN_API DLLEXPORT
//#define includeProceduralMeshStuff

// Note: The 4.27 code here is different from UE5+ code
// There are 3 Proxy functions that handle distance as float here - but in UE5+ they are doubles to support large world coordinates.
// UE4 doesn't handle double floats in Blueprint.
//

struct rdThreadResult {
	TMap<FName,TArray<FTransform>>				 addMap;
	TMap<FName,TArray<TTuple<int32,FTransform>>> moveMap;
	TMap<FName,TArray<int32>>					 remMap;
	TArray<AActor*>								 destroyActorList;
	TArray<FrdInstanceItems>					 scannedInstances;
	TArray<AActor*>								 scannedActors;
	DamageQueue									 damagedProxies;
};

UCLASS()
class RDINST_PLUGIN_API UrdInstancedStaticMeshComponent : public UInstancedStaticMeshComponent {

	GENERATED_BODY()

	bool IsNameStableForNetworking() const override { return true; }

public:
#if ENGINE_MAJOR_VERSION>4
	//virtual bool CanEditSMInstance(const FSMInstanceId& InstanceId) const override;
	//virtual bool CanMoveSMInstance(const FSMInstanceId& InstanceId, const ETypedElementWorldType InWorldType) const override;
	//virtual bool GetSMInstanceTransform(const FSMInstanceId& InstanceId, FTransform& OutInstanceTransform, bool bWorldSpace = false) const override;
	//virtual bool SetSMInstanceTransform(const FSMInstanceId& InstanceId, const FTransform& InstanceTransform, bool bWorldSpace = false, bool bMarkRenderStateDirty = false, bool bTeleport = false) override;
	//virtual void NotifySMInstanceMovementStarted(const FSMInstanceId& InstanceId) override;
	//virtual void NotifySMInstanceMovementOngoing(const FSMInstanceId& InstanceId) override;
	//virtual void NotifySMInstanceMovementEnded(const FSMInstanceId& InstanceId) override;
	//virtual void NotifySMInstanceSelectionChanged(const FSMInstanceId& InstanceId, const bool bIsSelected) override;
	//virtual bool DeleteSMInstances(TArrayView<const FSMInstanceId> InstanceIds) override;
	//virtual bool DuplicateSMInstances(TArrayView<const FSMInstanceId> InstanceIds, TArray<FSMInstanceId>& OutNewInstanceIds) override;
#endif
};

UCLASS()
class RDINST_PLUGIN_API UrdHierarchicalInstancedStaticMeshComponent : public UHierarchicalInstancedStaticMeshComponent {

	GENERATED_BODY()

	bool IsNameStableForNetworking() const override { return true; }

public:
#if ENGINE_MAJOR_VERSION>4
	//virtual bool CanEditSMInstance(const FSMInstanceId& InstanceId) const override;
	//virtual bool CanMoveSMInstance(const FSMInstanceId& InstanceId, const ETypedElementWorldType InWorldType) const override;
	//virtual bool GetSMInstanceTransform(const FSMInstanceId& InstanceId, FTransform& OutInstanceTransform, bool bWorldSpace = false) const override;
	//virtual bool SetSMInstanceTransform(const FSMInstanceId& InstanceId, const FTransform& InstanceTransform, bool bWorldSpace = false, bool bMarkRenderStateDirty = false, bool bTeleport = false) override;
	//virtual void NotifySMInstanceMovementStarted(const FSMInstanceId& InstanceId) override;
	//virtual void NotifySMInstanceMovementOngoing(const FSMInstanceId& InstanceId) override;
	//virtual void NotifySMInstanceMovementEnded(const FSMInstanceId& InstanceId) override;
	//virtual void NotifySMInstanceSelectionChanged(const FSMInstanceId& InstanceId, const bool bIsSelected) override;
	//virtual bool DeleteSMInstances(TArrayView<const FSMInstanceId> InstanceIds) override;
	//virtual bool DuplicateSMInstances(TArrayView<const FSMInstanceId> InstanceIds, TArray<FSMInstanceId>& OutNewInstanceIds) override;
#endif
};

// AutoInst Physics Options
UENUM()
enum rdAutoInstPhysics {
	// Leave as Actor
	RDAUTOINST_PHYSICS_LEAVE	UMETA(DisplayName="Leave as Actor"),
	// Convert to StaticMesh Instances (no physics)
	RDAUTOINST_PHYSICS_TOMESH	UMETA(DisplayName="Convert (No Physics)"),
	// Convert to StaticMesh Instances and add proxies to convert back to actors in proximity
	RDAUTOINST_PHYSICS_ADDPROXY	UMETA(DisplayName="Convert with Proxy"),
};

// AutoInst Destruction Options
UENUM()
enum rdAutoInstDestruction {
	// Leave as Actor
	RDAUTOINST_DESTRUCTION_LEAVE	UMETA(DisplayName="Leave as Actor"),
	// Convert to StaticMesh Instances (no destruction)
	RDAUTOINST_DESTRUCTION_TOMESH	UMETA(DisplayName="Convert (No Destruction)"),
	// Convert to StaticMesh Instances and add proxies to convert back to actors in proximity
	RDAUTOINST_DESTRUCTION_ADDPROXY	UMETA(DisplayName="Convert with Proxy"),
};

// Type of Runtime Population Override
UENUM()
enum rdPopulationOverride {
	// Don't override the population rendering
	RDFORCEPOP_NOOVERRIDE			UMETA(DisplayName="Don't override"),
	// Don't render any of the runtime population objects
	RDFORCEPOP_NONE					UMETA(DisplayName="Don't render population objects"),
	// Don't Render Frustum(FDP) based objects
	RDFORCEPOP_NOFRUSTUM			UMETA(DisplayName="Don't Render Frustum objects"),
	// Don't Render Spherical(SDP) based objects
	RDFORCEPOP_NOSPHERICAL			UMETA(DisplayName="Don't Render Frustum objects"),
	// Render all runtime population objects always
	RDFORCEPOP_ALL					UMETA(DisplayName="Render all population objects"),
	// Render Frustum based population as Spherical (useful for FPS swapping to TPS etc)
	RDFORCEPOP_FRUSTUMTOSPHERICAL	UMETA(DisplayName="Render Frustum as Spherical")
};

struct rdDistributedObjectData {
	FName						sid;
	bool						pooled=false;
	TArray<TArray<FTransform>>	transforms;
	FrdProxySetup				proxy;
/*
	bool						pooled=false;
	FrdProxySetup				proxy;
	AActor*						actorTemplate=nullptr;
	TArray<TArray<FTransform>>	tList;
	TArray<int32>*				iList;
	TArray<AActor*>*			aList;
	TArray<UNiagaraComponent*>*	fxList;
*/
};

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FrdInstProxySwapInDelegate,AActor*,ProxyActor,const FName,sid,int32,index,UInstancedStaticMeshComponent*,ismc,UrdStateDataAsset*,state);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FrdInstProxySwapOutDelegate,AActor*,ProxyActor,const FName,sid,int32,index,UInstancedStaticMeshComponent*,ismc,UrdStateDataAsset*,state);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FrdInstProxySpawnOnServerDelegate,UClass*,actorClass,const FTransform&,transform,UrdStateDataAsset*,state,const FName,sid,int32,index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FrdInstPickupFocusedDelegate,int32,pickupID,const FTransform&,transform,const FName,sid,int32,index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FrdInstPickupUnfocusedDelegate,int32,pickupID,const FTransform&,transform,const FName,sid,int32,index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FrdInstPickedUpDelegate,int32,pickupID,const FTransform&,transform,const FName,sid,int32,index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FrdActorPoolUsedDelegate,AActor*,actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FrdActorPoolReturnedDelegate,AActor*,actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FrdInstDamageDelegate,UPrimitiveComponent*,target,AActor*,actor,FVector,location,FVector,impulse,FHitResult,hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FrdPreSpawnDelegate,AActor*,actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FrdPostSpawnDelegate,AActor*,actor);

struct rdDelegateQueueItem {

	rdDelegateQueueItem(int32 tp,AActor* pa,const FName s,int32 i,UInstancedStaticMeshComponent* is,UrdStateDataAsset* st) : type(tp),proxyActor(pa),sid(s),index(i),ismc(is),state(st) {}
	rdDelegateQueueItem(int32 tp,UClass* ac,const FTransform& t,const FName s,int32 i,UrdStateDataAsset* st) : type(tp),sid(s),index(i),state(st),actorClass(ac),transform(t) {}
	rdDelegateQueueItem(int32 tp,int32 pid,const FTransform& t,const FName s,int32 i) : type(tp),sid(s),index(i),transform(t),pickupID(pid) {}
	rdDelegateQueueItem(int32 tp,UPrimitiveComponent* c,AActor* a,FVector& l,FVector& i,FHitResult& h) : type(tp),comp(c),actor(a),location(l),impulse(i),hit(h) {}

	int32 type=0; // 0=ProxySwapIn, 1=ProxySwapOut, 2=ProxySpawnOnServer, 3=PickupFocused, 4=PickupUnfocused, 5=PickedUp, 6=CollisionHitEvent

	AActor* proxyActor=nullptr;
	FName sid;
	int32 index;
	UInstancedStaticMeshComponent* ismc=nullptr;
	UrdStateDataAsset* state=nullptr;
	UClass* actorClass=nullptr;
	FTransform transform;
	int32 pickupID=0;
	UPrimitiveComponent* comp=nullptr;
	AActor* actor=nullptr;
	FVector location;
	FVector impulse;
	FHitResult hit;
};

// rdInstBaseActor
//
// This is the Base Actor that Maintains the Instanced Static Meshes in the level
//
// One is always created automatically in the scene if it can't be found, and it's hidden from 
// the outliner so the user doesn't even need to know it exists.
//
UCLASS(HideCategories=(Transform,Rendering,Replication,Collision,HLOD,Physics,Networking,WorldPartition,Input,Actor,LevelInstance,Cooking,DataLayers,rdInstProxies))
class RDINST_PLUGIN_API ArdInstBaseActor : public AActor {

	GENERATED_BODY()

	ArdInstBaseActor();
	virtual ~ArdInstBaseActor();
protected:

	// The OnConstruction Script for the BaseActor
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//void OnLevelStreamingStateChanged(UWorld* world,const ULevelStreaming* stream,ULevel* level,ELevelStreamingState prevState,ELevelStreamingState state);

#if WITH_EDITOR
	virtual void PostEditUndo() override;
	virtual void PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation) override;
	
	// Hack to stop the ISMC UpdateTransform etc functions from actually carrying through with the Modify (causes level save prompt for no reason)
	virtual bool Modify(bool bAlwaysMarkDirty=true) override;
public:
	int32 dontDoModify=0;
protected:
#endif

	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;

public:

	// The singular Base Actor for Instance Handling and Actor Pooling
	UFUNCTION(BlueprintCallable,Category="rdInst|Setup",meta=(DisplayName="rdSetUpInst",Keywords="rdInst Set Up"))
	void rdSetUpInst(bool useHISM=true,bool useISMforNanite=true,bool recycleInstances=true,bool autoInst=false,bool autoFromTags=false,bool autoMeshActors=true,bool autoBPs=true);

	// Returns the current rdInst version
	UFUNCTION(BlueprintCallable,CallInEditor,Category="rdInst|Utilities",meta=(DisplayName="rdGetrdInstVersion",Keywords="rdInst Version"))
	float rdGetrdInstVersion();

	// Always returns true
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsRdPrefab",Keywords="rdInst rdInstSettings Is"),Category="rdInst|Utilities")
	bool rdIsRdSettings()  { return true; }

// Instance Handling ------------------------------------------------------------------------------

	// Returns True if the mesh is Nanite
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsMeshNanite",Keywords="rdInst Mesh Nanite"),Category="rdInst|Instancing")
	bool rdIsMeshNanite(const UStaticMesh* mesh);

	// Finds and returns the UInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGen",Keywords="rdInst Instance Gen Get"),Category="rdInst|Instancing")
	UInstancedStaticMeshComponent* rdGetInstanceGen(const UStaticMesh* mesh,bool create=true);

	// Finds and returns the UHierarchicalInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetHInstanceGen",Keywords="rdInst HInstance Gen Get"),Category="rdInst|Instancing")
	UHierarchicalInstancedStaticMeshComponent* rdGetHInstanceGen(const UStaticMesh* mesh,bool create=true);

	// Finds and returns the UInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGenX",Keywords="rdInst Instance Gen Get"),Category="rdInst|Instancing")
	UInstancedStaticMeshComponent* rdGetInstanceGenX(const FName sid,bool create=true);

	// Finds and returns the UHierarchicalInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetHInstanceGenX",Keywords="rdInst HInstance Gen Get"),Category="rdInst|Instancing")
	UHierarchicalInstancedStaticMeshComponent* rdGetHInstanceGenX(const FName sid,bool create=true);

	// Finds and returns the UInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGenX",Keywords="rdInst Instance Gen Get"),Category="rdInst|Instancing")
	UInstancedStaticMeshComponent* rdGetPreferredInstanceGenX(const FName sid,bool create=true);

	// Add Instances ------------------------------------------------------------------------------

	// Adds an Instance of the passed in StaticMesh to the level using the passed in Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstance",Keywords="rdInst Instance Add"),Category="rdInst|Instancing")
	int32 rdAddInstance(const AActor* instOwner,UStaticMesh* mesh,const FTransform& transform);

	// Adds an Instance of the passed in StaticMesh to the level using the passed in Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceX",Keywords="rdInst Instance Add"),Category="rdInst|Instancing")
	int32 rdAddInstanceX(const AActor* instOwner,const FName sid,const FTransform& transform);

	// Adds Instances of the StaticMesh from the passed Sid to the level using the passed in Transforms, instances are reused from previously removed ones if wanted.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesX",Keywords="rdInst Instance Add Sid"),Category="rdInst|Instancing")
	void rdAddInstancesX(const FName sid,const TArray<FTransform>& transforms);

	// Adds an Instance of the StaticMesh from the passed ISMC to the level using the passed in Transform, instances are reused from previously removed ones if wanted.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceFast",Keywords="rdInst Instance Add Fast"),Category="rdInst|Instancing")
	int32 rdAddInstanceFast(UInstancedStaticMeshComponent* instGen,const FTransform& transform);

	// Adds Instances of the StaticMesh from the passed ISMC to the level using the passed in Transform Array, instances are reused from previously removed ones if wanted.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesFast",Keywords="rdInst Instances Add Fast"),Category="rdInst|Instancing")
	void rdAddInstancesFast(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<FTransform>& transforms);

	// Adds Instances of the StaticMesh from the passed ISMC to the level using the passed in Transform Array, instances are reused from previously removed ones if wanted.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesFastWithIndexes",Keywords="rdInst Instances Add Fast Indexes"),Category="rdInst|Instancing")
	void rdAddInstancesFastWithIndexes(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<FTransform>& transforms,TArray<int32>& indexList);

	// Adds Instances of the StaticMesh from the passed ISMC to the level using the passed in Transform Array, instances are reused from previously removed ones if wanted.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesFastWithIndexes",Keywords="rdInst Instances Add Fast Indexes"),Category="rdInst|Instancing")
	void rdAddInstanceArrayFastWithIndexes(UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bso);

	// Adds Instances of the StaticMesh from the passed ISMC to the level using the passed in Relative Transform Array, instances are reused from previously removed ones if wanted.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesFastWithIndexes",Keywords="rdInst Instances Add Fast Indexes"),Category="rdInst|Instancing")
	void rdAddInstanceArrayFastWithIndexesRel(const FTransform& t,UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bso);

	// Adds an Instance of the StaticMesh from the passed ISMC to the level using the passed in Transform using the raw Component calls
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceRaw",Keywords="rdInst Instance Add Raw"),Category="rdInst|Instancing")
	int32 rdAddInstanceRaw(UInstancedStaticMeshComponent* instGen,const FTransform& transform);

	// Adds Instances of the StaticMesh from the passed ISMC to the level using the passed in Transform Array using the raw Component calls
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesRaw",Keywords="rdInst Instances Add Raw"),Category="rdInst|Instancing")
	int32 rdAddInstancesRaw(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<FTransform>& transforms);

	// Adds Instances of the StaticMesh from the passed ISMC to the level using the passed in Transform Array using the raw Component calls
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesRawWithIndexes",Keywords="rdInst Instances Add Raw Indexes"),Category="rdInst|Instancing")
	int32 rdAddInstancesRawWithIndexes(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<FTransform>& transforms,TArray<int32>& indexList);

	// Adds an Instance of the StaticMesh from the passed HISMC to the level using the passed in Transform, instances are reused from previously removed ones.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddOwnedInstance",Keywords="rdInst Instance Add Owned"),Category="rdInst|Instancing")
	int32 rdAddOwnedInstance(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,const FTransform& transform);

	// Adds the array of Instances of the StaticMesh from the passed ISMC to the level using the passed in Transforms, instances are reused from previously removed ones.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddOwnedInstances",Keywords="rdInst Instances Add Owned"),Category="rdInst|Instancing")
	int32 rdAddOwnedInstances(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<FTransform>& transforms);

	// Adds and Removes the Instances in the passed ISMC to/from the level using the passed in Arrays, instances are reused from previously removed ones.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddRemoveInstancesFast",Keywords="rdInst Instances Add Remove Fast"),Category="rdInst|Instancing")
	void rdAddRemoveInstancesFast(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<int32>& delIndices,UPARAM(ref) const TArray<FTransform>& transforms);

	// Remove Instances ---------------------------------------------------------------------------

	// Removes the ISM for the UStaticMesh and Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstance",Keywords="rdInst Instance Remove"),Category="rdInst|Instancing")
	void rdRemoveInstance(UStaticMesh* mesh,int32 index);

	// Removes the ISM for the UStaticMesh and Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstanceX",Keywords="rdInst Instance Remove"),Category="rdInst|Instancing")
	void rdRemoveInstanceX(const FName sid,int32 index);

	// Removes the Instance of the StaticMesh from the passed HISMC from the level
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstanceFast",Keywords="rdInst Instance Remove Fast"),Category="rdInst|Instancing")
	void rdRemoveInstanceFast(UInstancedStaticMeshComponent* instGen,int32 index);

	// Removes the Instances of the StaticMesh from the passed HISMC from the level
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstancesFast",Keywords="rdInst Instance Remove Fast"),Category="rdInst|Instancing")
	void rdRemoveInstancesFast(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<int32>& indexes);

	// Removes the Instance of the StaticMesh from the passed HISMC from the level
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstanceRaw",Keywords="rdInst Instance Remove Raw"),Category="rdInst|Instancing")
	void rdRemoveInstanceRaw(UInstancedStaticMeshComponent* instGen,int32 index);

	// Removes the Instances of the StaticMesh from the passed HISMC from the level
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstancesX",Keywords="rdInst Instance Remove Raw"),Category="rdInst|Instancing")
	void rdRemoveInstancesX(const FName sid,UPARAM(ref) const TArray<int32>& indexes);

	// Removes the Instances of the StaticMesh from the passed HISMC from the level
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstancesRaw",Keywords="rdInst Instance Remove Raw"),Category="rdInst|Instancing")
	void rdRemoveInstancesRaw(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<int32>& indexes);

	// Removes the Instance of the StaticMesh from the passed HISMC from the level
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveOwnedInstance",Keywords="rdInst Instance Remove Owned"),Category="rdInst|Instancing")
	void rdRemoveOwnedInstance(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,int32 index);

	// Removes all ISMs that belong to the passed in Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstancesForOwner",Keywords="rdInst Instance Remove Owner"),Category="rdInst|Instancing")
	void rdRemInstancesForOwner(const AActor* instOwner);

	// Removes all HISMCs in this BaseActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveAllInstances",Keywords="rdInst Instance Remove All"),Category="rdInst|Instancing")
	void rdRemAllInstances();

	// Removes all HISMCs in this BaseActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveAllHISMCs",Keywords="rdInst Instance Remove All"),Category="rdInst|Instancing")
	void rdRemAllHISMCs();

	// Transform Instances ------------------------------------------------------------------------

	// Changes the Transform of the Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdChangeInstanceTransformsForOwner",Keywords="rdInst Instance Transform Owner"),Category="rdInst|Instancing")
	void rdChangeInstanceTransformsForOwner(const AActor* instOwner,const FTransform& transform,bool worldSpace,bool markDirty,bool teleport);

	// Gets the Transform of the Specified Instance of the UStaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceTransform",Keywords="rdInst Instance Transform Get"),Category="rdInst|Instancing")
	bool rdGetInstanceTransform(const UStaticMesh* mesh,int32 index,FTransform& transform);

	// Gets the Transform of the Specified Instance of the UStaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceTransformX",Keywords="rdInst Instance Transform Get"),Category="rdInst|Instancing")
	bool rdGetInstanceTransformX(const FName sid,int32 index,FTransform& transform);

	// Gets the Location of the Specified Instance of the UStaticMesh from the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceLocation",Keywords="rdInst Instance Location Get"),Category="rdInst|Instancing")
	bool rdGetInstanceLocation(UStaticMesh* mesh,int32 index,FVector& slocation);

	// Gets the Location of the Specified Instance of the UStaticMesh from the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceLocationX",Keywords="rdInst Instance Location Get Fast"),Category="rdInst|Instancing")
	bool rdGetInstanceLocationX(const FName sid,int32 index,FVector& slocation);

	// Gets the Transform of the Specified Instance of the UStaticMesh from the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceTransformFast",Keywords="rdInst Instance Transform Get Fast"),Category="rdInst|Instancing")
	bool rdGetInstanceTransformFast(UInstancedStaticMeshComponent* ismc,int32 index,FTransform& stransform);

	// Gets the Location of the Specified Instance of the UStaticMesh from the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceLocationFast",Keywords="rdInst Instance Location Get Fast"),Category="rdInst|Instancing")
	bool rdGetInstanceLocationFast(UInstancedStaticMeshComponent* ismc,int32 index,FVector& slocation);

	// Update the Transform for the specified Instance Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformFast",Keywords="rdInst Transform Update"),Category="rdInst|Instancing")
	void rdUpdateTransformFast(UInstancedStaticMeshComponent* ismc,int32 index,const FTransform& transform);

	// Update the Transform for the specified Instance Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransform",Keywords="rdInst Transform Update"),Category="rdInst|Instancing")
	void rdUpdateTransform(UStaticMesh* mesh,int32 index,const FTransform& transform);

	// Update the Transform for the specified Instance Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformX",Keywords="rdInst Transform Update"),Category="rdInst|Instancing")
	void rdUpdateTransformX(const FName sid,int32 index,const FTransform& transform);

	// Update the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformsFast",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransformsFast(UInstancedStaticMeshComponent* ismc,int32 startIndex,UPARAM(ref) const TArray<FTransform>& transforms,int32 arrayoffset=0);

	// Update the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransforms",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransforms(UStaticMesh* mesh,int32 startIndex,UPARAM(ref) const TArray<FTransform>& transforms);

	// Update the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformsX",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransformsX(const FName sid,int32 startIndex,UPARAM(ref) const TArray<FTransform>& transforms);

	// Update the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformsX",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransformArrayX(const FName sid,const TArray<int32> indices,const TArray<FTransform>& transforms);

	void rdUpdateTransformTuplesX(const FName sid,TArray<TTuple<int32,FTransform>>& transforms);

	// Increment the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementTransforms",Keywords="rdInst Transforms Increment"),Category="rdInst|Instancing")
	void rdIncrementTransforms(UStaticMesh* mesh,UPARAM(ref) const TArray<int32>& indexes,const FTransform& transform);

	// Increment the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementTransformsX",Keywords="rdInst Transforms Increment"),Category="rdInst|Instancing")
	void rdIncrementTransformsX(const FName sid,UPARAM(ref) const TArray<int32>& indexes,const FTransform& transform);

	// Increment the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementTransformsFast",Keywords="rdInst Transforms Increment"),Category="rdInst|Instancing")
	void rdIncrementTransformsFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) const TArray<int32>& indexes,const FTransform& transform);

	// Increment the All the Transforms for the specified ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementAllTransformsFast",Keywords="rdInst Transforms Increment All"),Category="rdInst|Instancing")
	void rdIncrementAllTransformsFast(UInstancedStaticMeshComponent* ismc,const FTransform& transform);

	// Gets a reference to the Per-Instance Transforms for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetTransformsPtr",Keywords="rdInst Transforms Get Pointer"),Category="rdInst|Instancing")
	TArray<FMatrix>& rdGetTransformsPtr(UStaticMesh* mesh,int32& numTransforms);

	// Gets a reference to the Per-Instance Transforms for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetTransformsPtrX",Keywords="rdInst Transforms Get Pointer"),Category="rdInst|Instancing")
	TArray<FMatrix>& rdGetTransformsPtrX(const FName sid,int32& numTransforms);

	// Gets a reference to the Per-Instance Transforms for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetTransformsPtrFast",Keywords="rdInst Transforms Get Pointer"),Category="rdInst|Instancing")
	TArray<FMatrix>& rdGetTransformsPtrFast(UInstancedStaticMeshComponent* ismc,int32& numTransforms);

	void rdBatchUpdateInstancesTransforms(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indices,const TArray<FTransform>& transforms,bool bWorldSpace=false,bool bMarkRenderStateDirty=false,bool bTeleport=true);

	void rdQueueUpdateInstanceTransform(UInstancedStaticMeshComponent* ismc,int32 index,const FTransform& transform);
	void rdQueueSpawnInstance(UInstancedStaticMeshComponent* ismc,const FTransform& transform);
	void rdQueueSetInstanceVisibility(UInstancedStaticMeshComponent* ismc,int32 index,bool bVis);
	void rdQueueSetInstanceVisibilityX(const FName sid,int32 index,bool bVis);

	// Instance Visibility ------------------------------------------------------------------------

	// Shows or Hides the Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceVisibility",Keywords="rdInst Instance Visibility"),Category="rdInst|Instancing")
	bool rdSetInstanceVisibility(UStaticMesh* mesh,int32 index,bool vis);

	// Shows or Hides the Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceVisibilityX",Keywords="rdInst Instance Visibility"),Category="rdInst|Instancing")
	bool rdSetInstanceVisibilityX(const FName sid,int32 index,bool vis);

	// Shows or Hides the Instance (with ISMC reference)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceVisibilityFast",Keywords="rdInst Instance Visibility Fast"),Category="rdInst|Instancing")
	bool rdSetInstanceVisibilityFast(UInstancedStaticMeshComponent* ismc,int32 index,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibility",Keywords="rdInst Instances Visibility"),Category="rdInst|Instancing")
	void rdSetInstancesVisibility(UStaticMesh* mesh,const TArray<int32>& indexes,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibilityX",Keywords="rdInst Instances Visibility"),Category="rdInst|Instancing")
	void rdSetInstancesVisibilityX(const FName sid,UPARAM(ref) const TArray<int32>& indexes,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibilityFast",Keywords="rdInst Instances Visibility Fast"),Category="rdInst|Instancing")
	void rdSetInstancesVisibilityFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) const TArray<int32>& indexes,bool vis);

	// Instance Utilities -------------------------------------------------------------------------

	// Returns the Actor that owns the Instance or a nullptr if the passed in component is not an HISMC or index not valid
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetActorFromInstanceIndex",Keywords="rdInst rdActor From Instance"),Category="rdInst|Instancing")
	AActor* rdGetActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index);

	// Returns the rdActor that owns the Instance or a nullptr if the passed in component is not an HISMC or index not valid
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetrdActorFromInstanceIndex",Keywords="rdInst rdActor From Instance"),Category="rdInst|Instancing")
	ArdActor* rdGetrdActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index);

	// Returns the last index created of the passed in StaticMesh. If none exist, -1 is returned
	int32 rdGetLastInstanceIndex(const AActor* instOwner,UStaticMesh* mesh);

	// Returns the last index created of the passed in StaticMesh. If none exist, -1 is returned
	int32 rdGetLastInstanceIndexX(const AActor* instOwner,const FName sid);

	// Goes through and Counts all Instances of all StaticMeshes in this BaseActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="Calc Instance Counts",Keywords="rdInst Instance Calculate Number"),Category="rdInst Tools",CallInEditor)
	void rdCalcInstanceCounts();

	// Recreates all the ISMs and HISMCs in this BaseActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="Recreate Instances",Keywords="rdInst Recreate"),Category="rdInst Tools",CallInEditor)
	void rdRecreateInstances();

	// Remove all the ISMs and HISMCs in this BaseActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="Remove All Instances",Keywords="rdInst Remove All"),Category="rdInst Tools",CallInEditor)
	void rdRemoveAllInstances();

	// Sets the Instance Settings (eg shadows) contained in the StaticMesh in the HISMC
	void SetHISMCdata(UStaticMesh* mesh,UInstancedStaticMeshComponent* hismc);

	// Sets the Instance Settings (eg shadows) contained in the StaticMesh in the HISMC
	void SetHISMCdataX(const FName sid,UInstancedStaticMeshComponent* hismc);

	// Finds the Attached ISMC for the StaticMesh (referenced by sid)
	UInstancedStaticMeshComponent* FindISMCforMesh(const FName& sid);

	// Finds the ISMC for the StaticMesh in the external volume e.g. PCG volume
	UInstancedStaticMeshComponent* FindISMCforMeshInVolume(AActor* volume,const UStaticMesh* mesh);

	// Finds the ISMC for the StaticMesh in the external volume e.g. PCG volume
	UInstancedStaticMeshComponent* FindISMCforMeshInVolumeX(AActor* volume,const FName sid);

	// Finds the Attached HISMC for the StaticMesh (referenced by sid)
	UHierarchicalInstancedStaticMeshComponent* FindHISMCforMesh(const FName& sid);

	// Returns an Array of all ISMCs used
	UFUNCTION(BlueprintPure,meta=(DisplayName="GetUsedISMCs",Keywords="rdInst used ISMCs"),Category="rdInst|Tools")
	TArray<UInstancedStaticMeshComponent*> GetUsedISMCs();

	// Fills the array with rdActors that currently have selected instances
	UFUNCTION(BlueprintPure,meta=(DisplayName="GetActorsWithSelectedISMS",Keywords="rdInst selected ISMCs"),Category="rdInst|Tools")
	int32 GetActorsWithSelectedISMS(TArray<ArdActor*>& actors,bool& beingEdited,bool filterOutSelected=false);

	static FString rdPackMaterialName(const FString& nm);
	static FString rdUnpackMaterialName(const FString& nm);

	// Returns the Instance Handle ID for the specified StaticMesh
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMsid",Keywords="rdInst Instance Handle ID"),Category="rdInst|Sids")
	FName rdGetSMsid(const TSoftObjectPtr<UStaticMesh> mesh,ErdSpawnType type=ErdSpawnType::UseDefaultSpawn,const FName groupName=NAME_None);

	// Returns the Instance Handle ID for the specified StaticMesh, Material list and optional scale for negative checks
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMXsid",Keywords="rdInst Instance Handle ID"),Category="rdInst|Sids")
	FName rdGetSMXsid(ErdSpawnType type,const TSoftObjectPtr<UStaticMesh> mesh,TArray<TSoftObjectPtr<UMaterialInterface>> mats,bool bReverseCulling=false,ErdCollision collision=ErdCollision::UseDefault,float startCull=-1.0f,float endCull=-1.0f,int32 id=0,const FName groupName=NAME_None);

	// Returns the Instance Handle ID for the specified StaticMesh, Material list and optional scale for negative checks
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetInstSid",Keywords="rdInst Instance Handle ID"),Category="rdInst|Sids")
	FName rdGetInstSid(const FrdInstanceSetup& instanceSetup);

	// Returns the Instance Handle ID for the specified StaticMesh Component, using it's materials and negative scale flags
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMCsid",Keywords="rdInst Instance Handle ID"),Category="rdInst|Sids")
	FName rdGetSMCsid(const UStaticMeshComponent* smc,ErdSpawnType type=ErdSpawnType::UseDefaultSpawn,float overrideStartCull=-1.0f,float overrideEndCull=-1.0f,int32 overrideID=0,const FName overrideGroupName=NAME_None);

	// Returns the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMeshFromSid",Keywords="rdInst Instance Handle ID Mesh"),Category="rdInst|Sids")
	UStaticMesh* rdGetMeshFromSid(const FName sid);

	// Returns a SoftObjectPtr to the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSoftMeshFromSid",Keywords="rdInst Instance Handle ID Mesh"),Category="rdInst|Sids")
	TSoftObjectPtr<UStaticMesh> rdGetSoftMeshFromSid(const FName sid);

	// Returns the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMaterialsFromSid",Keywords="rdInst Instance Handle ID Materials"),Category="rdInst|Sids")
	TArray<TSoftObjectPtr<UMaterialInterface>> rdGetMaterialsFromSid(const FName sid);

	// Returns the Name of the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMeshNameFromSid",Keywords="rdInst Instance Handle ID Mesh Name"),Category="rdInst|Sids")
	FString rdGetMeshNameFromSid(const FName sid);

	// Returns the type of StaticMesh Spawn (0=Instance, 1=StaticMesh Component, 2=Componentless StaticMesh)
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSpawnTypeFromSid",Keywords="rdInst Instance Handle ID Mesh Spawn Type"),Category="rdInst|Tools")
	ErdSpawnType rdGetSpawnTypeFromSid(const FName sid);

	// Returns the Details of the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSidDetails",Keywords="rdInst Instance Handle ID Details"),Category="rdInst|Sids")
	void rdGetSidDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,TSoftObjectPtr<UStaticMesh>& mesh,TArray<TSoftObjectPtr<UMaterialInterface>>& materials,bool& bReverseCulling,TEnumAsByte<ErdCollision>& collision,float& startCull,float& endCull,int32& id,FName& groupName);

	// Returns the sid for the Actor
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetActorSid",Keywords="rdInst Actor Handle ID"),Category="rdInst|Sids")
	FName rdGetActorSid(UClass* aclass,const FString& aStr,const FName groupName=NAME_None);

	// These are the main Actor types for sid serializing, if anyone wants any other types implemented, they can be added here, or contact me
	FName rdGetActorSid(ADecalActor* actor);
	FName rdGetActorSid(ASpotLight* actor);
	FName rdGetActorSid(ARectLight* actor);
	FName rdGetActorSid(APointLight* actor);
#if ENGINE_MAJOR_VERSION>4
	FName rdGetActorSid(ALocalFogVolume* actor);
#endif
	FName rdGetActorSid(APostProcessVolume* actor);
	FName rdGetActorSid(ANiagaraActor* actor);
	FName rdGetActorSid(ATextRenderActor* actor);
	FName rdGetActorSid(AActor* actor);

	// Applies the String based Properties to the Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdApplyStringProps",Keywords="rdInst Actor Handle Actor Apply"),Category="rdInst|Sids")
	void rdApplyStringProps(AActor* actor,const FString& strProps,int32 ver);

	// Returns the Actor Class for the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetActorClassFromSid",Keywords="rdInst Actor Handle ID Class Get"),Category="rdInst|Sids")
	TSoftClassPtr<UObject> rdGetActorClassFromSid(const FName sid);

	// Returns the Actor Class for the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSidActorDetails",Keywords="rdInst Actor Handle Details Get"),Category="rdInst|Sids")
	void rdGetSidActorDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,UClass*& uclass,FString& propStr,FName& groupName);

	// Spawns an Actor specified in the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdSpawnActorFromSid",Keywords="rdInst Spawn Actor sid"),Category="rdInst|Sids")
	AActor* rdSpawnActorFromSid(const FName sid,const FTransform& transform);

	// Spawns Actors specified in the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdSpawnActorsFromSid",Keywords="rdInst Spawn Actors sid"),Category="rdInst|Sids")
	int32 rdSpawnActorsFromSid(const FName sid,const TArray<FTransform>& transforms,TArray<AActor*>& outActors);
//	int32 rdSpawnActorsFromSid(const FName sid,const TArray<TTuple<FrdProxyActorItem*,FTransform>>& transforms,TArray<AActor*>& outActors);

	void rdSpawnActorsAndProxiesFromSid(const FName sid,TArray<FAddProxyQueueItem>& addQueue);

	// Returns the sid for the DataLayer
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetDataLayerSid",Keywords="rdInst DataLayer Handle ID"),Category="rdInst|Sids")
	FName rdGetDataLayerSid(TSoftObjectPtr<UDataLayerAsset>& dl,const FName groupName);

	// Returns the DataLayer SoftPointer for the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetDataLayerFromSid",Keywords="rdInst DataLayer Handle ID Get"),Category="rdInst|Sids")
	TSoftClassPtr<UDataLayerAsset> rdGetDataLayerFromSid(const FName sid);

	// Returns the DataLayer Details for the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSidDataLayerDetails",Keywords="rdInst DataLayer Handle ID Details Get"),Category="rdInst|Sids")
	void rdGetSidDataLayerDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,TSoftClassPtr<UDataLayerAsset>& dl,FName& groupName);

	// Returns the total number of instances in all meshes stored in the recycle cache
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetTotalNumberOfInstancesInRecycleCache",Keywords="rdInst Instances Recycled Total"),Category="rdInst|Tools")
	int32 rdGetTotalNumberOfInstancesInRecycleCache();

	// Returns the number of instances stored in the recycle cache for the specified mesh
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetNumberOfInstancesInRecycleCache",Keywords="rdInst Instances Recycled"),Category="rdInst|Tools")
	int32 rdGetNumberOfInstancesInRecycleCache(UStaticMesh* mesh);

	// Returns the number of instances stored in the recycle cache for the specified mesh
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetNumberOfInstancesInRecycleCacheX",Keywords="rdInst Instances Recycled"),Category="rdInst|Tools")
	int32 rdGetNumberOfInstancesInRecycleCacheX(const FName sid);

	// Returns the World Transform of the specified Relative Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRelativeToWorld",Keywords="rdInst Relative Transform To World"),Category="rdInst|Tools")
	FTransform rdRelativeToWorld(const FTransform& relTran,const FTransform& parentTran);

	// Returns the Relative Transform of the specified World Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdWorldToRelative",Keywords="rdInst Relative Transform From World"),Category="rdInst|Tools")
	FTransform rdWorldToRelative(const FTransform& worldTran,const FTransform& parentTran);

	// Returns the World Transform of the specified Relative Transform not including the scale
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRelativeToWorldNoScale",Keywords="rdInst Relative Transform To World"),Category="rdInst|Tools")
	FTransform rdRelativeToWorldNoScale(const FTransform& relTran,const FTransform& parentTran);

	// Returns the Relative Transform of the specified World Transform not including the scale
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdWorldToRelativeNoScale",Keywords="rdInst Relative Transform From World"),Category="rdInst|Tools")
	FTransform rdWorldToRelativeNoScale(const FTransform& worldTran,const FTransform& parentTran);

// Instance Custom Data ---------------------------------------------------------------------------

	// Sets the Number of CustomData per Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetNumInstCustomData",Keywords="rdInst CustomData Set Number"),Category="rdInst|CustomData")
	void rdSetNumInstCustomData(UStaticMesh* mesh,int32 numData);

	// Sets the Number of CustomData per Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetNumInstCustomDataX",Keywords="rdInst CustomData Set Number"),Category="rdInst|CustomData")
	void rdSetNumInstCustomDataX(const FName sid,int32 numData);

	// Sets the Number of CustomData per Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetNumInstCustomDataFast",Keywords="rdInst CustomData Set Number"),Category="rdInst|CustomData")
	void rdSetNumInstCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 numData);

	// Gets a READONLY reference to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCustomDataPtr",Keywords="rdInst CustomData Get Pointer"),Category="rdInst|CustomData")
	UPARAM(DisplayName="CustomFloats") TArray<float>& rdGetCustomDataPtr(UStaticMesh* mesh,int32& numData);

	// Gets a READONLY reference to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCustomDataPtrX",Keywords="rdInst CustomData Get Pointer"),Category="rdInst|CustomData")
	UPARAM(DisplayName="CustomFloats") TArray<float>& rdGetCustomDataPtrX(const FName sid,int32& numData);

	// Gets a READONLY reference to the Per-Instance CustomData for the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCustomDataPtrFast",Keywords="rdInst CustomData Get Pointer"),Category="rdInst|CustomData")
	UPARAM(DisplayName="CustomFloats") TArray<float>& rdGetCustomDataPtrFast(UInstancedStaticMeshComponent* ismc,int32& numData);

	// Gets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol3Data",Keywords="rdInst CustomData Get Color"),Category="rdInst|CustomData")
	FLinearColor rdGetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex);

	// Gets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol3DataX",Keywords="rdInst CustomData Get Color"),Category="rdInst|CustomData")
	FLinearColor rdGetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex);

	// Gets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol3DataFast",Keywords="rdInst CustomData Get Color"),Category="rdInst|CustomData")
	FLinearColor rdGetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex);

	// Gets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol4Data",Keywords="rdInst CustomData Get Color"),Category="rdInst|CustomData")
	FLinearColor rdGetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex);

	// Gets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol4DataX",Keywords="rdInst CustomData Get Color"),Category="rdInst|CustomData")
	FLinearColor rdGetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex);

	// Gets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol4DataFast",Keywords="rdInst CustomData Get Color"),Category="rdInst|CustomData")
	FLinearColor rdGetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex);

	// Sets a value to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomData",Keywords="rdInst CustomData Set Value"),Category="rdInst|CustomData")
	void rdSetCustomData(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,float value,bool batch=false);

	// Sets a value to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomDataX",Keywords="rdInst CustomData Set Value"),Category="rdInst|CustomData")
	void rdSetCustomDataX(const FName sid,int32 instanceIndex,int32 dataIndex,float value,bool batch=false);

	// Sets a value to the Per-Instance CustomData for the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomDataFast",Keywords="rdInst CustomData Set Value"),Category="rdInst|CustomData")
	void rdSetCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,float value,bool batch=false);

	// Sets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol3Data",Keywords="rdInst CustomData Set Color"),Category="rdInst|CustomData")
	void rdSetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol3DataX",Keywords="rdInst CustomData Set Color"),Category="rdInst|CustomData")
	void rdSetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol3DataFast",Keywords="rdInst CustomData Set Color"),Category="rdInst|CustomData")
	void rdSetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol4Data",Keywords="rdInst CustomData Set Color"),Category="rdInst|CustomData")
	void rdSetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol4DataX",Keywords="rdInst CustomData Set Color"),Category="rdInst|CustomData")
	void rdSetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol4DataFast",Keywords="rdInst CustomData Set Color"),Category="rdInst|CustomData")
	void rdSetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets all the values to the Per-Instance CustomData array for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetAllCustomData",Keywords="rdInst CustomData Set All"),Category="rdInst|CustomData")
	void rdSetAllCustomData(UStaticMesh* mesh,UPARAM(ref) TArray<float>& data,bool update=true);

	// Sets all the values to the Per-Instance CustomData array for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetAllCustomDataX",Keywords="rdInst CustomData Set All"),Category="rdInst|CustomData")
	void rdSetAllCustomDataX(const FName sid,UPARAM(ref) TArray<float>& data,bool update=true);

	// Sets all the values to the Per-Instance CustomData array for the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetAllCustomDataFast",Keywords="rdInst CustomData Set All"),Category="rdInst|CustomData")
	void rdSetAllCustomDataFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) TArray<float>& data,bool update=true);

	// Updates the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateCustomData",Keywords="rdInst CustomData Update"),Category="rdInst|CustomData")
	void rdUpdateCustomData(UStaticMesh* mesh);

	// Updates the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateCustomDataX",Keywords="rdInst CustomData Update"),Category="rdInst|CustomData")
	void rdUpdateCustomDataX(const FName sid);

	// Updates the Per-Instance CustomData for the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateCustomDataFast",Keywords="rdInst CustomData Update"),Category="rdInst|CustomData")
	void rdUpdateCustomDataFast(UInstancedStaticMeshComponent* ismc);

	// Fill CustomData with values created with the parameters you pass in
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFillCustomData",Keywords="rdInst CustomData Fill"),Category="rdInst|CustomData")
	void rdFillCustomData(UStaticMesh* mesh,int32 offset=0,int32 stride=1,float baseValue=0.0f,float randomVariance=0.0f,int32 granularity=1,float inc=0.0f,float incRandomVariance=0.0f);

	// Fill CustomData with values created with the parameters you pass in
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFillCustomDataFast",Keywords="rdInst CustomData Fill"),Category="rdInst|CustomData")
	void rdFillCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 offset=0,int32 stride=1,float baseValue=0.0f,float randomVariance=0.0f,int32 granularity=1,float inc=0.0f,float incRandomVariance=0.0f);

	// Fill CustomData with values created with the parameters you pass in
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFillCustomDataX",Keywords="rdInst CustomData Fill"),Category="rdInst|CustomData")
	void rdFillCustomDataX(const FName sid,int32 offset=0,int32 stride=1,float baseValue=0.0f,float randomVariance=0.0f,int32 granularity=1,float inc=0.0f,float incRandomVariance=0.0f);

	// Set CustomData value within the specified area and ISMCs
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFillCustomDataInArea",Keywords="rdInst CustomData Fill Area"),Category="rdInst|CustomData")
	int32 rdFillCustomDataInArea(const TArray<UInstancedStaticMeshComponent*> ismcs,const FVector& loc,float radius,const FVector& box,int32 index,float value);

	// Set CustomData values within the specified area and ISMCs
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFillCustomDataInAreaMulti",Keywords="rdInst CustomData Fill Area"),Category="rdInst|CustomData")
	int32 rdFillCustomDataInAreaMulti(const TArray<UInstancedStaticMeshComponent*> ismcs,const FVector& loc,float radius,const FVector& box,int32 index,TArray<float> values);

// Spawning ---------------------------------------------------------------------------------------

	// Helper Function that Spawns an actor
	UFUNCTION(BlueprintCallable,meta=(DeterminesOutputType="actorClass",DisplayName="rdSpawnActor",Keywords="rdInst Actor Spawn"),Category="rdInst|Spawning")
	AActor* rdSpawnActor(TSubclassOf<class AActor> actorClass,const FTransform& transform,AActor* actTemplate=nullptr,bool temp=false,bool attach=true,const FName label=TEXT(""),AActor* parent=nullptr,bool deferConstruction=false,const FString& strProps=TEXT(""));

	// Helper Function that Spawns a Niagara system
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSpawnVFX",Keywords="rdInst VFX Spawn"),Category="rdInst|Spawning")
	UNiagaraComponent* rdSpawnVFX(UNiagaraSystem* fx,const FTransform& transform,AActor* parent=nullptr);

	// Helper Function that Removes a Niagara Component
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveVFX",Keywords="rdInst VFX Remove"),Category="rdInst|Spawning")
	void rdRemoveVFX(UNiagaraComponent* fxc);

// Distributed Spawning and Instancing ------------------------------------------------------------

	// Adds the array of Instances of the StaticMesh distributed to "transactionsPerTick" each tick. Returns immediately
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDistributedInstances",Keywords="rdInst Instances Add Distributed"),Category="rdInst|Distributed")
	void rdAddDistributedInstances(UStaticMesh* mesh,UPARAM(ref) const TArray<FTransform>& transforms,int32 transactionsPerTick,TArray<int32>& fillList,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the array of Instances of the StaticMesh distributed to "transactionsPerTick" each tick. Returns immediately
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDistributedInstancesX",Keywords="rdInst Instances Add Distributed"),Category="rdInst|Distributed")
	void rdAddDistributedInstancesX(const FName sid,UPARAM(ref) const TArray<FTransform>& transforms,int32 transactionsPerTick,TArray<int32>& fillList,const FrdProxySetup proxy=FrdProxySetup());

	// Spawns the array of Actors distributed to "transactionsPerTick" each tick. Returns immediately
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDistributedActors",Keywords="rdInst Actors Spawn Distributed"),Category="rdInst|Distributed")
	void rdSpawnDistributedActors(UClass* actorClass,AActor* actorTemplate,UPARAM(ref) const TArray<FTransform>& transforms,int32 transactionsPerTick,bool pooled,TArray<AActor*>& fillList,const FrdProxySetup proxy=FrdProxySetup(),const FString& strProps=TEXT(""));

// Tracing Methods --------------------------------------------------------------------------------

	// Does a SphereTrace, returning found actors and instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSphereTrace",Keywords="rdInst Sphere Trace"),Category="rdInst|Tracing")
	void rdSphereTrace(const FVector location,float radius,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors);

	// Does a BoxTrace, returning found actors and instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBoxTrace",Keywords="rdInst Box Trace"),Category="rdInst|Tracing")
	void rdBoxTrace(const FVector location,FVector halfSize,FRotator rot,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors);

	// Returns instance indexes at the specified location in the ISMC, -1 means nothing found
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstancesAtLocation",Keywords="rdInst Find Instances Location"),Category="rdInst|Tracing")
	bool rdGetInstancesAtLocation(UInstancedStaticMeshComponent* ismc,const FVector location,TArray<int32>& indexes,float tolerance=0.01f);

	// Returns the first instance index at the specified location in the ISMC, -1 means nothing found
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetFirstInstanceAtLocation",Keywords="rdInst Instance Location First"),Category="rdInst|Tracing")
	int32 rdGetFirstInstanceAtLocation(UInstancedStaticMeshComponent* ismc,const FVector location,float tolerance=0.01f);

// Actor Pooling ----------------------------------------------------------------------------------

	// Allocates "numToPool" number of the actorClass for rapid reuse
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPoolActor",Keywords="rdInst Actor Pool"),Category="rdInst|Pooling")
	void rdPoolActor(UClass* actorClass,int32 numToPool=50,bool premake=true,bool doTick=false,bool startHidden=false,bool reuse=true,bool simplePool=false,int32 growBy=0);

	// ReAllocates "numToPool" number of the actorClass
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetActorPoolSize",Keywords="rdInst Actor Pool set Size"),Category="rdInst|Pooling")
	bool rdSetActorPoolSize(UClass* actorClass,int32 numToPool,bool premake,bool doTick=false,bool startHidden=false,bool reuse=true,bool simplePool=false,int32 growBy=0);

	// Removes the Pool of Actors for the actorClass
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveActorPool",Keywords="rdInst Actor Pool"),Category="rdInst|Pooling")
	void rdRemoveActorPool(UClass* actorClass);

	void rdResetPooledActor(rdPoolData& dat,rdPoolItem& item);

	// Gets the next free actorClass in the Actor Pool, returns nullptr if none are free and reuse and growby and zero, reuses the oldest pooled actor if 'reuse' is true or grows the pool if "GrowBy" is non-zero.
	UFUNCTION(BlueprintCallable,meta=(DeterminesOutputType="actorClass",DisplayName="rdGetActorFromPool",Keywords="rdInst Actor Pool Get"),Category="rdInst|Pooling")
	AActor* rdGetActorFromPool(TSubclassOf<class AActor> actorClass,const FTransform& tran);

	// *** Replace with rdReturnActorToPool ***
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveActorFromPool",Keywords="rdInst Actor Pool Remove"),Category="rdInst|Pooling")
	void rdRemoveActorFromPool(AActor* actor);

	// Returns an Actor that was got from the pool to the Actor Pool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdReturnActorToPool",Keywords="rdInst Actor Pool Return Actor"),Category="rdInst|Pooling")
	void rdReturnActorToPool(AActor* actor);

	// Sets the rdActor to use to handle Pooled and Depooled events
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetPoolListener",Keywords="rdInst Pool Listener Set"),Category="rdInst|Pooling")
	void rdSetPoolListener(ArdActor* actor);

	// Returns True if the specified Actor is in an Actor Pool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsActorInPool",Keywords="rdInst Pool Actor In"),Category="rdInst|Pooling")
	bool rdIsActorInPool(ArdActor* actor);

	// Returns the amount of Pooled Actors for this Class
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetActorPoolSize",Keywords="rdInst Pool Actor Get"),Category="rdInst|Pooling")
	int32 rdGetActorPoolSize(TSubclassOf<class AActor> actorClass);

	// Grows the amount of Pooled Actors for this Class by the specified amount
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGrowActorPool",Keywords="rdInst Pool Actor Grow"),Category="rdInst|Pooling")
	int32 rdGrowActorPool(TSubclassOf<class AActor> actorClass,int32 growAmount);

// Instance Conversion ----------------------------------------------------------------------------

	// Converts the Instance to a ChildActorComponent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToChildActor",Keywords="rdInst Instance Convert ChildActor"),Category="rdInstActorBase")
	UChildActorComponent* rdConvertInstanceToChildActor(ArdActor* actor,UStaticMesh* mesh,int32 index);

	// Converts the Instance to a ChildActorComponent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToChildActorX",Keywords="rdInst Instance Convert ChildActor"),Category="rdInst|Conversion")
	UChildActorComponent* rdConvertInstanceToChildActorX(ArdActor* actor,const FName sid,int32 index);

	// Converts the Instance to a new StaticMesh Actor in the level, setting its mesh to the instance mesh (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToLevelActor",Keywords="rdInst Instance Convert Level Actor"),Category="rdInstActorBase")
	AActor* rdConvertInstanceToLevelActor(UStaticMesh* mesh,int32 index);

	// Converts the Instance to a new StaticMesh Actor in the level, setting its mesh to the instance mesh (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToLevelActorX",Keywords="rdInst Instance Convert Level Actor"),Category="rdInstActorBase")
	AActor* rdConvertInstanceToLevelActorX(const FName sid,int32 index);

	// Converts the Instance to a new Actor - actorClass, in the level (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActor",Keywords="rdInst Instance Convert Actor",DepreciatedFunction),Category="rdInstActorBase")
	AActor* rdConvertInstanceToActor(UStaticMesh* mesh,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor - actorClass, in the level (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorX",Keywords="rdInst Instance Convert Actor",DepreciatedFunction),Category="rdInst|Conversion")
	AActor* rdConvertInstanceToActorX(const FName sid,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorFromPool",Keywords="rdInst Instance Convert Actor Pool",DepreciatedFunction),Category="rdInstActorBase")
	AActor* rdConvertInstanceToActorFromPool(UStaticMesh* mesh,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorFromPoolX",Keywords="rdInst Instance Convert Actor Pool",DepreciatedFunction),Category="rdInstActorBase")
	AActor* rdConvertInstanceToActorFromPoolX(const FName sid,UClass* actorClass,int32 index);

	// Converts all Instances in the volume to new Actors in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstancesToActorsFromPool",Keywords="rdInst Instance Convert Actors Pool Volume",DepreciatedFunction),Category="rdInstActorBase")
	int32 rdConvertInstancesToActorsFromPool(UStaticMesh* mesh,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors);

	// Converts all Instances in the volume to new Actors in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstancesToActorsFromPoolX",Keywords="rdInst Instance Convert Actors Pool Volume",DepreciatedFunction),Category="rdInst|Conversion")
	int32 rdConvertInstancesToActorsFromPoolX(const FName sid,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors);

	void rdHarvestInstancesFromActorList(TArray<AActor*> actorList,bool justInstances,TArray<AActor*>& addedList,TMap<FName,FrdInstItemX>& scrapeMap);

// Procedural Generation --------------------------------------------------------------------------

	// Procedurally places the meshes over the volume
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdProceduralGenerate",Keywords="rdInst instance placement procedural generation"),Category="rdInstActorBase")
	int32 rdProceduralGenerate(const ArdSpawnActor* spawner);

	int32 rdProceduralGenerateBake(const ArdSpawnActor* spawner,FrdSpawnData* bakeToSpawnData);

	double getLandscapeZ(double x,double y,float radius,FHitResult& hit,bool fixSlopes=false,bool hitSM=false);

// Proxies ----------------------------------------------------------------------------------------

	// Sets the process interval (in ticks), distance to swap normal proxies, and distance to swap long-distance proxies
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetupProxies",Keywords="rdInst Proxy Setup"),Category="rdInst|Proxies")
	void rdSetupProxies(int32 processInterval,double swapDistance=1000.0f,double longDistanceSwapDistance=20000.0f,int32 idleTicks=10,int32 idleDestructionTicks=40,float velocityThreshold=0.01f,bool scanOnlyBaked=false);

	// Adds the SpawnActor to the processing lists
	void rdRegisterSpawnActor(ArdSpawnActor* spawnActor);

	// Removes the SpawnActor from the processing lists
	void rdUnregisterSpawnActor(ArdSpawnActor* spawnActor);

	// Gets any ProxySetup settings contained in the meshes UserAssetData or any Actor Tags
	void rdGetProxySettings(AActor* meshActor,UStaticMesh* mesh,FrdProxySetup& proxy);

public:
	// Adds the instance to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceProxy",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup& proxy);

	// Adds the instance to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceProxyX",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddInstanceProxyX(const FName sid,int32 index,const FrdProxySetup& proxy);

	// Adds the Actor to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddActorProxy",Keywords="rdInst Actor Proxy Add"),Category="rdInst|Proxies")
	void rdAddActorProxy(AActor* actor,const FrdProxySetup& proxy);

	// Adds the Blueprint Prefab to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddPrefabProxy",Keywords="rdInst Prefab Proxy Add"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdAddPrefabProxy(ArdActor* prefab,const FrdProxySetup& proxy) { rdAddActorProxy((AActor*)prefab,proxy); }

	// Adds the Instance to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistanceInstanceProxy",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddLongDistanceInstanceProxy(UStaticMesh* mesh,int32 index,double distance,const FrdProxySetup& proxy);

	// Adds the Instance to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistanceInstanceProxyX",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddLongDistanceInstanceProxyX(const FName sid,int32 index,double distance,const FrdProxySetup& proxy);

	// Adds the Actor to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistanceActorProxy",Keywords="rdInst Instance Actor Add"),Category="rdInst|Proxies")
	void rdAddLongDistanceActorProxy(AActor* actor,double distance,const FrdProxySetup& proxy);

	// Adds the Prefab to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistancePrefabProxy",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdAddLongDistancePrefabProxy(ArdActor* prefab,double distance,const FrdProxySetup& proxy) { rdAddLongDistanceActorProxy((AActor*)prefab,distance,proxy); }

	// Adds the instance to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionInstanceProxy",Keywords="rdInst Destruction Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddDestructionInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup& proxy);

	// Adds the instance to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionInstanceProxyX",Keywords="rdInst Destruction Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddDestructionInstanceProxyX(const FName sid,int32 index,const FrdProxySetup& proxy);

	// Adds the Actor to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionActorProxy",Keywords="rdInst Destruction Actor Proxy Add"),Category="rdInst|Proxies")
	void rdAddDestructionActorProxy(AActor* actor,const FrdProxySetup& proxy);

	// Adds the Blueprint Prefab to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionPrefabProxy",Keywords="rdInst Destruction Prefab Proxy Add"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdAddDestructionPrefabProxy(ArdActor* prefab,const FrdProxySetup& proxy) { rdAddDestructionActorProxy((AActor*)prefab,proxy); }

	// Adds all instances of the mesh to the Proxy list as a proxy Pickup
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddPickupProxy",Keywords="rdInst Instance Proxy Pickup Add"),Category="rdInst|Proxies")
	void rdAddPickupProxy(UStaticMesh* mesh,int32 index,FrdPickup pickup);

	// Adds all instances of the mesh to the Proxy list as a proxy Pickup
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddPickupProxyX",Keywords="rdInst Instance Proxy Pickup Add"),Category="rdInst|Proxies")
	void rdAddPickupProxyX(const FName sid,int32 id,FrdPickup pickup);

	// Marks the Proxy to be set to the Destroyed Instance/Prefab when swapped out
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdMarkProxyAsDestroyed",Keywords="rdInst Proxy Destroyed Mark"),Category="rdInst|Proxies")
	void rdMarkProxyAsDestroyed(AActor* proxy,const FTransform& offset=FTransform());

	// Sets the Proxies state to Destroyed
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetProxyToDestroyed",Keywords="rdInst Proxy Destroyed Set"),Category="rdInst|Proxies")
	void rdSetProxyToDestroyed(AActor* proxy,const FTransform& offset=FTransform());

	// Removes the Proxy Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByInstance",Keywords="rdInst Instance Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByInstance(UStaticMesh* mesh,int32 index);

	// Removes the Proxy Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByInstanceX",Keywords="rdInst Instance Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByInstanceX(const FName sid,int32 index);

	// Removes the Proxy from the passed in proxy actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByProxy",Keywords="rdInst Actor Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByProxy(AActor* proxy,bool bKeepPinned=false);

	// Removes the Proxy from the passed in proxy actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByDataLayer",Keywords="rdInst DataLayer Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByDataLayer(TSoftObjectPtr<UDataLayerAsset> dla);

	// Removes the Proxy from the passed in proxy ISM
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByProxyISM",Keywords="rdInst Instance Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByProxyISM(UStaticMesh* mesh,int32 index);

	// Removes the Proxy from the passed in proxy ISM
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByProxyISMX",Keywords="rdInst Instance Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByProxyISMX(const FName sid,int32 index);

	// Removes the Proxy Prefab
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByPrefab",Keywords="rdInst Prefab Proxy Add"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdRemoveProxyByPrefab(ArdActor* prefab) { rdRemoveProxyByActor((AActor*)prefab); }

	// Removes the Proxy Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByActor",Keywords="rdInst Actor Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByActor(AActor* actor);

	// Removes the LongDistance Proxy Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveLongDistanceProxyByProxy",Keywords="rdInst Long Distance Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveLongDistanceProxyByProxy(AActor* proxy);

	// Removes the Pickup Proxy scanning for the instances (leaves the instances there)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemovePickupProxies",Keywords="rdInst Proxy Pickup Remove Scanning"),Category="rdInst|Proxies")
	void rdRemovePickupProxies(int32 id);

	// Spawns a Pickup Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSpawnPickup",Keywords="rdInst Proxy Pickup Spawn"),Category="rdInst|Proxies")
	int32 rdSpawnPickup(int32 id,const FTransform& transform);

	// Picks up a Pickup Instance (removes from the level) and broadcasts its PickUp event
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPickupPickup",Keywords="rdInst Proxy Pickup Pickup"),Category="rdInst|Proxies")
	void rdPickupPickup(int32 id,int32 instance);

	// Picks Up the Currently Highlighted Pickup and broadcasts its PickUp event
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPickupCurrent",Keywords="rdInst Proxy Pickup Current"),Category="rdInst|Proxies")
	void rdPickupCurrent();

	// Removes the Pickup Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemovePickup",Keywords="rdInst Proxy Pickup Remove"),Category="rdInst|Proxies")
	void rdRemovePickup(int32 id,int32 instance);

	// Removes all Instance and Prefab Proxies
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveAllProxies",Keywords="rdInst Proxies Remove All"),Category="rdInst|Proxies")
	void rdRemoveAllProxies();

	// Swaps the Mesh Instance over to its ProxyActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapInstanceToProxyActor",Keywords="rdInst Instance Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapInstanceToProxyActor(UStaticMesh* mesh,int32 index,const FTransform& transform,bool forImpact=false,bool forLongDistance=false);

	// Swaps the Mesh Instance over to its ProxyActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapInstanceToProxyActorX",Keywords="rdInst Instance Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapInstanceToProxyActorX(const FName sid,int32 index,const FTransform& transform,bool forImpact=false,bool forLongDistance=false);

	// Swaps the Mesh Instance over to its ProxyActor, also passing a HitResult (when swapping from impacts)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapInstanceToProxyActorWithHitX",Keywords="rdInst Instance Proxy Swap Hit"),Category="rdInst|Proxies")
	void rdSwapInstanceToProxyActorWithHitX(const FName sid,int32 index,const FTransform& transform,FHitResult& hitResult);

	// Swaps the Actor over to its ProxyActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapActorToProxyActor",Keywords="rdInst Actor Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapActorToProxyActor(AActor* actor,int32 index,bool forImpact=false,bool forLongDistance=false);

	// Swaps the Prefab over to its ProxyActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapPrefabToProxyActor",Keywords="rdInst Prefab Proxy Swap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdSwapPrefabToProxyActor(ArdActor* prefab,int32 index,bool forImpact=false,bool forLongDistance=false) { rdSwapActorToProxyActor((AActor*)prefab,index,forImpact,forLongDistance); }

	// Swaps the ProxyActor back to its Instanced Mesh (Queues the Swap to happen in another thread)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapProxyActorToInstance",Keywords="rdInst Instance Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapProxyActorToInstance(AActor* actor);

	// processes the Proxy Swap-Ins from the scan thread
	void processSwapToProxyActor(const FrdProxyActorItem& proxy);

	// processes the Proxy Swap-Outs from the scan thread
	void processSwapOutProxyActor(const FrdProxyActorItem& proxy);

	// Swaps the ProxyActor back to its Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapProxyActorToActor",Keywords="rdInst Actor Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapProxyActorToActor(AActor* actor);

	// Swaps the ProxyActor back to its Prefab
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapProxyActorToPrefab",Keywords="rdInst Prefab Proxy Swap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdSwapProxyActorToPrefab(AActor* actor) { rdSwapProxyActorToActor(actor); }

	// Swaps any Actors/Instances to ProxyActors around the impact point. A distance of 0.0 uses the default rdInstSettings value. Note that this fires rdOnProxyDamaged events for the affected proxies. Don't try referencing the proxies straight after this function.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdProcessImpactProxies",Keywords="rdInst Proxy Impact Process"),Category="rdInst|Proxies")
	void rdProcessImpactProxies(const AActor* caller,const FVector& loc,const FHitResult& hit,float distance=1000.0f,const FVector impulse=FVector(0,0,0),USceneComponent* comp=nullptr,int32 index=-1);

	// Spawn the assets from the rdSpawner struct
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSpawn",Keywords="rdInst Spawn"),Category="rdInst|Spawn")
	void rdSpawn(const ArdSpawnActor* spawner,bool bake=false);

	void rdSpawnNewlyStreamed(const ArdSpawnActor* spawner);

	void SpawnInstances(ArdSpawnActor* ssa,UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData);
	void SpawnActors(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData);
	void SpawnComponentMeshes(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData);
	void SpawnNiagaraMeshes(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData);
	UNiagaraSystem* ReplaceNiagaraMesh(UNiagaraSystem* existingSys,UStaticMesh* mesh);
	void SpawnRdMeshlets(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData);
	void SpawnCompless(ArdSpawnActor* sa,FrdBakedSpawnObjects& bd,const FrdSpawnData* spawnData);

	void rdSpawnData(ArdSpawnActor* ssa,const FrdSpawnData* spawnData,bool bake=false,bool justNewlyStreamed=false);

	// Returns a reference to the InstancedMeshToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetInstancedMeshToProxyMap"),Category="rdInst|Proxies")
	TMap<FName,FrdProxyItem>& rdGetInstancedMeshToProxyMap() { return instancedMeshToProxyMap; }

	// Returns a reference to the InstancesToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetInstancesToProxyMap"),Category="rdInst|Proxies")
	TMap<FName,FrdProxyInstanceItems>& rdGetInstancesToProxyMap() { return instancesToProxyMap; }

	// Returns a reference to the ActorsToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetActorsToProxyMap"),Category="rdInst|Proxies")
	TMap<AActor*,FrdProxyItem>& rdGetActorsToProxyMap() { return actorsToProxyMap; }

	// Returns a reference to the PrefabsToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetPrefabsToProxyMap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	TMap<ArdActor*,FrdProxyItem>& rdGetPrefabsToProxyMap() { static TMap<ArdActor*,FrdProxyItem> em; return em; }

	// Returns a reference to the LongDistanceInstancesToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetLongDistanceInstancesToProxyMap"),Category="rdInst|Proxies")
	TMap<FName,FrdProxyGridArray>& rdGetLongDistanceInstancesToProxyMap() { return longDistanceInstancesToProxyMap; }

	// Returns a reference to the LongDistanceActorsToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetLongDistanceActorsToProxyMap"),Category="rdInst|Proxies")
	TMap<AActor*,FrdProxyItem>& rdGetLongDistanceActorsToProxyMap() { return longDistanceActorsToProxyMap; }

	// Returns a reference to the LongDistancePrefabsToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetLongDistancePrefabsToProxyMap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	TMap<ArdActor*,FrdProxyGridArray>& rdGetLongDistancePrefabsToProxyMap() { static TMap<ArdActor*,FrdProxyGridArray> emp; return emp; }

	// Returns a reference to the ProxyActor Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetProxyActorMap"),Category="rdInst|Proxies")
	TMap<AActor*,FrdProxyActorItem>&  rdGetProxyActorMap() { return proxyActorMap; }

	// Make a rdProxySetup structure for an Actor Proxy
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdMakeActorProxySetup"),Category="rdInst|Proxies")
	FrdProxySetup rdMakeActorProxySetup(UClass* proxyActor,bool bDontHide=false,bool bDontRemove=false,float proxyPhysicsTimeout=30.0f,bool bPooled=false,bool bSimplePool=false,int32 pooledAmount=20,UrdStateDataAsset* savedState=nullptr,bool bCallSwapEvent=false);

	// Make a rdProxySetup structure for a DataLayer Proxy
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdMakeDataLayerProxySetup"),Category="rdInst|Proxies")
	FrdProxySetup rdMakeDataLayerProxySetup(TSoftObjectPtr<UDataLayerAsset> proxyDataLayer,bool bDontHide=false,bool bDontRemove=false,UrdStateDataAsset* savedState=nullptr,bool bCallSwapEvent=false);

	// Make a rdProxySetup structure for a StaticMesh Proxy
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdMakeStaticMeshProxySetup"),Category="rdInst|Proxies")
	FrdProxySetup rdMakeStaticMeshProxySetup(UStaticMesh* proxyMesh,TArray<TSoftObjectPtr<UMaterialInterface>> materials,bool reverseCulling=false,TEnumAsByte<ErdCollision> collision=ErdCollision::UseDefault,bool bDontHide=false,bool bDontRemove=false,UrdStateDataAsset* savedState=nullptr,bool bCallSwapEvent=false);

	// Add Destruction settings to a rdProxySetup structure 
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdAddDestructionToProxySetup"),Category="rdInst|Proxies",meta=(IgnoreForMemberInitializationTest))
	FrdProxySetup rdAddDestructionToProxySetup(const FrdProxySetup& inProxySetup,UStaticMesh* destroyedMesh=nullptr,UClass* destroyedPrefab=nullptr,const FTransform& destroyedOffset=FTransform());

	void rdAutoSetupProxies(UStaticMesh* mesh=nullptr);
	void rdAutoSetupProxiesX(const FName sid=FName());

	FrdProxyItem* rdGetProxyItemFromInstance(const FName sid,int32 index);
	FrdProxyItem* rdGetProxyItemFromActor(const AActor* prefab);
	FrdProxyItem* rdGetProxyItem(FrdProxyActorItem* proxyActorItem);
	void AddProxyForInstance(const FName sid,const FrdProxySetup& proxy,const int32 instIndex);
	void AddProxiesForInstances(const FName sid,const FrdProxySetup& proxy,const TArray<int32>& instIndexList);
	void AddProxyForActor(AActor* actor,const FrdProxySetup& proxy);

// Spline Utilities -------------------------------------------------------------------------------

	// Splits the Spline into X subsplines
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSplitSplineInto",Keywords="rdInst Spline Split"),Category="rdInst|Splines")
	UPARAM(DisplayName="SubSplines")TArray<AActor*> rdSplitSplineInto(AActor* actor,USplineComponent* spline,int32 num,bool callBPfunctions=false);

	// Get the Spline Points Position
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSplinePointPosition",Keywords="rdInst Spline Point Location"),Category="rdInst|Splines")
	void rdGetSplinePointPosition(int32 point,FVector& loc,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local);

	// Set the Spline Points Position
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetSplinePointPosition",Keywords="rdInst Spline Point Location"),Category="rdInst|Splines")
	TArray<AActor*> rdSetSplinePointPosition(int32 point,const FVector& loc,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local,bool updateSpline=true);

	// Get the Spline Points Rotation
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSplinePointRotation",Keywords="rdInst Spline Point Rotation"),Category="rdInst|Splines")
	void rdGetSplinePointRotation(int32 point,FRotator& rot,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local);

	// Set the Spline Points Rotation
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetSplinePointRotation",Keywords="rdInst Spline Point Rotation"),Category="rdInst|Splines")
	TArray<AActor*> rdSetSplinePointRotation(int32 point,const FRotator& rot,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local,bool updateSpline=true);

	// Get the Spline Points Scale
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSplinePointScale",Keywords="rdInst Spline Point Scale"),Category="rdInst|Splines")
	void rdGetSplinePointScale(int32 point,FVector& scale,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr);

	// Set the Spline Points Scale
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetSplinePointScale",Keywords="rdInst Spline Point Scale"),Category="rdInst|Splines")
	TArray<AActor*> rdSetSplinePointScale(int32 point,const FVector& scale,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,bool updateSpline=true);

	// Get the Spline Points Tangent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSplinePointTangent",Keywords="rdInst Spline Point Tangent"),Category="rdInst|Splines")
	void rdGetSplinePointTangent(int32 point,FVector& leaveTangent,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local);

	// Set the Spline Points Tangent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetSplinePointTangent",Keywords="rdInst Spline Point Tangent"),Category="rdInst|Splines")
	UPARAM(DisplayName="ModdedSubSplines")TArray<AActor*> rdSetSplinePointTangent(int32 point,const FVector& leaveTangent,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local,bool updateSpline=true);

	// Gets the SubSpline from the point
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSubSplineAtPoint",Keywords="rdInst SubSpline Point Get"),Category="rdInst|Splines")
	UPARAM(DisplayName="SplineActor")AActor* rdGetSubSplineAtPoint(int32 point,UPARAM(ref) const TArray<AActor*>& subsplines,int32& subPoint);

	// Finds the closest point on a SubSpline from the location
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFindClosestSplinePoint",Keywords="rdInst Spline Point Closest Find"),Category="rdInst|Splines")
	UPARAM(DisplayName="SplineActor")AActor* rdFindClosestSplinePoint(const FVector& loc,UPARAM(ref) const TArray<AActor*>& subsplines,int32& localPoint,int32& overallPoint,float& distance,bool& found);

	// Copy the Spline data from one to another
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdCopySpline",Keywords="rdInst Spline Copy"),Category="rdInst|Splines")
	void rdCopySpline(USplineComponent* spline1,USplineComponent* spline2,int32 start,int32 end);
	
	// Trim the Spline Points
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdTrimSpline",Keywords="rdInst Spline Trim"),Category="rdInst|Splines")
	void rdTrimSpline(USplineComponent* spline,int32 start,int32 len);

	// Joins the Splines into one
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdJoinSplines",Keywords="rdInst Splines Join"),Category="rdInst|Splines")
	void rdJoinSplines(AActor* actor,bool callBPfunctions=false);

	// Joins the SubSplines attached to the actor into the actors spline
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdMergeActorsSubSplines",Keywords="rdInst SubSplines Merge"),Category="rdInst|Splines")
	void rdMergeActorsSubSplines(AActor* actor,bool callBPfunctions=false);

	// Populate the Spline with SplineMeshComponents from a single mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPopulateSplineFast",Keywords="rdInst Spline Populate Fast SplineMesh"),Category="rdInst|Splines")
	int32 rdPopulateSplineFast(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,UStaticMesh* mesh,const FTransform offset=FTransform(),float gap=0.0f,UMaterialInterface* mat=nullptr,const TEnumAsByte<ESplineMeshAxis::Type> axis=ESplineMeshAxis::X,bool useCollision=false,bool useRoll=false);

	// Populate the Spline with SplineMeshComponents from an array of rdSplinePopulateData structs
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPopulateSpline",Keywords="rdInst Spline Populate SplineMesh"),Category="rdInst|Splines")
	int32 rdPopulateSpline(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,UPARAM(ref) const TArray<FrdSplinePopulateData>& data,TArray<FrdSplineInstanceData>& instData,bool random=false,bool useCollision=false,bool useRoll=false);

	// Remove SplineMeshComponents attached to this spline
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveSplineSMCs",Keywords="rdInst Spline Remove SplineMesh"),Category="rdInst|Splines")
	void rdRemoveSplineSMCs(AActor* actor,USplineComponent* spline);

#ifdef includeProceduralMeshStuff
	// Populate the Spline with ProceduralMeshComponents from a single mesh
	//UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPopulateSplinePMCFast",Keywords="rdInst Spline Populate Fast ProceduralMesh"),Category="rdInst|Splines")
	int32 rdPopulateSplinePMCFast(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,UStaticMesh* mesh,UMaterialInterface* mat=nullptr,const FVector scale=FVector(1,1,1),const TEnumAsByte<ESplineMeshAxis::Type> axis=ESplineMeshAxis::X);

	// Populate the Spline with ProceduralMeshComponents from an array of rdSplinePopulateData structs
	//UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPopulateSplinePMC",Keywords="rdInst Spline Populate ProceduralMesh"),Category="rdInst|Splines")
	int32 rdPopulateSplinePMC(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,const TArray<FrdSplinePopulateData>& data,TArray<FrdSplineInstanceData>& instData,bool random=false);

	// Remove ProceduralMeshComponents attached to this spline
	//UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveSplinePMCs",Keywords="rdInst Spline Remove ProceduralMesh"),Category="rdInst|Splines")
	void rdRemoveSplinePMCs(AActor* actor,USplineComponent* spline);
#endif

	UFUNCTION(BlueprintPure, meta=(DisplayName="splineInstData==splineInstData",CompactNodeTitle="==",Keywords="Equal",CommutativeAssociativeBinaryOperator="true"),Category=FrdSplineInstanceData)
	static bool equalEqual(const FrdSplineInstanceData& A,const FrdSplineInstanceData& B) { return A==B; }

	UFUNCTION(BlueprintPure, meta=(DisplayName="splineInstData!=splineInstData",CompactNodeTitle="!=",Keywords="NotEqual",CommutativeAssociativeBinaryOperator="true"),Category=FrdSplineInstanceData)
	static bool notEqual(const FrdSplineInstanceData& A,const FrdSplineInstanceData& B) { return A!=B; }


	FSplinePoint		FillSplinePoint(USplineComponent* spline,int32 point,float startKey=0.0f);
	float				BuildBPSplineSection(const AActor* actor,float startOffset,int32& index);
	AActor*				DuplicateActor(const AActor* actor);
	int32				GetSubSplineIndex(const AActor* actor);
	USplineComponent*	GetSpline(const AActor* actor);
	void				CopySplinePoint(USplineComponent* s1,int32 i1,USplineComponent* s2,int32 i2,FVector& loc,FRotator& rot,FVector& scale);

// Landscape Tools --------------------------------------------------------------------------------

	// Returns the height of the landscape using the heightmap
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetLandscapeZ",Keywords="rdInst Landscape Z"),Category="rdInst|Landscape")
	double rdGetLandscapeZ(double x,double y);

	// Paints the Landscape Paint Layer at the specified position, size and stregth
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPaintLandscape",Keywords="rdInst Landscape Paint"),Category="rdInst|Landscape")
	void rdPaintLandscape(ULandscapeLayerInfoObject* paintLayer,TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle=0.0f,float strength=1.0f,float falloff=0.3f,USplineComponent* spline=nullptr,const FString& layer=TEXT(""));

	// Flattens the landscape at the location/size by strength with falloff
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFlattenLandscapeArea",Keywords="rdInst Landscape Flatten"),Category="rdInst|Landscape")
	void rdFlattenLandscapeArea(TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle=0.0f,float strength=1.0f,float falloff=0.3f,USplineComponent* spline=nullptr,const FString& layer=TEXT(""));

	// Stamps the texture onto the landscape at the location/size by strength with falloff
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdStampLandscapeArea",Keywords="rdInst Landscape Stamp"),Category="rdInst|Landscape")
	void rdStampLandscapeArea(UTexture2D* stamp,float x,float y,float width,float height,float strength=1.0f,const FString& layer=TEXT(""));

	// Creates a Texture of the specified shape and size/rotation
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdCreateTextureForShape",Keywords="rdInst Landscape Stamp"),Category="rdInst|Landscape")
	UTexture2D* rdCreateTextureForShape(TEnumAsByte<rdLandscapeShape> shape,float x,float y,float width,float height,float angle=0.0f,float strength=1.0f,float falloff=0.3f,USplineComponent* spline=nullptr);

// Utilities --------------------------------------------------------------------------------------

	// Converts the Instances, SMs and ChildActorComponents with SMs contained by this actor to Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdHarvestSMs",Keywords="rdInst Harvest SMs"),Category="rdInst|Harvest")
	void rdHarvestSMs(AActor* actor,bool hide,int32& numConverted,int32& numStandard,TMap<FName,int32>& sidMap);

	// Removes the Instances harvested from this actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveHarvestedInstances",Keywords="rdInst Remove Harvest ISMs"),Category="rdInst|Harvest")
	void rdRemoveHarvestedInstances(AActor* actor);

	// Set to True to Enable runtime ConvertToISMs for Actors and blueprints
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetupAutoInst",Keywords="rdInst Convert ISM Play"),Category="rdInst|Utilities")
	void rdSetupAutoInst(bool autoInst=true,bool autoFromTags=false,bool autoMeshActors=true,bool autoBPs=true);

	FName rdGetAutoInstSidIncludingCullVolumes(UStaticMeshComponent* smc);
	void rdGetStaticMeshCounts(AActor* actor,TMap<FName,int32>& sidMap);

	// Converts Actors in the level to ISMs and Hides the Actors
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertAutoISMs",Keywords="rdInst Convert ISM Play"),Category="rdInst|Utilities")
	void rdConvertAutoISMs();

	// Reverts Actors in the level from ISMs and Shows the Actors
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRevertFromAutoISMs",Keywords="rdInst Revert Auto ISM"),Category="rdInst|Utilities")
	void rdRevertFromAutoISMs();

	// Helper Function that Sets the Label (Editor only) of the Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetActorLabel",Keywords="rdInst Actor Label Set Editor Only"),Category="rdInst|Utilities")
	void rdSetActorLabel(AActor* actor,const FName label);

	// Returns the number of assets compiling or waiting for compile
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetNumAssetsCompiling",Keywords="rdInst Assets Compile"),Category="rdInst|Utilities")
	int32 rdGetNumAssetsCompiling();

	// waits for all assets to finish compiling
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdWaitForCompiling",Keywords="rdInst Assets Compile"),Category="rdInst|Utilities")
	void rdWaitForCompiling();

	// Submits all materials for compile
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSubmitMaterialsForCompile",Keywords="rdInst Assets Materials Compile"),Category="rdInst|Utilities")
	void rdSubmitMaterialsForCompile();

	// Is Editor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsEditor",Keywords="rdInst Is Editor"),Category="rdInst|Utilities")
	bool rdIsEditor();

	// Is Playing
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsPlaying",Keywords="rdInst Is Playing"),Category="rdInst|Utilities")
	bool rdIsPlaying();

	// Get Landscape Bounds
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetLandscapeBounds",Keywords="rdInst Landscape Bounds"),Category="rdInst|Utilities")
	FBox rdGetLandscapeBounds(ALandscapeProxy* landscape);

	// Get Current average Scalabity (0-4)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCurrentScalability",Keywords="rdInst Scalability Settings Get"),Category="rdInst|Utilities")
	rdScalabilityScale rdGetCurrentScalability();

	// Get Current Scalabity for specified area (0-4)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCurrentScalabilityFor",Keywords="rdInst Scalability Settings Get"),Category="rdInst|Utilities")
	rdScalabilityScale rdGetCurrentScalabilityFor(rdScalabilityType stype);

	// Get Current Resolution Scale (0-100)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCurrentResolutionScale",Keywords="rdInst Scalability Settings Get"),Category="rdInst|Utilities")
	float rdResolutionScale();

	// Get Scalability Benchmark Results for CPU and GPU
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetScalabilityBenchmarks",Keywords="rdInst Scalability Settings Get"),Category="rdInst|Utilities")
	bool rdGetScalabilityBenchmarks(float& bmCpu,float& bmGpu);

	// Create a temporary BakedDataAsset
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdCreateTempBakedDataAsset",Keywords="rdInst BakedDataAsset Temp Create"),Category="rdInst|Utilities")
	UrdBakedDataAsset* rdCreateTempBakedDataAsset();

	// Load (or optionally create if not found or null) the BakedDataAsset from disk
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdLoadBakedDataAsset",Keywords="rdInst BakedDataAsset Load"),Category="rdInst|Utilities")
	UrdBakedDataAsset* rdLoadBakedDataAsset(TSoftObjectPtr<UrdBakedDataAsset>& data,bool create=false);

	// Adds SpawnData with the baked transforms of the volume to the proxy scan list, useful for turning off collision and using a proxy collision when close
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBakeVolumeTransformsForProxies",Keywords="rdInst Bake Volume Proxies"),Category="rdInst|Utilities")
	UPARAM(DisplayName="NumProxies")int32 rdBakeVolumeTransformsForProxies(AActor* volume,TArray<FrdSpawnData>& bakedObjectList,int32 rows=1,int32 cols=1,bool storeTransforms=false);

	// Adds SpawnData with the baked transforms of the volume to the proxy scan list, useful for turning off collision and using a proxy collision when close
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBakeVolumeTransformsForProxiesX",Keywords="rdInst Bake Volume Proxies"),Category="rdInst|Utilities")
	UPARAM(DisplayName="NumProxies")int32 rdBakeVolumeTransformsForProxiesX(AActor* volume,UPARAM(ref) const TArray<FName>& sids,TArray<FrdSpawnData>& bakedObjectList,int32 rows=1,int32 cols=1,bool storeTransforms=false);

	// Removes the SpawnData associated with this volume from the proxy scan list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveVolumeTransformsForProxies",Keywords="rdInst Remove Volume Proxies"),Category="rdInst|Utilities")
	void rdRemoveVolumeTransformsForProxies(AActor* volume);

	// Creates a SpawnActor from the passed in volume
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdCreateSpawnActorFromVolume",Keywords="rdInst Create SpawnActor Volume"),Category="rdInst|Utilities")
	ArdSpawnActor* rdCreateSpawnActorFromVolume(AActor* volume,double distance,int32 distFrames,bool spatial,TEnumAsByte<rdSpawnMode> spawnMode=rdSpawnMode::RDSPAWNMODE_ALL);

	void rdAddSpawnSidTransform(const FName sid,const FTransform& stransform,FrdSpawnData& sd,TArray<FrdBakedSpawnObjects>& bd,UrdBakedDataAsset* bda,const FTransform& transform,FrdProxySetup* proxy=nullptr,int32 gridW=16,int32 gridH=16);
	void rdAddSpawnSidTransforms(const FName sid,TArray<FTransform>& transforms,FrdSpawnData& sd,TArray<FrdBakedSpawnObjects>& bd,UrdBakedDataAsset* bda,const FTransform& transform,FrdProxySetup* proxy=nullptr,int32 gridW=16,int32 gridH=16);

	FrdBakedSpawnObjects* AddNewPlacementData(FrdSpawnData& sd,UrdBakedDataAsset* bda,const FName sid,FrdProxySetup* proxy=nullptr,int32 gridW=16,int32 gridH=16);

	// Updates the NavMesh with the rdInst Instances
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdUpdateNav",Keywords="rdInst Nav Update"),Category="rdInst|Utilities")
	void rdUpdateNav();

	// Creates a SpawnActor from the passed in objects
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdCreateSpawnActorFromObjectList",Keywords="rdInst Create SpawnActor"),Category="rdInst|Utilities",meta=(IgnoreForMemberInitializationTest))
	ArdSpawnActor* rdCreateSpawnActorFromObjectList(UrdSetObjectsList* instances,FTransform& transform,const FString& filename=TEXT(""),double distance=0.0f,int32 distFrames=0,bool spatial=true,TEnumAsByte<rdSpawnMode> spawnMode=rdSpawnMode::RDSPAWNMODE_ALL,bool harvestInstances=false,int32 spawnActorGridX=16,int32 spawnActorGridY=16);

	// Fills the Location and Rotation vectors with the editors current camera position
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetEditorCameraPos",Keywords="Editor Camera Position"),Category="rdInst|Utilities")
	void rdGetEditorCameraPos(FVector& loc,FRotator& rot);

	// Returns the currently controlled Pawn. If this returns null, it means a multiplayer pawn is waiting to spawn
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetControlledPawn",Keywords="Pawn Controlled Get"),Category="rdInst|Utilities")
	APawn* rdGetControlledPawn();

	// Draws a line from start to end in col for duration
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdDrawLine",Keywords="Debug Line Draw"),Category="rdInst|Utilities")
	void	rdDrawLine(const FVector& start,const FVector& end,const FColor& col,float duration=10000.0f);

	// Shifts the Origin of the rdInst Base Actor. Useful to remove jitter from large distances away. Call when transitioning to a new part of your world.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdMoveBaseActor",Keywords="Base Actor Shift"),Category="rdInst|Utilities")
	void rdMoveBaseActor(const FVector& location,const FRotator& rot);

	// Shifts the Origin of the rdInst Base Actor and updates existing instances to be where they were. Useful to remove jitter from large distances away. Call when transitioning to a new part of your world.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdReseatBaseActor",Keywords="Base Actor Reseat Shift"),Category="rdInst|Utilities")
	void rdReseatBaseActor(const FVector& location,const FRotator& rot);

	// Returns true is the mesh is Nanite
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdMeshIsNaniteX",Keywords="Mesh Nanite"),Category="rdInst|Utilities")
	bool rdMeshIsNaniteX(const FName sid);

	// Returns true is the mesh is Nanite
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdMeshIsNanite",Keywords="Mesh Nanite"),Category="rdInst|Utilities")
	bool rdMeshIsNanite(const UStaticMesh* mesh);

	// Builds an internal list of lights in the level wanting to swap to VSM shadow rendering in proximity. Call this if you add/remove lights in the level at runtime
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildMegaLightList",Keywords="MegaLight Build List"),Category="rdInst|Utilities")
	void rdBuildMegaLightList();

// Data -------------------------------------------------------------------------------------------

	// Filled by rdCalcInstanceCounts, contains the total number of managed instances rdInst has
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Managed Instances"))
	int32 numInstances=0;

	// Filled by rdCalcInstanceCounts, contains the total number of instances rdInst has both managed and raw
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Raw Instances"))
	int32 numRawInstances=0;

	// Filled by rdCalcInstanceCounts, contains the total number of managed components rdInst is using
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Managed ISMCs/HISMCs"))
	int32 numComponents=0;

	// Filled by rdCalcInstanceCounts, contains the total number of components rdInst is using both managed and raw
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Raw ISMCs/HISMCs"))
	int32 numRawComponents=0;

	// Filled by rdCalcInstanceCounts, contains the total number of instances in recycled list
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Instances in Recycle Lists"))
	int32 numRecycleInstances=0;

	// Filled by rdCalcInstanceCounts, contains the total number of components in recycled list
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Components in Recycle Lists"))
	int32 numRecycleComponents=0;

	// Filled by rdCalcInstanceCounts, contains the total number of instances used by the AutoInstancing
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Instances used by AutoInstancing"))
	int32 numAutoInstances=0;

	// Filled by rdCalcInstanceCounts, contains the total number of components used by the AutoInstancing
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="rdInst Stats",meta=(DisplayName="Number of Components used by AutoInstancing"))
	int32 numAutoComponents=0;

	// Bool to use HISMs rather than ISMs - defaults to True, but will change once UE4.27 is obsolete
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	bool bUseHISMs=false;

	// Bool to use ISMs rather than HISMs when the mesh is Nanite (UE5.0 up)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	bool bUseISMsForNanite=true;

	// Bool to recycle Instances rather than remove them
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	bool bRecycleInstances=true;

	// Bool to use WorldSpace transforms for Instancing
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	bool bWorldSpaceInstancing=false;

	// When True, MegaLights using Hardware RT swap to using VSM for high quality and WPO motion
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	bool bSwapMegaLightsToVSMinProximity=false;

	// Set to True to only parse Lights with the "MegaLightSwap" tag
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	bool bOnlySwapMegaLightsWithTag=true;

	// The maximum distance at which everything outside gets culled
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	float swapMegaLightDistance=1000.0f;


	// The maximum distance at which everything outside gets culled
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings")
	float maxCullDistance=400000.0f;

	// How often the scan for swapping proxies should happen (tick count with movement) -1 turns it off
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	int32 proxyProcessInterval=-1;

	// Default Distance to swap ShortDistance Proxy Actors (can be overidden with proxy 'distance' property)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	double proxySwapDistance=1000.0f;

	// Default Distance to swap LongDistance Proxy Actors (can be overidden with proxy 'distance' property)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	double proxyLongDistanceSwapDistance=20000.0f;

	// Distance to swap proxy actors
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	double proxyImpactScanDistance=600.0f;

	// List of Actors to ignore when doing the proxy scans
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	TArray<AActor*> proxyScanIgnoreActors;

	// Type of override for the runtime population system in rdSpawnActors
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	TEnumAsByte<rdPopulationOverride> runPopOverride;

	// How long to leave the Interactive Actor in place after leaving the radius (in ticks)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	int32 proxyIdleTicks=10;

	// How long to leave the Destruction Actor in place after it's been swapped (in ticks)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	int32 proxyDestructionIdleTicks=200;

	// Only swap back to ISM once physics velocity is below this threshhold or the proxyPhysicsTimout is reached
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings")
	float proxyCutOffPhysicsVelocity=0.01f;

	// When true, only baked proxies are scanned for swapping
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Only Scan Baked Proxies"))
	bool bProxyOnlyScanBaked=false;

	// Texture with landscape Heightmap/Mask for Cluster Spawns
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Height/Mask Map"))
	UTexture* heightMaskMap=nullptr;

	// Fraction of the CPU size to use for the GPU map (only with rdMeshlets)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Height/Mask Map GPU size"))
	float heightMaskGPUsize=1.0f;

	// When true, only baked proxies are scanned for swapping
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Spawn Proxies on Server"))
	bool bSpawnProxysOnServer=false;

	// When true, rdInst AutoInstancing is activated
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Enable Auto-Instancing"))
	bool ConvertToISMAtPlay=false;

	// When true, rdInst sets the cull distance from any CullVolumes present in the level (each treated as infinite bounds)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Use CullDistanceVolume Cull Values"))
	bool bAutoInstanceSetCullFromVolumes=false;

	// When true, Only Actors with the Tag 'AutoInstance' are instanced
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Only Convert with Tag"))
	bool ConvertToISM_FromTags=false;

	// When true, StaticMesh Actors are included in the AutoInstancing (more for UE4)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Include StaticMesh Actors"))
	bool ConvertToISM_IncMeshActors=true;

	// When true, Blueprints and PackedLevelActors are included in the AutoInstancing
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Include Blueprints"))
	bool ConvertToISM_IncBPs=true;

	// When true, Foliage is included in the AutoInstancing
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Include Foliage"))
	bool ConvertToISM_IncFoliage=false;

	// Mesh Count at which to instance with AutoInstancing
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Instance Threshold"))
	int32 ConvertToISM_threshold=3;

	// How to convert StaticMesh Actors with Physics Enabled
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Physics Conversion"))
	TEnumAsByte<rdAutoInstPhysics> ConvertToISM_PhysicConversion=RDAUTOINST_PHYSICS_LEAVE;

	// How to convert StaticMesh Actors with Destruction Enabled
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Destruction Conversion"))
	TEnumAsByte<rdAutoInstDestruction> ConvertToISM_DestructionConversion=RDAUTOINST_DESTRUCTION_LEAVE;

	// When true, Processes the Actors Asset Proxy Settings, set using the rdInst ProxySettings window
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Process Asset Proxy Settings"))
	bool ConvertToISM_ProcessAssetProxySettings=false;

	// Set to true to include the objects Randomization Settings, defaulting from the Asset and overridable from the Actor
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Process Randomization Settings"))
	bool ConvertToISM_bIncludeRandomization=false;

	// Random Seed for the Randomization
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Randomization Seed"))
	int32 ConvertToISM_randomSeed=0;

	// Theme for the Randomization
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst AutoInst Settings",meta=(DisplayName="Randomization Theme"))
	FString ConvertToISM_theme;

	// Add PhysicalMaterials to this Ignore List (for the procedural population)
	TArray<UPhysicalMaterial*> ignorePMaps;

	// Map of StaticMesh Keys with Arrays of Instances as the Values
	TMap<FName,FrdInstList> instList;

	UrdInstData*	GetInstUserData(const UStaticMesh* mesh,bool make);
	UrdInstData*	GetInstUserDataX(const FName sid,bool make);

	void			SetInstUserData(const UStaticMesh* mesh,UrdInstData* instData);
	void			SetInstUserDataX(const FName sid,UrdInstData* instData);

#if WITH_EDITOR
	void			AddProxyToReferencedAssets(const FrdProxySetup& proxy);
#endif

private:

	ArdActor*	poolListener=nullptr;

	TMap<UClass*,rdPoolData> poolMap;
//	TMap<UInstancedStaticMeshComponent*,rdDistributedObjectData> dtMap;	// Distributed Transactions for Instances
//	TMap<UClass*,rdDistributedObjectData> dtMapActors;					// Distributed Transactions for Actors
//	TMap<UNiagaraSystem*,rdDistributedObjectData> dtMapVFX;				// Distributed Transactions for VFX


	TMap<FName,rdDistributedObjectData> dtMap;
	TMap<float,float> cullVolMap;

public:

	TMap<FName,TArray<int32>> freeInstanceMap;

	// List of Actors that are within the scan radius of the player
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxies",meta=(DisplayName="Actors within scan distance"))
	TArray<AActor*> scannedActors;

	// List of Instances that are within the scan radius of the player
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxies",meta=(DisplayName="Instances within scan distance"))
	TArray<FrdInstanceItems> scannedInstances;

	// List of strings that get replaced with tokens in the Instance sid to save space
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings",meta=(DisplayName="Instances sid tokens"))
	TArray<FString>	sidTokenTable;

	// The base folder to save BakedDataAssets created with the rdInst tools
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Settings",meta=(DisplayName="BakedDataAsset Base Folder"))
	FString baseBakedAssetFolder;

	// Event fired when Instance/Prefab swaps to Proxy
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnProxySwapIn"))
	FrdInstProxySwapInDelegate OnProxySwapInDelegate;

	// Event fired when Instance/Prefab swaps in from Proxy
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnProxySwapOut"))
	FrdInstProxySwapOutDelegate OnProxySwapOutDelegate;

	// Event fired when Proxies want the server to handle spawning the Proxy Actor
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnSpawnOnServer"))
	FrdInstProxySpawnOnServerDelegate OnSpawnOnServerDelegate;

	// Event fired when an Interactive Pickup Proxy comes into focus - use to display text on the HUD
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnPickupFocused"))
	FrdInstPickupFocusedDelegate OnPickupFocusedDelegate;

	// Event fired when an Interactive Pickup Proxy goes out of focus - use to remove test from the HUD
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnPickupUnfocused"))
	FrdInstPickupUnfocusedDelegate OnPickupUnfocusedDelegate;

	// Event fired when an Overlap Pickup Proxy has been picked up
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnPickedUp"))
	FrdInstPickedUpDelegate OnPickedUpDelegate;

	// Event fired when item in the pool is used
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnPoolUsedDelegate"))
	FrdActorPoolUsedDelegate OnPoolUsedDelegate;

	// Event fired when item from the pool is returned to the pool
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnPoolReturnedDelegate"))
	FrdActorPoolReturnedDelegate OnPoolReturnedDelegate;

	// Event fired when a DestructionProxy has swapped and is ready for an Impulse or Field etc
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnDamageDelegate"))
	FrdInstDamageDelegate OnDamageDelegate;

	// Event fired Before Spawn or Procedural Actors render their objects
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnPreSpawnDelegate"))
	FrdPreSpawnDelegate OnPreSpawnDelegate;

	// Event fired After Spawn or Procedural Actors render their objects
	UPROPERTY(BlueprintAssignable,Category="rdInst Delegates",meta=(DisplayName="OnPostSpawnDelegate"))
	FrdPostSpawnDelegate OnPostSpawnDelegate;

private:

	// Data for procedural generation (no touching)
	float			pgLandscapeZ1=0.0f;
	float			pgLandscapeZ2=0.0f;
	int32			pgNumObjects=0;

	// Entities
	FCriticalSection critProcessEntities;
	std::atomic<bool> processingEntities;
public:
	FVector			playerLoc;
	FRotator		playerRot;
	bool			hasPlayer;

// Delegate Queue
	TArray<rdDelegateQueueItem> delegateQueue;

	void rdQueueProxySwapInDelegate(AActor* proxyActor,const FName sid,int32 index,UInstancedStaticMeshComponent* ismc,UrdStateDataAsset* state);
	void rdQueueProxySwapOutDelegate(AActor* proxyActor,const FName sid,int32 index,UInstancedStaticMeshComponent* ismc,UrdStateDataAsset* state);
	void rdQueueProxySpawnOnServerDelegate(UClass* actorClass,const FTransform& transform,const FName sid,int32 index,UrdStateDataAsset* state);
	void rdQueuePickupFocusedDelegate(int32 pickupID,const FTransform& transform,const FName sid,int32 index);
	void rdQueuePickupUnfocusedDelegate(int32 pickupID,const FTransform& transform,const FName sid,int32 index);
	void rdQueuePickedUpDelegate(int32 pickupID,const FTransform& transform,const FName sid,int32 index);
	void rdQueueProxyDamage(UPrimitiveComponent* ocomp,AActor* actor,FVector& loc,FVector& impulse,FHitResult& hit);
	void rdProcessBeforeDelegateQueue();
	void rdProcessAfterDelegateQueue();

	TArray<FrdProxySetupQueueItem>	proxySetupQueue;
	TArray<ProxyRemoveQueueItem>	removeProxyQueue;
	TArray<FrdProxyActorItem>		swapProxyActorQueue;
	TArray<FrdProxyActorItem>		swapOutProxyActorQueue;
	DamageQueue						damageProxyActorQueue;

	TMap<FName,TArray<FTransform>>								addMap;
	TMap<FName,TArray<FAddProxyQueueItem>>						addProxyMap;
	TMap<FName,TArray<FrdProxyActorItem>>						remProxyMap;
	TMap<FName,TArray<TTuple<int32,FTransform>>>				moveMap;
	TArray<AActor*>												destroyActorList;
	TArray<TTuple<AActor*,FTransform>>							showActorList;
	TArray<AActor*>												hideActorList;
	int32														currentPickupHighID=-1;
	int32														currentPickupHighInstance=-1;

private:
	void			ProcessEntities();
	void			processEntities_processDistributedTransactions();
	void			processEntities_processRenderQueues();
	void			processEntities_processProxyQueues();
	void			processEntities_processScannedEntities();
	void			processEntities_SwapOutInactiveProxies();
	void			processEntities_SwapMegaLights();

	bool			ShouldScanFromDistance(ArdSpawnActor* sa,float dist);

	// Proxies
	void			rdSwapInactiveProxy(FrdProxyActorItem& a,const FVector& aloc,TArray<AActor*>& removeProxyActorList,TArray<TSoftObjectPtr<UDataLayerAsset>>& removeProxyDataLayerList,TMap<FName,TArray<int32>>& removeProxyISMList);

	// Utilities
	rdPixelWrapper* createPixelWrapper(uint8* dat,const int16 w,int16 h,uint16 fmt,int8 interpType,int16 rot=0,bool flipV=false,bool flipH=false);
public:
	TSoftObjectPtr<UNiagaraSystem>				niagaraSpawner;//(FSoftObjectPath(TEXT("/Game/rdTools/System/Niagara/NS_rdMeshArray.NS_rdMeshArray")));
#if WITH_EDITOR
	FAssetData*									lastDraggedSpawnActorDataAsset=nullptr;
	FAssetData*									lastDraggedProceduralActorDataAsset=nullptr;
#endif

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="rdInst Stats")
	TArray<TSoftObjectPtr<UStaticMesh>>			LevelReferencedMeshes;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="rdInst Stats")
	TArray<TSoftObjectPtr<UMaterialInterface>>	LevelReferencedMaterials;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="rdInst Stats")
	TArray<TSoftClassPtr<UObject>>				LevelReferencedActorClasses;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="rdInst Stats")
	TArray<TSoftClassPtr<UDataLayerAsset>>		LevelReferencedDataLayers;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,Category="rdInst Stats")
	TArray<TSoftClassPtr<UNiagaraSystem>>		LevelReferencedVFX;

private:
	// Proxies
	FVector										lastPawnLocation[64];
	FRotator									lastPawnRotation[64];
	FVector										lastLongDistancePawnTransform[64];
	FVector										lastLocalOnlyPawnTransform[64];
	int32										proxyProcessCount=0;
	int32										threadCount=0;
	int32										threadID=0;
	TArray<rdThreadResult>						threadResults;
	float										maxLongDistance=10000.0f;

	TArray<ArdSpawnActor*>						spawnActorRegister;

	TMap<FName,FrdProxyItem>					instancedMeshToProxyMap;
	TMap<FName,FrdProxyInstanceItems>			instancesToProxyMap;
	TMap<AActor*,FrdProxyItem>					actorsToProxyMap;
	TMap<FName,FrdProxyGridArray>				longDistanceInstancesToProxyMap;
//	TMap<AActor*,FrdProxyGridArray>				longDistanceActorsToProxyMap;
	TMap<AActor*,FrdProxyItem>					longDistanceActorsToProxyMap;
	TMap<AActor*,FrdProxyActorItem>				proxyActorMap;
	TMap<TSoftObjectPtr<UDataLayerAsset>,FrdProxyActorItem>	proxyDataLayerMap;
	TMap<FName,TMap<int32,FrdProxyActorItem>>	proxyISMMap;
	TArray<AActor*>								proxyScanIgnoreActorList;
	TArray<FrdSpawnDataScanItem>				spawnVolumeProxyScanList;
	TMap<int32,FName>							pickupIDtoSidMap;

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	TArray<ALight*>								megaLightsToSwap;
	TArray<ALight*>								swappedLights;
#endif
	TMap<FName,TArray<int32>>					autoInstSavedIndexes;

	UMaterialInterface*							pickupHighlightMat=nullptr;
	UMaterialInterface*							pickupHighlightStencilMat=nullptr;
	static bool									bIsPlaying;
};
