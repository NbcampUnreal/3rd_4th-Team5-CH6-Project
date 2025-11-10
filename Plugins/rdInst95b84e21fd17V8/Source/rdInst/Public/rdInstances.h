// rdInstances.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/DataAsset.h"
#include "Engine/AssetUserData.h"
#include "rdProxies.h"
#include "rdInstances.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

class UNiagaraSystem;

//
// BitField Instance Settings (correspond to the settings found in Foliage Instances etc)
//
#define rdInst_CastShadow						0x00000001
#define rdInst_CastStaticShadow					0x00000002
#define rdInst_CastDynamicShadow				0x00000004
#define rdInst_CastFarShadow					0x00000008
#define rdInst_CastInsetShadow					0x00000010
#define rdInst_CastHiddenShadow					0x00000020
#define rdInst_CastShadowAsTwoSided				0x00000040
#define rdInst_CastVolumetrixTranslucentShadow	0x00000080

#define rdInst_ReceivesDecals					0x00000100
#define rdInst_DisableCollision					0x00000200
#define rdInst_CanEverAffectNavigation			0x00000400
#define rdInst_HasPerInstanceHitProxies			0x00000800
#define rdInst_RayTracingFarField				0x00001000
#define rdInst_AffectDynamicIndirectLighting	0x00002000
#define rdInst_AffectDistanceFieldLighting		0x00004000
#define rdInst_Selectable						0x00008000

#define rdInst_ReceiveMobileCSMShadow			0x00010000
#define rdInst_SelfShadowOnly					0x00020000
#define rdInst_CastContactShadow				0x00040000
#define rdInst_WPOWritesVelocity				0x00080000

#define rdInst_RenderInMainPass					0x00100000
#define rdInst_RenderInDepthPass				0x00200000
#define rdInst_EvaluateWPO						0x00400000

#define rdInst_FillUnderForNav					0x00800000
#define rdInst_FillNavConvex					0x01000000

#define rdInst_DefaultFlags 	(rdInst_CastShadow|rdInst_CastStaticShadow|rdInst_CastDynamicShadow|rdInst_CastFarShadow|rdInst_ReceivesDecals| \
								rdInst_RenderInMainPass|rdInst_RenderInDepthPass|rdInst_EvaluateWPO|rdInst_FillNavConvex)

UENUM(BlueprintType)
enum ErdSpawnType { 
	UseDefaultSpawn UMETA(DisplayName="Use Levels Default SpawnType"),
	ISM UMETA(DisplayName="Instanced Static Mesh"),
	HISM UMETA(DisplayName="Hierachicial Instanced Static Mesh"),
	SMC UMETA(DisplayName="StaticMesh Component"),
	NiagaraMesh UMETA(DisplayName="Niagara Mesh"),
	rdMeshlet UMETA(DisplayName="rdMeshlet (not currently implemented)"),
	ComponentlessMesh UMETA(DisplayName=" Componentless Mesh (not currently implemented)"),
	Actor UMETA(DisplayName="Actor"),
	DataLayer UMETA(DisplayName="DataLayer"),
	VFX UMETA(DisplayName="Niagara VFX")
};

USTRUCT(BlueprintType) 
struct RDINST_PLUGIN_API FrdSidDetails {
	GENERATED_BODY()
public:
	UPROPERTY(Category="rdSidDetails",EditAnywhere)
	TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn;

	UPROPERTY(Category="rdSidDetails",EditAnywhere)
	TSoftObjectPtr<UStaticMesh> mesh=nullptr;

	UPROPERTY(Category="rdSidDetails",EditAnywhere)
	TArray<TSoftObjectPtr<UMaterialInterface>> materials;

	UPROPERTY(Category="rdSidDetails",EditAnywhere)
	TEnumAsByte<ErdCollision> collision=ErdCollision::UseDefault;

	UPROPERTY(Category="rdSidDetails",EditAnywhere)
	float startCull=-1.0f;

	UPROPERTY(Category="rdSidDetails",EditAnywhere)
	float endCull=-1.0f;

	UPROPERTY(Category="rdSidDetails",EditAnywhere)
	int32 id=0;
};

