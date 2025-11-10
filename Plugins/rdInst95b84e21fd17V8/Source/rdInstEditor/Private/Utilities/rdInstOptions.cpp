//
// rdInstOptions
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 18th August 2024
// Last Modified: 14th May 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstOptions.h"
#include "rdInstances.h"

//.............................................................................
// Options constructor
//.............................................................................
UrdInstOptions::UrdInstOptions() {
}

//----------------------------------------------------------------------------------------------------------------
// ToString
//----------------------------------------------------------------------------------------------------------------
FString	UrdInstOptions::ToString() {

	FString str="6"; // version

	str+=FString::Printf(TEXT(	",%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,"
								"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s,%d,"
								"%f,%d,%d,%d,"
								"%d,%d,%d,%d,%d,%s,"
								"%s,%d,%d,%f,%f,%d,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,"
								"%d,%d,%d,%d,%f,%f,%d,%f,%d,%f,%f,%f,%f,%f,%f,"
								"%d,%d,%d,%d,%d,%f,%s,%d,%d,%d,"
								"%f,"
								"%d,%d,%f,%d,%d,%f,%f,%f,%d,%d,"
								"%f,%f,%f,%d,%d"),
		startCull,
		endCull,
		mobility,
		collision,
		minLOD,
		lightChannel,
		randomSeed,
		instFlags,
		numPerInstCustData,
		WPODisableDistance,
		shadowInvalidation,
		*collisionProfile, 

		rndLocation1.X,
		rndLocation1.Y,
		rndLocation1.Z,
		rndLocation2.X,
		rndLocation2.Y,
		rndLocation2.Z,
		rndRotation1.Roll,
		rndRotation1.Pitch,
		rndRotation1.Yaw,
		rndRotation2.Roll,
		rndRotation2.Pitch,
		rndRotation2.Yaw,
		rndFlipProbability.X,
		rndFlipProbability.Y,
		rndFlipProbability.Z,
		rndScale1.X,
		rndScale1.Y,
		rndScale1.Z,
		rndScale2.X,
		rndScale2.Y,
		rndScale2.Z,
		rndShowProbability,
		*(themes.Replace(TEXT(","),TEXT("~"))),
		lockAxis,

		spawnDistance,
		spatial,
		(int32)spawnMode,
		distFrames,

		proxyType,
		bProxyAllMesh,
		bProxyPool,
		bProxySimplePool,
		ProxyNumToPool,
		*(ProxyDataLayer.ToString()),

		*(ProxyStaticMesh.ToString()),
		bProxyDontRemove,
		bProxyDontHide,
		ProxyPhysicsTimeout,
		ProxyLongDistance,
		bProxyUseDestruction,
		*(ProxyDestroyedMesh.ToString()),
		*(ProxyDestroyedPrefab.ToString()),
		ProxyDestroyedOffset.GetTranslation().X,
		ProxyDestroyedOffset.GetTranslation().Y,
		ProxyDestroyedOffset.GetTranslation().Z,
		ProxyDestroyedOffset.GetScale3D().X,
		ProxyDestroyedOffset.GetScale3D().Y,
		ProxyDestroyedOffset.GetScale3D().Z,
		ProxyDestroyedOffset.Rotator().Roll,
		ProxyDestroyedOffset.Rotator().Pitch,
		ProxyDestroyedOffset.Rotator().Yaw,
		ProxyCallSwapEvent,

		pickupType,
		pickupHighlightType,
		pickupHighlightStyle,
		pickupID,
		pickupStrobeSpeed,
		pickupEbbSpeed,
		bPickupFadeHighlight,
		pickupRespawnTime,
		bPickupRespawnWhenLonely,
		pickupHighlightColor1.R,
		pickupHighlightColor1.G,
		pickupHighlightColor1.B,
		pickupHighlightColor2.R,
		pickupHighlightColor2.G,
		pickupHighlightColor2.B,

		harvestInstances,
		loadOnDemandBakedData,
		freeOnHideBakedData,
		numSpawnActorRows,
		numSpawnActorCols,
		maxCullDistance,
		*dataAssetFolder,
		updateSA_addItems,
		updateSA_removeItems,
		updateSA_updateTransforms,

		pickupHighlightThickness,

		spawnActorGridX,
		spawnActorGridY,
		spawnActorStreamDistance,
		calcSpawnDistanceBySize,
		calcSpawnDistanceDeepScan,
		localRadius,
		viewFrustumDistance,
		viewFrustumExtend,
		bRelativeSpawning,
		bSpawnOnStaticMeshes,

		pickupHighlightDistance,
		pickupPickupDistance,
		pickupRespawnDistance,
		bPickupDistanceFadeHighlight,
		bPickupUseStencilBuffer

	);
	return str;
}

