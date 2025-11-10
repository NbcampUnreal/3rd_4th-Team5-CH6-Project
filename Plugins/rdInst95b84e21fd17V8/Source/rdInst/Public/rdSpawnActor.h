// rdSpawnActor.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "Components/BoxComponent.h"
#include "LandscapeProxy.h"
#include "rdSpawn.h"
#include "rdEntities.h"
#include "rdSpawnActor.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

USTRUCT(BlueprintType)
struct FrdProxyStateItem {
	GENERATED_BODY()
public:
	FName				sid;
	int32				index=-1;
	UrdStateDataAsset*	state=nullptr;
};

// The AActor class you can drag into your level or subclass from and add your own populating systems
UCLASS(HideCategories=(rdHidden,rdActor,"rd Actor| Conversions","rd Actor| Build",Rendering,Collision,HLOD,Physics,Networking,Input,LevelInstance,Cooking,DataLayers,rdInstProxies))
class RDINST_PLUGIN_API ArdSpawnActor : public ArdActor {

	GENERATED_BODY()
public:

	ArdSpawnActor();
	ArdSpawnActor(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;
	virtual void Destroyed() override;

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION<5
	virtual void PreSave(const class ITargetPlatform* TargetPlatform);
#else
	virtual void PreSave(FObjectPreSaveContext context);
#endif
#endif

	virtual void rdBuild() override;

	// Gets the current SpawnDatas BakedDataAsset, note that this can be a duplicate when 'sharedDataAsset' is unticked
	UFUNCTION(BlueprintCallable,meta=(DisplayName="SetBakedDataAsset",Keywords="Baked DataAsset Set"),Category="rdSpawnActor")
	UrdBakedDataAsset* GetBakedDataAsset(bool bLoad=true);

	// Sets the SpawnData to an existing BakedDataAsset
	UFUNCTION(BlueprintCallable,meta=(DisplayName="SetBakedDataAsset",Keywords="Baked DataAsset Set"),Category="rdSpawnActor")
	void SetBakedDataAsset(UrdBakedDataAsset* dataAsset);

	// Synchronisly Loads the Baked Data and contained Assets
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBlockingLoad",Keywords="Load Assets Async"),Category="rdSpawnActor")
	void rdBlockingLoad();

	// Loads the Baked Data Asset and contained Assets in another thread
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAsyncLoad",Keywords="Load Assets Async"),Category="rdSpawnActor")
	void rdAsyncLoad();

	// Synchronisly loads the Assets referenced in the Baked/Unbaked Data
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBlockingLoadAssets",Keywords="Load Assets Async"),Category="rdSpawnActor")
	void rdBlockingLoadAssets();

	// Loads all the Assets referenced in the Unbaked Data in another thread
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAsyncLoadAllAssets",Keywords="Load All Assets Async"),Category="rdSpawnActor")
	void rdAsyncLoadAllAssets();
	void rdAsyncLoadAllAssetsFinished();

	// Loads all the Assets referenced in the Baked Data in another thread
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAsyncLoadAllBakedAssets",Keywords="Load All Baked Assets Async"),Category="rdSpawnActor")
	void rdAsyncLoadAllBakedAssets();
	void rdAsyncLoadAllBakedAssetsFinished();

	// Loads the Assets referenced in the Baked Data marked as "To Load" in another thread
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAsyncLoadMarkedBakedAssets",Keywords="Load Marked Baked Assets Async"),Category="rdSpawnActor")
	void rdAsyncLoadMarkedBakedAssets();
	void rdAsyncLoadMarkedBakedAssetsFinished();

	// Unloads the Spawn Baked Data
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUnloadData",Keywords="Unload Spawn Data"),Category="rdSpawnActor")
	void rdUnloadData();

	// Unloads all Assets (not used in the rest of the level) from this SpawnActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUnloadAllAssets",Keywords="Unload All Spawn Assets"),Category="rdSpawnActor")
	void rdUnloadAllAssets();

	// Unloads all marked Assets (not used in the rest of the level) from this SpawnActor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUnloadMarkedAssets",Keywords="Unload Marked Spawn Assets"),Category="rdSpawnActor")
	void rdUnloadMarkedAssets();

	// Fully load all baked data and Asset data
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdFullyLoad",Keywords="Spawner load"),Category="rdSpawnActor")
	void rdFullyLoad();

	// Fully unload all baked data and Asset data
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdFullyUnload",Keywords="Spawner Unload"),Category="rdSpawnActor")
	void rdFullyUnload();

	// Save the DataAsset
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdSaveDataAsset",Keywords="Spawner Save"),Category="rdSpawnActor")
	bool rdSaveDataAsset();

	// SaveAs the DataAsset
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdSaveAsDataAsset",Keywords="Spawner Save"),Category="rdSpawnActor")
	bool rdSaveAsDataAsset(const FString& name);

	// If the amount of objects in your SpawnActors is small (under 20,000 objects) the data needed is small so Serializing works well. Alterations are stored here
	virtual void Serialize(FArchive& Ar) override;


	// Clears the List of Alterations
	virtual void rdClearAlteredList() override;

	// Builds a List of Alterations of the SpawnActors Baked data (each remove and change function stores the alterations)
	virtual FString rdBuildAlteredList() override;

	// Applies the List of Alterations to the SpawnActors Baked data
	virtual int32 rdApplyAlteredList(const FString& str) override;

	// Spawns the Objects described in the list of items
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdSpawn",Keywords="Spawn Objects Placement"),Category="rdSpawnActor")
	virtual void rdSpawn();

	void rdSpawnNewlyStreamed();

	// Returns the origin of the SpawnActor (center - half bounds) as a Transform
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdGetSpawnOriginTransform",Keywords="Spawn Actor Origin"),Category="rdSpawnActor")
	FTransform rdGetSpawnOriginTransform() const;

	// Returns the origin of the SpawnActor (center - half bounds)
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdGetSpawnOrigin",Keywords="Spawn Actor Origin"),Category="rdSpawnActor")
	FVector rdGetSpawnOrigin() const;

	void ProcessStreamingObjects(float dist);

	// Marks the SpawnActor ready to Spawn when possible
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdMarkForSpawn",Keywords="Spawn Objects Placement"),Category="rdSpawnActor")
	virtual void rdMarkForStreamIn(FrdBakedSpawnObjects& bso);

	// Marks the SpawnActor ready to Despawn when possible
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdMarkForDespawn",Keywords="Despawn Objects Placement"),Category="rdSpawnActor")
	virtual void rdMarkForStreamOut(FrdBakedSpawnObjects& bso);

	void ProcessEntity(rdThreadResult* tr);

	// Overridable function to do custom spawning
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,meta=(DisplayName="rdSpawnEvent",Keywords="Spawn Objects Placement Event"),Category="rdSpawnActor")
	void rdSpawnEvent();

