// rdProxies.h - Copyright (c) 2022 Recourse Design ltd.
//
// See rdInstBPLibrary.cpp for main Documentation
//
#pragma once
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MAJOR_VERSION>4
#if ENGINE_MINOR_VERSION>2
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#elif ENGINE_MINOR_VERSION>0
#include "WorldPartition/DataLayer/DataLayerAsset.h"
#endif
#include "WorldPartition/DataLayer/DataLayerSubsystem.h"
#endif
#include "Engine/DataAsset.h"
#include "Engine/AssetUserData.h"
#include "Engine/StaticMesh.h"
#include "Engine/EngineTypes.h"
#include "Materials/MaterialInterface.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "rdProxies.generated.h"

class ArdActor;
class UrdStateDataAsset;

#define RDINST_PLUGIN_API DLLEXPORT

UENUM(BlueprintType)
enum ErdCollision 
{ 
	/** Use the Default Collision Type specified in the Assets Instance Settings */
	UseDefault UMETA(DisplayName="Use Default"), 
	/** Will not create any representation in the physics engine. Cannot be used for spatial queries (raycasts, sweeps, overlaps) or simulation (rigid body, constraints). Best performance possible (especially for moving objects) */
	NoCollision UMETA(DisplayName="No Collision"), 
	/** Only used for spatial queries (raycasts, sweeps, and overlaps). Cannot be used for simulation (rigid body, constraints). Useful for character movement and things that do not need physical simulation. Performance gains by keeping data out of simulation tree. */
	QueryOnly UMETA(DisplayName="Query Only (No Physics Collision)"),
	/** Only used only for physics simulation (rigid body, constraints). Cannot be used for spatial queries (raycasts, sweeps, overlaps). Useful for jiggly bits on characters that do not need per bone detection. Performance gains by keeping data out of query tree */
	PhysicsOnly UMETA(DisplayName="Physics Only (No Query Collision)"),
	/** Can be used for both spatial queries (raycasts, sweeps, overlaps) and simulation (rigid body, constraints). */
	QueryAndPhysics UMETA(DisplayName="Collision Enabled (Query and Physics)"),
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	/** Only used for probing the physics simulation (rigid body, constraints). Cannot be used for spatial queries (raycasts,
	sweeps, overlaps). Useful for when you want to detect potential physics interactions and pass contact data to hit callbacks
	or contact modification, but don't want to physically react to these contacts. */
	ProbeOnly UMETA(DisplayName="Probe Only (Contact Data, No Query or Physics Collision)"),
	/** Can be used for both spatial queries (raycasts, sweeps, overlaps) and probing the physics simulation (rigid body,
	constraints). Will not allow for actual physics interaction, but will generate contact data, trigger hit callbacks, and
	contacts will appear in contact modification. */
	QueryAndProbe UMETA(DisplayName="Query and Probe (Query Collision and Contact Data, No Physics Collision)")
#endif
}; 

// Types of Proxy
UENUM()
enum rdProxyType {
	// No Proxy
	RDPROXYTYPE_NONE				UMETA(DisplayName="No Proxy"),
	// Short Distance Proxy
	RDPROXYTYPE_SHORTDISTANCE		UMETA(DisplayName="Short Distance Proxy"),
	// Long Distance Proxy
	RDPROXYTYPE_LONGDISTANCE		UMETA(DisplayName="Long Distance Proxy"),
	// Destruction Only Proxy
	RDPROXYTYPE_DESTRUCTION			UMETA(DisplayName="Destruction only Proxy"),
	// Pickup Proxy
	RDPROXYTYPE_PICKUP				UMETA(DisplayName="Pickup Proxy"),
};

// Types of tracing to use for the proxy scan
UENUM()
enum rdTraceMode {
	// Finds all instances along a line
	RDTRACEMODE_LINE			UMETA(DisplayName="Line"),
	// Finds all instances within the box
	RDTRACEMODE_BOX				UMETA(DisplayName="Box"),
	// Finds all instances within the sphere
	RDTRACEMODE_SPHERE			UMETA(DisplayName="Sphere"),
};

// Types of Pickup
UENUM()
enum rdPickupType {
	// Item is not a PickUp
	RDPICKUP_NONE				UMETA(DisplayName="None"),
	// Item is Picked Up when player overlaps
	RDPICKUP_OVERLAP			UMETA(DisplayName="Overlap"),
	// Item is Picked Up when player presses interact
	RDPICKUP_INTERACT			UMETA(DisplayName="Interact"),
	// Item is Picked Up when player presses and holds the interact button for the amount of time set in rdInstSettings
	RDPICKUP_LONGINTERACT		UMETA(DisplayName="Long Interact"),
};

