// rdActor.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "rdInstBPLibrary.h"
#include "rdInstBaseActor.h"
#include "rdInstances.h"
#include "rdActor.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

#if WITH_EDITOR
DECLARE_STATS_GROUP(TEXT("rdInst"),STATGROUP_rdInst,STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("rdFunctionCyclesFunction"),STAT_rdFunctionCyclesFunction,STATGROUP_rdInst);
#endif

class ArdInstBaseActor;
class UrdInstanceVault;

// Modes that the rdActor can have:
UENUM()
enum rdActorMode {
	// does no processing
	RDACTORMODE_NONE			UMETA(DisplayName="None"),		
	// Organize your actors into folders - in this mode, only one randomly selected folder of actors will ever be visible
	RDACTORMODE_ONLYONE			UMETA(DisplayName="OnlyOne"),	
	// Processes all children actors and applies their Randomizations before placing in the level
	RDACTORMODE_RANDOMIZE		UMETA(DisplayName="Randomize"), 
	// User-made, triggers an rdBuildEvent event in the Blueprint where you can process your actors
	RDACTORMODE_BUILD			UMETA(DisplayName="Build"),		
	// Just builds Visible Instances in the InstanceData Array
	RDACTORMODE_BUILDINSTANCES	UMETA(DisplayName="BuildInstances")		
};

//
// rdActor
//
// SubClass off this class to get the benefits of fast Randomization and Managed Instanced Static Meshes
//
UCLASS()
class RDINST_PLUGIN_API ArdActor : public AActor {

	GENERATED_BODY()

public:
	ArdActor();
	ArdActor(const FObjectInitializer& ObjectInitializer);

	virtual ~ArdActor();

// Events -----------------------------------------------------------------------------------------

	// The OnConstruction Script has code to generate a new Random Seed if this Actors bRandomStart flag is True
	virtual void OnConstruction(const FTransform &Transform) override;

	// Used Internally to Destroy ISMs associated with this Actor
	virtual void BeginDestroy() override;
	virtual void Destroyed() override;

	// The BeginPlay event, has code to generate a new Random Seed if this Actors bRandomStart flag is True
	virtual void BeginPlay() override;

	// The EndPlay event, has code to remove instances - this gets called when actors are unloaded with WorldPartition
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called when Actor falls out of world - usually destroys the actor
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	// Called when the Actors lifetime expires
	virtual void LifeSpanExpired() override;

	virtual void SetActorHiddenInGame(bool bHidden) override;

	// Implement this Event to handle removing things when actor is destroyed. Works in the editor
	UFUNCTION(BlueprintImplementableEvent,Category="rdActor|Events")
	void rdDestroyed();

#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished);
	virtual void SetIsTemporarilyHiddenInEditor(bool bIsHidden);

	void		AddSidToReferencedAssets(const FName sid);
#if ENGINE_MAJOR_VERSION<5
	virtual void PreSave(const class ITargetPlatform* TargetPlatform);
#else
	virtual void PreSave(FObjectPreSaveContext context);
#endif
#endif

	// Alterations are stored here
	virtual void Serialize(FArchive& Ar) override;

	// Always returns true
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsRdActor",Keywords="rdInst rdActor Is"),Category="rdActor|Utility")
	bool rdIsRdActor() { return true; }

	// Always returns true
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsRdPrefab",Keywords="rdInst rdPrefab Is"),Category="rdActor|Utility")
	bool rdIsRdPrefab()  { return true; }

	// Returns the Instance Handle ID for the specified StaticMesh
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMsid",Keywords="rdInst Instance Handle ID"),Category="rdActor|Sids")
	FName rdGetSMsid(const UStaticMesh* m,TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn,int32 id=0,const FName groupName=NAME_None);

	// Returns the Instance Handle ID for the specified StaticMesh, Material list and optional scale for negative checks
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMXsid",Keywords="rdInst Instance Handle ID"),Category="rdActor|Sids")
	FName rdGetSMXsid(TEnumAsByte<ErdSpawnType> type,const UStaticMesh* m,TArray<TSoftObjectPtr<UMaterialInterface>> mats,bool bReverseCulling=false,ErdCollision collision=ErdCollision::UseDefault,float startCull=-1.0f,float endCull=-1.0f,int32 id=0,const FName groupName=NAME_None);

	// Returns the Instance Handle ID for the specified StaticMesh Component, using it's materials and negative scale flags
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSMCsid",Keywords="rdInst Instance Handle ID"),Category="rdActor|Sids")
	FName rdGetSMCsid(const UStaticMeshComponent* smc,TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn,float overrideStartCull=-1.0f,float overrideEndCull=-1.0f,int32 overrideID=0,const FName overrideGroupName=NAME_None);

	// Returns the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMeshFromSid",Keywords="rdInst Instance Handle ID Mesh"),Category="rdActor|Sids")
	UStaticMesh* rdGetMeshFromSid(const FName sid);

	// Returns a SoftObjectPtr to the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetSoftMeshFromSid",Keywords="rdInst Instance Handle ID Mesh"),Category="rdActor|Sids")
	TSoftObjectPtr<UStaticMesh> rdGetSoftMeshFromSid(const FName sid);

	// Returns the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMaterialsFromSid",Keywords="rdInst Instance Handle ID Materials"),Category="rdActor|Sids")
	TArray<TSoftObjectPtr<UMaterialInterface>> rdGetMaterialsFromSid(const FName sid);

	// Returns the Name of the StaticMesh referenced by the sid
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetMeshNameFromSid",Keywords="rdInst Instance Handle ID Mesh Name"),Category="rdActor|Sids")
	FString rdGetMeshNameFromSid(const FName sid);

// Timing Utilities -------------------------------------------------------------------------------

	// Call regularly to maintain profile information
	UFUNCTION(BlueprintCallable,Category="rdActor|Tools")
	void rdGetFunctionCycles();

	// Implement this Function with your Blueprint code you want to profile
	UFUNCTION(BlueprintImplementableEvent,Category="rdActor|Tools")
	void rdFunctionCyclesFunction();

// Build Methods ----------------------------------------------------------------------------------

	// Changes the Seed used for the Random Generation and rebuilds the Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRandomize",Keywords="rdInst Random Randomize"),Category="rdActor|Build")
	void rdRandomize();

	// Applies the Randomizations on this and its Children and creates the ISMs
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdBuild",Keywords="rdInst Build"),Category="rdActor|Build")
	virtual void rdBuild();

	// Applies the Randomizations on this and its Children and removes hidden objects completely
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdBakeRandomization",Keywords="rdInst Randomization Bake"),Category="rdActor|Build")
	virtual void rdBakeRandomization();

	// Event called when prefab is hidden
	UFUNCTION(BlueprintImplementableEvent,CallInEditor,meta=(DisplayName="rdOnHidePrefab",Keywords="rdInst Prefab Hide"),Category="rdActor|Build")
	void rdOnHidePrefab();

	// Event called when prefab is shown
	UFUNCTION(BlueprintImplementableEvent,CallInEditor,meta=(DisplayName="rdOnShowPrefab",Keywords="rdInst Prefab Show"),Category="rdActor|Build")
	void rdOnShowPrefab();

	// Queues this actor to be built next tick
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdDirty",Keywords="rdInst Queue Build"),Category="rdActor|Build")
	void rdDirty(int32 countdown=0);

	// Event called when in Build Mode, just fires the rdBuild Event where you can process your actors in the Blueprint
	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="rdBuildEvent",Keywords="rdInst Build Event"),Category="rdActor|Events")
	void rdBuildEvent();

	// Enumerates the folder, building the children if they're rdActors
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildFolder",Keywords="rdInst Build Folder"),Category="rdActor|Build")
	void rdBuildFolder(USceneComponent* comp);

	// Goes through all the Child Components of this Actor applying their Randomization Settings
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildRandomizedComponents",Keywords="rdInst Components Random BP"),Category="rdActor|Build")
	void rdBuildRandomizedComponents();

	// Builds the Instanced Static Meshes for this Actor with their Randomizations
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildRandomizedISMs",Keywords="rdInst Instance Create BP"),Category="rdActor|Build")
	void rdBuildRandomizedISMs();

	// Like rdBuildRandomizedISMs, but it resets the rdInst BaseActor as well
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRecreateBPInstances",Keywords="rdInst Instance Recreate BP"),Category="rdActor|Build")
	void rdRecreateBPInstances();

	// Builds the Instanced Static meshes from our InstanceList, ignoring Hidden Folders. No Randomization is applied
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildShowOnlyOneISMs_Folder",Keywords="rdInst Instance Create BP Filtered Folder"),Category="rdActor|Build")
	void rdBuildShowOnlyOneISMs_Folder();

	// Usually called internally, this hides all but one Actor or Folder of Actors contained as children of this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdThereCanBeOnlyOne",Keywords="rdInst Show One"),Category="rdActor|Build")
	void rdThereCanBeOnlyOne();

	void rdThereCanBeOnlyOne_Asset();
	void rdThereCanBeOnlyOne_Folder();

	void HidePrefabOutline();
	void ShowPrefabOutline();

	// Clears the List of Alterations
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdClearAlteredList",Keywords="Spawn Altered Objects Clear"),Category="rdActor|Build")
	virtual void rdClearAlteredList();

	// Builds a List of Alterations of the SpawnActors Baked data (each remove and change function stores the alterations)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildAlteredList",Keywords="Spawn Altered Objects Build"),Category="rdActor|Build")
	virtual FString rdBuildAlteredList();

	// Applies the List of Alterations to the SpawnActors Baked data
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdApplyAlteredList",Keywords="Spawn Altered Objects Apply"),Category="rdActor|Build")
	virtual int32 rdApplyAlteredList(const FString& str);

	// Moves the Instance Array Data to the Fast Tables
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdMoveArraysToTables",Keywords="rdInst Instance Data Move"),Category="rdActor|Build")
	void rdMoveArraysToTables();

	// Moves the Instance Fast Tables Data to the Arrays
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdMoveTablesToArrays",Keywords="rdInst Instance Data Move"),Category="rdActor|Build")
	void rdMoveTablesToArrays();