UENUM()
enum rdClusterType {
	// Use Default
	RDCLUSTER_USEDEFAULT			UMETA(DisplayName="Use Default"),
	// Single Instance
	RDCLUSTER_SINGLE				UMETA(DisplayName="Single Instance"),
	// Radial Cluster
	RDCLUSTER_RADIAL				UMETA(DisplayName="Radial Cluster"),
	// Radial Cluster Fade
	RDCLUSTER_RADIALFADE			UMETA(DisplayName="Radial Fade Cluster"),
	// Square Cluster
	RDCLUSTER_SQUARE				UMETA(DisplayName="Square Cluster"),
	// Square Cluster Fade
	RDCLUSTER_SQUAREFADE			UMETA(DisplayName="SquareFade Cluster"),
};

// InstanceSetup Structure
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakInstanceSetup",HasNativeMake="/Script/rdInst.rdActor.MakeInstanceSetup"))
struct RDINST_PLUGIN_API FrdInstanceSetup {
	GENERATED_BODY()
public:
	FrdInstanceSetup() {}
	FrdInstanceSetup(const FrdInstanceSetup& o): type(o.type),sid(o.sid),mesh(o.mesh),materials(o.materials),collision(o.collision),startCull(o.startCull),endCull(o.endCull),streamInDistance(o.streamInDistance),id(o.id),groupName(o.groupName),clusterType(o.clusterType),clusterRadius(o.clusterRadius),clusterDensity(o.clusterDensity) {}
	FrdInstanceSetup(TEnumAsByte<ErdSpawnType> tp,const FName s,TSoftObjectPtr<UStaticMesh> m,const TArray<TSoftObjectPtr<UMaterialInterface>> mats,ErdCollision col,float sc,float ec,float sd,int32 i,const FName gn) : type(tp),sid(s),mesh(m),materials(mats),collision(col),startCull(sc),endCull(ec),streamInDistance(sd),id(i),groupName(gn) {}
	FrdInstanceSetup(TEnumAsByte<ErdSpawnType> tp,const FName s,TSoftObjectPtr<UStaticMesh> m,const TArray<UMaterialInterface*> mats,ErdCollision col,float sc,float ec,int32 i,const FName gn) : type(tp),sid(s),mesh(m),collision(col),startCull(sc),endCull(ec),id(i),groupName(gn) { for(auto mt:mats) { materials.Add(TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(mt->GetPathName()))); } }
#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<1
	FrdInstanceSetup(TEnumAsByte<ErdSpawnType> tp,const FName s,UStaticMesh* m,const TArray<TSoftObjectPtr<UMaterialInterface>> mats,ErdCollision col,float sc,float ec,float sd,int32 i,const FName gn) : type(tp),sid(s),mesh(m),materials(mats),collision(col),startCull(sc),endCull(ec),streamInDistance(sd),id(i),groupName(gn) {}
	FrdInstanceSetup(TEnumAsByte<ErdSpawnType> tp,const FName s,UStaticMesh* m,const TArray<UMaterialInterface*> mats,ErdCollision col,float sc,float ec,float sd,int32 i,const FName gn) : type(tp),sid(s),mesh(m),collision(col),startCull(sc),endCull(ec),streamInDistance(sd),id(i),groupName(gn) { for(auto mt:mats) { materials.Add(TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(mt->GetPathName()))); } }
#endif

	// The SpawnType
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn;

	// The objects sid
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere,meta=(AssetBundles="InstanceData"))
	FName sid;

	// The StaticMesh to spawn Instances from
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere,meta=(AssetBundles="InstanceData"))
	TSoftObjectPtr<UStaticMesh>	mesh=nullptr;

	// Override Materials for the StaticMesh
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere,meta=(AssetBundles="InstanceData"))
	TArray<TSoftObjectPtr<UMaterialInterface>> materials;

	// Collision Type for the StaticMesh
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	TEnumAsByte<ErdCollision> collision=ErdCollision::UseDefault;

	// Start Cull Distance for the Static Mesh (-1.0 means use default start cull distance)
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	float startCull=-1.0f;

	// End Cull Distance for the Static Mesh (-1.0 means use default end cull distance)
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	float endCull=-1.0f;

	// Stream-in distance
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	float streamInDistance=0.0f;

	// ID for the StaticMesh. Specifying a unique ID can be used to separate this mesh into its own Component
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	int32 id=0;

	// Group Name for collating objects
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	FName groupName;

	// Type of clustering for this mesh
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	TEnumAsByte<rdClusterType> clusterType=rdClusterType::RDCLUSTER_USEDEFAULT;

	// Cluster radius
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	float clusterRadius=30.0f;

	// Cluster radius2
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	float clusterRadius2=30.0f;

	// Cluster density
	UPROPERTY(Category="rdInstanceSetup",EditAnywhere)
	float clusterDensity=0.5f;

};

