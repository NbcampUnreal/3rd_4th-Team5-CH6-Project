// rdPlacement.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once

#include "rdInstBPLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DataAsset.h"
#include "rdProxies.h"
#include "rdInstances.h"
#include "rdSpawnMode.h"
#include "rdPlacement.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

class UrdEntityMovementBase;

UENUM()
enum rdLoadState {
	// Unloaded
	RDLOADSTATE_UNLOADED		UMETA(DisplayName="Unloaded"),
	// ToLoad
	RDLOADSTATE_TOLOAD			UMETA(DisplayName="To Load"),
	// ToUnload
	RDLOADSTATE_TOUNLOAD		UMETA(DisplayName="To Unload"),
	// Patially loaded
	RDLOADSTATE_PARTIALLYLOADED	UMETA(DisplayName="Partially loaded"),
	// Fully loaded
	RDLOADSTATE_FULLYLOADED		UMETA(DisplayName="Fully loaded"),
};

// ObjectPlacement Structure
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakPlacementData",HasNativeMake="/Script/rdInst.rdActor.MakePlacementData"))
struct RDINST_PLUGIN_API FrdObjectPlacementData {
	GENERATED_BODY()
public:
	FrdObjectPlacementData() {}
	FrdObjectPlacementData(const FrdInstanceSetup& m) : mesh(m) {}
	FrdObjectPlacementData(TEnumAsByte<ErdSpawnType> tp,const FName s,UStaticMesh* m,TArray<TSoftObjectPtr<UMaterialInterface>> mats,ErdCollision ct,float sc,float ec,float sd,int32 id,const FName gn) : mesh(tp,s,m,mats,ct,sc,ec,sd,id,gn) {}
	FrdObjectPlacementData(UClass* ac,const FString& atm) : actorClass(ac),strProps(atm) {}
	FrdObjectPlacementData(UNiagaraSystem* fx) : vfx(fx) {}
	FrdObjectPlacementData(const FrdObjectPlacementData& o): 
							mesh(o.mesh),
							actorClass(o.actorClass),strProps(o.strProps),vfx(o.vfx),density(o.density),
							gridOffsetX(o.gridOffsetX),gridOffsetY(o.gridOffsetY),gridVarianceW(o.gridVarianceW),gridVarianceH(o.gridVarianceH),
							spawnMode(o.spawnMode),streamDistance(o.streamDistance),localRadius(o.localRadius),viewFrustumDistance(o.viewFrustumDistance),viewFrustumExtend(o.viewFrustumExtend),
							pooled(o.pooled),numToPool(o.numToPool),offset(o.offset),minGap(o.minGap),bUniformScale(o.bUniformScale),minScale(o.minScale),maxScale(o.maxScale),
							bRandomYaw(o.bRandomYaw),bAlignToNormal(o.bAlignToNormal),minSlope(o.minSlope),bPlaceOnGround(o.bPlaceOnGround),
							bOnNavMesh(o.bOnNavMesh),bFixSlopePlacement(o.bFixSlopePlacement),slopedRadius(o.slopedRadius),
							bUseSplatDensity(o.bUseSplatDensity),splatChannel(o.splatChannel),splatMinRange(o.splatMinRange),splatMaxRange(o.splatMaxRange),
							filterToPhysicalMatsList(o.filterToPhysicalMatsList),proxy(o.proxy),movement(o.movement),
							PlacementDataAsset(o.PlacementDataAsset),PlacementDataAssetID(o.PlacementDataAssetID),MultiPlacementDataAsset(o.MultiPlacementDataAsset),
							sid(o.sid),ismc(o.ismc) {}

	FrdObjectPlacementData(TEnumAsByte<ErdSpawnType> tp,const FName s,UStaticMesh* m,const TArray<TSoftObjectPtr<UMaterialInterface>> mats,ErdCollision col,float sc,float ec,float sd,int32 id,const FName gn,UClass* ac,const FString& atm,UNiagaraSystem* fx,
							float dns,float gox,float goy,float gvw,float gvh,
							TEnumAsByte<rdSpawnMode> sm,float sl,float lr,float vfd,float vfe,
							bool pl,int32 npl,const FVector& of,float mGp,bool UScl,FVector mnScl,
							FVector mxScl,bool RndYaw,bool ATN,float mnSlp,bool POG,bool ONV,bool FSP,float slpRad,bool spltDen,uint8 spltC,
							float spltMnR,float spltMxR,const TArray<UPhysicalMaterial*>& pml,const FrdProxySetup& px,UInstancedStaticMeshComponent* ism) :
								mesh(tp,s,m,mats,col,sc,ec,sd,id,gn),
								actorClass(ac),strProps(atm),vfx(fx),density(dns),gridOffsetX(gox),gridOffsetY(goy),gridVarianceW(gvw),gridVarianceH(gvh),
								spawnMode(sm),streamDistance(sl),localRadius(lr),viewFrustumDistance(vfd),viewFrustumExtend(vfe),
								pooled(pl),numToPool(npl),offset(of),minGap(mGp),bUniformScale(UScl),minScale(mnScl),maxScale(mxScl),bRandomYaw(RndYaw),bAlignToNormal(ATN),
								minSlope(mnSlp),bPlaceOnGround(POG),bOnNavMesh(ONV),bFixSlopePlacement(FSP),slopedRadius(slpRad),
								bUseSplatDensity(spltDen),splatChannel(spltC),splatMinRange(spltMnR),splatMaxRange(spltMxR),filterToPhysicalMatsList(pml),proxy(px),ismc(ism) {}