// Add Instances ----------------------------------------------------------------------------------

	// Adds an Instance to the scene of the StaticMesh.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstance",Keywords="rdInst Instance Add"),Category="rdActor|Instancing")
	int32 rdAddInstance(UStaticMesh* mesh,const FTransform& transform);

	// Adds an Instance to the scene of the StaticMesh.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceX",Keywords="rdInst Instance Add"),Category="rdActor|Instancing")
	int32 rdAddInstanceX(const FName sid,const FTransform& transform);

	// Adds an Instance to the scene of the StaticMesh in WorldSpace.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceWorld",Keywords="rdInst Instance Add WorldSpace"),Category="rdActor|Instancing")
	int32 rdAddInstanceWorld(UStaticMesh* mesh,const FTransform& transform);

	// Adds an Instance to the scene of the StaticMesh in WorldSpace.
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceWorldX",Keywords="rdInst Instance Add WorldSpace"),Category="rdActor|Instancing")
	int32 rdAddInstanceWorldX(const FName sid,const FTransform& transform);

	// Adds an Instance of the StaticMesh from the passed ISMC to the level using the passed in Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceFast",Keywords="rdInst Instance Add Fast"),Category="rdActor")
	int32 rdAddInstanceFast(UInstancedStaticMeshComponent* instGen,const FTransform& transform);

	// Adds an Instance of the StaticMesh from the passed HISMC to the level using the passed in Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceFastWorld",Keywords="rdInst Instance Add Fast WorldSpace"),Category="rdActor|Instancing")
	int32 rdAddInstanceFastWorld(UInstancedStaticMeshComponent* instGen,const FTransform& transform);

	// Adds to the InstanceData for this Actor from the passed in Array of Transforms for the specified StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstances",Keywords="rdInst Instances Add"),Category="rdActor|Instancing")
	bool rdAddInstances(UStaticMesh* mesh,const TArray<FTransform>& transforms);

	// Adds to the InstanceData for this Actor from the passed in Array of Transforms for the specified StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesX",Keywords="rdInst Instances Add"),Category="rdActor|Instancing")
	bool rdAddInstancesX(const FName sid,const TArray<FTransform>& transforms);

	// Adds Instances for the passed in Array of Transforms for the specified StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesFast",Keywords="rdInst Instances Add"),Category="rdActor|Instancing")
	void rdAddInstancesFast(UStaticMesh* mesh,const TArray<FTransform>& transforms);

	// Adds Instances for the passed in Array of Transforms for the specified StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstancesFastX",Keywords="rdInst Instances Add"),Category="rdActor|Instancing")
	void rdAddInstancesFastX(const FName sid,const TArray<FTransform>& transforms);

	// Adds a new Instance of the StaticMesh to our Actors Instance List
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceToList",Keywords="rdInst Instance Add List"),Category="rdActor|Instancing")
	void rdAddInstanceToList(UStaticMesh* mesh,const FTransform& transform);

	// Adds a new Instance of the StaticMesh to our Actors Instance List
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceToListX",Keywords="rdInst Instance Add List"),Category="rdActor|Instancing")
	void rdAddInstanceToListX(const FName sid,const FTransform& transform);

	// Adds a new Instance of the StaticMesh to our Actors Instance List with the passed in folder and Component to get Randomization from
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddAdvInstanceToList",Keywords="rdInst Advanced Instance Add List"),Category="rdActor|Instancing")
	void rdAddAdvInstanceToList(UStaticMesh* mesh,const FTransform& transform,const FString& folder,UActorComponent* comp);

	// Adds a new Instance of the StaticMesh to our Actors Instance List with the passed in folder and Component to get Randomization from
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddAdvInstanceToListX",Keywords="rdInst Advanced Instance Add List"),Category="rdActor|Instancing")
	void rdAddAdvInstanceToListX(const FName sid,const FTransform& transform,const FString& folder,UActorComponent* comp);

	// Adds a new Instance of the StaticMesh to our Actors Instance List with the passed in Randomized properties
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdAddInstanceLong",Keywords="rdInst Instance Add List"),Category="rdActor|Instancing")
	void rdAddInstanceLong(UStaticMesh* mesh,USceneComponent* comp,const FTransform& transform,const FString& name,const FString& folder);

	// Adds a new Instance of the StaticMesh to our Actors Instance List with the passed in Randomized properties
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdAddInstanceLongX",Keywords="rdInst Instance Add List"),Category="rdActor|Instancing")
	void rdAddInstanceLongX(const FName sid,USceneComponent* comp,const FTransform& transform,const FString& name,const FString& folder);

	// Duplicates the selected Instances in the prefab. This is an editor only function
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdDuplicateInstancesLongX",Keywords="rdInst Instance Duplicate List"),Category="rdActor|Instancing")
	void rdDuplicateInstancesLongX(FrdSidInstancesMap sidMap);

	// Removes the selected Instances from the prefab. This is an editor only function
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdRemoveInstancesLongX",Keywords="rdInst Instance Remove List"),Category="rdActor|Instancing")
	void rdRemoveInstancesLongX(FrdSidInstancesMap sidMap);

	// Replaces the selected Instances in the prefab with a new Mesh (opens a window). This is an editor only function
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdReplaceInstancesLongX",Keywords="rdInst Instance Replace List"),Category="rdActor|Instancing")
	void rdReplaceInstancesLongX(FrdSidInstancesMap sidMap,FName newSid);


// Remove Instances -------------------------------------------------------------------------------

	// Removes the Instance referred to by index for the specified StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstance",Keywords="rdInst Instance Remove"),Category="rdActor|Instancing")
	void rdRemoveInstance(UStaticMesh* mesh,int32 index);

	// Removes the Instance referred to by index for the specified StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstanceX",Keywords="rdInst Instance Remove"),Category="rdActor|Instancing")
	void rdRemoveInstanceX(const FName sid,int32 index);

	// Removes the Instance referred to by index for the specified StaticMesh and stores in the alteration list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdApplyRemoveInstance",Keywords="rdInst Instance Remove"),Category="rdActor|Instancing")
	void rdApplyRemoveInstance(UStaticMesh* mesh,int32 index);

	// Removes the Instance referred to by index for the specified StaticMesh and stores in the alteration list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdApplyRemoveInstanceX",Keywords="rdInst Instance Remove"),Category="rdActor|Instancing")
	void rdApplyRemoveInstanceX(const FName sid,int32 index);

	// Removes the Instance referred to by index in the HISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstanceFast",Keywords="rdInst Instance Remove"),Category="rdActor|Instancing")
	void rdRemoveInstanceFast(UInstancedStaticMeshComponent* instGen,int32 index);

	// Removes all the StaticMesh Instances in our Actors Instance List
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdClearInstanceList",Keywords="rdInst Instance List Clear"),Category="rdActor|Instancing")
	void rdClearInstanceList();

	// Removes all Instanced Static Meshes associated with this Actor
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdRemoveInstances",Keywords="rdInst Instance Remove"),Category="rdActor|Instancing")
	void rdRemoveInstances();

	// Removes all Instanced Static Meshes on all actors
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdResetAllInstances",Keywords="rdInst Instances Reset"),Category="rdActor|Instancing")
	void rdResetAllInstances();

// Instance Transforms ----------------------------------------------------------------------------

	// Sets the Specified Instance of the UStaticMeshes FTransform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceTransform",Keywords="rdInst Instance Transform Set"),Category="rdActor|Instancing")
	void rdSetInstanceTransform(UStaticMesh* mesh,int32 index,const FTransform& transform);

	// Sets the Specified Instance of the UStaticMeshes FTransform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceTransformX",Keywords="rdInst Instance Transform Set"),Category="rdActor|Instancing")
	void rdSetInstanceTransformX(const FName sid,int32 index,const FTransform& transform);

	// Sets the Specified Instance of the UStaticMeshes FTransform and records it in the alteration list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdApplySetInstanceTransform",Keywords="rdInst Instance Transform Set"),Category="rdActor|Instancing")
	void rdApplySetInstanceTransform(UStaticMesh* mesh,int32 index,const FTransform& stransform);

	// Sets the Specified Instance of the UStaticMeshes FTransform and records it in the alteration list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdApplySetInstanceTransformX",Keywords="rdInst Instance Transform Set"),Category="rdActor|Instancing")
	void rdApplySetInstanceTransformX(const FName sid,int32 index,const FTransform& stransform);

	// Gets the Transform of the Specified Instance of the UStaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceTransform",Keywords="rdInst Instance Transform Get"),Category="rdActor|Instancing")
	bool rdGetInstanceTransform(UStaticMesh* mesh,int32 index,FTransform& transform);

	// Gets the Transform of the Specified Instance of the UStaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceTransformX",Keywords="rdInst Instance Transform Get"),Category="rdActor|Instancing")
	bool rdGetInstanceTransformX(const FName sid,int32 index,FTransform& transform);

	// Adds to the Specified Instances Transform with the specified FTransform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceTransform",Keywords="rdInst Instance Transform Add"),Category="rdActor|Instancing")
	void rdAddInstanceTransform(UStaticMesh* mesh,int32 index,const FTransform& transform);

	// Adds to the Specified Instances Transform with the specified FTransform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddInstanceTransformX",Keywords="rdInst Instance Transform Add"),Category="rdActor|Instancing")
	void rdAddInstanceTransformX(const FName sid,int32 index,const FTransform& transform);

	// Updates this Actors Instances to reflect the Actors new Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateISMTransforms",Keywords="rdInst Instance Transform Update"),Category="rdActor|Instancing")
	void rdUpdateISMTransforms();

	// Returns the World Transform of the specified Relative Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdInstToWorld",Keywords="rdInst Instance Transform World"),Category="rdActor|Tools")
	FTransform rdInstToWorld(const FTransform& tran);

	// Returns the Relative Transform of the specified World Transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdWorldToInst",Keywords="rdInst Instance Transform From World"),Category="rdActor|Tools")
	FTransform rdWorldToInst(const FTransform& tran);

	// Update the Transform for the specified Instance Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransform",Keywords="rdInst Transform Update"),Category="rdActor|Instancing")
	void rdUpdateTransform(UStaticMesh* mesh,int32 index,const FTransform& transform);

	// Update the Transform for the specified Instance Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformX",Keywords="rdInst Transform Update"),Category="rdActor|Instancing")
	void rdUpdateTransformX(const FName sid,int32 index,const FTransform& transform);

	// Update the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransforms",Keywords="rdInst Transforms Update"),Category="rdActor|Instancing")
	void rdUpdateTransforms(UStaticMesh* mesh,int32 startIndex,const TArray<FTransform>& transforms);

	// Update the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateTransformsX",Keywords="rdInst Transforms Update"),Category="rdActor|Instancing")
	void rdUpdateTransformsX(const FName sid,int32 startIndex,const TArray<FTransform>& transforms);

	// Increment the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementTransforms",Keywords="rdInst Transforms Increment"),Category="rdActor|Instancing")
	void rdIncrementTransforms(UStaticMesh* mesh,const TArray<int32>& indexes,const FTransform& transform);

	// Increment the Transforms for the specified Instance Indexes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIncrementTransformsX",Keywords="rdInst Transforms Increment"),Category="rdActor|Instancing")
	void rdIncrementTransformsX(const FName sid,const TArray<int32>& indexes,const FTransform& transform);

	// Gets a reference to the Per-Instance Transforms for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetTransformsPtr",Keywords="rdInst Transforms Get Pointer"),Category="rdActor|Instancing")
	TArray<FMatrix>& rdGetTransformsPtr(UStaticMesh* mesh,int32& numTransforms);

	// Gets a reference to the Per-Instance Transforms for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetTransformsPtrX",Keywords="rdInst Transforms Get Pointer"),Category="rdActor|Instancing")
	TArray<FMatrix>& rdGetTransformsPtrX(const FName sid,int32& numTransforms);

	// Get the Transform of the FastInstance item
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetFastArrayItemTransform",Keywords="rdInst Actor Instance Transform"),Category="rdActor|Instancing")
	FTransform rdGetFastArrayItemTransform(int32 index,int32 tindex);

	// Set the Transform of the FastInstance item
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetFastArrayItemTransform",Keywords="rdInst Actor Instance Transform"),Category="rdActor|Instancing")
	void rdSetFastArrayItemTransform(int32 index,int32 tindex,const FTransform transform);

	// Gets the Transform of the RandomizedInstance item
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetRandomizedArrayItemTransform",Keywords="rdInst Actor Instance Transform"),Category="rdActor|Instancing")
	FTransform rdGetRandomizedArrayItemTransform(int32 index,int32 tindex);

	// Set the Transform of the RandomizedInstance item
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetRandomizedArrayItemTransform",Keywords="rdInst Actor Instance Transform"),Category="rdActor|Instancing")
	void rdSetRandomizedArrayItemTransform(int32 index,int32 tindex,const FTransform transform);