USTRUCT()
struct RDINST_PLUGIN_API FrdSetObjectItem {
	GENERATED_BODY()
public:
	FrdSetObjectItem() {}
	FrdSetObjectItem(const FName s) : sid(s) {}
	FrdSetObjectItem(const FName s,UClass* pa,const FString& pdl,const FName ps,bool pam,bool dh,bool dr) : sid(s) {}
	FrdSetObjectItem(UClass* ac,AActor* at,bool p,int32 np,UClass* pa,const FString& pdl,const FName ps,bool pam,bool dh,bool dr) : actorClass(ac),actorTemplate(at) {}
	FrdSetObjectItem(UClass* ac,AActor* at,bool p,int32 np) : actorClass(ac),actorTemplate(at) {}
	FrdSetObjectItem(UNiagaraSystem* fx) : vfx(fx) {}

	FName				sid;
	UClass*				actorClass=nullptr;
	AActor*				actorTemplate=nullptr;
	UNiagaraSystem*		vfx=nullptr;
	bool				pool=false;
	int32				numToPool=0;
	TArray<FTransform>	transforms;
	FrdProxySetup       proxy;
};

UCLASS()
class RDINST_PLUGIN_API UrdSetObjectsList : public UObject {
	GENERATED_BODY()
public:
	TArray<FrdSetObjectItem> objects;
};

// This is the base struct that stores the Instance index along with it's owning actor.
//  The Struct after this one is a TArray of these along with the Name of the HISMC used to create the instance.
//
USTRUCT()
struct RDINST_PLUGIN_API FrdInstItem {
	GENERATED_BODY()
public:
			FrdInstItem() {}
			FrdInstItem(const AActor* o,int32 i) : owner(o),index(i) {}
			FrdInstItem(const FrdInstItem& o) : owner(o.owner),index(o.index) {}

	// The Actor that owns the Instance
	const AActor*					owner=nullptr;

	// Index of the Instance
	int32							index=-1;
};

//
// This Struct is the Value of the BaseActors InstList Map (The StaticMeshes being the Key)
//
USTRUCT()
struct RDINST_PLUGIN_API FrdInstList {
	GENERATED_BODY()
public:
	FrdInstList() {}
	FrdInstList(const FName s,TSoftObjectPtr<UStaticMesh> m) : sid(s),mesh(m) {}
	FrdInstList(const FName s,UStaticMesh* m) : sid(s),mesh(m) {}

	// ID of the StaticMesh (Comprised of Mesh FileName, Material FileNames and if its negative scale
	FName										sid;

	// StaticMesh associated with the sid
	TSoftObjectPtr<UStaticMesh>					mesh=nullptr;

	// Array of all Instances for this HISMC/ISMC
	TArray<FrdInstItem*>						instItems;
};

//
// UrdRandomSettings
//   These settings describe the variation wanted for Randomization of the Instance
//
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakRandomSettings", HasNativeMake="/Script/rdInst.rdActor.MakeRandomSettings"))
struct RDINST_PLUGIN_API FrdRandomSettings {
	
	GENERATED_BODY()

	FrdRandomSettings():location1(0,0,0),location2(0,0,0),rotation1(0,0,0),rotation2(0,0,0),flipProbability(0,0,0),lockAxis(0),scale1(1,1,1),scale2(1,1,1),showProbability(1.0) {}

	FrdRandomSettings(const FrdRandomSettings& o):location1(o.location1),location2(o.location2),rotation1(o.rotation1),rotation2(o.rotation2),flipProbability(o.flipProbability),lockAxis(o.lockAxis),
													scale1(o.scale1),scale2(o.scale2),showProbability(o.showProbability),relyActorName(o.relyActorName),themes(o.themes) {}