	// Overridable function to bake the custom spawning
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,meta=(DisplayName="rdBakeEvent",Keywords="Bake Objects Placement Event"),Category="rdSpawnActor")
	void rdBakeEvent();

	// Add Instances you've spawned in Custom Spawners to this list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddSpawnedInstance",Keywords="Spawned Instance Add"),Category="rdSpawnActor")
	void rdAddSpawnedInstance(const FrdObjectPlacementData& placementData,int32 instance);

	// Add Actors and Prefabs you've spawned in Custom Spawners to this list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddSpawnedActor",Keywords="Spawned Actor Add"),Category="rdSpawnActor")
	void rdAddSpawnedActor(const FrdObjectPlacementData& placementData,AActor* actor);

	// Add VFX you've spawned in Custom Spawners to this list
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddSpawnedVFX",Keywords="Spawned VFX Add"),Category="rdSpawnActor")
	void rdAddSpawnedVFX(const FrdObjectPlacementData& placementData,UNiagaraComponent* vfx);

	// Remove the Instance from the Spawn Actor specified by Instance Index
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rd",Keywords="Spawned Instance Remove Index"),Category="rdSpawnActor")
	bool rdRemoveInstanceByIndex(const FName sid,int32 index);

	// Remove the Instance from the Spawn Actor specified by Location
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rd",Keywords="Spawned Instance Remove Location"),Category="rdSpawnActor")
	bool rdRemoveInstanceByLocation(const FName sid,const FVector& location);

	// Remove the Actor from the Spawn Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rd",Keywords="Spawned Actor Remove"),Category="rdSpawnActor")
	bool rdRemoveActor(const AActor* actor);

	// Remove the VFX from the Spawn Actor
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rd",Keywords="Spawned VFX Remove"),Category="rdSpawnActor")
	bool rdRemoveVFX(const UNiagaraComponent* vfx);


	// Remove the Instances from the Spawn Actor that fall within specified radius of the Location
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveInstancesWithinRadius",Keywords="Spawned Instance Remove Location"),Category="rdSpawnActor")
	int32 rdRemoveInstancesWithinRadius(const FName sid,const FVector& location,const float radius);

	// Remove All Instances from the Spawn Actor that fall within specified radius of the Location
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveAllInstancesWithinRadius",Keywords="Spawned Instance Remove Location"),Category="rdSpawnActor")
	int32 rdRemoveAllInstancesWithinRadius(const FVector& location,const float radius);

	// Remove All Objects from the Spawn Actor that fall within specified radius of the Location
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdRemoveAllObjectsWithinRadius",Keywords="Spawned Objects Remove Location"),Category="rdSpawnActor")
	int32 rdRemoveAllObjectsWithinRadius(const FVector& location,const float radius);

	// Changes all Instances of Sid1 with Sid2 within the radius of the Location
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdChangeInstancesWithinRadius",Keywords="Spawned Objects Remove Location"),Category="rdSpawnActor")
	int32 rdChangeInstancesWithinRadius(const FName sid1,const FName sid2,const FVector& location,const float radius);

	// Changes all Instances of the Sid1 Array with the Sid2 Array within the radius of the Location
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdChangeInstanceListWithinRadius",Keywords="Spawned Objects Remove Location"),Category="rdSpawnActor")
	int32 rdChangeInstanceListWithinRadius(const TArray<FName>& sidList1,const TArray<FName> sidList22,const FVector& location,const float radius);

	// Spawn the Object from the PlacementData at the transform
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSpawnObject",Keywords="Spawn Object"),Category="rdSpawnActor")
	void rdSpawnObject(UPARAM(ref) FrdObjectPlacementData& placementData,const FTransform& transform);

	// Add Objects to the Baked Data in Custom Spawners with this function
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddBakedObject",Keywords="Spawner Baked Object Add"),Category="rdSpawnActor")
	void rdAddBakedObject(const FrdObjectPlacementData& placementData,const FTransform& transform);

	// Add Mesh Instances to the Baked Data in Custom Spawners with this function
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddBakedInstance",Keywords="Spawner Baked Mesh Instance Add"),Category="rdSpawnActor")
	void rdAddBakedInstance(const FrdObjectPlacementData& placementData,UStaticMesh* mesh,const FTransform& transform);

	// Add Mesh Instances to the Baked Data in Custom Spawners with this function
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddBakedInstanceX",Keywords="Spawner Baked Mesh Instance Add"),Category="rdSpawnActor")
	void rdAddBakedInstanceX(const FrdObjectPlacementData& opd,const FName sid,const FTransform& t);

	// Add Actors to the Baked Data in Custom Spawners with this function
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddBakedActor",Keywords="Spawner Baked Actor Add"),Category="rdSpawnActor")
	void rdAddBakedActor(const FrdObjectPlacementData& placementData,UClass* actorClass,const FTransform& transform);

	// Add VFX to the Baked Data in Custom Spawners with this function
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddBakedVFX",Keywords="Spawner Baked VFX Add"),Category="rdSpawnActor")
	void rdAddBakedVFX(const FrdObjectPlacementData& placementData,UNiagaraSystem* vfx,const FTransform& transform);

	// Add Mesh Instances to the Baked Data in Custom Spawners or PCG nodes with this function
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdAddBakedInstances",Keywords="Spawner Baked Mesh Instances Add"),Category="rdSpawnActor")
	void rdAddBakedInstances(const FName sid,const TArray<FTransform>& t);


	// Bakes the spawn items into lists ready to be spawned very quickly
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdBakeSpawnData",Keywords="Data Bake"),Category="rdSpawnActor")
	virtual void rdBakeSpawnData(const FrdSpawnData& bakeData);

	// Bakes the spawn items into lists ready to be spawned very quickly
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdBakeSpawnedData",Keywords="Data Bake"),Category="rdSpawnActor")
	virtual void rdBakeSpawnedData();

	// Clears the Baked Data
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdClearBakedData",Keywords="Data Bake"),Category="rdSpawnActor")
	virtual void rdClearBakedData();

	// Removes all the Spawned Objects
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdClearSpawn",Keywords="Data Bake"),Category="rdSpawnActor")
	virtual void rdClearSpawn(bool justMarked=false);

	// Updates the Placement Data from any DataAssets used
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdUpdateFromDataAssets",Keywords="Data Update Assets"),Category="rdSpawnActor")
	virtual void rdUpdateFromDataAssets();

	// Does a SphereTrace, returning found actors and instances, if there is Baked data, it scans those transforms
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdSphereTrace",Keywords="rdInst Sphere Trace"),Category="rdSpawnActor")
	void rdSphereTrace(const FVector location,float radius,TArray<AActor*>& actors,TArray<FrdInstanceItems>& instances,TArray<AActor*>& ignoreActors);

	// Returns the Distance at which to spawn objects when using the population spawn types
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSpawnDistance",Keywords="Spawn Distance"),Category="rdSpawnActor")
	double rdGetSpawnDistance();

	// Returns the Bounds of the Spawner Volume
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSpawnBounds",Keywords="Spawn Bounds"),Category="rdSpawnActor")
	const FBoxSphereBounds rdGetSpawnerBounds() const;

	// Returns the BoundingBox of the Spawner Volume
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSpawnBoundingBox",Keywords="Spawn BoundingBox"),Category="rdSpawnActor")
	const FBox rdGetSpawnerBoundingBox() const;

	// Returns the Center of the Spawner Volume in World Space
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSpawnCenter",Keywords="Spawn Center"),Category="rdSpawnActor")
	const FVector rdGetSpawnerCenter() const;

	// Returns the Size of the Spawner Volume in Unreal Units
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSpawnSize",Keywords="Spawn Size"),Category="rdSpawnActor")
	const FVector rdGetSpawnerSize() const;

	// Returns the Radius of the Spawner Volume in Unreal Units
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdGetSpawnRadius",Keywords="Spawn Radius"),Category="rdSpawnActor")
	const float rdGetSpawnerRadius() const;

	// Set the Spawner to the same size as the landscape
	UFUNCTION(BlueprintCallable,CallInEditor,meta=(DisplayName="rdSnapToLandscape",Keywords="Landscape Snap"),Category="rdSpawnActor")
	void rdSnapToLandscape();

	// Finds the landscape z position at 'loc', optionally ignoring physmats and fixing slopes
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdTraceToLandscape",Keywords="Landscape Trace"),Category="rdSpawnActor")
	bool rdTraceToLandscape(const FrdObjectPlacementData& opd,const FTransform& transform,FTransform& outTransform,bool addToZ=false);

	// Returns True if 'loc' is inside the NavMesh space or no NavMesh
	UFUNCTION(BlueprintCallable,meta=(DisplayName="rdIsOnNavMesh",Keywords="On NavMesh"),Category="rdSpawnActor")
	bool rdIsOnNavMesh(const FVector& loc,const FVector& ext);

	// Returns an Array of all PlacementData instances
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetPlacementData",Keywords="rdSpawnActor All PlacementData"),Category="rdSpawnActor")
	TArray<FrdObjectPlacementData>& rdGetPlacementData();

	// Returns the ProxyState Pointer (if exists) for the instance
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetProxyState",Keywords="rdSpawnActor ProxyState Pointer"),Category="rdSpawnActor")
	UrdStateDataAsset* rdGetProxyState(const FName sid,int32 index);

	// Returns an Array of all ProxyState Pointers
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetAllProxyStates",Keywords="rdSpawnActor All ProxyState Pointers"),Category="rdSpawnActor")
	TArray<FrdProxyStateItem> rdGetAllProxyStates();

	// Returns an Array of all ISMCs used
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetUsedISMCs",Keywords="rdSpawnActor used ISMCs"),Category="rdSpawnActor")
	TArray<UInstancedStaticMeshComponent*> rdGetUsedISMCs();

	// Returns an Array of all Meshes used
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetUsedMeshes",Keywords="rdSpawnActor used Meshes"),Category="rdSpawnActor")
	TArray<UStaticMesh*> rdGetUsedMeshes();

	// Returns an Array of all SIDs used
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetUsedSIDs",Keywords="rdSpawnActor used SIDs"),Category="rdSpawnActor")
	TArray<FName> rdGetUsedSIDs();

	// Returns an Array indices being used for the mesh in the SpawnActor
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetInstanceIndices",Keywords="rdSpawnActor used ISMCs"),Category="rdSpawnActor")
	TArray<int32>& rdGetInstanceIndices(UStaticMesh* mesh);

	// Returns an Array indices being used for the mesh in the SpawnStuffActor
	UFUNCTION(BlueprintPure,meta=(DisplayName="rdGetInstanceIndicesX",Keywords="rdSpawnActor used ISMCs"),Category="rdSpawnActor")
	TArray<int32>& rdGetInstanceIndicesX(const FName sid);

	// Shows objects that have come into view and hides objects no longer in view
	//void UpdateLocalArea(rdThreadResult* tr);