// Per-Instance Custom Data -----------------------------------------------------------------------

	// Sets the Number of CustomData per Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetNumInstCustomData",Keywords="rdInst CustomData Set Number"),Category="rdActor|CustomData")
	void rdSetNumInstCustomData(UStaticMesh* mesh,int32 numData);

	// Sets the Number of CustomData per Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetNumInstCustomDataX",Keywords="rdInst CustomData Set Number"),Category="rdActor|CustomData")
	void rdSetNumInstCustomDataX(const FName sid,int32 numData);

	// Sets the Number of CustomData per Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetNumInstCustomDataFast",Keywords="rdInst CustomData Set Number"),Category="rdActor|CustomData")
	void rdSetNumInstCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 numData);

	// Sets the PerInstance CustomData Value for all instances used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataValueForMesh",Keywords="rdInst Mesh Instance Data Set"),Category="rdActor|CustomData")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataValueForMesh(const UStaticMesh* mesh,int32 index,float data);

	// Sets the PerInstance CustomData Value for all instances used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataValueForMeshX",Keywords="rdInst Mesh Instance Data Set"),Category="rdActor|CustomData")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataValueForMeshX(const FName sid,int32 index,float data);

	// Sets the PerInstance CustomData Values for all instances used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataForMesh",Keywords="rdInst Mesh Instance Data Set"),Category="rdActor|CustomData")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataForMesh(const UStaticMesh* mesh,TArray<float> data);

	// Sets the PerInstance CustomData Values for all instances used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataForMeshX",Keywords="rdInst Mesh Instance Data Set"),Category="rdActor|CustomData")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataForMeshX(const FName sid,TArray<float> data);

	// Sets the PerInstance CustomData Value for all instances with ID (first custom float) used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataValueForMeshID",Keywords="rdInst Mesh Instance ID Data Set"),Category="rdActor")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataValueForMeshID(const UStaticMesh* mesh,float ID,int32 index,float data);

	// Sets the PerInstance CustomData Value for all instances with ID (first custom float) used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataValueForMeshIDX",Keywords="rdInst Mesh Instance ID Data Set"),Category="rdActor|CustomData")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataValueForMeshIDX(const FName sid,float ID,int32 index,float data);

	// Sets the PerInstance CustomData Values for instances with the ID (first custom float) used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataForMeshID",Keywords="rdInst Mesh Instance ID Data Set"),Category="rdActor|CustomData")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataForMeshID(const UStaticMesh* mesh,float ID,TArray<float> data);

	// Sets the PerInstance CustomData Values for instances with the ID (first custom float) used by this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataForMeshIDX",Keywords="rdInst Mesh Instance ID Data Set"),Category="rdActor|CustomData")
	UPARAM(DisplayName="ErrorCode") int32 rdSetPerInstanceDataForMeshIDX(const FName sid,float ID,TArray<float> data);

	// Gets a reference to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCustomDataPtr",Keywords="rdInst CustomData Get Pointer"),Category="rdActor|CustomData")
	UPARAM(DisplayName="CustomFloats") TArray<float>& rdGetCustomDataPtr(UStaticMesh* mesh,int32& numData);

	// Gets a reference to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCustomDataPtrX",Keywords="rdInst CustomData Get Pointer"),Category="rdActor|CustomData")
	UPARAM(DisplayName="CustomFloats") TArray<float>& rdGetCustomDataPtrX(const FName sid,int32& numData);

	// Gets a reference to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetCustomDataPtrFast",Keywords="rdInst CustomData Get Pointer"),Category="rdActor|CustomData")
	UPARAM(DisplayName="CustomFloats") TArray<float>& rdGetCustomDataPtrFast(UInstancedStaticMeshComponent* mesh,int32& numData);

	// Gets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol3Data",Keywords="rdInst CustomData Get Color"),Category="rdActor|CustomData")
	FLinearColor rdGetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex);

	// Gets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol3DataX",Keywords="rdInst CustomData Get Color"),Category="rdActor|CustomData")
	FLinearColor rdGetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex);

	// Gets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol3DataFast",Keywords="rdInst CustomData Get Color"),Category="rdActor|CustomData")
	FLinearColor rdGetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex);

	// Gets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol4Data",Keywords="rdInst CustomData Get Color"),Category="rdActor|CustomData")
	FLinearColor rdGetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex);

	// Gets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol4DataX",Keywords="rdInst CustomData Get Color"),Category="rdActor|CustomData")
	FLinearColor rdGetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex);

	// Gets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdGetCustomCol4DataFast",Keywords="rdInst CustomData Get Color"),Category="rdActor|CustomData")
	FLinearColor rdGetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex);

	// Sets a value to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomData",Keywords="rdInst CustomData Set Value"),Category="rdActor|CustomData")
	void rdSetCustomData(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,float value,bool batch=false);

	// Sets a value to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomDataX",Keywords="rdInst CustomData Set Value"),Category="rdActor|CustomData")
	void rdSetCustomDataX(const FName sid,int32 instanceIndex,int32 dataIndex,float value,bool batch=false);

	// Sets a value to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomDataFast",Keywords="rdInst CustomData Set Value"),Category="rdActor|CustomData")
	void rdSetCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,float value,bool batch=false);

	// Sets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol3Data",Keywords="rdInst CustomData Set Color"),Category="rdActor|CustomData")
	void rdSetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol3DataX",Keywords="rdInst CustomData Set Color"),Category="rdActor|CustomData")
	void rdSetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGB Linear Color (3 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol3DataFast",Keywords="rdInst CustomData Set Color"),Category="rdActor|CustomData")
	void rdSetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol4Data",Keywords="rdInst CustomData Set Color"),Category="rdActor|CustomData")
	void rdSetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol4DataX",Keywords="rdInst CustomData Set Color"),Category="rdActor|CustomData")
	void rdSetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets an RGBA Linear Color (4 floats) to the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetCustomCol4DataFast",Keywords="rdInst CustomData Set Color"),Category="rdActor|CustomData")
	void rdSetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch=false);

	// Sets all the values to the Per-Instance CustomData array for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetAllCustomData",Keywords="rdInst CustomData Set All"),Category="rdActor|CustomData")
	void rdSetAllCustomData(UStaticMesh* mesh,UPARAM(ref) TArray<float>& data,bool update=true);

	// Sets all the values to the Per-Instance CustomData array for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetAllCustomDataX",Keywords="rdInst CustomData Set All"),Category="rdActor|CustomData")
	void rdSetAllCustomDataX(const FName sid,UPARAM(ref) TArray<float>& data,bool update=true);

	// Sets all the values to the Per-Instance CustomData array for the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetAllCustomDataFast",Keywords="rdInst CustomData Set All"),Category="rdActor|CustomData")
	void rdSetAllCustomDataFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) TArray<float>& data,bool update=true);

	// Updates the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateCustomData",Keywords="rdInst CustomData Update"),Category="rdActor|CustomData")
	void rdUpdateCustomData(UStaticMesh* mesh);

	// Updates the Per-Instance CustomData for the mesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateCustomDataX",Keywords="rdInst CustomData Update"),Category="rdActor|CustomData")
	void rdUpdateCustomDataX(const FName sid);

	// Updates the Per-Instance CustomData for the ISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateCustomDataFast",Keywords="rdInst CustomData Update"),Category="rdActor|CustomData")
	void rdUpdateCustomDataFast(UInstancedStaticMeshComponent* ismc);