	FrdRandomSettings(FVector& loc1,FVector& loc2,FRotator& rot1,FRotator& rot2,FVector flpP,uint8 lckA,FVector& scl1,FVector& scl2,float showP,FString& rlyNm,FString& thms) :
													location1(loc1),location2(loc2),rotation1(rot1),rotation2(rot2),flipProbability(flpP),lockAxis(lckA),
													scale1(scl1),scale2(scl2),showProbability(showP),relyActorName(rlyNm),themes(thms) {}


	// The lower Range Vector of any Random Location movement
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			location1;

	// The upper Range Vector of any Random Location movement
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			location2;

	// The lower Range Rotator of any Random Rotation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FRotator		rotation1;

	// The upper Range Rotator of any Random Rotation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FRotator		rotation2;

	// The Probability between 0.0 and 1.0 of this Actor being Flipped in the Vectors Axis
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			flipProbability;

	// Type of Axis Lock for Scaling, 0=no lock, 1=lock X&Y, 2=lock All
	UPROPERTY(Category=rdRandom,EditAnywhere)
	uint8			lockAxis;

	// The lower Range Vector of any Random Scale Variation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			scale1;

	// The upper Range Vector of any Random Scale Variation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			scale2;

	// The Probability between 0.0 and 1.0 of this Actor being Visible
	UPROPERTY(Category=rdRandom,EditAnywhere)
	float			showProbability;

	// The Name of an actor that must be Visible for this one to be
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FString			relyActorName;

	// String Containing a comma separated list of Themes that this Actor is shown in
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FString			themes;

	// Used by the Copy Construct, which is used by the Maps Get Copy - good to use Get Reference though
	void CopyFromOther(const FrdRandomSettings& other);
};

//
// FrdInstanceFastSettings
//   Describes each of the Instances wanted by the ArdActor (contained in the InstanceList).
//
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakInstanceFastSettings", HasNativeMake="/Script/rdInst.rdActor.MakeInstanceFastSettings"))
struct RDINST_PLUGIN_API FrdInstanceFastSettings {
	
	GENERATED_BODY()

	FrdInstanceFastSettings():transform(),bNoVis(false),bVisible(true),index(-1) {}
	FrdInstanceFastSettings(const FTransform& t):transform(t),bNoVis(false),bVisible(true),index(-1) {}
	FrdInstanceFastSettings(const FrdInstanceFastSettings& o):transform(o.transform),name(o.name),folder(o.folder),theme(o.theme),bNoVis(o.bNoVis),bVisible(o.bVisible),index(o.index) {}
	FrdInstanceFastSettings(FTransform& tran,FString& nm,FString& fld,FString& thm,bool noVis,bool vis,int32 ind):transform(tran),name(nm),folder(fld),theme(thm),bNoVis(noVis),bVisible(vis),index(ind) {}

	// The Tranform of this Instance
	UPROPERTY(Category=rdInst,EditAnywhere)
	FTransform		transform;

	// The Name of this Instance if wanted
	UPROPERTY(Category=rdInst,EditAnywhere)
	FString			name;

	// The folder this Instance belongs to (if any). Used to set the Visibility of Folders in the Actor
	UPROPERTY(Category=rdInst,EditAnywhere)
	FString			folder;

	// The Theme to use this item with
	UPROPERTY(Category=rdInst,EditAnywhere)
	FString			theme;

	// This is set to True when the object has been hidden by the user
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8	bNoVis:1;

	// When True, this Instance is not Hidden by Hidden Folders
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8	bVisible:1;

	// The index of the Instance
	UPROPERTY(Category=rdInst,EditAnywhere)
	int32	index;
};

//
// FrdInstanceRandomSettings
//   Describes each of the Instances wanted by the ArdActor including Randomization (contained in the InstanceList).
//
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakInstanceRandomSettings", HasNativeMake="/Script/rdInst.rdActor.MakeInstanceRandomSettings"))
struct RDINST_PLUGIN_API FrdInstanceRandomSettings {
	
	GENERATED_BODY()