//----------------------------------------------------------------------------------------------------------------
// SetAllDefault
//----------------------------------------------------------------------------------------------------------------
void UrdInstOptions::SetAllDefault() {

	defLocX1=defLocX2=defLocY1=defLocY2=defLocZ1=defLocZ2=true;
	defRotX1=defRotX2=defRotY1=defRotY2=defRotZ1=defRotZ2=true;
	defFlipX=defFlipY=defFlipZ=true;
	defScaleX1=defScaleX2=defScaleY1=defScaleY2=defScaleZ1=defScaleZ2=true;
	defLockedAxis=true;
	defShow=true;
	defRely=true;
	defThemes=true;

	defStartCull=defEndCull=defMobility=defCollision=defMinLOD=defRandomSeed=defNumPerInstCustData=defWPODisableDistance=true;
	defLightChannel=0xFF;
	defInstFlags=0x0FFFFFFF;
	defShadowInvalidation=true;
	defCollisionProfile=true;

	defProxyType=defProxyActor=defProxyAllMesh=defProxyPool=defProxySimplePool=defProxyNumToPool=defProxyDataLayerName=defProxyStaticMesh=defProxyMeshMaterials=defProxyMeshCollision=true;
	defProxyDontRemove=defProxyDontHide=defProxyPhysicsTimeout=defCallSwapEvent=true;
	defProxyLongDistance=defProxyUseDestruction=defProxyDestroyedMesh=defProxyDestroyedMaterials=defProxyDestroyedCollision=defProxyDestroyedPrefab=defProxyDestroyedOffsetX=defProxyDestroyedOffsetY=defProxyDestroyedOffsetZ=true;
	defPickupType=defPickupHighlightType=defPickupHighlightStyle=defPickupID=defPickupHighlightThickness=defPickupStrobeSpeed=defPickupEbbSpeed=defPickupFadeHighlight=true;
	defPickupHighlightColor1=defPickupHighlightColor2=defPickupRespawnTime=defPickupRespawnWhenLonely=true;
	defMinDrawDistance=defNaniteProgDistance=true;
	defPickupHighlightDistance=defPickupPickupDistance=defPickupRespawnDistance=defPickupDistanceFadeHighlight=defPickupUseStencilBuffer=true;
}