	FrdObjectPlacementData(const FrdInstanceSetup& m,UClass* ac,const FString& atm,UNiagaraSystem* fx,float dns,float gox,float goy,float gvw,float gvh,
							TEnumAsByte<rdSpawnMode> sm,float sl,float lr,float vfd,float vfe,
							bool pl,int32 npl,const FVector& of,float mGp,bool UScl,FVector mnScl,
							FVector mxScl,bool RndYaw,bool ATN,float mnSlp,bool POG,bool ONV,bool FSP,float slpRad,bool spltDen,uint8 spltC,
							float spltMnR,float spltMxR,const TArray<UPhysicalMaterial*>& pml,const FrdProxySetup& px,UInstancedStaticMeshComponent* ism) :
								mesh(m),
								actorClass(ac),strProps(atm),vfx(fx),density(dns),gridOffsetX(gox),gridOffsetY(goy),gridVarianceW(gvw),gridVarianceH(gvh),
								spawnMode(sm),streamDistance(sl),localRadius(lr),viewFrustumDistance(vfd),viewFrustumExtend(vfe),
								pooled(pl),numToPool(npl),offset(of),minGap(mGp),bUniformScale(UScl),minScale(mnScl),maxScale(mxScl),bRandomYaw(RndYaw),bAlignToNormal(ATN),
								minSlope(mnSlp),bPlaceOnGround(POG),bOnNavMesh(ONV),bFixSlopePlacement(FSP),slopedRadius(slpRad),
								bUseSplatDensity(spltDen),splatChannel(spltC),splatMinRange(spltMnR),splatMaxRange(spltMxR),filterToPhysicalMatsList(pml),proxy(px),ismc(ism) {}

	// The StaticMesh to spawn Instances from
	UPROPERTY(Category="rdPlacement|Source",EditAnywhere,meta=(ShowOnlyInnerProperties))
	FrdInstanceSetup mesh;

	// Actor Class for Spawning Actors
	UPROPERTY(Category="rdPlacement|Source",EditAnywhere)
	UClass*			actorClass=nullptr;

	// Property data for Spawning Actors (must have a valid Actor Class)
	UPROPERTY(Category="rdPlacement|Source",EditAnywhere)
	FString			strProps;

	// Niagara System to Spawn
	UPROPERTY(Category="rdPlacement|Source",EditAnywhere)
	UNiagaraSystem*	vfx=nullptr;

	// Density for spawning this Object (1.0=default)
	UPROPERTY(Category="rdPlacement|Positioning",EditAnywhere)
	float			density=1.0f;

	// X Offset into the spawn grid
	UPROPERTY(Category="rdPlacement|Positioning",EditAnywhere)
	float			gridOffsetX=0.0f;

	// Y Offset into the spawn grid
	UPROPERTY(Category="rdPlacement|Positioning",EditAnywhere)
	float			gridOffsetY=0.0f;

	// Grid X Variance within the spawn grid (1.0=whole grid width, 0.5=inner half)
	UPROPERTY(Category="rdPlacement|Positioning",EditAnywhere)
	float			gridVarianceW=0.8f;

	// Grid Y Variance within the spawn grid (1.0=whole grid height, 0.5=inner half)
	UPROPERTY(Category="rdPlacement|Positioning",EditAnywhere)
	float			gridVarianceH=0.8f;

	UPROPERTY(Category="rdPlacement|SpawnMode",EditAnywhere)
	TEnumAsByte<rdSpawnMode> spawnMode=rdSpawnMode::RDSPAWNMODE_DEFAULT;

	// Stream Distance - distance to stream this object in (-1 means use Actors default)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdPlacement|Streaming")
	float streamDistance=-1;

	// The Radius to spawn actors from the player when using rdSpawnMode::Local
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdPlacement|SpawnMode")
	float localRadius=10000.0f;

	// The Distance to spawn actors within the View Frustum of the player when using rdSpawnMode::ViewFrustum
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdPlacement|SpawnMode")
	float viewFrustumDistance=50000.0f;

	// The Extended area around the View Frustum (in degrees) to spawn when using rdSpawnMode::ViewFrustum
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdPlacement|SpawnMode")
	float viewFrustumExtend=2.0f;

	// Option to Pool the Actor that is being Spawned
	UPROPERTY(Category="rdPlacement|Pooling",EditAnywhere)
	bool			pooled=false;