	FrdInstanceRandomSettings():origTransform(),randomSettings(),bNoVis(false),bVisible(true),bProcessed(false),index(-1) {}
	FrdInstanceRandomSettings(const FTransform& t):origTransform(t),randomSettings(),bNoVis(false),bVisible(true),bProcessed(false),index(-1) {}
	FrdInstanceRandomSettings(const FrdInstanceRandomSettings& o):name(o.name),folder(o.folder),origTransform(o.origTransform),actualTransform(o.actualTransform),randomSettings(o.randomSettings),bNoVis(o.bNoVis),bVisible(o.bVisible),bProcessed(o.bProcessed),index(o.index) {}
	FrdInstanceRandomSettings(FString& nm,FString& fld,FTransform& oTran,FTransform& aTran,FrdRandomSettings rndSettings,bool noVis,bool vis,bool proc,int32 ind):name(nm),folder(fld),origTransform(oTran),actualTransform(aTran),randomSettings(rndSettings),bNoVis(noVis),bVisible(vis),bProcessed(proc),index(ind) {}

	// The Name of this Instance if wanted
	UPROPERTY(Category=rdInst,EditAnywhere)
	FString			name;

	// The folder this Instance belongs to (if any). Used to set the Visibility of Folders in the Actor
	UPROPERTY(Category=rdInst,EditAnywhere)
	FString			folder;

	// The original Tranform of this Instance (The Randomization uses it as the origin)
	UPROPERTY(Category=rdInst,EditAnywhere)
	FTransform		origTransform;

	// The Transform of this Instance with the Randomization applied
	UPROPERTY(Category=rdInst,EditAnywhere)
	FTransform		actualTransform;

	// The Randomizations Settings of this Instance
	UPROPERTY(Category=rdInst,EditAnywhere)
	FrdRandomSettings randomSettings;

	// This is set to True when the object has been hidden by the user
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8	bNoVis:1;

	// When True, this Instance is not Hidden by Hidden Folders
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8	bVisible:1;

	// This instance has been processed by the Randomization system
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8	bProcessed:1;

	// The index of the Instance
	UPROPERTY(Category=rdInst,EditAnywhere)
	int32	index;
};

//
// This Struct contains an Array of InstanceSettings, as used in ArdActors InstanceList for describing the desired Instances.
//   ArdActor contains a Map of StaticMeshes as the Key and this as the Value.
// 
USTRUCT()
struct RDINST_PLUGIN_API FrdInstanceSettingsArray {

	GENERATED_BODY()
	
	FrdInstanceSettingsArray() {}
	FrdInstanceSettingsArray(const FTransform& tran) { settingsFast.Add(FrdInstanceFastSettings(tran)); }
	FrdInstanceSettingsArray(const FrdInstanceSettingsArray& o):settingsRandom(o.settingsRandom),settingsFast(o.settingsFast) {}

	UPROPERTY(Category=rdActor,EditAnywhere)
	TArray<FrdInstanceRandomSettings> settingsRandom;

	UPROPERTY(Category=rdActor,EditAnywhere)
	TArray<FrdInstanceFastSettings> settingsFast;
};

USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakFastItem", HasNativeMake="/Script/rdInst.rdActor.MakeFastItem"))
struct RDINST_PLUGIN_API FrdAddInstanceFastArray {
	GENERATED_BODY()

	FrdAddInstanceFastArray() {}
	FrdAddInstanceFastArray(const FrdAddInstanceFastArray& o) : mesh(o.mesh),meshSetup(o.meshSetup),transforms(o.transforms) {}
	FrdAddInstanceFastArray(const FrdInstanceSetup& m,const TArray<FTransform>& t) : meshSetup(m),transforms(t) {}

	// Mesh to instance (Depreciated, use the 'meshSetup' now instead)
	UPROPERTY(Category=rdAddInstances,EditAnywhere,meta=(DepreciatedFunction))
	UStaticMesh* mesh=nullptr;

	// Mesh Description for the instances
	UPROPERTY(Category=rdAddInstances,EditAnywhere)
	FrdInstanceSetup meshSetup;
	
	UPROPERTY(Category=rdAddInstances,EditAnywhere)
	TArray<FTransform> transforms;
};

USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakRandomizedItem", HasNativeMake="/Script/rdInst.rdActor.MakeRandomizedItem"))
struct RDINST_PLUGIN_API FrdAddInstanceRandomizedArray {
	GENERATED_BODY()

