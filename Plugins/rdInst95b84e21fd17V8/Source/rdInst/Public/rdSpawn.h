// rdSpawn.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "rdProxies.h"
#include "rdPlacement.h"
#include "rdSpawnMode.h"
#include "ComponentInstanceDataCache.h"
#include "rdGridArray.h"
#include "rdSpawn.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

class ArdSpawnActor;

// Contains the type of object to create and a list of the transforms
USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdBakedSpawnObjects {
	GENERATED_BODY()
public:
	FrdBakedSpawnObjects() {}
	FrdBakedSpawnObjects(const FName s) : sid(s) {}
	FrdBakedSpawnObjects(const FName s,TArray<FTransform>& t) : sid(s) { transformMap.Add(t); }
	FrdBakedSpawnObjects(UClass* ac,const FString& at,bool p,TArray<FTransform>& t) : actorClass(ac),strProps(at),pooled(p),itemIndex(-1) { transformMap.Add(t); }
	FrdBakedSpawnObjects(UNiagaraSystem* fx,TArray<FTransform>& t) : vfx(fx) { transformMap.Add(t); }

	void rdAsyncLoadEntityAssets();
	void rdAsyncLoadEntityAssetsFinished();

	UPROPERTY(Category=rdBakedSpawnObjects,EditAnywhere)
	FName sid;

	UPROPERTY(Category=rdBakedSpawnObjects,EditAnywhere)
	UClass*			actorClass=nullptr;

	UPROPERTY(Category=rdBakedSpawnObjects,EditAnywhere,meta=(AssetBundles="BakedData"))
	FString			strProps;

	UPROPERTY(Category=rdBakedSpawnObjects,EditAnywhere)
	UNiagaraSystem*	vfx=nullptr;

	UPROPERTY(Category=rdBakedSpawnObjects,EditAnywhere)
	bool			pooled=false;

	UPROPERTY(Category=rdBakedSpawnObjects,EditAnywhere)
	int32			itemIndex=-1;

	UPROPERTY(Category=rdBakedSpawnObjects,EditAnywhere)
	TEnumAsByte<rdSpawnMode> spawnMode=rdSpawnMode::RDSPAWNMODE_DEFAULT;

	// Stream Distance - distance to stream this object in (-1 means use Actors default)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdPlacement|SpawnMode")
	float streamDistance=-1;

	// Current Load State of this Baked Placement set
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="rdPlacement|Streaming")
	TEnumAsByte<rdLoadState> loadState=rdLoadState::RDLOADSTATE_UNLOADED;

	// The Radius to spawn actors from the player when using rdSpawnMode::Local
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdBakedSpawnObjects)
	float localRadius=10000.0f;

	// The Distance to spawn actors within the View Frustum of the player when using rdSpawnMode::ViewFrustum
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdBakedSpawnObjects)
	float viewFrustumDistance=50000.0f;

	// The Extended area around the View Frustum (in degrees) to spawn when using rdSpawnMode::ViewFrustum
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdBakedSpawnObjects)
	float viewFrustumExtend=2.0f;

	UPROPERTY(EditDefaultsOnly,Category=rdBakedSpawnObjects)
	FrdGridArray transformMap;

	UPROPERTY(EditDefaultsOnly,Category=rdBakedSpawnObjects)
	FrdProxySetup proxy;
};

// BakedData (rdSpawnActor) DataAsset
UCLASS(BlueprintType)
class RDINST_PLUGIN_API UrdBakedDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()
public:

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("rdInstBakedDataAssets",GetFName()); }

	// Array of BakedData
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdBakedData)
	TArray<FrdBakedSpawnObjects>	bakedData;

	// Bounds of the entire data
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdBakedData)
	FBoxSphereBounds				bounds;
};

// The details of the spawn area and type, the rdSpawnActor has one of these, but when 
//   spawning SpawnPoints - just this struct is stored to avoid the need for unnecessary actors in the level
USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdSpawnData {
	FrdSpawnData() {}
	FrdSpawnData(const FrdSpawnData& o) : items(o.items),distance(o.distance),bounds(o.bounds),mainDensity(o.mainDensity),
													gridWidth(o.gridWidth),gridHeight(o.gridHeight),splatMap(o.splatMap),splatMapCoversLandscape(o.splatMapCoversLandscape),
													distributeFrames(o.distributeFrames),bakedDataAsset(o.bakedDataAsset),bakedData(o.bakedData),center(o.center) {
	}
	GENERATED_BODY()
public:
	
	// Array of object Placement types
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdSpawnActor)
	TArray<FrdObjectPlacementData> items;

	// The distance at which to spawn items, 0 = always spawn
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdSpawnActor)
	double				distance=0.0f;

	// The bounds of the spawn volume
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdSpawnActor,meta=(IgnoreForMemberInitializationTest))
	FBoxSphereBounds	bounds=FBoxSphereBounds();

	// The over-all density for the spawn data
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdSpawnActor)
	float				mainDensity=1.0f;

	// Width of the grid - the larger the value, the less populated 
	UPROPERTY(Category=rdSpawnActor,EditAnywhere,BlueprintReadWrite)
	float				gridWidth=1000.0f;

	// Height of the grid - the larger the value, the less populated 
	UPROPERTY(Category=rdSpawnActor,EditAnywhere,BlueprintReadWrite)
	float				gridHeight=1000.0f;

	// Splatmap to use for controlling the spawn data
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdSpawnActor)
	UTexture2D*			splatMap=nullptr;

	// Splatmap covers the entire Landscape
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdSpawnActor)
	bool				splatMapCoversLandscape=true;

	// Number of frames to split the spawning over
	UPROPERTY(Category=rdSpawnActor,EditAnywhere,BlueprintReadWrite)
	int32				distributeFrames=1;

	// Data stored by the baking process - do not touch  
	UPROPERTY(BlueprintReadWrite,Category=rdHidden)
	TSoftObjectPtr<UrdBakedDataAsset> bakedDataAsset=nullptr;

	// Data stored by the baking process - do not touch  
	UPROPERTY(BlueprintReadWrite,Category=rdHidden)
	TObjectPtr<UrdBakedDataAsset> bakedData=nullptr;

	// Number of frames to split the spawning over
	UPROPERTY(Category=rdSpawnActor,EditAnywhere,BlueprintReadWrite)
	int32				numBakedMeshes=0;

	// Number of frames to split the spawning over
	UPROPERTY(Category=rdSpawnActor,EditAnywhere,BlueprintReadWrite)
	int32				numBakedInstances=0;

	FVector				center;
};

// FrdSpawnDataScanItem
USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdSpawnDataScanItem {
	GENERATED_BODY()
public:
	FrdSpawnDataScanItem() {}
	FrdSpawnDataScanItem(AActor* a,TArray<FrdSpawnData>& sd) : volume(a),spawnDataList(sd) {}

	AActor*					volume=nullptr;
	TArray<FrdSpawnData>	spawnDataList;
};
