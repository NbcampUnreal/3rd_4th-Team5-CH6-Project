// rdInstSubsystem.h - Copyright (c) 2022 Recourse Design ltd.
//
#pragma once
#include "rdInstBaseActor.h"
#include "Subsystems/EngineSubsystem.h"
#include "Engine/Engine.h"
#include "rdInstSubsystem.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

UCLASS()
class RDINST_PLUGIN_API UrdInstSubsystem : public UEngineSubsystem {
	GENERATED_BODY()
public:

	// The singular Base Actor for Instance Handling and Actor Pooling
	UFUNCTION(BlueprintCallable,Category="rdInst|Setup",meta=(DisplayName="rdSetUpInst",Keywords="rdInst Set Up"))
	void rdSetUpInst(bool useHISM=true,bool useISMforNanite=true,bool recycleInstances=true,bool autoInst=false,bool autoFromTags=false,bool autoMeshActors=true,bool autoBPs=true);

	// The singular Base Actor for Instance Handling and Actor Pooling
	UFUNCTION(BlueprintPure,Category="rdInst|Tools",BlueprintCallable,meta=(DisplayName="rdGetBase",Keywords="rdInst Get Base"))
	ArdInstBaseActor* rdGetBase();

	// Reset the Base Actor
	UFUNCTION(BlueprintCallable,Category="rdInst|Tools",meta=(DisplayName="rdResetBase",Keywords="rdInst Reset Base"))
	void rdResetBase();

	// Finds and returns the UInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGen",Keywords="rdInst Instance Gen Get"),Category="rdInst|Instancing")
	UInstancedStaticMeshComponent* rdGetInstanceGen(const UStaticMesh* mesh,bool create=true);

	// Finds and returns the UInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGenX",Keywords="rdInst Instance Gen Get"),Category="rdInst|Instancing")
	UInstancedStaticMeshComponent* rdGetInstanceGenX(const FName sid,bool create=true);

	// Finds and returns the UHierarchicalInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetHInstanceGen",Keywords="rdInst HInstance Gen Get"),Category="rdInst|Instancing")
	UHierarchicalInstancedStaticMeshComponent* rdGetHInstanceGen(const UStaticMesh* mesh,bool create=true);

	// Finds and returns the UHierarchicalInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetHInstanceGenX",Keywords="rdInst HInstance Gen Get"),Category="rdInst|Instancing")
	UHierarchicalInstancedStaticMeshComponent* rdGetHInstanceGenX(const FName sid,bool create=true);

	// Returns the Instance Handle ID for the specified StaticMesh
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMsid",Keywords="rdInst Instance Handle ID"),Category="rdInst|Tools")
	FName rdGetSMsid(const TSoftObjectPtr<UStaticMesh> m,ErdSpawnType type=ErdSpawnType::UseDefaultSpawn,const FName groupName=NAME_None);

	// Returns the Instance Handle ID for the specified StaticMesh, Material list and optional scale for negative checks
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMXsid",Keywords="rdInst Instance Handle ID"),Category="rdInst|Sids")
	FName rdGetSMXsid(ErdSpawnType type,const TSoftObjectPtr<UStaticMesh> m,TArray<TSoftObjectPtr<UMaterialInterface>> mats,bool bReverseCulling=false,ErdCollision collision=ErdCollision::UseDefault,float startCull=-1.0f,float endCull=-1.0f,int32 id=0,const FName groupName=NAME_None);

	// Returns the Instance Handle ID for the specified StaticMesh Component, using it's materials and negative scale flags
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMCsid",Keywords="rdInst Instance Handle ID"),Category="rdInst|Sids")
	FName rdGetSMCsid(const UStaticMeshComponent* smc,ErdSpawnType type=ErdSpawnType::UseDefaultSpawn,float overrideStartCull=-1.0f,float overrideEndCull=-1.0f,int32 overrideID=0,const FName groupName=NAME_None);

	// Returns the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMeshFromSid",Keywords="rdInst Instance Handle ID Mesh"),Category="rdInst|Sids")
	UStaticMesh* rdGetMeshFromSid(const FName sid);