	FrdAddInstanceRandomizedArray() {}
	FrdAddInstanceRandomizedArray(const FrdAddInstanceRandomizedArray& o) : mesh(o.mesh),meshSetup(o.meshSetup),settings(o.settings) {}
	FrdAddInstanceRandomizedArray(const FrdInstanceSetup& m,const TArray<FrdInstanceRandomSettings>& t) : meshSetup(m),settings(t) {}

	// Mesh to instance (Depreciated, use the 'meshSetup' now instead)
	UPROPERTY(Category=rdAddInstances,EditAnywhere,meta=(DepreciatedFunction))
	UStaticMesh* mesh=nullptr;

	// Mesh Description for the instances
	UPROPERTY(Category=rdAddInstances,EditAnywhere)
	FrdInstanceSetup meshSetup;
	
	UPROPERTY(Category=rdAddInstances,EditAnywhere)
	TArray<FrdInstanceRandomSettings> settings;
};

USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakPositionInfo", HasNativeMake="/Script/rdInst.rdActor.MakePositionInfo"))
struct RDINST_PLUGIN_API FrdPositionInfo {
	GENERATED_BODY()

	FrdPositionInfo():currentLevel(0),currentRow(0),currentColumn(0),totalLevels(0),totalRows(0),totalColumns(0),randomSeed(0),bitField(0) {}
	FrdPositionInfo(const FrdPositionInfo& o):currentLevel(o.currentLevel),currentRow(o.currentRow),currentColumn(o.currentColumn),totalLevels(o.totalLevels),totalRows(o.totalRows),totalColumns(o.totalColumns),randomSeed(o.randomSeed),bitField(o.bitField),customData(o.customData) {}
	FrdPositionInfo(int32 cl,int32 cr,int32 cc,int32 tl,int32 tr,int32 tc,int32 rs,uint64 bf,TArray<float>& custData):currentLevel(cl),currentRow(cr),currentColumn(cc),totalLevels(tl),totalRows(tr),totalColumns(tc),randomSeed(rs),bitField(bf),customData(custData) {}

	// Current Level of the build
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	int16			currentLevel;

	// Current Row of the build
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	int16			currentRow;

	// Current Column of the build
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	int16			currentColumn;

	// total Levels in the build
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	int16			totalLevels;

	// total Rows in the build
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	int16			totalRows;

	// total Columns in the build
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	int16			totalColumns;

	// RandomSeed
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	int32			randomSeed;

	// BitField
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	uint64			bitField;

	// CustomPerInstanceData
	UPROPERTY(Category=rdPositionInfo,EditAnywhere)
	TArray<float> customData;
};

//
// This is the AssetUserData Struct containing the Randomization data used while processing the actors.
// 
UCLASS(NotBlueprintable,hidecategories=(Object))
class RDINST_PLUGIN_API UrdRandomizeAssetUserData : public UAssetUserData {
	GENERATED_BODY()
public:
	const FString&	GetName() { return name; }
	void			SetName(const FString& nm) { name=nm; }

	// The lower Range Vector of any Random Location movement
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			location1;

	// The upper Range Vector of any Random Location movement
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			location2;

	// The lower Range Rotator of any Random Rotation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FRotator		rotation1;

	// The upper Range Rotator of any Random Rotation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FRotator		rotation2;

	// The Probability between 0.0 and 1.0 of this Actor being Flipped in the Vectors Axis
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			flipProbability;

	// Type of Axis Lock for Scaling, 0=no lock, 1=lock X&Y, 2=lock All
	UPROPERTY(Category=rdRandom,EditAnywhere)
	int8			lockAxis;

	// The lower Range Vector of any Random Scale Variation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			scale1;

	// The upper Range Vector of any Random Scale Variation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			scale2;

	// The Probability between 0.0 and 1.0 of this Actor being Visible
	UPROPERTY(Category=rdRandom,EditAnywhere)
	float			showProbability;

	// The Name of an actor that must be Visible for this one to be
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FString			relyActorName;

	// String Containing a comma separated list of Themes that this Actor is shown in
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FString			themes;

protected:
	UPROPERTY()
	FString			name;
};