// Instance Utilties ------------------------------------------------------------------------------

	// Returns the InstancedStaticMeshComponent for the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGen",Keywords="rdInst Instance Gen Get"),Category="rdActor|Instancing")
	UInstancedStaticMeshComponent* rdGetInstanceGen(const UStaticMesh* mesh,bool create=true);

	// Returns the InstancedStaticMeshComponent for the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGenX",Keywords="rdInst Instance Gen Get"),Category="rdActor|Instancing")
	UInstancedStaticMeshComponent* rdGetInstanceGenX(const FName sid,bool create=true);

	// Returns the HierarchicalInstancedStaticMeshComponent for the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetHInstanceGen",Keywords="rdInst Hierachial Instance Gen Get"),Category="rdActor|Instancing")
	UHierarchicalInstancedStaticMeshComponent* rdGetHInstanceGen(UStaticMesh* mesh,bool create=true);

	// Returns the HierarchicalInstancedStaticMeshComponent for the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetHInstanceGenX",Keywords="rdInst Hierachial Instance Gen Get"),Category="rdActor|Instancing")
	UHierarchicalInstancedStaticMeshComponent* rdGetHInstanceGenX(const FName sid,bool create=true);

	// Finds and returns the UInstancedStaticMeshComponent assigned to the StaticMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceGenX",Keywords="rdInst Instance Gen Get"),Category="rdActor|Instancing")
	UInstancedStaticMeshComponent* rdGetPreferredInstanceGenX(const FName sid,bool create=true);

	// Returns the StaticMesh belonging to the HISMC or a nullptr if the passed in component is not a HISMC
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetStaticMeshFromInstanceIndex",Keywords="rdInst StaticMesh From Instance"),Category="rdActor|Instancing")
	UStaticMesh* rdGetStaticMeshFromInstanceIndex(UPrimitiveComponent* comp);

	// Returns the Actor that owns the Instance or a nullptr if the passed in component is not a HISMC or index not valid
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetActorFromInstanceIndex",Keywords="rdInst rdActor From Instance"),Category="rdActor|Instancing")
	AActor* rdGetActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index);

	// Returns the rdActor that owns the Instance or a nullptr if the passed in component is not a HISMC or index not valid
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetrdActorFromInstanceIndex",Keywords="rdInst rdActor From Instance"),Category="rdActor|Instancing")
	ArdActor* rdGetrdActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index);

	// Returns all the instance indexes for the Mesh, used by this actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceIndexesForMesh",Keywords="rdInst Mesh Instance Indexes"),Category="rdActor|Instancing")
	UPARAM(DisplayName="NumIndexes") int32 rdGetInstanceIndexesForMesh(const UStaticMesh* mesh,TArray<int32>& indexes);

	// Returns all the instance indexes for the Mesh, used by this actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetInstanceIndexesForMeshX",Keywords="rdInst Mesh Instance Indexes"),Category="rdActor|Instancing")
	UPARAM(DisplayName="NumIndexes") int32 rdGetInstanceIndexesForMeshX(const FName sid,TArray<int32>& indexes);

	// Builds the InstanceData for this Actor from the passed in Arrays (must be the same length)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceData",Keywords="rdInst Instance Data Set"),Category="rdActor|Instancing")
	bool rdSetInstanceData(const TArray<UStaticMesh*>& meshList,const TArray<FTransform>& transforms);

	// Builds the InstanceData for this Actor from the passed in Arrays (must be the same length)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceDataX",Keywords="rdInst Instance Data Set"),Category="rdActor|Instancing")
	bool rdSetInstanceDataX(const TArray<FName>& sidList,const TArray<FTransform>& transforms);

	// Returns the last Instance Index created for the StaticMesh, returns -1 if there are none
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetLastInstanceIndex",Keywords="rdInst Instance Index Get"),Category="rdActor|Instancing")
	int32 rdGetLastInstanceIndex(UStaticMesh* mesh);

	// Returns the last Instance Index created for the StaticMesh, returns -1 if there are none
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetLastInstanceIndexX",Keywords="rdInst Instance Index Get"),Category="rdActor|Instancing")
	int32 rdGetLastInstanceIndexX(const FName sid);

	// Gets the Bounds of all the instances in this blueprint
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdGetInstancesBounds",Keywords="rdInst Instance Bounds"),Category="rdActor|Instancing")
	void rdGetInstancesBounds(const FVector& inMin,const FVector& inMax,FVector& outMin,FVector& outMax);

	// Mirror the instances in this blueprint
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdMirrorInstanceLocations",Keywords="rdInst Instance Locations Mirror"),Category="rdActor|Instancing")
	void rdMirrorInstanceLocations(int32 axis,const FVector& center);

	// Place the instances in this blueprint on the ground
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPlaceOnGround",Keywords="rdInst Instances Locations Ground"),Category="rdActor|Instancing")
	void rdPlaceOnGround(bool smartPlacement);

	// Sets or Clears Instance Edit Mode
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdSetInstanceEditMode",Keywords="rdInst Instances Edit Mode Set"),Category="rdActor|Instancing")
	void rdSetInstanceEditMode(bool editMode);

	// Gets the current status this rdActors Instance Edit Mode
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdIsEditInstanceMode",Keywords="rdInst Instances Edit Mode Get"),Category="rdActor|Instancing")
	bool rdIsEditInstanceMode();

	// Updates the Instance transforms in this actor to that of the src
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdUpdatePrefabBP",Keywords="rdInst Prefab Update"),Category="rdActor|Prefabs")
	bool rdUpdatePrefabBP(const AActor* src);

	// Updates the Instance transforms in this actor to that of the src
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdUpdatePrefabWithActorList",Keywords="rdInst Prefab Update"),Category="rdActor")
	bool rdUpdatePrefabWithActorList(AActor* src,TArray<FrdInstItemX> meshList,TArray<FrdActorItemX> list,bool updateTransforms=true);

// Static Mesh Component Utilities ----------------------------------------------------------------

	// Adds a StaticMesh Component to the actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddStaticMesh",Keywords="rdInst StaticMesh Add"),Category="rdActor|Tools")
	UStaticMeshComponent* rdAddStaticMesh(UStaticMesh* mesh,const FTransform& transform,const FString& label=FString(""));

	// Adds a StaticMesh Component to the actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddStaticMeshX",Keywords="rdInst StaticMesh Add"),Category="rdActor|Tools")
	UStaticMeshComponent* rdAddStaticMeshX(const FName sid,const FTransform& transform,const FString& label=FString(""));

	// Removes all the StaticMesh Components for this actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveAllStaticMeshes",Keywords="rdInst StaticMeshes Remove"),Category="rdActor|Tools")
	void rdRemoveAllStaticMeshes();