// data
#if WITH_EDITORONLY_DATA
	// Existing BakedSpawnData DataAsset to use
	UPROPERTY(EditInstanceOnly,Category="0. DragIn Objects")
	UrdBakedDataAsset* existingDataAsset;

	// Drag Meshes from the content browser into this list in the editor to populate the spawn data
	UPROPERTY(EditInstanceOnly,Category="0. DragIn Objects")
	TArray<UStaticMesh*> dragInMeshes;

	// Drag Actors or Placement DataAssets from the content browser into this list in the editor to populate the spawn data
	UPROPERTY(EditInstanceOnly,Category="0. DragIn Objects")
	TArray<UObject*> dragInActorsAndDataAssets;

	// Drag NiagaraSystems from the content browser into this list in the editor to populate the spawn data
	UPROPERTY(EditInstanceOnly,Category="0. DragIn Objects")
	TArray<UNiagaraSystem*> dragInVFX;
#endif

	// Object Data that gets spawned
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="1. Object Data")
	TArray<FrdSpawnData> spawnData={FrdSpawnData()};

	// Set to true to include the objects Randomization Settings
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="1. Object Data")
	bool bIncludeRandomization=false;

	// Random Seed for the Randomization
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="1. Object Data")
	int32 randomizationSeed=0;

	// Theme for the Randomization
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="1. Object Data")
	FString theme;

	// Current SpawnData index (usually controlled by the scalability level)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="1. Object Data")
	int32 currentSpawnDataIndex=0;