// Types of Pickup Highlight
UENUM()
enum rdPickupHighlightType {
	// Don't Highlight the pickup
	RDPICKUPHIGHLIGHT_NONE			UMETA(DisplayName="No Highlight"),
	// Highlight the pickups with an outline in proximity
	RDPICKUPHIGHLIGHT_OUTLINE		UMETA(DisplayName="Outline Proximity Highlights"),
	// Highlight the Looked At pickup with an outline in proximity
	RDPICKUPHIGHLIGHT_LOOKATOUTLINE	UMETA(DisplayName="Outline LookAt Highlight"),
};

// Types of Pickup Highlight Styles
UENUM()
enum rdPickupHighlightStyle {
	// Solid Color
	RDHIGHLIGHTSTYLE_SOLID		UMETA(DisplayName="Solid Color"),
	// Pulse the Hightlight between the 2 Colors
	RDHIGHLIGHTSSTYLE_PULSE		UMETA(DisplayName="Pulsing between 2 Colors"),
	// Flash the Highlight between the 2 Colors
	RDHIGHLIGHTSTYLE_FLASH		UMETA(DisplayName="Flashing between 2 Colors"),
};

// Types of Mass Movement
UENUM()
enum rdMassMovementType {
	// No movement
	RDMASSMOVE_NONE			UMETA(DisplayName="None"),
	// StraightAhead slowly
	RDMASSMOVE_AHEADSLOW	UMETA(DisplayName="AheadSlow"),
	// StraightAhead medium
	RDMASSMOVE_AHEADMEDIUM	UMETA(DisplayName="AheadMedium"),
	// StraightAhead fast
	RDMASSMOVE_AHEADFAST	UMETA(DisplayName="AheadFast"),
	// Random movement
	RDMASSMOVE_RANDOM		UMETA(DisplayName="Random"),
	// Follow target slowly
	RDMASSMOVE_FOLLOWSLOW	UMETA(DisplayName="FollowSlow"),
	// Follow target medium
	RDMASSMOVE_FOLLOWMEDIUM	UMETA(DisplayName="FollowMedium"),
	// Follow target fast
	RDMASSMOVE_FOLLOWFAST	UMETA(DisplayName="FollowFast"),
	// Move away from target slowly
	RDMASSMOVE_AWAYSLOW		UMETA(DisplayName="AwaySlow"),
	// Move away from target medium
	RDMASSMOVE_AWAYMEDIUM	UMETA(DisplayName="AwayMedium"),
	// Move away from target fast
	RDMASSMOVE_AWAYFAST		UMETA(DisplayName="AwayFast"),
	// Flock
	RDMASSMOVE_FLOCK		UMETA(DisplayName="Flock"),
	// Swarm
	RDMASSMOVE_SWARM		UMETA(DisplayName="Swarm")
};

// Proxies

// rdPickup
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakPickup",HasNativeMake="/Script/rdInst.rdActor.MakePickup"))
struct RDINST_PLUGIN_API FrdPickup {
	GENERATED_BODY()
public:
	FrdPickup() {}
	FrdPickup(const FrdPickup& o) : 
					type(o.type),highlightType(o.highlightType),highlightStyle(o.highlightStyle),id(o.id),highlightDistance(o.highlightDistance),pickupDistance(o.pickupDistance),useStencilBuffer(o.useStencilBuffer),
					thickness(o.thickness),color1(o.color1),color2(o.color2),strobeSpeed(o.strobeSpeed),ebbSpeed(o.ebbSpeed),
					fadeToGround(o.fadeToGround),fadeInDistance(o.fadeInDistance),respawnTime(o.respawnTime),respawnLonely(o.respawnLonely),respawnDistance(o.respawnDistance)
				{}
	FrdPickup(TEnumAsByte<rdPickupType> t,TEnumAsByte<rdPickupHighlightType> ht,TEnumAsByte<rdPickupHighlightStyle> hs,int32 i,float hd,float pd,float th,FLinearColor& c1,FLinearColor& c2,float ss,float es,bool fg,bool fd,float st,bool sl,float rd,bool upp) : 
					type(t),highlightType(ht),highlightStyle(hs),id(i),highlightDistance(hd),pickupDistance(pd),useStencilBuffer(upp),
					thickness(th),color1(c1),color2(c2),strobeSpeed(ss),ebbSpeed(es),fadeToGround(fg),fadeInDistance(fd),respawnTime(st),respawnLonely(sl),respawnDistance(rd)
				{}