// Conversion Utilties ----------------------------------------------------------------------------

	// Converts the Instance to a ChildActorComponent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToChildActor",Keywords="rdInst Instance Convert ChildActor"),Category="rdActor|Conversions")
	UChildActorComponent* rdConvertInstanceToChildActor(UStaticMesh* mesh,int32 index);

	// Converts the Instance to a ChildActorComponent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToChildActorX",Keywords="rdInst Instance Convert ChildActor"),Category="rdActor|Conversions")
	UChildActorComponent* rdConvertInstanceToChildActorX(const FName sid,int32 index);

	// Converts the Instance to a new StaticMesh Actor in the level, setting its mesh to the instance mesh (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToLevelActor",Keywords="rdInst Instance Convert Level Actor"),Category="rdActor|Conversions")
	AActor* rdConvertInstanceToLevelActor(UStaticMesh* mesh,int32 index);

	// Converts the Instance to a new StaticMesh Actor in the level, setting its mesh to the instance mesh (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToLevelActorX",Keywords="rdInst Instance Convert Level Actor"),Category="rdActor|Conversions")
	AActor* rdConvertInstanceToLevelActorX(const FName sid,int32 index);

	// Converts the Instance to a new Actor - actorClass, in the level (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActor",Keywords="rdInst Instance Convert Actor"),Category="rdActor|Conversions")
	AActor* rdConvertInstanceToActor(UStaticMesh* mesh,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor - actorClass, in the level (spawn)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorX",Keywords="rdInst Instance Convert Actor"),Category="rdActor|Conversions")
	AActor* rdConvertInstanceToActorX(const FName sid,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorFromPool",Keywords="rdInst Instance Convert Actor Pool"),Category="rdActor|Conversions")
	AActor* rdConvertInstanceToActorFromPool(UStaticMesh* mesh,UClass* actorClass,int32 index);

	// Converts the Instance to a new Actor in the level from the ActorPool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdConvertInstanceToActorFromPoolX",Keywords="rdInst Instance Convert Actor Pool"),Category="rdActor|Conversions")
	AActor* rdConvertInstanceToActorFromPoolX(const FName sid,UClass* actorClass,int32 index);

	// Helper Function that Converts all the Instances into Actors in the level
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdConvertInstancesToActors",Keywords="rdInst Actor Spawn from Instances"),Category="rdActor|Conversions")
	void rdConvertInstancesToActors(bool stripFolders,bool useRandom,bool group,const FTransform& destTransform,const FVector& centerLocation,const FString& baseFolder,TArray<AActor*>& addedActors);

	// Helper Function that Returns Arrays of all Instances in this actor
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdHarvestInstances",Keywords="rdInst Instances Harvest"),Category="rdActor|Conversions")
	void rdHarvestInstances(bool incRandom,TArray<UStaticMesh*>& meshList,TArray<FTransform>& transformList,TArray<FString>& nameList,TArray<FString>& folderList);

	// Helper Function that Returns Arrays of all Instances in this actor
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdHarvestInstancesX",Keywords="rdInst Instances Harvest"),Category="rdActor|Conversions")
	void rdHarvestInstancesX(bool incRandom,TArray<FName>& sidList,TArray<FTransform>& transformList,TArray<FString>& nameList,TArray<FString>& folderList);

	// Harvests or adds the list of actors into our list of objects
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdHarvestActors",Keywords="rdInst Actor Harvest"),Category="rdActor|Conversions")
	int32 rdHarvestActors(TArray<AActor*> actors,bool harvest);

	// Converts all ISMs and HISMs to StaticMeshComponents
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdConvertISMsToSMs",Keywords="rdInst Instances to StaticMeshes"),Category="rdActor|Conversions")
	void rdConvertISMsToSMs();

	// Converts all StaticMeshComponents to instances
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdConvertSMsToISMs",Keywords="rdInst StaticMeshes to Instances"),Category="rdActor|Conversions")
	void rdConvertSMsToISMs();

// Randomization and Themes -----------------------------------------------------------------------

	// Adds the Randomization from the RandomSettings to the Transform. If this Actor is reliant on another Actor that has not been processed yet, processed is set to false
	bool rdAddRandomization(FTransform& outTransform,FrdRandomSettings* rnd);

	// Utility Node that returns a Random Vector between the two passed in Vector limits
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdRandomVectorInRange",Keywords="rdInst Instance Randomization BP"),Category="rdActor|Randomization")
	FVector rdRandomVectorInRange(const FVector& v1,const FVector& v2);

	// Utility Node that returns a Random Rotator between the two passed in rotation limits, and the Flip Probability Vector
	UFUNCTION(BlueprintCallable,BlueprintPure,meta=(DisplayName="rdRandomRotatorInRange",Keywords="rdInst Instance Randomization BP"),Category="rdActor|Randomization")
	FRotator rdRandomRotatorInRange(const FRotator& r1,const FRotator& r2,const FVector& flipProb);

	// Returns the Current Theme, if this Actor has Parents, the Eldest Parents Theme is returned
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetTheme",Keywords="rdInst Theme Get"),Category="rdActor|Randomization")
	FString rdGetTheme();

	// Tests to see if the passed in Component is included with the current theme. If there is no theme, this returns True
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFolderIncludedWithCurrentTheme",Keywords="rdInst Theme Included"),Category="rdActor|Randomization")
	bool rdFolderIncludedWithCurrentTheme(USceneComponent* comp);

	// Tests to see if the passed in folder name is included with the current theme. If there is no theme, this returns True
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFolderFromNameIncludedWithCurrentTheme",Keywords="rdInst Theme Included"),Category="rdActor|Randomization")
	bool rdFolderFromNameIncludedWithCurrentTheme(const FString& folder);

// Visibility Methods -----------------------------------------------------------------------------

	// Removes and destroys the instances and components
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildAsHidden",Keywords="rdInst Actor Hidden"),Category="rdActor|Visibility")
	void rdBuildAsHidden();

	// Adds and Creates the instances and components
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBuildAsVisible",Keywords="rdInst Actor Visible"),Category="rdActor|Visibility")
	void rdBuildAsVisible();

	// Shows or Hides the Actor along with all its components and instances, just hides objects rather than destroying them
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetActorVisibility",Keywords="rdInst Actor Visibility"),Category="rdActor|Visibility")
	void rdSetActorVisibility(bool vis);

	// Shows or Hides the Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceVisibility",Keywords="rdInst Instance Visibility"),Category="rdActor|Visibility")
	bool rdSetInstanceVisibility(UStaticMesh* mesh,int32 index,bool vis);

	// Shows or Hides the Instance
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceVisibilityX",Keywords="rdInst Instance Visibility"),Category="rdActor|Visibility")
	bool rdSetInstanceVisibilityX(const FName sid,int32 index,bool vis);

	// Shows or Hides the Instance (with ISMC reference)
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstanceVisibilityFast",Keywords="rdInst Instance Visibility Fast"),Category="rdActor|Visibility")
	bool rdSetInstanceVisibilityFast(UInstancedStaticMeshComponent* ismc,int32 index,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibility",Keywords="rdInst Instances Visibility"),Category="rdActor|Visibility")
	void rdSetInstancesVisibility(UStaticMesh* mesh,const TArray<int32>& indexes,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibilityX",Keywords="rdInst Instances Visibility"),Category="rdActor|Visibility")
	void rdSetInstancesVisibilityX(const FName sid,const TArray<int32>& indexes,bool vis);

	// Shows or Hides the Array of Instances
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetInstancesVisibilityFast",Keywords="rdInst Instances Visibility Fast"),Category="rdActor|Visibility")
	void rdSetInstancesVisibilityFast(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indexes,bool vis);

	// Rebuilds the Prefab, Showing or Hiding the Folder of Actors
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRebuildFolderVisibility",Keywords="rdInst Folder Visibility"),Category="rdActor|Visibility")
	void rdBuildFolderVisibility(USceneComponent* comp,const FString& folder,bool vis);

	// Rebuilds the Prefab, Showing or Hiding the Folder of Actors (just from name)
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdRebuildFolderNameVisibility",Keywords="rdInst Folder Visibility"),Category="rdActor|Visibility")
	void rdBuildFolderNameVisibility(const FString& folder,bool vis);

	// Shows or Hides the Folder of Actors
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetFolderVisibility",Keywords="rdInst Folder Visibility"),Category="rdActor|Visibility")
	void rdSetFolderVisibility(USceneComponent* comp,const FString& folder,bool vis);

	// Shows or Hides the Folder of Actors (just from name)
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdSetFolderNameVisibility",Keywords="rdInst Folder Visibility"),Category="rdActor|Visibility")
	void rdSetFolderNameVisibility(const FString& folder,bool vis);

	// Hides all Children and ISMs of this Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdHideAllChildren",Keywords="rdInst Children Hide"),Category="rdActor|Visibility")
	void rdHideAllChildren();

// Assimilation -----------------------------------------------------------------------------------

	// Assimilate all StaticMesh Instances from the specified rdActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAssimilateChildInstances",Keywords="rdInst Children Instances Assimilate"),Category="rdActor|Assimilation")
	void rdAssimilateChildInstances(ArdActor* rdActor,bool justVisible,bool recurse);

	void rdAssimilate(ArdActor* rdActorParent,bool justVisible);
	void rdAssimilateRecurse(ArdActor* rdActorParent,bool justVisible);

// Spawning Methods -------------------------------------------------------------------------------

	// Helper Function that Spawns an actor - this can be called from the ConstructionScript (just call rdDestroyAttachedActors() first)
	UFUNCTION(BlueprintCallable,meta=(DeterminesOutputType="actorClass",DisplayName="rdSpawnActor",Keywords="rdInst Actor Spawn"),Category="rdActor|Spawning")
	AActor* rdSpawnActor(TSubclassOf<class AActor> actorClass,const FTransform& transform,const FName label=TEXT(""),AActor* parent=nullptr,bool deferConstruction=false);

	// Helper Function that Spawns an actor, with extra Position information - this can be called from the ConstructionScript (just call rdDestroyAttachedActors() first)
	UFUNCTION(BlueprintCallable,meta=(DeterminesOutputType="actorClass",DisplayName="rdSpawnActorPos",Keywords="rdInst Actor Spawn"),Category="rdActor|Spawning")
	AActor* rdSpawnActorPos(TSubclassOf<class AActor> actorClass,const FTransform& transform,const FrdPositionInfo& pos,const FName label=TEXT(""));

	// Helper Function that adds a Component
	UFUNCTION(BlueprintCallable,meta=(DeterminesOutputType="compClass",DisplayName="rdAddComponent",Keywords="rdInst Component Add"),Category="rdActor|Spawning")
	UActorComponent* rdAddComponent(TSubclassOf<class UActorComponent> compClass);

	// Helper Function that adds a ChildActorComponent
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddChildComponent",Keywords="rdInst Actor Spawn"),Category="rdActor|Spawning")
	UChildActorComponent* rdAddChildComponent(UClass* actorClass,const FTransform& transform);

	// Helper Function that adds a ChildActorComponent, with extra Position information
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddChildComponentPos",Keywords="rdInst Actor Spawn"),Category="rdActor|Spawning")
	UChildActorComponent* rdAddChildComponentPos(UClass* actorClass,const FTransform& transform,const FrdPositionInfo& pos);

	// Destroys all Actors that have been Spawned with the rdSpawnActor Node
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdDestroyAttachedActors",Keywords="rdInst Actors Destroy"),Category="rdActor|Spawning")
	void rdDestroyAttachedActors();

	// Destroys all ChildComponents that have been added with the rdAddChildComponent Node
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdDestroyAttachedComponents",Keywords="rdInst ChildComponents Destroy"),Category="rdActor|Spawning")
	void rdDestroyAttachedComponents();

// Pooling ----------------------------------------------------------------------------------------

	// Allocates "numToPool" number of the componentClass for rapid reuse
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdPoolComponent",Keywords="rdInst Component Pool"),Category="rdActor|Pooling")
	void rdPoolComponent(UClass* compClass,int32 numToPool,bool premake=true,bool doTick=false,bool startHidden=false,bool reuse=false,bool simplePool=false,int32 growBy=0);

	// ReAllocates "numToPool" number of the compClass
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetComponentPoolSize",Keywords="rdInst Component Pool set Size"),Category="rdActor|Pooling")
	bool rdSetComponentPoolSize(UClass* compClass,int32 numToPool,bool premake=true,bool doTick=false,bool startHidden=false,bool reuse=false,bool simplePool=false,int32 growBy=0);

	// Removes the Pool of Components for the compClass
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveComponentPool",Keywords="rdInst Component Pool"),Category="rdActor|Pooling")
	void rdRemoveComponentPool(UClass* compClass);

	// Gets the next free compClass in the Component Pool, returns nullptr if none are free
	UFUNCTION(BlueprintCallable,meta=(DeterminesOutputType="compClass",DisplayName="rdGetComponentFromPool",Keywords="rdInst Component Pool Get"),Category="rdActor|Pooling")
	UActorComponent* rdGetComponentFromPool(TSubclassOf<class UActorComponent> compClass);

	// Removes an allocated Component from the Component Pool
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveComponentFromPool",Keywords="rdInst Component Pool Remove"),Category="rdActor|Pooling")
	void rdRemoveComponentFromPool(UActorComponent* comp);

	// Event called when an actor is created from the pool
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,meta=(DisplayName="rdOnActorPooled",Keywords="rdInst Actor Pool Added"),Category="rdActor|Pooling")
	void rdOnActorPooled(AActor* actor);

	// Event called when an actor is removed back to the pool
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,meta=(DisplayName="rdOnActorDepooled",Keywords="rdInst Actor Pool Removed"),Category="rdActor|Pooling")
	void rdOnActorDepooled(AActor* actor);

	// Event called when a component is created from the pool
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,meta=(DisplayName="rdOnCompPooled",Keywords="rdInst Component Pool Added"),Category="rdActor|Pooling")
	void rdOnCompPooled(AActor* actor,UActorComponent* comp);

	// Event called when a component is removed back to the pool
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,meta=(DisplayName="rdOnCompDepooled",Keywords="rdInst Component Pool Removed"),Category="rdActor|Pooling")
	void rdOnCompDepooled(AActor* actor,UActorComponent* comp);

	// Returns the amount of Pooled Components for this Class
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetComponentPoolSize",Keywords="rdInst Pool Component Get"),Category="rdActor|Pooling")
	int32 rdGetComponentPoolSize(TSubclassOf<class UActorComponent> compClass);

	// Grows the amount of Pooled Components for this Class by the specified amount
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGrowComponentPool",Keywords="rdInst Pool Component Grow"),Category="rdActor|Pooling")
	int32 rdGrowComponentPool(TSubclassOf<class UActorComponent> compClass,int32 growAmount);

