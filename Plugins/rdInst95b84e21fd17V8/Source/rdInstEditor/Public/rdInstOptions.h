// rdInstOptions.h Copyright (c) 2024 Recourse Design ltd. All rights reserved.
#pragma once
#include "Editor/PropertyEditor/Public/IDetailRootObjectCustomization.h"
#include "Engine/DataAsset.h"
#include "Engine/AssetUserData.h"
#include "rdProxies.h"
#include "rdInstOptions.generated.h"

#define RDINST_PLUGIN_API DLLEXPORT

UCLASS(ComponentWrapperClass)
class RDINST_PLUGIN_API UrdInstOptions : public UObject {
	GENERATED_BODY()
public:
				UrdInstOptions();

	void		Clear();
	void		SetAllDefault();

	FString		ToString();
	bool		FromString(const FString& str);

	// internal
	bool				defLocX1=true,defLocX2=true,defLocY1=true,defLocY2=true,defLocZ1=true,defLocZ2=true;
	bool				defRotX1=true,defRotX2=true,defRotY1=true,defRotY2=true,defRotZ1=true,defRotZ2=true;
	bool				defFlipX=true,defFlipY=true,defFlipZ=true;
	bool				defScaleX1=true,defScaleX2=true,defScaleY1=true,defScaleY2=true,defScaleZ1=true,defScaleZ2=true,defLockedAxis=true;
	bool				defShow=true;
	bool				defRely=true;
	bool				defThemes=true;

	bool				defStartCull=true,defEndCull=true,defMobility=true,defCollision=true,defMinLOD=true,defRandomSeed=true,defNumPerInstCustData=true,defWPODisableDistance=true;
	uint32				defInstFlags=0xFFFFFFFF,defLightChannel=0xFFFFFFFF;
	bool				defShadowInvalidation=true;
	bool				defCollisionProfile=true;

	bool				defProxyType=true,defProxyActor=true,defProxyAllMesh=true,defProxyPool=true,defProxySimplePool=true,defProxyNumToPool=true,defProxyDataLayerName=true,defProxyStaticMesh=true,defProxyMeshMaterials=true,defProxyMeshCollision=true;
	bool				defProxyDontRemove=true,defProxyDontHide=true,defProxyPhysicsTimeout=true,defCallSwapEvent=true;
	bool				defProxyLongDistance=true,defProxyUseDestruction=true,defProxyDestroyedMesh=true,defProxyDestroyedMaterials=true,defProxyDestroyedCollision=true,defProxyDestroyedPrefab=true,defProxyDestroyedOffsetX=true,defProxyDestroyedOffsetY=true,defProxyDestroyedOffsetZ=true;
	bool				defPickupType=true,defPickupHighlightType=true,defPickupHighlightStyle=true,defPickupID=true,defPickupHighlightThickness=true,
						defPickupStrobeSpeed=true,defPickupEbbSpeed=true,defPickupFadeHighlight=true,defPickupDistanceFadeHighlight=true;
						
	bool				defPickupHighlightColor1=true,defPickupHighlightColor2=true,defPickupRespawnTime=true,defPickupRespawnWhenLonely=true;
	bool				defMinDrawDistance=true,defNaniteProgDistance=true;
	bool				defPickupHighlightDistance=true,defPickupPickupDistance=true,defPickupRespawnDistance=true,defPickupUseStencilBuffer=true;

	// data
	FString				themes;
	FVector				rndLocation1,rndLocation2;
	FRotator			rndRotation1,rndRotation2;
	FVector				rndFlipProbability;
	FVector				rndScale1,rndScale2;
	float				rndShowProbability=1.0f;
	FString				rndRelyOnActorName;
	AActor*				relyActor=nullptr;
	int32				lockAxis=0;

	float				startCull=0.0f;
	float				endCull=0.0f;
	uint32				mobility=0;
	uint32				collision=0;
	uint32				minLOD=0;
	int32				lightChannel=0;
	int32				randomSeed=0;
	uint32				instFlags=0x170010F;   //0x17A13F;//rdInst_DefaultFlags;
	uint32				numPerInstCustData=0;
	int32				WPODisableDistance=0;
	uint8				shadowInvalidation=0;
	FString				collisionProfile;

	float				spawnDistance=0.0f;
	bool				spatial=true;
	int8				spawnMode=1;
	int32				distFrames=-1;