	UPROPERTY(Category=rdProxies,EditAnywhere)
	TEnumAsByte<rdPickupType> type=RDPICKUP_NONE;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	TEnumAsByte<rdPickupHighlightType> highlightType=RDPICKUPHIGHLIGHT_NONE;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	TEnumAsByte<rdPickupHighlightStyle> highlightStyle=RDHIGHLIGHTSTYLE_SOLID;

	// ID of the pickup
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup ID"))
	int32 id=0;

	// Distance from players for highlight
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Highlight distance"))
	float highlightDistance=600.0f;

	// Distance from players for pickup
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Pickup distance"))
	float pickupDistance=75.0f;

	// When Ticked, uses a StencilBuffer to render the highlight, otherwise it uses a inverse-hull mesh highlight
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Highlight Use StencilBuffer"))
	bool useStencilBuffer=false;

	// Thickness of the Pickup Proxy Outlines
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Outline Thickness"))
	float thickness=0.1f;

	// Color1 of the Pickup Proxy Outlines
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Outline Color1"))
	FLinearColor color1=FLinearColor(1.0f,1.0f,0.0f);

	// Color2 of the Pickup Proxy Outlines
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Outline Color2"))
	FLinearColor color2=FLinearColor(1.0f,1.0f,0.0f);

	// Strobe speed of the Pickup Proxy Highlight
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Strobe speed"))
	float strobeSpeed=0.0f;

	// Ebb speed of the Pickup Proxy Highlight
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Ebb speed"))
	float ebbSpeed=0.0f;

	// When Ticked, Highlight Fades to Black as it nears the ground
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Fade Ground Highlight"))
	bool fadeToGround=false;

	// When Ticked, Highlight Fades when further from the camera
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Fade Distance"))
	bool fadeInDistance=false;

	// Time in seconds before respawning the pickup (-1 = never)
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Respawn time"))
	float respawnTime=30.0f;

	// When Ticked, Delay Respawning pickup until no players are present
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Delay Respawn until lonely"))
	bool respawnLonely=false;

	// Distance from players for lonely respawn
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="rdInst Proxy Settings",meta=(DisplayName="Pickup Respawn distance"))
	float respawnDistance=5000.0f;
};

// rdProxySetup
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakProxySetup",HasNativeMake="/Script/rdInst.rdActor.MakeProxySetup"))
struct RDINST_PLUGIN_API FrdProxySetup {
	GENERATED_BODY()
public:
	FrdProxySetup() {}

	FrdProxySetup(const FrdProxySetup& o) : proxyType(o.proxyType),centerOffset(o.centerOffset),proxyActor(o.proxyActor),proxyDataLayer(o.proxyDataLayer),proxyStaticMesh(o.proxyStaticMesh),proxyMeshMaterials(o.proxyMeshMaterials),proxyMeshCollision(o.proxyMeshCollision),
						destroyedMesh(o.destroyedMesh),destroyedMeshMaterials(o.destroyedMeshMaterials),destroyedMeshCollision(o.destroyedMeshCollision),destroyedPrefab(o.destroyedPrefab),destroyedOffset(o.destroyedOffset),
						longDistance(o.longDistance),scanDistance(o.scanDistance),proxyPhysicsTimeout(o.proxyPhysicsTimeout),
						bUseWithDestruction(o.bUseWithDestruction),bPooled(o.bPooled),bSimplePool(o.bSimplePool),pooledAmount(o.pooledAmount),bDontRemove(o.bDontRemove),
						bDontHide(o.bDontHide),bEntireMeshProxy(o.bEntireMeshProxy),savedState(o.savedState),bCallSwapEvent(o.bCallSwapEvent),
						pickup(o.pickup) {}