// Utilities --------------------------------------------------------------------------------------

	// Finds the USceneComponent attached to the RootComponent that is a Folder for ChildActors, finding it from the specified Name
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdFindFolderCompFromName",Keywords="rdInst Find Component"),Category="rdActor|Tools")
	USceneComponent* rdFindFolderCompFromName(const FString& folder);

	// Tests if the Actor has moved or changed visibility
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdTestForActorChange",Keywords="rdInst Actor Changed"),Category="rdActor|Tools")
	void rdTestForActorChange(bool testTransform,bool testVisibility);

	// rdReseatMeshInstances
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdReseatMeshInstances",Keywords="rdInst Mesh Reseat"),Category="rdActor|Tools")
	int32 rdReseatMeshInstances(UStaticMesh* mesh,const FVector& shift);

	// rdReseatMeshInstances
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdReseatMeshInstancesX",Keywords="rdInst Mesh Reseat"),Category="rdActor|Tools")
	int32 rdReseatMeshInstancesX(const FName sid,const FVector& shift);

	// rdUpdateInstanceTransforms
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdUpdateInstanceTransforms",Keywords="rdInst Instances Update Transform"),Category="rdActor")
	void rdUpdateInstanceTransforms();

	// rdUpdateInstanceTransformsFromList
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdUpdateInstanceTransformsFromList",Keywords="rdInst Instances Update Transform"),Category="rdActor|Tools")
	void rdUpdateInstanceTransformsFromList(const FrdInstancesMap& selISMCmap);

	// Helper Function that Sets the Label (Editor only) of the Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetActorLabel",Keywords="rdInst Actor Label Set Editor Only"),Category="rdActor|Tools")
	void rdSetActorLabel(const FName label);

	// Gets the Name of the Component Folder
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetFolderName",Keywords="rdInst Folder Name"),Category="rdActor")
	FString rdGetFolderName(USceneComponent* comp);

	// Returns True if the world pointed to by this actor is the same as the game viewport
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsRealWorld",Keywords="rdInst World Real"),Category="rdActor|Tools")
	bool rdIsRealWorld();

	// Returns the Static Array of Dirty rdActors
	static TArray<ArdActor*>&	rdGetDirtyList();

	// Returns the Static Array of rdActor with tick hooks
	static TArray<ArdActor*>&	rdGetTickHookList();

	// Add this Actor to the TickHook List - receive ticks in editor
	UFUNCTION(BlueprintCallable,Category="rdActor|Tools",meta=(DisplayName="rdAddToTickHookList",Keywords="rdInst TickHook Add"))
	void	rdAddToTickHookList();

	// Remove this Actor from the TickHook List
	UFUNCTION(BlueprintCallable,Category="rdActor|Tools",meta=(DisplayName="rdRemFromTickHookList",Keywords="rdInst TickHook Remove"))
	void	rdRemFromTickHookList();

	// The singular Base Actor for Instance Handling and Actor Pooling
	UFUNCTION(BlueprintPure,Category="rdActor|Tools",BlueprintCallable,meta=(DisplayName="rdGetBase",Keywords="rdInst Get Base"))
	ArdInstBaseActor* rdGetBase();

	// Getter and Setter of the SplineInstanceData from rdPopulateSpline and rdPopulateSplinePMC
	TArray<FrdSplineInstanceData>& GetSplineInstanceData();
	void SetSplineInstanceData(const TArray<FrdSplineInstanceData>& instData);

	// Node to get the current RandomStream for the Actor. If this Actor has parents, the Eldest's RandomStream is returned
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetRandomStream",Keywords="rdInst Random Stream Get"),Category="rdActor|Randomization")
	FRandomStream& rdGetRandomStream();

	// Sets the Geometry Collection for the Component and Resets it
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSetGeometryCollection",Keywords="rdInst Geometry Collection Set"),Category="rdActor|Destruction")
	void rdSetGeometryCollection(UGeometryCollectionComponent* gcComp,UGeometryCollection* gc);

// Make and Break Functions -----------------------------------------------------------------------

	// Breaks out the PositionInfo
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakPositionInfo(FrdPositionInfo pos,int32& currentLevel,int32& currentRow,int32& currentColumn,int32& totalLevels,int32& totalRows,int32& totalColumns,int32& RandomSeedOverride,int64& bitField,TArray<float>& custData);

	// Makes a PositionInfo from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdPositionInfo MakePositionInfo(int32 currentLevel,int32 currentRow,int32 currentColumn,int32 totalLevels,int32 totalRows,int32 totalColumns,int32 RandomSeedOverride,int64 bitField,TArray<float> custData);

	// Breaks out the InstanceSetup
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakInstanceSetup(FrdInstanceSetup instanceSetup,TEnumAsByte<ErdSpawnType>& type,FName& sid,TSoftObjectPtr<UStaticMesh>& mesh,TArray<TSoftObjectPtr<UMaterialInterface>>& materials,TEnumAsByte<ErdCollision>& collision,float& startCull,float& endCull,float& streamInDistance,int32& id,FName& groupName);

	// Creates an InstanceSetup from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdInstanceSetup MakeInstanceSetup(TEnumAsByte<ErdSpawnType> type,const FName sid,TSoftObjectPtr<UStaticMesh> mesh,const TArray<TSoftObjectPtr<UMaterialInterface>> materials,ErdCollision collision,float startCull,float endCull,float streamInDistance,int32 id,const FName groupName);

// ----------------

	// Breaks out the PlacementData
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakPlacementData(FrdObjectPlacementData pd,FrdInstanceSetup& m,
									UClass*& actorClass,FString& strProps,UNiagaraSystem*& vfx,
									float& density,float& gridOffsetX,float& gridOffsetY,float& gridVariationW,float& gridVariationH,
									TEnumAsByte<rdSpawnMode>& spawnMode,float& streamDistance,float& localDistance,float& viewFrustumDistance,float& viewFrustumExtend,
									bool& pool,int32& numToPool,FVector& offset,float& minGap,bool& bUniformScale,FVector& minScale,FVector& maxScale,
									bool& bRandomYaw,bool& bAlignToNormal,float& minSlope,bool& bPlaceOnGround,bool& bOnNavMesh,
									bool& bFixSlopePlacement,float& slopedRadius,bool& bUseSplatDensity,uint8& splatChannel,
									float& splatMinRange,float& splatMaxRange,TArray<UPhysicalMaterial*>& filterToPhysicalMaterials,FrdProxySetup& proxy,
									UInstancedStaticMeshComponent*& ismc);

	// Creates a PlacementData from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdObjectPlacementData MakePlacementData(const FrdInstanceSetup& m,
											UClass* actorClass,const FString& strProps,UNiagaraSystem* vfx,
											float density,float gridOffsetX,float gridOffsetY,float gridVariationW,float gridVariationH,
											TEnumAsByte<rdSpawnMode> spawnMode,float streamDistance,float localDistance,float viewFrustumDistance,float viewFrustumExtend,
											bool pool,int32 numToPool,const FVector& offset,float minGap,bool bUniformScale,FVector minScale,FVector maxScale,
											bool bRandomYaw,bool bAlignToNormal,float minSlope,bool bPlaceOnGround,bool bOnNavMesh,bool bFixSlopePlacement,
											float slopedRadius,bool bUseSplatDensity,uint8 splatChannel,float splatMinRange,float splatMaxRange,const TArray<UPhysicalMaterial*>& filterToPhysicalMaterials,
											const FrdProxySetup& proxy,const UInstancedStaticMeshComponent* ismc);

// ----------------

	// Breaks out the FrdInstanceRandomSettings
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakInstanceRandomSettings(FrdInstanceRandomSettings rdInstanceRandomSettings,FString& nm,FString& fld,FTransform& oTran,FTransform& aTran,FrdRandomSettings& randomSettings,bool& noVis,bool& vis,bool& proc,int32& index);

	// Creates a PlacementData from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdInstanceRandomSettings MakeInstanceRandomSettings(FString nm,FString fld,FTransform oTran,FTransform aTran,const FrdRandomSettings& randomSettings,bool noVis,bool vis,bool proc,int32 index);

// ----------------

	// Breaks out the FrdInstanceFastSettings
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakInstanceFastSettings(FrdInstanceFastSettings rdInstanceFastSettings,FTransform& tran,FString& nm,FString& fld,FString& thm,bool& noVis,bool& vis,int32& ind);
	
	// Breaks out the FrdInstanceFastSettings
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdInstanceFastSettings MakeInstanceFastSettings(FTransform tran,FString nm,FString fld,FString thm,bool noVis,bool vis,int32 ind);

// ----------------

	// Creates a FrdAddInstanceFastArray from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakFastItem(FrdAddInstanceFastArray rdAddInstanceFastArray,FrdInstanceSetup& instSetup,TArray<FTransform>& transforms);

	// Breaks out the FrdAddInstanceFastArray
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdAddInstanceFastArray MakeFastItem(const FrdInstanceSetup& instSetup,const TArray<FTransform>& transforms);