	// Returns a SoftObjectPtr to the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSoftMeshFromSid",Keywords="rdInst Instance Handle ID Mesh"),Category="rdInst|Sids")
	TSoftObjectPtr<UStaticMesh> rdGetSoftMeshFromSid(const FName sid);

	// Returns the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMaterialsFromSid",Keywords="rdInst Instance Handle ID Materials"),Category="rdInst|Tools")
	TArray<TSoftObjectPtr<UMaterialInterface>> rdGetMaterialsFromSid(const FName sid);

	// Returns the Name of the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMeshNameFromSid",Keywords="rdInst Instance Handle ID Mesh Name"),Category="rdInst|Tools")
	FString rdGetMeshNameFromSid(const FName sid);

	// Returns the type of StaticMesh Spawn (0=Instance, 1=StaticMesh Component, 2=Componentless StaticMesh)
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSpawnTypeFromSid",Keywords="rdInst Instance Handle ID Mesh Spawn Type"),Category="rdInst|Tools")
	ErdSpawnType rdGetSpawnTypeFromSid(const FName sid);

	// Returns the Details of the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSidDetails",Keywords="rdInst Instance Handle ID Details"),Category="rdInst|Sids")
	void rdGetSidDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,TSoftObjectPtr<UStaticMesh>& mesh,TArray<TSoftObjectPtr<UMaterialInterface>>& materials,bool& bReverseCulling,TEnumAsByte<ErdCollision>& collision,float& startCull,float& endCull,int32& id,FName& groupName);

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
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesFastWithIndexes",Keywords="rdInst Instances Add Fast"),Category="rdInst|Instancing")
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

	// Remove Instances ---------------------------------------------------------------------------

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

	// Removes the ISM for the UStaticMesh and Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstance",Keywords="rdInst Instance Remove"),Category="rdInst|Instancing")
	void rdRemoveInstance(UStaticMesh* mesh,int32 index);

	// Removes the ISM for the UStaticMesh and Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstanceX",Keywords="rdInst Instance Remove"),Category="rdInst|Instancing")
	void rdRemoveInstanceX(const FName sid,int32 index);

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

	// Update the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformsFast",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransformsFast(UInstancedStaticMeshComponent* ismc,int32 startIndex,UPARAM(ref) const TArray<FTransform>& transforms,int32 arrayoffset=0);

	// Update the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransforms",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransforms(UStaticMesh* mesh,int32 startIndex,UPARAM(ref) const TArray<FTransform>& transforms);

	// Update the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformsX",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransformsX(const FName sid,int32 startIndex,UPARAM(ref) const TArray<FTransform>& transforms);

	// Update the Transforms for the specified Instance Indices
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformsX",Keywords="rdInst Transforms Update"),Category="rdInst|Instancing")
	void rdUpdateTransformArrayX(const FName sid,const TArray<int32> indices,const TArray<FTransform>& transforms);

	// Increment the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementTransforms",Keywords="rdInst Transforms Increment"),Category="rdInst|Instancing")
	void rdIncrementTransforms(UStaticMesh* mesh,UPARAM(ref) const TArray<int32>& indexes,const FTransform& transform);

	// Increment the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementTransformsX",Keywords="rdInst Transforms Increment"),Category="rdInst|Instancing")
	void rdIncrementTransformsX(const FName sid,UPARAM(ref) const TArray<int32>& indexes,const FTransform& transform);

	// Increment the Transforms for the specified Instance Indexes
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

	// Instance Utilities -------------------------------------------------------------------------

	// Converts the Instances, SMs and ChildActorComponents with SMs contained by this actor to Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdHarvestSMs",Keywords="rdInst Harvest SMs"),Category="rdInst|Harvest")
	void rdHarvestSMs(AActor* actor,bool hide,int32& numConverted,int32& numStandard,TMap<FName,int32>& sidMap);

	// Removes the Instances harvested from this actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveHarvestedInstances",Keywords="rdInst Remove Harvest ISMs"),Category="rdInst|Harvest")
	void rdRemoveHarvestedInstances(AActor* actor);

	// Set to True to Enable runtime ConvertToISMs for Actors and blueprints
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetupAutoInst",Keywords="rdInst Convert ISM Play"),Category="rdInst|Harvest")
	void rdSetupAutoInst(bool autoInst=true,bool autoFromTags=false,bool autoMeshActors=true,bool autoBPs=true);

	// Converts Actors in the level to ISMs and Hides the Actors
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertAutoISMs",Keywords="rdInst Convert ISM Play"),Category="rdInst|AutoInst")
	void rdConvertAutoISMs();

	// Reverts Actors in the level from ISMs and Shows the Actors
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRevertFromAutoISMs",Keywords="rdInst Revert Auto ISM"),Category="rdInst|AutoInst")
	void rdRevertFromAutoISMs();

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
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdCalcInstanceCounts",Keywords="rdInst Instance Calculate Number"),Category="rdInst|Tools",CallInEditor)
	void rdCalcInstanceCounts();

	// Recreates all the ISMs and HISMCs in this BaseActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRecreateInstances",Keywords="rdInst Recreate"),Category="rdInst|Tools",CallInEditor)
	void rdRecreateInstances();

	// Sets the Instance Settings (eg shadows) contained in the StaticMesh in the HISMC
	void SetHISMCdata(UStaticMesh* mesh,UInstancedStaticMeshComponent* hismc);

	// Sets the Instance Settings (eg shadows) contained in the StaticMesh in the HISMC
	void SetHISMCdataX(const FName sid,UInstancedStaticMeshComponent* hismc);

	// Finds the Attached ISMC for the StaticMesh (referenced by sid)
	UInstancedStaticMeshComponent* FindISMCforMesh(const FName& sid);

	// Finds the Attached HISMC for the StaticMesh (referenced by sid)
	UHierarchicalInstancedStaticMeshComponent* FindHISMCforMesh(const FName& sid);

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
	void rdSetInstancesVisibility(UStaticMesh* mesh,UPARAM(ref) const TArray<int32>& indexes,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibilityX",Keywords="rdInst Instances Visibility"),Category="rdInst|Instancing")
	void rdSetInstancesVisibilityX(const FName sid,UPARAM(ref) const TArray<int32>& indexes,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibilityFast",Keywords="rdInst Instances Visibility Fast"),Category="rdInst|Instancing")
	void rdSetInstancesVisibilityFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) const TArray<int32>& indexes,bool vis);

	// Returns an Array of all ISMCs used
	UFUNCTION(BlueprintPure,meta=(DisplayName="GetUsedISMCs",Keywords="rdInst used ISMCs"),Category="rdInst|Tools")
	TArray<UInstancedStaticMeshComponent*> GetUsedISMCs();

	// Returns the total number of instances in all meshes stored in the recycle cache
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetTotalNumberOfInstancesInRecycleCache",Keywords="rdInst Instances Recycled Total"),Category="rdInst|Tools")
	int32 rdGetTotalNumberOfInstancesInRecycleCache();

	// Returns the number of instances stored in the recycle cache for the specified mesh
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetNumberOfInstancesInRecycleCache",Keywords="rdInst Instances Recycled"),Category="rdInst|Tools")
	int32 rdGetNumberOfInstancesInRecycleCache(UStaticMesh* mesh);

	// Returns the number of instances stored in the recycle cache for the specified mesh
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetNumberOfInstancesInRecycleCacheX",Keywords="rdInst Instances Recycled"),Category="rdInst|Tools")
	int32 rdGetNumberOfInstancesInRecycleCacheX(const FName sid);

	// Creates a SpawnActor from the passed in volume
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdCreateSpawnActorFromVolume",Keywords="rdInst Create SpawnActor Volume"),Category="rdInst|Utilities")
	ArdSpawnActor* rdCreateSpawnActorFromVolume(AActor* volume,double distance,int32 distFrames,bool spatial,TEnumAsByte<rdSpawnMode> spawnMode=rdSpawnMode::RDSPAWNMODE_ALL);

	// Creates a SpawnActor from the passed in objects
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdCreateSpawnActorFromObjectList",Keywords="rdInst Create SpawnActor"),Category="rdInst|Utilities",meta=(IgnoreForMemberInitializationTest))
	ArdSpawnActor* rdCreateSpawnActorFromObjectList(UrdSetObjectsList* instances,FTransform& transform,const FString& filename=TEXT(""),double distance=0.0f,int32 distFrames=0,bool spatial=true,TEnumAsByte<rdSpawnMode> spawnMode=rdSpawnMode::RDSPAWNMODE_ALL,bool harvestInstances=false,int32 spawnActorGridX=16,int32 spawnActorGridY=16);

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
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFillCustomDataX",Keywords="rdInst CustomData Fill"),Category="rdInst|CustomData")
	void rdFillCustomDataX(const FName sid,int32 offset=0,int32 stride=1,float baseValue=0.0f,float randomVariance=0.0f,int32 granularity=1,float inc=0.0f,float incRandomVariance=0.0f);

	// Fill CustomData with values created with the parameters you pass in
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFillCustomDataFast",Keywords="rdInst CustomData Fill"),Category="rdInst|CustomData")
	void rdFillCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 offset=0,int32 stride=1,float baseValue=0.0f,float randomVariance=0.0f,int32 granularity=1,float inc=0.0f,float incRandomVariance=0.0f);

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