	FrdProxySetup(TEnumAsByte<rdProxyType> pt,TSoftClassPtr<AActor> pa,TSoftObjectPtr<UDataLayerAsset> pdl,TSoftObjectPtr<UStaticMesh> psm,TArray<TSoftObjectPtr<UMaterialInterface>> smm,TEnumAsByte<ErdCollision> smCol,TSoftObjectPtr<UStaticMesh> dm,TArray<TSoftObjectPtr<UMaterialInterface>> dmm,TEnumAsByte<ErdCollision> dmCol,TSoftClassPtr<AActor> dp,const FTransform& dof,float sd,float pto,bool ud,bool p,bool sp,int32 pam,bool dr,bool dh,bool em,UrdStateDataAsset* ss,bool cse,FrdPickup pu) :
						proxyType(pt),centerOffset(0,0,0),proxyActor(pa),proxyDataLayer(pdl),proxyStaticMesh(psm),proxyMeshMaterials(smm),proxyMeshCollision(smCol),destroyedMesh(dm),destroyedMeshMaterials(dmm),destroyedMeshCollision(dmCol),destroyedPrefab(dp),destroyedOffset(dof),longDistance(sd),scanDistance(sd),proxyPhysicsTimeout(pto),
						bUseWithDestruction(ud),bPooled(p),bSimplePool(sp),pooledAmount(pam),bDontRemove(dr),bDontHide(dh),bEntireMeshProxy(em),savedState(ss),bCallSwapEvent(cse),
						pickup(pu) {}

	UPROPERTY(Category="rdProxySetup|Proxy",EditAnywhere)
	TEnumAsByte<rdProxyType> proxyType=RDPROXYTYPE_NONE;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	FVector			centerOffset=FVector(0,0,0);

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	TSoftClassPtr<AActor> proxyActor=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|DataLayer",EditAnywhere)
	TSoftObjectPtr<UDataLayerAsset> proxyDataLayer=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere)
	TSoftObjectPtr<UStaticMesh> proxyStaticMesh=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere,meta=(IgnoreForMemberInitializationTest))
	TArray<TSoftObjectPtr<UMaterialInterface>> proxyMeshMaterials;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere)
	TEnumAsByte<ErdCollision> proxyMeshCollision=ErdCollision::UseDefault;

	UPROPERTY(Category="rdProxySetup|Destruction",EditAnywhere)
	TSoftObjectPtr<UStaticMesh> destroyedMesh=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere,meta=(IgnoreForMemberInitializationTest))
	TArray<TSoftObjectPtr<UMaterialInterface>> destroyedMeshMaterials;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere)
	TEnumAsByte<ErdCollision> destroyedMeshCollision=ErdCollision::UseDefault;

	UPROPERTY(Category="rdProxySetup|Destruction",EditAnywhere)
	TSoftClassPtr<AActor> destroyedPrefab=nullptr;

	UPROPERTY(Category="rdProxySetup|Destruction",EditAnywhere)
	FTransform	destroyedOffset=FTransform::Identity;

	// This property is now depreciated, use scanDistance instead
	UPROPERTY(Category="rdProxySetup|System",EditAnywhere,meta=(DepreciatedFunction))
	float longDistance=10000.0f;

	// The distance to scan (either Short or Long Distance). -1 means use the Default in rdInstSettings
	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	float scanDistance=-1.0f;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	float proxyPhysicsTimeout=30.0f;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bUseWithDestruction:1=0;

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	uint8 bPooled:1=0;

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	uint8 bSimplePool:1=0;

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	int32 pooledAmount=0;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bDontRemove:1=0;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bDontHide:1=0;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bEntireMeshProxy:1=1;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	UrdStateDataAsset* savedState=nullptr;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	bool bCallSwapEvent=false;

	UPROPERTY(Category="rdProxySetup|Pickup",EditAnywhere)
	FrdPickup pickup;

	FString ToString();
	bool FromString(const FString& str);
};