// ----------------

	// Breaks out the FrdRandomSettings
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakRandomSettings(FrdRandomSettings rdRandomSettings,FVector& loc1,FVector& loc2,FRotator& rot1,FRotator& rot2,FVector& flpP,uint8& lckA,FVector& scl1,FVector& scl2,float& showP,FString& rlyNm,FString& thms);

	// Creates a FrdRandomSettings from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdRandomSettings MakeRandomSettings(FVector loc1,FVector loc2,FRotator rot1,FRotator rot2,FVector flpP,uint8 lckA,FVector scl1,FVector scl2,float showP,FString rlyNm,FString thms);

// ----------------

	// Breaks out the FrdInstanceRandomSettings
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakRandomizedItem(FrdAddInstanceRandomizedArray rdAddInstanceRandomizedArray,FrdInstanceSetup& instSetup,TArray<FrdInstanceRandomSettings>& randomsettings);

	// Creates a FrdInstanceRandomSettings from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdAddInstanceRandomizedArray MakeRandomizedItem(const FrdInstanceSetup& instSetup,const TArray<FrdInstanceRandomSettings>& randomSettings);

// ----------------

	// Breaks out the FrdSplinePopulateData
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakSplinePopulateData(FrdSplinePopulateData rdSplinePopulateData,UStaticMesh*& mesh,UMaterialInterface*& material,FTransform& offset,float& gap,TEnumAsByte<ESplineMeshAxis::Type>& axis,TArray<float>& customData);

	// Creates a FrdSplinePopulateData from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdSplinePopulateData MakeSplinePopulateData(UStaticMesh* mesh,UMaterialInterface* material,const FTransform& offset,float gap,const ESplineMeshAxis::Type axis,const TArray<float>& customData);

// ----------------

	// Breaks out the FrdSplineInstanceData
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakSplineInstanceData(FrdSplineInstanceData o,FVector& startLocation,FVector& startTangent,FVector& startScale,float& startRoll,FVector& endLocation,FVector& endTangent,FVector& endScale,float& endRoll,USplineMeshComponent*& splineMeshComponent);
//	static void BreakSplineInstanceData(FrdSplineInstanceData o,FVector& startLocation,FVector& startTangent,FVector& startScale,float& startRoll,FVector& endLocation,FVector& endTangent,FVector& endScale,float& endRoll,USplineMeshComponent*& splineMeshComponent,UProceduralMeshComponent*& proceduralMeshComponent);

	// Creates a FrdSplineInstanceData from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdSplineInstanceData MakeSplineInstanceData(const FVector& startLocation,const FVector& startTangent,const FVector& startScale,float startRoll,const FVector& endLocation,const FVector& endTangent,const FVector& endScale,float endRoll,USplineMeshComponent* splineMeshComponent);
//	static FrdSplineInstanceData MakeSplineInstanceData(const FVector& startLocation,const FVector& startTangent,const FVector& startScale,float startRoll,const FVector& endLocation,const FVector& endTangent,const FVector& endScale,float endRoll,USplineMeshComponent* splineMeshComponent,UProceduralMeshComponent* proceduralMeshComponent);

	// Breaks out the FrdProxyItem
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakProxyItem(FrdProxyItem rdProxyItem,FName& sid,UInstancedStaticMeshComponent*& ismc,FName& destroyedMesh,UClass*& destroyedPrefab,FTransform& destroyedOffset,int32& instanceIndex,AActor*& prefab,UClass*& actorClass,int32& proxyInstanceIndex,UInstancedStaticMeshComponent*& proxyInstanceComponent,TSoftObjectPtr<UDataLayerAsset>& proxyDataLayer,FName& proxyStaticMeshSid,float& distance,FTransform& transform,AActor*& proxyActor,float& proxyPhysicsTimeout,TEnumAsByte<rdProxyType>& proxyType,bool& bUsedDestruction,bool& bPooled,bool& bSimplePool,bool& bDontRemove,bool& bDontHide,AActor*& instanceVolume,bool& bDestroyed,UrdStateDataAsset*& ss,bool& bCallSwapEvent,FrdPickup& pickup);

	// Creates a FrdProxyItem from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdProxyItem MakeProxyItem(const FName sid,UInstancedStaticMeshComponent* ismc,const FName destroyedMesh,UClass* destroyedProxy,const FTransform destroyedOffset,int32 instanceIndex,AActor* prefab,UClass* actorClass,int32 proxyInstanceIndex,UInstancedStaticMeshComponent* proxyInstanceComponent,TSoftObjectPtr<UDataLayerAsset> proxyDataLayer,const FName proxyStaticMeshSid,float distance,const FTransform& transform,AActor* proxyActor,float proxyPhysicsTimeout,TEnumAsByte<rdProxyType> proxyType,bool bUseDestruction,bool bPooled,bool bSimplePool,bool bDontRemove,bool bDontHide,AActor* instanceVolume,bool bDestroyed,UrdStateDataAsset* ss,bool bCallSwapEvent,FrdPickup pickup);

	// Breaks out the FrdPickup
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakPickup(FrdPickup pickup,TEnumAsByte<rdPickupType>& pickupType,TEnumAsByte<rdPickupHighlightType>& highlightType,TEnumAsByte<rdPickupHighlightStyle>& highlightStyle,int32& id,float& highlightDistance,float& pickupDistance,float& thickness,FLinearColor& color1,FLinearColor& color2,float& strobeSpeed,float& ebbSpeed,bool& fadeToGround,bool& fadeInDistance,float& respawnTime,bool& respawnLonely,float& respawnDistance,bool& useStencilBuffer);

	// Creates a FrdPickup from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdPickup MakePickup(TEnumAsByte<rdPickupType> pickupType=RDPICKUP_OVERLAP,TEnumAsByte<rdPickupHighlightType> highlightType=RDPICKUPHIGHLIGHT_LOOKATOUTLINE,TEnumAsByte<rdPickupHighlightStyle> highlightStyle=RDHIGHLIGHTSTYLE_SOLID,int32 id=0,float highlightDistance=5000.0f,float pickupDistance=50.0f,float thickness=0.1f,FLinearColor color1=FLinearColor(3.0f,3.0f,0.0f),FLinearColor color2=FLinearColor(3.0f,3.0f,0.0f),float strobeSpeed=0.0f,float ebbSpeed=0.0f,bool fadeToGround=false,bool fadeInDistance=false,float respawnTime=30.0f,bool respawnLonely=false,float respawnDistance=5000.0f,bool useStencilBuffer=false);

	// Breaks out the FrdProxySetup
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakProxySetup(FrdProxySetup rdProxySetup,TSoftClassPtr<AActor>& proxyActor,TSoftObjectPtr<UDataLayerAsset>& proxyDataLayer,TSoftObjectPtr<UStaticMesh>& proxyStaticMesh,TArray<TSoftObjectPtr<UMaterialInterface>>& proxyMeshMaterials,TEnumAsByte<ErdCollision>& proxyMeshCollision,TSoftObjectPtr<UStaticMesh>& destroyedMesh,TArray<TSoftObjectPtr<UMaterialInterface>>& destroyedSMmaterials,TEnumAsByte<ErdCollision>& destroyedSMollision,TSoftClassPtr<AActor>& destroyedPrefab,FTransform& destroyedOffset,TEnumAsByte<rdProxyType>& proxyType,float& scanDistance,float& proxyPhysicsTimeout,
								bool& bUseWithDestruction,bool& bPooled,bool& bSimplePool,int32& pooledAmount,bool& bDontRemove,bool& bDontHide,bool& bEntireMeshProxy,UrdStateDataAsset*& savedState,bool& bCallSwapEvent,FrdPickup& rdPickup);

	// Creates a FrdProxySetup from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc),meta=(AutoCreateRefTerm="proxyMeshMaterials,destroyedMeshMaterials,pickup"))
	static FrdProxySetup MakeProxySetup(TEnumAsByte<rdProxyType> proxyType,TSoftClassPtr<AActor> proxyActor,TSoftObjectPtr<UDataLayerAsset> proxyDataLayer,TSoftObjectPtr<UStaticMesh> proxyStaticMesh,const TArray<TSoftObjectPtr<UMaterialInterface>>& proxyMeshMaterials,TEnumAsByte<ErdCollision> proxyMeshCollision,TSoftObjectPtr<UStaticMesh> destroyedMesh,const TArray<TSoftObjectPtr<UMaterialInterface>>& destroyedMeshMaterials,TEnumAsByte<ErdCollision> destroyedMeshCollision,TSoftClassPtr<AActor> destroyedPrefab,const FTransform destroyedOffset,float scanDistance,float proxyPhysicsTimeout,bool bUseWithDestruction,bool bPooled,bool bSimplePool,int32 pooledAmount,bool bDontRemove,bool bDontHide,bool bEntireMeshProxy,UrdStateDataAsset* savedState,bool bCallSwapEvent,const FrdPickup& pickup);

	// Breaks out the FrdInstanceItems
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakInstanceItems(FrdInstanceItems rdInstanceItem,FName& sid,TArray<int32>& instances,TArray<FTransform>& transforms);

	// Creates a FrdInstanceItems from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdInstanceItems MakeInstanceItems(const FName sid,const TArray<int32>& instances,const TArray<FTransform>& transforms);

	// Breaks out the FrdProxyActorItem
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeBreakFunc))
	static void BreakProxyActorItem(FrdProxyActorItem o,AActor*& proxyActor,int32& proxyInstanceIndex,UInstancedStaticMeshComponent*& proxyInstanceComponent,FrdProxyItem& proxyItem,int32& proxySource,int32& instanceIndex,FTransform& transform,int32& idleCountdown,bool& forDestruction,bool& forLongDistance,bool& forImpact,bool& dontRemove,bool& dontHide);

	// Creates a FrdProxyActorItem from the specified information
	UFUNCTION(BlueprintPure,Category="rdActor",meta=(NativeMakeFunc))
	static FrdProxyActorItem MakeProxyActorItem(AActor* proxyActor,int32 proxyInstanceIndex,UInstancedStaticMeshComponent* proxyInstanceComponent,const FrdProxyItem& proxyItem,int32 proxySource,int32 instanceIndex,const FTransform& transform,int32 idleCountdown,bool forDestruction,bool forLongDistance,bool forImpact,bool dontRemove,bool dontHide);