#if WITH_EDITOR
	bool bForceShowAllItemsDetails=false;
#endif

#if WITH_EDITORONLY_DATA
	// True when this SpawnActor has spawned
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bDrawInEditor=true;
#endif

	// True when this SpawnActor has spawned
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bCleared=false;

	// Spawn Mode for this SpawnActor
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	TEnumAsByte<rdSpawnMode> spawnMode=rdSpawnMode::RDSPAWNMODE_ALL;

	// Array of distances used to stream in objects in the SpawnActor, each type has an index into this array to use as its distance
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	float defaultStreamDistance=0.0f;

	// When True, the SpawnActors objects have their streaming distance assigned by their size
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bStreamCalcDistancesBySize=true;

	// When True, the SpawnActors objects have their streaming distance assigned by their max size of the object with its instances scales
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bStreamDeepCalcDistances=false;

	// When True, the SpawnActors Baked Data and Assets are only loaded the first time it's spawned
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bLoadOnDemand=true;

	// Current Load State of this SpawnActor
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="2. Misc Data")
	TEnumAsByte<rdLoadState> loadState=rdLoadState::RDLOADSTATE_UNLOADED;

#if WITH_EDITORONLY_DATA
	// Editor Load State of this SpawnActor
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	TEnumAsByte<rdLoadState> editorLoadState=rdLoadState::RDLOADSTATE_FULLYLOADED;

	TEnumAsByte<rdLoadState> lastEditorLoadState=rdLoadState::RDLOADSTATE_FULLYLOADED;