// FrdProxyItem
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakProxyItem",HasNativeMake="/Script/rdInst.rdActor.MakeProxyItem"))
struct RDINST_PLUGIN_API FrdProxyItem {
	GENERATED_BODY()
	FrdProxyItem() : bHasProxy(false),bUseWithDestruction(false),bPooled(true),bSimplePool(false),bDontRemove(false),bDontHide(false),bDestroyed(false),bCallSwapEvent(false) {}
	FrdProxyItem(const FrdProxyItem& o) : centerOffset(o.centerOffset),bHasProxy(o.bHasProxy),sid(o.sid),ismc(o.ismc),destroyedSid(o.destroyedSid),destroyedPrefab(o.destroyedPrefab),destroyedOffset(o.destroyedOffset),
					instanceIndex(o.instanceIndex),prefab(o.prefab),actorClass(o.actorClass),
					proxyDataLayer(o.proxyDataLayer),proxyStaticMeshSid(o.proxyStaticMeshSid),distance(o.distance),transform(o.transform),
					proxyActor(o.proxyActor),proxyPhysicsTimeout(o.proxyPhysicsTimeout),proxyInstanceIndex(o.proxyInstanceIndex),proxyInstanceComponent(o.proxyInstanceComponent),
					proxyType(o.proxyType),bUseWithDestruction(o.bUseWithDestruction),bPooled(o.bPooled),bSimplePool(o.bSimplePool),bDontRemove(o.bDontRemove),
					bDontHide(o.bDontHide),savedState(o.savedState),instanceVolume(o.instanceVolume),bDestroyed(o.bDestroyed),bCallSwapEvent(o.bCallSwapEvent),
					pickup(o.pickup),swappedInstances(o.swappedInstances) {}
	FrdProxyItem(const FName& s,UInstancedStaticMeshComponent* isc,const FName dmSid,UClass* dp,const FTransform& dof,int32 ii,AActor* pf,UClass* ac,float d,const FTransform& t,AActor* ia,float pto,int32 pii,UInstancedStaticMeshComponent* pic,TSoftObjectPtr<UDataLayerAsset> pdl,const FName& psd,TEnumAsByte<rdProxyType> pt,bool ud,bool p,bool sp,bool dr,bool dh,AActor* iv,bool dy,UPARAM(ref)UrdStateDataAsset* ss,bool cse,FrdPickup pu) : 
					bHasProxy(false),sid(s),ismc(isc),destroyedSid(dmSid),destroyedPrefab(dp),destroyedOffset(dof),instanceIndex(ii),prefab(pf),actorClass(ac),
					proxyDataLayer(pdl),proxyStaticMeshSid(psd),distance(d),transform(t),proxyActor(ia),proxyPhysicsTimeout(pto),proxyInstanceIndex(pii),proxyInstanceComponent(pic),
					proxyType(pt),bUseWithDestruction(ud),bPooled(p),bSimplePool(sp),bDontRemove(dr),bDontHide(dh),savedState(ss),instanceVolume(iv),bDestroyed(dy),
					bCallSwapEvent(cse),pickup(pu) {}
	FrdProxyItem(const FrdProxySetup& o,const FName sourceSid,const FName proxySid,const FName destroyedProxySid) : 
					centerOffset(o.centerOffset),bHasProxy(false),sid(sourceSid),destroyedSid(destroyedProxySid),destroyedPrefab(o.destroyedPrefab.LoadSynchronous()),destroyedOffset(o.destroyedOffset),
					actorClass(o.proxyActor.LoadSynchronous()),
					proxyDataLayer(o.proxyDataLayer),proxyStaticMeshSid(proxySid),distance(o.longDistance),
					proxyPhysicsTimeout(o.proxyPhysicsTimeout),
					proxyType(o.proxyType),bUseWithDestruction(o.bUseWithDestruction),bPooled(o.bPooled),bSimplePool(o.bSimplePool),bDontRemove(o.bDontRemove),
					bDontHide(o.bDontHide),savedState(o.savedState),bDestroyed(false),bCallSwapEvent(o.bCallSwapEvent),
					pickup(o.pickup) {}
public:

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	FVector			centerOffset=FVector(0,0,0);

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	uint8			bHasProxy:1;

	UPROPERTY(Category="rdProxy|Source",EditAnywhere)
	FName sid;

	UPROPERTY(Category="rdProxy|Source",EditAnywhere)
	UInstancedStaticMeshComponent* ismc=nullptr;

	UPROPERTY(Category="rdProxy|Destruction",EditAnywhere)
	FName destroyedSid;

	UPROPERTY(Category="rdProxy|Destruction",EditAnywhere)
	UClass* destroyedPrefab=nullptr;

	UPROPERTY(Category="rdProxy|Destruction",EditAnywhere)
	FTransform	destroyedOffset;

	UPROPERTY(Category="rdProxy|Source",EditAnywhere)
	int32 instanceIndex=-1;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	AActor* prefab=nullptr;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	UClass*	actorClass=nullptr;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	TSoftObjectPtr<UDataLayerAsset> proxyDataLayer;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	FName proxyStaticMeshSid;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	double distance=-1.0f;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	FTransform	transform;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	AActor* proxyActor=nullptr;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	float proxyPhysicsTimeout=30.0f;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	int32 proxyInstanceIndex=-1;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	UInstancedStaticMeshComponent*	proxyInstanceComponent=nullptr;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	TEnumAsByte<rdProxyType> proxyType=RDPROXYTYPE_SHORTDISTANCE;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	uint8 bUseWithDestruction:1;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	uint8 bPooled:1;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	uint8 bSimplePool:1;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	uint8 bDontRemove:1;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	uint8 bDontHide:1;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	UrdStateDataAsset* savedState=nullptr;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	AActor* instanceVolume=nullptr;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	uint8 bDestroyed:1;