// ------------------------------------------------------------------------------------------------

	// When True, the whole actor and all ISMs will be hidden by default
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	bool bStartHidden=false;

	// When True, other rdActors can copy our InstanceData and delete us (we're just a container)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	uint8 bCanBeAssimilated:1;

	// The Mode this ArdActor is. Each Mode behaves differently, see the rdActorMode Enumeration
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	TEnumAsByte<rdActorMode> actorMode;

	// Bool to Destroy any Attached Actors when this is Destroyed
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	uint8 bDestroyAttachedActors:1;

	// Bool when set, recurses all components when setting the visibility
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	uint8 bRecurseVisibilityChanges:1;

	// True when StaticMesh mode is on (ISMs are replaced with SMs)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	uint8 bStaticMeshMode:1;

	// Set to true to edit individual instances
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	uint8 bEditInstances:1;

	// TickHookOnlyOneShot
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Defaults")
	uint8 bTickHookOneShot:1;


	// Internal collated list of instances (do not touch) - now 
	UPROPERTY(EditDefaultsOnly,Category="rdActor|System",meta=(DisplayName="(Do Not Edit)"))
	TMap<UStaticMesh*,FrdInstanceSettingsArray> InstanceData;

	// Internal collated list of instances (do not touch)
	UPROPERTY(EditDefaultsOnly,Category="rdActor|System",meta=(DisplayName="(Do Not EditX)"))
	TMap<FName,FrdInstanceSettingsArray> InstanceDataX;

	// When ticked, the instances are created from the InstanceFastArray and the InstanceRandomizedArray. Please note that ticking this in an existing BP will REMOVE all existing instance data created from older versions
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Instancing")
	uint8 bCreateFromArrays:1;

	// This is an array of UStaticMeshes and transforms for adding as instances (Fast, no randomization)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Instancing")
	TArray<FrdAddInstanceFastArray> InstanceFastArray;

	// This is an array of UStaticMeshes and transforms for adding as instances (Randomized)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Instancing")
	TArray<FrdAddInstanceRandomizedArray> InstanceRandomizedArray;

	// This contains the position in row/column/layer of a built system (see rdBuildBuddy)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Building")
	FrdPositionInfo positionInfo;


	// Set to false to be ignored by proxy scans
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Proxies")
	uint8 bScanForProxy:1;

	// the index of the instance for a proxy instance
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Proxies")
	int32 proxyInstanceIndex=-1;

	// the ISMC of the instance for a proxy instance
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Proxies")
	UInstancedStaticMeshComponent* proxyInstanceComponent=nullptr;

	// A DataAsset containing all the data of a prefab. Will be used in next release for dynamic prefab swapping and template pooled actors
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|System")
	UrdInstanceVault*	instanceVault=nullptr;



	// The Folder of Actors to Show when this Actor is in ThereCanBeOnlyOne Mode
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	FString showFolder;

	// The Current Theme name, can be blank meaning themes are disabled
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	FString currentTheme;

	// The Location Randomization for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	FVector locMultiplier;

	// The Rotation Randomization for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	FRotator rotMultiplier;

	// The Scale Randomization for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	FVector scaleMultiplier;

	// The Show Probability for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	float showMultiplier;

	// When True, a new Random Seed is generated every time this Actor is Constructed
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	uint8 bRandomStart:1;

	// The Seed to use for the RandomStream
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	int32 randomSeed=0;

	// When this is True, the RandomStream specified in this Actor is used, otherwise the OuterMost ArdActor's one is used
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	uint8 bOverrideRandom:1;

	// The RandomStream used by this Actor. If this Actor has Outer ArdActors, their RandomStream is used for congruity
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	FRandomStream randomStream;

	// When this is True, the Theme specified in this Actor is used, otherwise the OuterMost ArdActor's Theme is used
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor|Randomization")
	uint8 bOverrideTheme:1;


protected:

	// Our base Actor that manages the ISMs - gets created if it doesn't exist in the level
	bool			rdGetBaseActor();

	// Returns True if the specified actor or component has Randomization settings
	bool			rdHasRandomTags(AActor* actor);
	bool			rdHasRandomTags(UActorComponent* comp);

	// Fill in the FrdRandomSettings from the passed in Actor, if the UAssetData is not present, it's created from the Tags
	void			rdSetRandomFromJustTags(TArray<FName>& tags,FrdRandomSettings* options);
	void			rdSetRandomFromActor(AActor* actor,FrdRandomSettings* options);
	void			rdSetRandomFromComponent(UActorComponent* comp,FrdRandomSettings* options);

	// Gets the Original FTransform of the USceneComponent, if the UAssetData is not present, it's created from the Tags
	FTransform		rdGetOrigTransform(USceneComponent* cac);

	static FString rdPackMaterialName(const FString& nm);
	static FString rdUnpackMaterialName(const FString& nm);

public:

	UFUNCTION(BlueprintImplementableEvent,CallInEditor,meta=(DisplayName="rdLoadProxyState",Keywords="rdInst Proxy Load State"),Category="rdActor|Events")
	bool	rdLoadProxyState(UrdStateDataAsset* state,AActor* proxy,UStaticMesh* mesh,int32 index=-1,ArdActor* prefab=nullptr);

	UFUNCTION(BlueprintImplementableEvent,CallInEditor,meta=(DisplayName="rdSaveProxyState",Keywords="rdInst Proxy Save State"),Category="rdActor|Events")
	bool	rdSaveProxyState(UrdStateDataAsset* state,AActor* proxy,UStaticMesh* mesh,int32 index=-1,ArdActor* prefab=nullptr);

	// Pointer to the rdInst BaseActor that does all the ISM juggling
	ArdInstBaseActor*		rdBaseActor=nullptr;

private:

	bool					lastIsVis=false;
	FTransform				lastActorTransform;

	USceneComponent*		rdFindComponentByName(const FString& name);
	bool					rdRelianceChainVisible(const FString& name);
	bool					rdRelianceChainVisibleComponent(const FString& name);
	void					rdBuildRandomizedComponent(USceneComponent* comp);

	UChildActorComponent*	rdAddChildActorComponent(UChildActorComponent* srcCac);
	AActor*					rdSpawnActor(AActor* srcActor);
	bool					rdSetRandomFromTags(TArray<FName>& tags,FrdRandomSettings* options);
	void					rdCreateTagsFromRandom(TArray<FName>& tags,const FrdRandomSettings& random);

	// Helpers to parse strings for FVectors and FRotators - used for the Tags fallback
	bool					getThreeRotators(const FString& str,FRotator& r1,FRotator& r2,FVector& v);
	bool					getTwoRotators(const FString& str,FRotator& r1,FRotator& r2);
	bool					getOneRotator(const FString& str,FRotator& r1);
	bool					getTwoVectors(const FString& str,FVector& v1,FVector& v2);
	bool					getOneVector(const FString& str,FVector& v1);

	TMap<UClass*,rdPoolData>		compPoolMap;
	TArray<FrdSplineInstanceData>   splineInstanceData;
public:
	TArray<FString>					alterations;
	int32							dirtyCountDown=0;
};

UCLASS(BlueprintType)
class UrdInstanceVault : public UrdStateDataAsset {
	GENERATED_BODY()
public:

	// The Current Theme name, can be blank meaning themes are disabled
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	FString currentTheme;

	// When True, the whole actor and all ISMs will be hidden by default
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	bool bStartHidden=false;

	// The Location Randomization for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	FVector locMultiplier;

	// The Rotation Randomization for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	FRotator rotMultiplier;

	// The Scale Randomization for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	FVector scaleMultiplier;

	// The Show Probability for each of the child Actors is multiplied with this
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	float showMultiplier;

	// When True, other rdActors can copy our InstanceData and delete us (we're just a container)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	uint8 bCanBeAssimilated:1;

	// When True, a new Random Seed is generated every time this Actor is Constructed
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	uint8 bRandomStart:1;

	// The Seed to use for the RandomStream
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	int32 randomSeed=0;

	// When this is True, the RandomStream specified in this Actor is used, otherwise the OuterMost ArdActor's one is used
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	uint8 bOverrideRandom:1;

	// The RandomStream used by this Actor. If this Actor has Outer ArdActors, their RandomStream is used for congruity
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	FRandomStream randomStream;

	// When this is True, the Theme specified in this Actor is used, otherwise the OuterMost ArdActor's Theme is used
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	uint8 bOverrideTheme:1;

	// The Mode this ArdActor is. Each Mode behaves differently, see the rdActorMode Enumeration
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	TEnumAsByte<rdActorMode> actorMode;

	// The Folder of Actors to Show when this Actor is in ThereCanBeOnlyOne Mode
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	FString showFolder;

	// Internal collated list of instances (do not touch)
	UPROPERTY(EditDefaultsOnly,Category="rdActor",meta=(DisplayName="(Do Not Edit)"))
	TMap<UStaticMesh*,FrdInstanceSettingsArray> InstanceData;

	// Internal collated list of instances (do not touch)
	UPROPERTY(EditDefaultsOnly,Category="rdActor System",meta=(DisplayName="(Do Not EditX)"))
	TMap<FName,FrdInstanceSettingsArray> InstanceDataX;

	// When ticked, the instances are created from the InstanceFastArray and the InstanceRandomizedArray. Please note that ticking this in an existing BP will REMOVE all existing instance data created from older versions
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	uint8 bCreateFromArrays:1;

	// This is an array of UStaticMeshes and transforms for adding as instances (Fast, no randomization)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	TArray<FrdAddInstanceFastArray> InstanceFastArray;

	// This is an array of UStaticMeshes and transforms for adding as instances (Randomized)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	TArray<FrdAddInstanceRandomizedArray> InstanceRandomizedArray;

	// This contains the position in row/column/layer of a built system (see rdBuildBuddy)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdActor")
	FrdPositionInfo positionInfo;
};