#endif
//@@
	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	//UStaticMesh* nmesh=nullptr;
//@@

	// When True, the SpawnActors Baked Data and Assets are released from memory when it's hidden
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bReleaseOnHide=true;

	// The Radius to spawn actors from the player when using one of the Local rdSpawnModes
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	float localRadius=10000.0f;

	// The Distance to spawn actors within the View Frustum of the player when using rdSpawnMode::ViewFrustum
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	float viewFrustumDistance=50000.0f;

	// The Extended area around the View Frustum (in degrees) to spawn when using rdSpawnMode::ViewFrustum
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	float viewFrustumExtend=2.0f;

	// When True, objects are spawned relative to the SpawnActor
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bRelativeSpawning=true;

	// True when this SpawnActor has spawned
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool hasSpawned=false;

	// non-zero when objects are still spawning with distributed spawns
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	uint8 isStillSpawning=0;

	// Pointer to the Landscape
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	ALandscapeProxy* landscape=nullptr;

	// Pointer to the Landscape
	UPROPERTY(BlueprintReadOnly,Category="2. Misc Data")
	FVector4 landscapeScale=FVector4(0.0f,0.0f,1.0f,1.0f);

	// True to use StaticMesh landscapes
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool bSpawnOnStaticMeshes=true;

	// Root Component
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="2. Misc Data")
	TObjectPtr<USceneComponent> popRootComponent=nullptr;

	// Volume Box
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	UBoxComponent* volumeBox=nullptr;

	// Physical materials to ignore when linetracing
	UPROPERTY(Category="2. Misc Data",EditAnywhere)
	TArray<UPhysicalMaterial*> ignorePhysicalMatsList;

	// Collision Profile for linetracing
	UPROPERTY(Category="2. Misc Data",EditAnywhere)
	FName collisionProfile;

	// used with the scanning routines
	UPROPERTY()
	FVector spawnActorCenter;

	UPROPERTY()
	float	spawnActorRadius=0.0f;

	// used with the scanning routines
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	FVector currentLocation;

	// used with the scanning routines
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	FRotator currentRotation;

	// used with the scanning routines
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	FVector lastLocation;

	// used with the scanning routines
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	FRotator lastRotation;

	// The types of scanning to use for this SpawnActor, 
	//		 0 = no scanning
	//		 1 = proxy scanning
	//		 2 = global collision scanning
	//		 4 = local collision scanning
	//		 8 = global mass movement processing
	//		16 = local mass movement processing
	//		32 = local or frustrum updating
	//		64 = mass physics updating
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	int32	scanType=0;

	// The maxiumum distance of contained proxy scanning distances, calculated when spawning
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	float   maxProxyScanDistance=0.0f;

	// When True, mass movement is run through this SpawnActor
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool   bHasMassMovement=false;

	// When True, mass physics is run through this SpawnActor
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool   bHasMassPhysics=false;

	// When True, the BakedDataAsset is directly referenced, duplicates will have conflicting instance indexes for proxies
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="2. Misc Data")
	bool   bSharedDataAsset=false;

	TMap<FName,FrdProxyInstanceItems>			proxyItemsMap;
	TMap<FName,TArray<int32>>					ismIndexList;
};