//
// This is the AssetUserData Struct containing data used while processing the actors.
//
// As there is a possible reliance on another actor, it can't decide to show or hide until that Actor has been processed,
//	  so in the code, we start with an Array of all Child Actors, and Loop around that, removing processed Actors until 
//    the Array is empty (or a timeout is reached in case of recursive dependencies).
//
UCLASS(NotBlueprintable,hidecategories=(Object))
class RDINST_PLUGIN_API UrdTransformAssetUserData : public UAssetUserData {
	GENERATED_BODY()
public:
	const FString&	GetName() { return name; }
	void			SetName(const FString& nm) { name=nm; }

	// origTransform is what any Randomizations use as their origin.
	UPROPERTY(Category=rdTransform,EditAnywhere)
	FTransform	origTransform;

	// bNoVis is set when showing and hiding Folders - when it's True we never show or process the Actor
	UPROPERTY(Category=rdTransform,EditAnywhere)
	uint8		bNoVis:1;

	// bHide is set to True when the Randomization decides the Actor should be hidden
	UPROPERTY(Category=rdTransform,EditAnywhere)
	uint8		bHide:1;

	// bProcessed is set to True once this Actors Randomizations have been Applied.
	UPROPERTY(Category=rdTransform,EditAnywhere)
	uint8		bProcessed:1;

protected:
	UPROPERTY()
	FString				name;
};

//
// This is the AssetUserData Struct containing the Instance Settings used when creating ISMS for this StaticMesh.
// 
UCLASS(NotBlueprintable,hidecategories=(Object))
class RDINST_PLUGIN_API UrdInstAssetUserData : public UAssetUserData {
	GENERATED_BODY()
public:
	const FString&	GetName() { return name; }
	void			SetName(const FString& nm) { name=nm; }

	// Start Distance for Culling of the Instances of this StaticMesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	float				startCull=0.0f;

	// End Distance for Culling of the Instances of this StaticMesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	float				endCull=0.0f;

	// End Distance for WorldPositionOffset of the Instances of this StaticMesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	int32				WPODisableDistance=0.0f;

	// Mobility Setting for the Instances of this StaticMesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8				mobility=0;

	// Type of Collision to use for the Instances of this StaticMesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8				collision=3;

	// Collision Profile
	UPROPERTY(Category=rdInst,EditAnywhere)
	FString				collisionProfile;

	// Minimum LOD to use for the Instances of this StaticMesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint8				minLOD=0;

	// Light Channel to use for Instances of this StaticMesh (defaults to 2)
	UPROPERTY(Category=rdInst,EditAnywhere)
	int8				lightChannel=1;

	// Random Seed for the Instances of this StaticMesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	int32				randomSeed=0;

	// 32bit Int containing BitField of Instance Flags (See defines above this)
	UPROPERTY(Category=rdInst,EditAnywhere)
	uint32				instFlags=rdInst_DefaultFlags;

	// The number of Custom Float values each Instance has (For their materials)
	UPROPERTY(Category=rdInst,EditAnywhere)
	int32				numPerInstCustData=0; 

	// The type of Shadow Invalidation for the mesh
	UPROPERTY(Category=rdInst,EditAnywhere)
	int8				shadowInvalidation=0;

	// Proxy Settings
	UPROPERTY(Category=rdInst,EditAnywhere)
	FrdProxySetup		proxySettings;

	// Minimum Draw Distance, is culled when closer to the camera than this
	UPROPERTY(Category=rdInst,EditAnywhere)
	float minDrawDistance=0.0f;

	// Nanite Programmable Draw Distance (Disables Raster Operations such as Masking)
	UPROPERTY(Category=rdInst,EditAnywhere)
	float naniteProgDistance=0.0f;

protected:
	UPROPERTY()
	FString				name;
};

UCLASS(BlueprintType)
class RDINST_PLUGIN_API UrdInstData : public UObject {
	GENERATED_BODY()
public:
	UrdInstData() {}

	void				FromString(const FString& str);
	FString				ToString();

	UPROPERTY(Category=rdInstData,EditAnywhere)
	float				startCull=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	float				endCull=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	uint8				mobility=2;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	uint8				collision=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	uint8				minLOD=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	int8				lightChannel=1;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	int32				randomSeed=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	int32				numPerInstCustData=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	int32				WPODisableDistance=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	int8				shadowInvalidation=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	uint32				instFlags=rdInst_DefaultFlags;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	FString				collisionProfile;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	FrdProxySetup		proxySettings;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	float				minDrawDistance=0;