	// Number of Actors to Pool
	UPROPERTY(Category="rdPlacement|Pooling",EditAnywhere)
	int32			numToPool=20;

	// Offset to apply to the spawned Object from its generated location
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	FVector			offset=FVector(0,0,0);
	// Minimum Gap between Objects
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	float			minGap=0.0f;
	// When Ticked, Overall Scale for the Object is taken from the X scale
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	bool			bUniformScale=true;
	// Minimum Scale to Apply to the Object
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	FVector			minScale=FVector(0.8f,0.8f,0.8f);
	// Maximum Scale to Apply to the Object
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	FVector			maxScale=FVector(1.3f,1.3f,1.3f);

	// When true, The Yaw of the Object is Randomized
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	bool			bRandomYaw=true;
	// When true, the object is aligned to the normal of the Landscape or Mesh
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	bool			bAlignToNormal=false;
	// Angle defining the minimum slope from vertical to restrict to
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	float			minSlope=45.0f;
	// Scan to the ground and place on the Z location
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	bool			bPlaceOnGround=true;
	// Filter to only add where there is NavMesh
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdPlacement|Randomization")
	bool			bOnNavMesh=false;
	// Ensure the base of the Object is at the lowest Z location of the placement area
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	bool			bFixSlopePlacement=true;
	// Radius to scan around to fix the Z placement
	UPROPERTY(Category="rdPlacement|Randomization",EditAnywhere)
	float			slopedRadius=20.0f;

	// When true, the density value is set to the current splatmaps value
	UPROPERTY(Category="rdPlacement|Splatmap",EditAnywhere)
	bool			bUseSplatDensity=true;
	// Channel of the splatmap to use for the placement
	UPROPERTY(Category="rdPlacement|Splatmap",EditAnywhere)
	int8			splatChannel=0; // same splat time, same splat channel...
	// minimum splat color for including
	UPROPERTY(Category="rdPlacement|Splatmap",EditAnywhere)
	float			splatMinRange=0.333f;
	// maximum splat color for including
	UPROPERTY(Category="rdPlacement|Splatmap",EditAnywhere)
	float			splatMaxRange=1.0f;

	// When placing on ground, you can add a list of PhysicalMaterials to populate on
	UPROPERTY(Category="rdPlacement|Misc",EditAnywhere)
	TArray<UPhysicalMaterial*>	filterToPhysicalMatsList;

	// Proxy for this (StaticMesh) Object
	UPROPERTY(Category="rdPlacement|Misc",EditAnywhere)
	FrdProxySetup				proxy;

	// Movement class for this Placement type
	UPROPERTY(Category="rdPlacement|Misc",EditAnywhere)
	TObjectPtr<UrdEntityMovementBase>		movement=nullptr;

	// The DataAsset to use as the source of the Placement Data. If you want to customize, just set this to null
	UPROPERTY(Category="rdPlacement|System",EditAnywhere)
	FString	PlacementDataAsset;

	// The DataAsset Placement Item ID used as the source of the Placement Data. If you want to customize, just set this to null
	UPROPERTY(Category="rdPlacement|System",EditAnywhere)
	FString	PlacementDataAssetID;

	// The MultiDataAsset used as the source of the Placement Data. If you want to customize, just set this to null
	UPROPERTY(Category="rdPlacement|System",EditAnywhere)
	FString	MultiPlacementDataAsset;

	UPROPERTY(Category=rdHidden,BlueprintReadWrite)
	TArray<AActor*>				actorList;

	UPROPERTY(Category=rdHidden,BlueprintReadWrite)
	TArray<UStaticMeshComponent*> smcList;

	UPROPERTY(Category=rdHidden,BlueprintReadWrite)
	TArray<UNiagaraSystem*>		niagaraList;

	TArray<UNiagaraComponent*>	vfxList;

	FName						sid;

	UPROPERTY(Category="rdPlacement|System",EditAnywhere)
	TWeakObjectPtr<UInstancedStaticMeshComponent> ismc=nullptr;
};

// Placement DataAsset
UCLASS(BlueprintType)
class RDINST_PLUGIN_API UrdPlacementDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()
public:

	// Label that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdPlacement)
	FName label;
	// Tooltip that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdPlacement)
	FString tooltip;

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("rdInstPlacementAssets",GetFName()); }

	// Array of Placement DataAssets describing your objects to populate with
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdMultiPlacement)
	TArray<FrdObjectPlacementData> Placements;
};

// MultiPlacement DataAsset
UCLASS(BlueprintType)
class RDINST_PLUGIN_API UrdMultiPlacementDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()
public:

	// Label that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdMultiPlacement)
	FName label;
	// Tooltip that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdMultiPlacement)
	FString tooltip;

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("rdInstMultiPlacementAssets",GetFName()); }

	// Array of Placement DataAssets describing your objects to populate with
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdMultiPlacement)
	TArray<TSoftObjectPtr<UrdPlacementDataAsset>> PlacementDataAssets;
};