	// Proxy Settings
	int32						proxyType=0;
	TSoftClassPtr<AActor>		proxyActor=nullptr;
	bool						bProxyAllMesh=true;
	bool						bProxyPool=false;
	bool						bProxySimplePool=false;
	int32						ProxyNumToPool=0;
	TSoftObjectPtr<UDataLayerAsset> ProxyDataLayer=nullptr;
	TSoftObjectPtr<UStaticMesh>	ProxyStaticMesh=nullptr;
	TArray<TSoftObjectPtr<UMaterialInterface>> ProxyStaticMeshMaterials;
	int32						ProxyStaticMeshCollision=0;
	bool						bProxyDontRemove=false;
	bool						bProxyDontHide=false;
	float						ProxyPhysicsTimeout=30.0f;
	double						ProxyLongDistance=-1.0f;
	bool						bProxyUseDestruction=false;
	TSoftObjectPtr<UStaticMesh>	ProxyDestroyedMesh=nullptr;
	TArray<TSoftObjectPtr<UMaterialInterface>> ProxyDestroyedMeshMaterials;
	int32						ProxyDestroyedMeshCollision=0;
	TSoftClassPtr<AActor>		ProxyDestroyedPrefab=nullptr;
	FTransform					ProxyDestroyedOffset;
	bool						ProxyCallSwapEvent=false;
	FVector						pivotOffset=FVector(0,0,0);

	int32				pickupType=0;
	int32				pickupHighlightType=0;
	int32				pickupHighlightStyle=0;
	int32				pickupID=0;
	float				pickupHighlightThickness=0.25f;
	FLinearColor		pickupHighlightColor1=FLinearColor(1.0f,1.0f,0.0f);
	FLinearColor		pickupHighlightColor2=FLinearColor(1.0f,0.0f,1.0f);
	float				pickupStrobeSpeed=1.0f;
	float				pickupEbbSpeed=1.0f;
	bool				bPickupFadeHighlight=false;
	float				pickupRespawnTime=30.0f;
	bool				bPickupRespawnWhenLonely=false;

	bool				harvestInstances=true;
	bool				loadOnDemandBakedData=true;
	bool				freeOnHideBakedData=true;
	int32				numSpawnActorRows=1;
	int32				numSpawnActorCols=1;
	float				maxCullDistance=500000.0f;
	FString				dataAssetFolder;
	bool				updateSA_addItems=true;
	bool				updateSA_removeItems=false;
	bool				updateSA_updateTransforms=true;

	float				minDrawDistance=0.0f;
	float				naniteProgDistance=0.0f;

	int32				spawnActorGridX=16;
	int32				spawnActorGridY=16;
	float				spawnActorStreamDistance=50000.0f;
	bool				calcSpawnDistanceBySize=true;
	bool				calcSpawnDistanceDeepScan=false;

	float				localRadius=10000.0f;
	float				viewFrustumDistance=50000.0f;
	float				viewFrustumExtend=2.0f;

	bool				bRelativeSpawning=true;
	bool				bSpawnOnStaticMeshes=true;

	float				pickupHighlightDistance=400.0f;
	float				pickupPickupDistance=75.0f;
	float				pickupRespawnDistance=5000.0f;
	bool				bPickupDistanceFadeHighlight=false;
	bool				bPickupUseStencilBuffer=false;

	// not saved
	FString				dataAssetFilename;
	bool				replace=false;
	bool				outputExists=false;
	bool				overwrite=false;
	TArray<UMaterialInterface*> materialList;
	TArray<UMaterialInterface*> materialList2;
};

class FSimpleRootObjectCustomization : public IDetailRootObjectCustomization {
public:
#if ENGINE_MAJOR_VERSION<5 && ENGINE_MINOR_VERSION<25
	virtual TSharedPtr<SWidget> CustomizeObjectHeader(const UObject* InRootObject) override;
	virtual bool IsObjectVisible(const UObject* InRootObject) const override { return true; }
	virtual bool ShouldDisplayHeader(const UObject* InRootObject) const override { return false; }
#else
	virtual TSharedPtr<SWidget> CustomizeObjectHeader(const FDetailsObjectSet& InRootObjectSet) override;
	virtual bool AreObjectsVisible(const FDetailsObjectSet& InRootObjectSet) const override { return true; }
	virtual bool ShouldDisplayHeader(const FDetailsObjectSet& InRootObjectSet) const override { return false; }
#endif	
};

UCLASS()
class RDINST_PLUGIN_API UrdInstTagAssetUserData : public UAssetUserData {
	GENERATED_BODY()
public:
	const FString&	GetName() { return name; }
	void			SetName(const FString& nm) { name=nm; }

	const FString&	GetItem(const FString& nm) { if(tags.Find(nm)==nullptr){ return blankString; } return tags[nm]; }
	void			SetItem(const FString& nm,const FString& val) { tags.Add(nm,val); }

protected:
	UPROPERTY()
	FString					name;
	FString					blankString;
	UPROPERTY()
	TMap<FString,FString>	tags;
};

//----------------------------------------------------------------------------------------------------------------