	UPROPERTY(Category=rdInstData,EditAnywhere)
	float				naniteProgDistance=0;
};


USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdInstanceIndexes {
	GENERATED_BODY()
public:
	TArray<int32> indexes;
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdInstancesMap {
	GENERATED_BODY()
public:
	TMap<UInstancedStaticMeshComponent*,FrdInstanceIndexes> instances;
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdSidInstancesMap {
	GENERATED_BODY()
public:
	TMap<FName,FrdInstanceIndexes> instances;
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdInstItemX {
	GENERATED_BODY()
public:
	FName sid;
	TArray<FTransform> transforms;
};

USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdActorItemX {
	GENERATED_BODY()
public:
	UClass* actorClass;
	TArray<FTransform> transforms;
	TArray<AActor*>    actors;
};

// Instance Settings DataAsset
UCLASS(BlueprintType)
class RDINST_PLUGIN_API UrdInstanceSettingsDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()
public:

	// Label that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdInstanceSettings)
	FName label;
	// Tooltip that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdInstanceSettings)
	FString tooltip;

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("rdInstanceSettingsAssets",GetFName()); }

	// Start Distance for Culling of the Instances of this StaticMesh
	UPROPERTY(Category="rdInstanceSettings|Culling",EditAnywhere)
	float				startCull;

	// End Distance for Culling of the Instances of this StaticMesh
	UPROPERTY(Category="rdInstanceSettings|Culling",EditAnywhere)
	float				endCull;

	// End Distance for WorldPositionOffset of the Instances of this StaticMesh
	UPROPERTY(Category="rdInstanceSettings|Culling",EditAnywhere)
	int32				WPODisableDistance;

	// Mobility Setting for the Instances of this StaticMesh
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	uint8				mobility;

	// Type of Collision to use for the Instances of this StaticMesh
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	uint8				collision;

	// Minimum LOD to use for the Instances of this StaticMesh
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	uint8				minLOD;

	// Light Channel to use for Instances of this StaticMesh (defaults to 2)
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	int8				lightChannel;

	// Random Seed for the Instances of this StaticMesh
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	int32				randomSeed;

	// 32bit Int containing BitField of Instance Flags (See defines above this)
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	uint32				instFlags;

	// The number of Custom Float values each Instance has (For their materials)
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	int32				numPerInstCustData; 

	// The type of Shadow Invalidation for the mesh
	UPROPERTY(Category="rdInstanceSettings|General",EditAnywhere)
	int8				shadowInvalidation;
};

// Randomization DataAsset
UCLASS(BlueprintType)
class RDINST_PLUGIN_API UrdRandomizationDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()
public:

	// Label that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdRandomizationSettings)
	FName label;
	// Tooltip that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdRandomizationSettings)
	FString tooltip;

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("rdInstRandomizationAssets",GetFName()); }

	// The lower Range Vector of any Random Location movement
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			location1;

	// The upper Range Vector of any Random Location movement
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			location2;

	// The lower Range Rotator of any Random Rotation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FRotator		rotation1;

	// The upper Range Rotator of any Random Rotation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FRotator		rotation2;

	// The Probability between 0.0 and 1.0 of this Actor being Flipped in the Vectors Axis
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			flipProbability;

	// Type of Axis Lock for Scaling, 0=no lock, 1=lock X&Y, 2=lock All
	UPROPERTY(Category=rdRandom,EditAnywhere)
	int8			lockAxis;

	// The lower Range Vector of any Random Scale Variation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			scale1;

	// The upper Range Vector of any Random Scale Variation
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FVector			scale2;

	// The Probability between 0.0 and 1.0 of this Actor being Visible
	UPROPERTY(Category=rdRandom,EditAnywhere)
	float			showProbability;

	// The Name of an actor that must be Visible for this one to be
	//UPROPERTY(Category=rdRandom,EditAnywhere)
	//FString			relyActorName;

	// String Containing a comma separated list of Themes that this Actor is shown in
	UPROPERTY(Category=rdRandom,EditAnywhere)
	FString			themes;
};