// Instance Conversion ----------------------------------------------------------------------------

	// Converts the Instance to a ChildActorComponent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToChildActor",Keywords="rdInst Instance Convert ChildActor"),Category="rdInst|Conversion")
	UChildActorComponent* rdConvertInstanceToChildActor(ArdActor* actor,UStaticMesh* mesh,int32 index);

	// Converts the Instance to a ChildActorComponent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToChildActorX",Keywords="rdInst Instance Convert ChildActor"),Category="rdInst|Conversion")
	UChildActorComponent* rdConvertInstanceToChildActorX(ArdActor* actor,const FName sid,int32 index);

	// Converts the Instance to a new StaticMesh Actor in the level, setting its mesh to the instance mesh (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToLevelActor",Keywords="rdInst Instance Convert Level Actor"),Category="rdInst|Conversion")
	AActor* rdConvertInstanceToLevelActor(UStaticMesh* mesh,int32 index);

	// Converts the Instance to a new StaticMesh Actor in the level, setting its mesh to the instance mesh (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToLevelActorX",Keywords="rdInst Instance Convert Level Actor"),Category="rdInstActorBase")
	AActor* rdConvertInstanceToLevelActorX(const FName sid,int32 index);

	// Converts the Instance to a new Actor - actorClass, in the level (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActor",Keywords="rdInst Instance Convert Actor",DepreciatedFunction),Category="rdInst|Conversion")
	AActor* rdConvertInstanceToActor(UStaticMesh* mesh,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor - actorClass, in the level (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorX",Keywords="rdInst Instance Convert Actor",DepreciatedFunction),Category="rdInst|Conversion")
	AActor* rdConvertInstanceToActorX(const FName sid,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorFromPool",Keywords="rdInst Instance Convert Actor Pool",DepreciatedFunction),Category="rdInst|Conversion")
	AActor* rdConvertInstanceToActorFromPool(UStaticMesh* mesh,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorFromPoolX",Keywords="rdInst Instance Convert Actor Pool",DepreciatedFunction),Category="rdInstActorBase")
	AActor* rdConvertInstanceToActorFromPoolX(const FName sid,UClass* actorClass,int32 index);

	// Converts all Instances in the volume to new Actors in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstancesToActorsFromPool",Keywords="rdInst Instance Convert Actors Pool Volume",DepreciatedFunction),Category="rdInst|Conversion")
	int32 rdConvertInstancesToActorsFromPool(UStaticMesh* mesh,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors);

	// Converts all Instances in the volume to new Actors in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstancesToActorsFromPoolX",Keywords="rdInst Instance Convert Actors Pool Volume",DepreciatedFunction),Category="rdInst|Conversion")
	int32 rdConvertInstancesToActorsFromPoolX(const FName sid,UClass* actorClass,rdTraceMode mode,int32 radius,const FVector start,const FVector finish,TArray<AActor*>& actors);

// Procedural Generation --------------------------------------------------------------------------

	// Procedurally places the meshes over the volume
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdProceduralGenerate",Keywords="rdInst instance placement procedural generation"),Category="rdInst|Procedural")
	int32 rdProceduralGenerate(const ArdSpawnActor* spawner);

	double getLandscapeZ(double x,double y,float radius,FHitResult& hit,bool fixSlopes=false,bool hitSM=false);

// Proxies ----------------------------------------------------------------------------------------

	// Sets the process interval (in ticks), distance to swap normal proxies, and distance to swap long-distance proxies
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetupProxies",Keywords="rdInst Proxy Setup"),Category="rdInst|Proxies")
	void rdSetupProxies(int32 processInterval,double swapDistance=1000.0f,double longDistanceSwapDistance=20000.0f,int32 idleTicks=10,int32 idleDestructionTicks=40,float velocityThreshold=0.01f,bool scanOnlyBaked=false);

	// Adds the instance to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceProxy",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the instance to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceProxyX",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddInstanceProxyX(const FName sid,int32 index,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Actor to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddActorProxy",Keywords="rdInst Actor Proxy Add"),Category="rdInst|Proxies")
	void rdAddActorProxy(AActor* actor,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Blueprint Prefab to the Proxy list, to be switched to the specified actor when in the sphere trace around the character
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddPrefabProxy",Keywords="rdInst Prefab Proxy Add"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdAddPrefabProxy(ArdActor* prefab,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Instance to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistanceInstanceProxy",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddLongDistanceInstanceProxy(UStaticMesh* mesh,int32 index,double distance=10000.0f,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Instance to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistanceInstanceProxyX",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddLongDistanceInstanceProxyX(const FName sid,int32 index,double distance=10000.0f,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Actor to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistanceActorProxy",Keywords="rdInst Actor Proxy Add"),Category="rdInst|Proxies")
	void rdAddLongDistanceActorProxy(AActor* actor,double distance,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Prefab to the LongDistance Proxy list, to be switched to the specified actor when in proximity to the player
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddLongDistancePrefabProxy",Keywords="rdInst Instance Proxy Add"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdAddLongDistancePrefabProxy(ArdActor* prefab,double distance,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the instance to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionInstanceProxy",Keywords="rdInst Destruction Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddDestructionInstanceProxy(UStaticMesh* mesh,int32 index,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the instance to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionInstanceProxyX",Keywords="rdInst Destruction Instance Proxy Add"),Category="rdInst|Proxies")
	void rdAddDestructionInstanceProxyX(const FName sid,int32 index,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Actor to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionActorProxy",Keywords="rdInst Destruction Actor Proxy Add"),Category="rdInst|Proxies")
	void rdAddDestructionActorProxy(AActor* actor,const FrdProxySetup proxy=FrdProxySetup());

	// Adds the Blueprint Prefab to the Proxy list, to be switched to the specified actor when an impulse is near
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddDestructionPrefabProxy",Keywords="rdInst Destruction Prefab Proxy Add"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdAddDestructionPrefabProxy(ArdActor* prefab,const FrdProxySetup proxy=FrdProxySetup());

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

	// Removes the Proxy Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByActor",Keywords="rdInst Actor Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveProxyByActor(AActor* actor);

	// Removes the Proxy Prefab
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveProxyByPrefab",Keywords="rdInst Prefab Proxy remove"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdRemoveProxyByPrefab(ArdActor* prefab);

	// Removes the LongDistance Proxy Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveLongDistanceProxyByProxy",Keywords="rdInst Long Distance Proxy remove"),Category="rdInst|Proxies")
	void rdRemoveLongDistanceProxyByProxy(AActor* proxy);

	// Removes the Pickup Proxy scanning for the instances (leaves the instances there)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemovePickupProxies",Keywords="rdInst Proxy Pickup Remove Scannign"),Category="rdInst|Proxies")
	void rdRemovePickupProxies(int32 id);

	// Spawns a Pickup Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSpawnPickup",Keywords="rdInst Proxy Pickup Spawn"),Category="rdInst|Proxies")
	int32 rdSpawnPickup(int32 id,const FTransform& transform);

	// Picks up a Pickup Instance (removes from the level)
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

	// Swaps the Actor over to its ProxyActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapActorToProxyActor",Keywords="rdInst Actor Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapActorToProxyActor(AActor* actor,int32 index,bool forImpact=false,bool forLongDistance=false);

	// Swaps the Prefab over to its ProxyActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapPrefabToProxyActor",Keywords="rdInst Prefab Proxy Swap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdSwapPrefabToProxyActor(ArdActor* prefab,int32 index,bool forImpact=false,bool forLongDistance=false);

	// Swaps the ProxyActor back to its Instanced Mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapProxyActorToInstance",Keywords="rdInst Instance Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapProxyActorToInstance(AActor* actor);

	// Swaps the ProxyActor back to its Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapProxyActorToActor",Keywords="rdInst Actor Proxy Swap"),Category="rdInst|Proxies")
	void rdSwapProxyActorToActor(AActor* actor);

	// Swaps the ProxyActor back to its Prefab
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSwapProxyActorToPrefab",Keywords="rdInst Prefab Proxy Swap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	void rdSwapProxyActorToPrefab(AActor* actor);

	// Swaps any Actors/Instances to ProxyActors around the impact point. A distance of 0.0 uses the default rdInstSettings value. Note that this fires rdOnProxyDamaged events for the affected proxies. Don't try referencing the proxies straight after this function.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdProcessImpactProxies",Keywords="rdInst Proxy Impact Process"),Category="rdInst|Proxies")
	void rdProcessImpactProxies(const AActor* caller,const FVector& loc,const FHitResult& hit,float distance=1000.0f,FVector impulse=FVector(0,0,0),USceneComponent* comp=nullptr,int32 index=-1);

	// Spawn the assets from the rdSpawner struct
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSpawn",Keywords="rdInst Spawn"),Category="rdInst|Spawn")
	void rdSpawn(const ArdSpawnActor* spawner,bool bake=false);

	// Returns a reference to the InstancedMeshToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetInstancedMeshToProxyMap"),Category="rdInst|Proxies")
	TMap<FName,FrdProxyItem>& rdGetInstancedMeshToProxyMap();

	// Returns a reference to the InstancesToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetInstancesToProxyMap"),Category="rdInst|Proxies")
	TMap<FName,FrdProxyInstanceItems>& rdGetInstancesToProxyMap();

	// Returns a reference to the ActorsToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetActorsToProxyMap"),Category="rdInst|Proxies")
	TMap<AActor*,FrdProxyItem>& rdGetActorsToProxyMap();

	// Returns a reference to the PrefabsToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetPrefabsToProxyMap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	TMap<ArdActor*,FrdProxyItem>& rdGetPrefabsToProxyMap();

	// Returns a reference to the LongDistanceInstancesToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetLongDistanceInstancesToProxyMap"),Category="rdInst|Proxies")
	TMap<FName,FrdProxyGridArray>& rdGetLongDistanceInstancesToProxyMap();

	// Returns a reference to the LongDistancePrefabsToProxy Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetLongDistanceActorsToProxyMap"),Category="rdInst|Proxies")
	TMap<AActor*,FrdProxyItem>& rdGetLongDistanceActorsToProxyMap();

	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetLongDistancePrefabsToProxyMap"),Category="rdInst|Proxies",meta=(DepreciatedFunction))
	TMap<ArdActor*,FrdProxyGridArray>& rdGetLongDistancePrefabsToProxyMap();

	// Returns a reference to the ProxyActor Map
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetProxyActorMap"),Category="rdInst|Proxies")
	TMap<AActor*,FrdProxyActorItem>&  rdGetProxyActorMap();

	// Make a rdProxySetup structure for an Actor Proxy
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdMakeActorProxySetup"),Category="rdInst|Proxies")
	FrdProxySetup rdMakeActorProxySetup(UClass* proxyActor,bool bDontHide=false,bool bDontRemove=false,float proxyPhysicsTimeout=30.0f,bool bPooled=false,bool bSimplePool=false,int32 pooledAmount=20,UrdStateDataAsset* savedState=nullptr,bool bCallSwapEvent=false);

	// Make a rdProxySetup structure for a DataLayer Proxy
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdMakeDataLayerProxySetup"),Category="rdInst|Proxies")
	FrdProxySetup rdMakeDataLayerProxySetup(TSoftObjectPtr<UDataLayerAsset> proxyDataLayerName,bool bDontHide=false,bool bDontRemove=false,UrdStateDataAsset* savedState=nullptr,bool bCallSwapEvent=false);

	// Make a rdProxySetup structure for a StaticMesh Proxy
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdMakeStaticMeshProxySetup"),Category="rdInst|Proxies")
	FrdProxySetup rdMakeStaticMeshProxySetup(UStaticMesh* proxyMesh,TArray<TSoftObjectPtr<UMaterialInterface>> materials,bool reverseCulling=false,TEnumAsByte<ErdCollision> collision=ErdCollision::UseDefault,bool bDontHide=false,bool bDontRemove=false,UrdStateDataAsset* savedState=nullptr,bool bCallSwapEvent=false);

	// Add Destruction settings to a rdProxySetup structure 
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdAddDestructionToProxySetup"),Category="rdInst|Proxies",meta=(IgnoreForMemberInitializationTest))
	FrdProxySetup rdAddDestructionToProxySetup(const FrdProxySetup& inProxySetup,UStaticMesh* destroyedMesh=nullptr,UClass* destroyedPrefab=nullptr,const FTransform& destroyedOffset=FTransform());

// Spline Utilities -------------------------------------------------------------------------------

	// Splits the Spline into X subsplines
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSplitSplineInto",Keywords="rdInst Spline Split"),Category="rdInst|Splines")
	UPARAM(DisplayName="SubSplines")TArray<AActor*> rdSplitSplineInto(AActor* actor,USplineComponent* spline,int32 num,bool callBPfunctions=false);

	// Get the Spline Points Position
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSplinePointPosition",Keywords="rdInst Spline Point Location"),Category="rdInst|Splines")
	void rdGetSplinePointPosition(int32 point,FVector& loc,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local);

	// Set the Spline Points Position
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetSplinePointPosition",Keywords="rdInst Spline Point Location"),Category="rdInst|Splines")
	UPARAM(DisplayName="ModdedSubSplines")TArray<AActor*> rdSetSplinePointPosition(int32 point,const FVector& loc,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local,bool updateSpline=true);

	// Get the Spline Points Rotation
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSplinePointRotation",Keywords="rdInst Spline Point Rotation"),Category="rdInst|Splines")
	void rdGetSplinePointRotation(int32 point,FRotator& rot,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local);

	// Set the Spline Points Rotation
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetSplinePointRotation",Keywords="rdInst Spline Point Rotation"),Category="rdInst|Splines")
	UPARAM(DisplayName="ModdedSubSplines")TArray<AActor*> rdSetSplinePointRotation(int32 point,const FRotator& rot,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,const ESplineCoordinateSpace::Type cordSpace=ESplineCoordinateSpace::Local,bool updateSpline=true);

	// Get the Spline Points Scale
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSplinePointScale",Keywords="rdInst Spline Point Scale"),Category="rdInst|Splines")
	void rdGetSplinePointScale(int32 point,FVector& scale,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr);

	// Set the Spline Points Scale
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetSplinePointScale",Keywords="rdInst Spline Point Scale"),Category="rdInst|Splines")
	UPARAM(DisplayName="ModdedSubSplines")TArray<AActor*> rdSetSplinePointScale(int32 point,const FVector& scale,UPARAM(ref) const TArray<AActor*>& subsplines,const AActor* splineActor=nullptr,bool updateSpline=true);

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

// Landscape Deforming ----------------------------------------------------------------------------

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

	// Shifts the Origin of the rdInst Base Actor. Useful to remove jitter from large distances away. Call when transitioning to a new part of your world.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdReseatBaseActor",Keywords="Base Actor Reseat Shift"),Category="rdInst|Utilities")
	void rdReseatBaseActor(const FVector& location,const FRotator& rot);

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

protected:

	// Our base Actor that manages the ISMs - gets created if it doesn't exist in the level
	inline bool		rdGetBaseActor();

private:

	// Pointer to the rdInst BaseActor that does all the ISM juggling
	ArdInstBaseActor*		rdBaseActor=nullptr;
	UWorld*					lastWorld=nullptr;
};