//----------------------------------------------------------------------------------------------------------------
// FromString
//----------------------------------------------------------------------------------------------------------------
bool UrdInstOptions::FromString(const FString& str) {

	SetAllDefault();

	TArray<FString> vals;
	int32 num=str.ParseIntoArray(vals,TEXT(","),false);
	if(num<13) {
		return false;
	}

	int32 version=FCString::Atoi(*vals[0]);
	if(version>6 || (version==1 && num!=13) || (version==2 && num!=37) || (version==3 && num!=79) || (version==4 && num!=89) || (version==5 && num!=90) ||
		            (version==6 && num!=105) ) {
		return false;
	}

	int i=1;

	// Version 1 properties
	startCull=FCString::Atof(*vals[i++]);
	endCull=FCString::Atof(*vals[i++]);
	mobility=FCString::Atoi(*vals[i++]);
	collision=FCString::Atoi(*vals[i++]);
	minLOD=FCString::Atoi(*vals[i++]);
	lightChannel=FCString::Atoi(*vals[i++]);
	randomSeed=FCString::Atoi(*vals[i++]);
	instFlags=FCString::Atoi(*vals[i++]);

	numPerInstCustData=FCString::Atoi(*vals[i++]);
	WPODisableDistance=FCString::Atoi(*vals[i++]);
	shadowInvalidation=FCString::Atoi(*vals[i++]);
	collisionProfile=vals[i++];

	if(version<=1) return true;

	// Version 2 properties
	rndLocation1.X=FCString::Atof(*vals[i++]);
	rndLocation1.Y=FCString::Atof(*vals[i++]);
	rndLocation1.Z=FCString::Atof(*vals[i++]);
	rndLocation2.X=FCString::Atof(*vals[i++]);
	rndLocation2.Y=FCString::Atof(*vals[i++]);
	rndLocation2.Z=FCString::Atof(*vals[i++]);
	rndRotation1.Roll=FCString::Atof(*vals[i++]);
	rndRotation1.Pitch=FCString::Atof(*vals[i++]);
	rndRotation1.Yaw=FCString::Atof(*vals[i++]);
	rndRotation2.Roll=FCString::Atof(*vals[i++]);
	rndRotation2.Pitch=FCString::Atof(*vals[i++]);
	rndRotation2.Yaw=FCString::Atof(*vals[i++]);
	rndFlipProbability.X=FCString::Atof(*vals[i++]);
	rndFlipProbability.Y=FCString::Atof(*vals[i++]);
	rndFlipProbability.Z=FCString::Atof(*vals[i++]);
	rndScale1.X=FCString::Atof(*vals[i++]);
	rndScale1.Y=FCString::Atof(*vals[i++]);
	rndScale1.Z=FCString::Atof(*vals[i++]);
	rndScale2.X=FCString::Atof(*vals[i++]);
	rndScale2.Y=FCString::Atof(*vals[i++]);
	rndScale2.Z=FCString::Atof(*vals[i++]);
	rndShowProbability=FCString::Atof(*vals[i++]);
	themes=vals[i++].Replace(TEXT("~"),TEXT(","));
	lockAxis=FCString::Atoi(*vals[i++]);

	if(version==2) return true;

	// Version 3 properties
	spawnDistance=FCString::Atof(*vals[i++]);
	spatial=vals[i++].ToBool();
	spawnMode=FCString::Atoi(*vals[i++]);
	distFrames=FCString::Atoi(*vals[i++]);

	proxyType=FCString::Atoi(*vals[i++]);
	bProxyAllMesh=vals[i++].ToBool();
	bProxyPool=vals[i++].ToBool();
	bProxySimplePool=vals[i++].ToBool();
	ProxyNumToPool=FCString::Atoi(*vals[i++]);
	ProxyDataLayer=vals[i++];
	ProxyStaticMesh=FSoftObjectPath(vals[i++]);
	ProxyStaticMesh.ToSoftObjectPath().PostLoadPath(nullptr);
	bProxyDontRemove=vals[i++].ToBool();
	bProxyDontHide=vals[i++].ToBool();
	ProxyPhysicsTimeout=vals[i++].ToBool();
	ProxyLongDistance=FCString::Atof(*vals[i++]);
	bProxyUseDestruction=vals[i++].ToBool();
	ProxyDestroyedMesh=FSoftObjectPath(vals[i++]);
	ProxyDestroyedMesh.ToSoftObjectPath().PostLoadPath(nullptr);
	ProxyDestroyedPrefab=FSoftObjectPath(vals[i++]);
	ProxyDestroyedPrefab.ToSoftObjectPath().PostLoadPath(nullptr);
	FVector t,s;
	FRotator r;
	t.X=FCString::Atof(*vals[i++]);
	t.Y=FCString::Atof(*vals[i++]);
	t.Z=FCString::Atof(*vals[i++]);
	s.X=FCString::Atof(*vals[i++]);
	s.Y=FCString::Atof(*vals[i++]);
	s.Z=FCString::Atof(*vals[i++]);
	r.Roll=FCString::Atof(*vals[i++]);
	r.Pitch=FCString::Atof(*vals[i++]);
	r.Yaw=FCString::Atof(*vals[i++]);
	ProxyDestroyedOffset=FTransform(r,t,s);
	ProxyCallSwapEvent=vals[i++].ToBool();

	pickupType=FCString::Atoi(*vals[i++]);
	pickupHighlightType=FCString::Atoi(*vals[i++]);
	pickupHighlightStyle=FCString::Atoi(*vals[i++]);
	pickupID=FCString::Atoi(*vals[i++]);
	pickupStrobeSpeed=FCString::Atof(*vals[i++]);
	pickupEbbSpeed=FCString::Atof(*vals[i++]);
	bPickupFadeHighlight=vals[i++].ToBool();
	pickupRespawnTime=FCString::Atof(*vals[i++]);
	bPickupRespawnWhenLonely=vals[i++].ToBool();
	pickupHighlightColor1.R=FCString::Atof(*vals[i++]);
	pickupHighlightColor1.G=FCString::Atof(*vals[i++]);
	pickupHighlightColor1.B=FCString::Atof(*vals[i++]);
	pickupHighlightColor2.R=FCString::Atof(*vals[i++]);
	pickupHighlightColor2.G=FCString::Atof(*vals[i++]);
	pickupHighlightColor2.B=FCString::Atof(*vals[i++]);

	if(version==3) return true;

	// Version 4 properties
	harvestInstances=vals[i++].ToBool();
	loadOnDemandBakedData=vals[i++].ToBool();
	freeOnHideBakedData=vals[i++].ToBool();
	numSpawnActorRows=FCString::Atoi(*vals[i++]);
	numSpawnActorCols=FCString::Atoi(*vals[i++]);
	maxCullDistance=FCString::Atof(*vals[i++]);
	dataAssetFolder=vals[i++];
	updateSA_addItems=vals[i++].ToBool();
	updateSA_removeItems=vals[i++].ToBool();
	updateSA_updateTransforms=vals[i++].ToBool();

	if(version==4) return true;

	// Version 5 properties
	pickupHighlightThickness=FCString::Atof(*vals[i++]);

	if(version==5) return true;

	// Version 6 properties
	spawnActorGridX=FCString::Atoi(*vals[i++]);
	spawnActorGridY=FCString::Atoi(*vals[i++]);
	spawnActorStreamDistance=FCString::Atof(*vals[i++]);
	calcSpawnDistanceBySize=vals[i++].ToBool();
	calcSpawnDistanceDeepScan=vals[i++].ToBool();
	localRadius=FCString::Atof(*vals[i++]);
	viewFrustumDistance=FCString::Atof(*vals[i++]);
	viewFrustumExtend=FCString::Atof(*vals[i++]);
	bRelativeSpawning=vals[i++].ToBool();
	bSpawnOnStaticMeshes=vals[i++].ToBool();

	pickupHighlightDistance=FCString::Atof(*vals[i++]);
	pickupPickupDistance=FCString::Atof(*vals[i++]);
	pickupRespawnDistance=FCString::Atof(*vals[i++]);
	bPickupDistanceFadeHighlight=vals[i++].ToBool();
	bPickupUseStencilBuffer=vals[i++].ToBool();

	return true;
}

//----------------------------------------------------------------------------------------------------------------