	UPROPERTY(Category="rdProxy|System",EditAnywhere)
	uint8 bCallSwapEvent:1;

	UPROPERTY(Category="rdProxy|Proxy",EditAnywhere)
	FrdPickup pickup;

	TArray<int32> swappedInstances;
};

// FrdProxyInstanceItems
USTRUCT(BlueprintType)
struct RDINST_PLUGIN_API FrdProxyInstanceItems {
	GENERATED_BODY()
public:
	UPROPERTY(Category=rdProxies,EditAnywhere)
	TMap<int32,FrdProxyItem> proxyInstanceIndexList;
	UPROPERTY(Category=rdProxies,EditAnywhere)
	TMap<AActor*,FrdProxyItem> proxyActorList;
};

// FrdProxyActorItem
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakProxyActorItem",HasNativeMake="/Script/rdInst.rdActor.MakeProxyActorItem"))
struct RDINST_PLUGIN_API FrdProxyActorItem {
	GENERATED_BODY()
public:
	FrdProxyActorItem() : forDestruction(false),forLongDistance(false),forImpact(false),bDontRemove(false),bDontHide(false) {}
	FrdProxyActorItem(const FrdProxyActorItem& o) : sid(o.sid),proxyActor(o.proxyActor),proxyInstanceIndex(o.proxyInstanceIndex),proxyInstanceComponent(o.proxyInstanceComponent),proxyItem(o.proxyItem),proxySource(o.proxySource),instanceIndex(o.instanceIndex),instanceTransform(o.instanceTransform),ismc(o.ismc),idleCountdown(o.idleCountdown),forDestruction(o.forDestruction),forLongDistance(o.forLongDistance),forImpact(o.forImpact),bDontRemove(o.bDontRemove),bDontHide(o.bDontHide),spawnMillis(o.spawnMillis),hit(o.hit) {}
	FrdProxyActorItem(const FrdProxyItem& o) : sid(o.sid),proxyActor(o.proxyActor),proxyInstanceIndex(o.proxyInstanceIndex),proxyInstanceComponent(o.proxyInstanceComponent),proxyItem(o),proxySource(0),instanceIndex(o.instanceIndex),instanceTransform(o.transform),ismc(o.ismc),idleCountdown(0),forDestruction(o.proxyType==RDPROXYTYPE_DESTRUCTION),forLongDistance(o.proxyType==RDPROXYTYPE_LONGDISTANCE),forImpact(false),bDontRemove(o.bDontRemove),bDontHide(o.bDontHide) {}
	FrdProxyActorItem(AActor* pa,int32 pii,UInstancedStaticMeshComponent* pic,const FrdProxyItem& pi,int32 ps,int32 ii,const FTransform& t,int32 ic,bool fd,bool fld,bool fim,bool dr,bool dh) : proxyActor(pa),proxyInstanceIndex(pii),proxyInstanceComponent(pic),proxyItem(pi),proxySource(0),instanceIndex(-1),instanceTransform(pi.transform),idleCountdown(10),forDestruction(fd),forLongDistance(fld),forImpact(fim),bDontRemove(dr),bDontHide(dh) {}
	FrdProxyActorItem(const FName s,int32 i,const FrdProxyItem& o,bool fim,int32 ps,FHitResult* h=nullptr) : sid(s),proxyActor(o.proxyActor),proxyInstanceIndex(o.instanceIndex),proxyInstanceComponent(nullptr),proxyItem(o),proxySource(ps),instanceIndex(i),instanceTransform(o.transform),ismc(o.ismc),idleCountdown(0),forDestruction(o.proxyType==RDPROXYTYPE_DESTRUCTION),forLongDistance(o.proxyType==RDPROXYTYPE_LONGDISTANCE),forImpact(fim),bDontRemove(o.bDontRemove),bDontHide(o.bDontHide) { if(h) hit=*h; }

	UPROPERTY(Category=rdProxies,EditAnywhere)
	FName			sid;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	AActor*			proxyActor=nullptr;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	int32			proxyInstanceIndex=-1;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	UInstancedStaticMeshComponent*	proxyInstanceComponent=nullptr;

	FrdProxyItem	proxyItem;
	int32			proxySource=0; // 0=per-instance proxy,1=all-instance proxy,2=long distance instance proxy,3=prefab proxy,4=long distance prefab proxy

	UPROPERTY(Category=rdProxies,EditAnywhere)
	int32			instanceIndex=-1;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	FTransform		instanceTransform;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	UInstancedStaticMeshComponent*	ismc=nullptr;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	int32			idleCountdown=10;

	UPROPERTY(Category=rdProxies,EditAnywhere)
	uint8			forDestruction:1;
	UPROPERTY(Category=rdProxies,EditAnywhere)
	uint8			forLongDistance:1;
	UPROPERTY(Category=rdProxies,EditAnywhere)
	uint8			forImpact:1;
	UPROPERTY(Category=rdProxies,EditAnywhere)
	uint8			bDontRemove:1;
	UPROPERTY(Category=rdProxies,EditAnywhere)
	uint8			bDontHide:1;

	uint64			spawnMillis=0;
	FHitResult		hit;
};

// FrdInstanceItems
USTRUCT(BlueprintType,meta=(HasNativeBreak="/Script/rdInst.rdActor.BreakInstanceItems",HasNativeMake="/Script/rdInst.rdActor.MakeInstanceItems"))
struct RDINST_PLUGIN_API FrdInstanceItems {
	GENERATED_BODY()
public:

	FrdInstanceItems() {}
	FrdInstanceItems(const FrdInstanceItems& o) : sid(o.sid),volume(o.volume),instances(o.instances),transforms(o.transforms) {}
	FrdInstanceItems(const FName s,const TArray<int32>& i,const TArray<FTransform>& t) : sid(s),instances(i),transforms(t) {}

	UPROPERTY(Category=rdInstances,EditAnywhere)
	FName sid;

	UPROPERTY(Category=rdInstances,EditAnywhere)
	AActor* volume=nullptr;

	UPROPERTY(Category=rdInstances,EditAnywhere)
	TArray<int32> instances;

	UPROPERTY(Category=rdInstances,EditAnywhere)
	TArray<FTransform> transforms;
};

struct DamageQueueItem {
				DamageQueueItem(AActor* c,int32 i,FTransform& t,FVector& im,FHitResult& h) : caller(c),instance(i),transform(t),impulse(im),hit(h) {}
				DamageQueueItem(AActor* c,ArdActor* p,FTransform& t,FVector& im,FHitResult& h) : caller(c),transform(t),impulse(im),prefab(p),hit(h) {}
	AActor*		caller=nullptr;
	int32		instance=-1;
	FTransform	transform;
	FVector		impulse;
	ArdActor*	prefab=nullptr;
	FHitResult	hit;
};

struct DamageQueueArray {
public:
	DamageQueueItem*		Find(ArdActor* p) { for(auto& i:items) if(i.prefab==p) return &i; return nullptr; }
	TArray<DamageQueueItem>	items;
};

struct DamageQueue {
public:
	void				 	     Add(DamageQueue& dq) { for(auto& it:dq.queueMap) Add(it.Key,it.Value); }
	void				 	     Add(FName sid,DamageQueueArray& dqa) { DamageQueueArray& dqa2=queueMap.FindOrAdd(sid); dqa2.items.Append(dqa.items); }
	void				 	     Add(FName sid,DamageQueueItem& dqi) { DamageQueueArray& dqa=queueMap.FindOrAdd(sid); dqa.items.Add(dqi); }
	void				 	     Add(AActor* caller,FName sid,int32 i,FTransform& t,FVector& im,FHitResult& h) { DamageQueueArray& dqa=queueMap.FindOrAdd(sid); dqa.items.Add(DamageQueueItem(caller,i,t,im,h)); }
	void				 	     Add(AActor* caller,FName sid,ArdActor* p,FTransform& t,FVector& im,FHitResult& h) { DamageQueueArray& dqa=queueMap.FindOrAdd(sid); if(!dqa.Find(p)) dqa.items.Add(DamageQueueItem(caller,p,t,im,h)); }
	TMap<FName,DamageQueueArray> queueMap;
};

struct FAddProxyQueueItem {
public:
						FAddProxyQueueItem(FrdProxyActorItem& ai,FTransform& t) : actorItem(ai),transform(t) {}
						FAddProxyQueueItem(FrdProxyActorItem& ai,FTransform& t,FHitResult& h) : actorItem(ai),transform(t),forImpact(true),hit(h) {}
	FrdProxyActorItem	actorItem;
	FTransform			transform;
	bool				forImpact=false;
	FHitResult			hit;
};

// ProxySetup DataAsset
UCLASS(BlueprintType)
class RDINST_PLUGIN_API UrdProxyDataAsset : public UPrimaryDataAsset {
	GENERATED_BODY()
public:

	// Label that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdProxySetup)
	FName label;
	// Tooltip that gets displayed in the Quick Settings Menu
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=rdProxySetup)
	FString tooltip;

	FPrimaryAssetId GetPrimaryAssetId() const override { return FPrimaryAssetId("rdInstProxyAssets",GetFName()); }

	UPROPERTY(Category="rdProxySetup|Proxy",EditAnywhere)
	TEnumAsByte<rdProxyType> proxyType=RDPROXYTYPE_SHORTDISTANCE;

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	TSoftClassPtr<UClass> proxyActor=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|DataLayer",EditAnywhere)
	TSoftObjectPtr<UDataLayerAsset> proxyDataLayer=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere)
	TSoftObjectPtr<UStaticMesh> proxyStaticMesh=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere,meta=(IgnoreForMemberInitializationTest))
	TArray<TSoftObjectPtr<UMaterialInterface>> proxyMeshMaterials;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere)
	TEnumAsByte<ErdCollision> proxyMeshCollision;

	UPROPERTY(Category="rdProxySetup|Destruction",EditAnywhere)
	TSoftObjectPtr<UStaticMesh> destroyedMesh=nullptr;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere,meta=(IgnoreForMemberInitializationTest))
	TArray<TSoftObjectPtr<UMaterialInterface>> destroyedMeshMaterials;

	UPROPERTY(Category="rdProxySetup|Proxy|StaticMesh",EditAnywhere)
	TEnumAsByte<ErdCollision> destroyedMeshCollision;

	UPROPERTY(Category="rdProxySetup|Destruction",EditAnywhere)
	UClass* destroyedPrefab=nullptr;

	UPROPERTY(Category="rdProxySetup|Destruction",EditAnywhere)
	FTransform	destroyedOffset=FTransform::Identity;

	// This property is now depreciated, use scanDistance instead
	UPROPERTY(Category="rdProxySetup|System",EditAnywhere,meta=(DepreciatedFunction))
	float longDistance=10000.0f;

	// The distance to scan (either Short or Long Distance). -1 means use the Default in rdInstSettings
	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	float scanDistance=-1.0f;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	float proxyPhysicsTimeout=30.0f;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bUseWithDestruction:1;

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	uint8 bPooled:1;

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	uint8 bSimplePool:1;

	UPROPERTY(Category="rdProxySetup|Proxy|Actor",EditAnywhere)
	int32 pooledAmount=0;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bDontRemove:1;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bDontHide:1;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	uint8 bEntireMeshProxy:1;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	UrdStateDataAsset* savedState=nullptr;

	UPROPERTY(Category="rdProxySetup|System",EditAnywhere)
	bool bCallSwapEvent=false;

	UPROPERTY(Category="rdProxySetup|Pickup",EditAnywhere)
	FrdPickup pickup;
};

// ProxySetupQueue Item
struct FrdProxySetupQueueItem {
public:
	FrdProxySetupQueueItem(int32 t,FName s,AActor* p,int32 i,double d,FrdProxyItem& px,const FTransform& tn,UInstancedStaticMeshComponent* ism) : type(t),sid(s),prefab(p),index(i),distance(d),proxy(px),transform(tn),ismc(ism) {}
	int32			type=0; // 0=Local Proxy,1=LongDistance Proxy,2=Destruction Proxy,3=Pickup
	FName			sid;
	AActor*			prefab=nullptr;
	int32			index=-1;
	double			distance=0.0f;
	FrdProxyItem	proxy;
	FTransform		transform;
	UInstancedStaticMeshComponent* ismc=nullptr;
};

struct ProxyRemoveQueueItem {
	ProxyRemoveQueueItem(int32 t,FrdProxyItem& i,FrdProxyActorItem* pa) : type(t),item(i),pai(pa) {}
	int32				type;
	FrdProxyItem		item;
	FrdProxyActorItem*	pai;
};

UCLASS()
class RDINST_PLUGIN_API UrdInstProxyAssetUserData : public UAssetUserData {
	GENERATED_BODY()
public:
	UPROPERTY()
	FrdProxySetup		proxy;
};
