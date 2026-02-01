//
// rdInstBreakAndMake.cpp
//
// Date Added: 30th June 2024
// Last Modified: 26th June 2025
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"

//.............................................................................
// Make and Break Utility Methods for the rdPositionInfo Struct
//.............................................................................
void ArdActor::BreakPositionInfo(FrdPositionInfo pos,int32& cl,int32& cr,int32& cc,int32& tl,int32& tr,int32& tc,int32& rs,int64& bf,TArray<float>& custData) { 
	cl=pos.currentLevel; cr=pos.currentRow; cc=pos.currentColumn; tl=pos.totalLevels; tr=pos.totalRows; tc=pos.totalColumns; rs=pos.randomSeed; bf=pos.bitField; custData=pos.customData;
}

FrdPositionInfo ArdActor::MakePositionInfo(int32 cl,int32 cr,int32 cc,int32 tl,int32 tr,int32 tc,int32 rs,int64 bf,TArray<float> custData) { 
	return FrdPositionInfo(cl,cr,cc,tl,tr,tc,rs,bf,custData); 
}

//.............................................................................
// Make and Break Utility Methods for the InstanceSetup Struct
//.............................................................................

void ArdActor::BreakInstanceSetup(FrdInstanceSetup o,TEnumAsByte<ErdSpawnType>& type,FName& sid,TSoftObjectPtr<UStaticMesh>& mesh,TArray<TSoftObjectPtr<UMaterialInterface>>& materials,TEnumAsByte<ErdCollision>& collision,float& startCull,float& endCull,float& streamInDistance,int32& id,FName& groupName) {
	type=o.type; sid=o.sid; mesh=o.mesh; materials=o.materials; collision=o.collision; startCull=o.startCull; endCull=o.endCull; streamInDistance=o.streamInDistance; id=o.id; groupName=o.groupName;
}

FrdInstanceSetup ArdActor::MakeInstanceSetup(TEnumAsByte<ErdSpawnType> type,const FName sid,TSoftObjectPtr<UStaticMesh> mesh,const TArray<TSoftObjectPtr<UMaterialInterface>> materials,ErdCollision collision,float startCull,float endCull,float streamInDistance,int32 id,const FName groupName) {
	return FrdInstanceSetup(type,sid,mesh,materials,collision,startCull,endCull,streamInDistance,id,groupName);
}

//.............................................................................
// Make and Break Utility Methods for the various Structs
//.............................................................................

// FrdObjectPlacementData
void ArdActor::BreakPlacementData(FrdObjectPlacementData pd,
								FrdInstanceSetup& m,
								UClass*& ac,FString& atm,UNiagaraSystem*& fx,float& dns,float& gox,float& goy,float& gvw,float& gvh,
								TEnumAsByte<rdSpawnMode>& sm,float& sl,float& lr,float& vfd,float& vfe,
								bool& pl,int32& npl,FVector& of,float& mGp,bool& USl,FVector& mnScl,FVector& mxScl,bool& RndYaw,bool& ATN,float& mnSlp,bool& POG,
								bool& ONV,bool& FSP,float& slpRad,bool& spltDen,uint8& spltC,float& spltMnR,float& spltMxR,TArray<UPhysicalMaterial*>& pml,FrdProxySetup& px,UInstancedStaticMeshComponent*& ismc) {

	m=pd.mesh;
	ac=pd.actorClass; atm=pd.strProps; fx=pd.vfx; dns=pd.density; gox=pd.gridOffsetX; goy=pd.gridOffsetY; gvw=pd.gridVarianceW; gvh=pd.gridVarianceH; 
	sm=pd.spawnMode; sl=pd.streamDistance; lr=pd.localRadius; vfd=pd.viewFrustumDistance; vfe=pd.viewFrustumExtend;
	pl=pd.pooled; npl=pd.numToPool; of=pd.offset; mGp=pd.minGap; USl=pd.bUniformScale; mnScl=pd.minScale; mxScl=pd.maxScale; RndYaw=pd.bRandomYaw; 
	ATN=pd.bAlignToNormal; mnSlp=pd.minSlope; POG=pd.bPlaceOnGround; ONV=pd.bOnNavMesh; FSP=pd.bFixSlopePlacement; slpRad=pd.slopedRadius;
	spltDen=pd.bUseSplatDensity; spltC=pd.splatChannel; spltMnR=pd.splatMinRange; spltMxR=pd.splatMaxRange; pml=pd.filterToPhysicalMatsList; px=pd.proxy; ismc=pd.ismc.Get();

}

FrdObjectPlacementData ArdActor::MakePlacementData(const FrdInstanceSetup& m,
											UClass* ac,const FString& atm,UNiagaraSystem* fx,float dns,float gox,float goy,float gvw,float gvh,
											TEnumAsByte<rdSpawnMode> sm,float sl,float lr,float vfd,float vfe,
											bool pl,int32 npl,const FVector& of,float mGp,bool UScl,FVector mnScl,FVector mxScl,bool RndYaw,bool ATN,float mnSlp,bool POG,bool ONV,
											bool FSP,float slpRad,bool spltDen,uint8 spltC,float spltMnR,float spltMxR,const TArray<UPhysicalMaterial*>& pml,const FrdProxySetup& px,const UInstancedStaticMeshComponent* ismc) {

	return FrdObjectPlacementData(m,ac,atm,fx,dns,gox,goy,gvw,gvh,sm,sl,lr,vfd,vfe,pl,npl,of,mGp,UScl,mnScl,mxScl,RndYaw,ATN,mnSlp,POG,ONV,FSP,slpRad,spltDen,spltC,spltMnR,spltMxR,pml,px,(UInstancedStaticMeshComponent*)ismc);
}

// FrdAddInstanceFastArray
void ArdActor::BreakFastItem(FrdAddInstanceFastArray o,FrdInstanceSetup& is,TArray<FTransform>& t) {
	is=o.meshSetup; t=o.transforms;
}

FrdAddInstanceFastArray ArdActor::MakeFastItem(const FrdInstanceSetup& is,const TArray<FTransform>& t) {
	return FrdAddInstanceFastArray(is,t);
}

// FrdInstanceRandomSettings
void ArdActor::BreakRandomizedItem(FrdAddInstanceRandomizedArray o,FrdInstanceSetup& is,TArray<FrdInstanceRandomSettings>& t) {
	is=o.meshSetup; t=o.settings;
}

FrdAddInstanceRandomizedArray ArdActor::MakeRandomizedItem(const FrdInstanceSetup& is,const TArray<FrdInstanceRandomSettings>& t) {
	return FrdAddInstanceRandomizedArray(is,t);
}

// FrdInstanceRandomSettings
void ArdActor::BreakInstanceRandomSettings(FrdInstanceRandomSettings o,FString& nm,FString& fld,FTransform& oTran,FTransform& aTran,FrdRandomSettings& randomSettings,bool& noVis,bool& vis,bool& proc,int32& index) {
	nm=o.name; fld=o.folder; oTran=o.origTransform; aTran=o.actualTransform; randomSettings=o.randomSettings; noVis=o.bNoVis; vis=o.bVisible; proc=o.bProcessed; index=o.index;
}

FrdInstanceRandomSettings ArdActor::MakeInstanceRandomSettings(FString nm,FString fld,FTransform oTran,FTransform aTran,const FrdRandomSettings& randomSettings,bool noVis,bool vis,bool proc,int32 index) {
	return FrdInstanceRandomSettings(nm,fld,oTran,aTran,randomSettings,noVis,vis,proc,index);
}

// FrdInstanceFastSettings
void ArdActor::BreakInstanceFastSettings(FrdInstanceFastSettings o,FTransform& tran,FString& nm,FString& fld,FString& thm,bool& noVis,bool& vis,int32& ind) {
	tran=o.transform; nm=o.name; fld=o.folder; thm=o.theme; noVis=o.bNoVis; vis=o.bVisible; ind=o.index;
}

FrdInstanceFastSettings ArdActor::MakeInstanceFastSettings(FTransform tran,FString nm,FString fld,FString thm,bool noVis,bool vis,int32 ind) {
	return FrdInstanceFastSettings(tran,nm,fld,thm,noVis,vis,ind);
}

// FrdRandomSettings
void ArdActor::BreakRandomSettings(FrdRandomSettings o,FVector& loc1,FVector& loc2,FRotator& rot1,FRotator& rot2,FVector& flpP,uint8& lckA,FVector& scl1,FVector& scl2,float& showP,FString& rlyNm,FString& thms) {
	loc1=o.location1; loc2=o.location2; rot1=o.rotation1; rot2=o.rotation2; flpP=o.flipProbability; lckA=o.lockAxis; scl1=o.scale1; scl2=o.scale2; showP=o.showProbability; rlyNm=o.relyActorName; thms=o.themes;
}

FrdRandomSettings ArdActor::MakeRandomSettings(FVector loc1,FVector loc2,FRotator rot1,FRotator rot2,FVector flpP,uint8 lckA,FVector scl1,FVector scl2,float showP,FString rlyNm,FString thms) {
	return FrdRandomSettings(loc1,loc2,rot1,rot2,flpP,lckA,scl1,scl2,showP,rlyNm,thms);
}

// FrdSplinePopulateData
void ArdActor::BreakSplinePopulateData(FrdSplinePopulateData o,UStaticMesh*& m,UMaterialInterface*& mt,FTransform& ofs,float& g,TEnumAsByte<ESplineMeshAxis::Type>& ax,TArray<float>& cd) {
	m=o.mesh; mt=o.mat; ofs=o.offset; g=o.gap; ax=o.axis; cd=o.customData;
}

FrdSplinePopulateData ArdActor::MakeSplinePopulateData(UStaticMesh* m,UMaterialInterface* mt,const FTransform& ofs,float g,const ESplineMeshAxis::Type ax,const TArray<float>& cd) {
	return FrdSplinePopulateData(m,mt,ofs,g,ax,cd);
}

// FrdSplineInstanceData
//void ArdActor::BreakSplineInstanceData(FrdSplineInstanceData o,FVector& sl,FVector& st,FVector& ss,float& sr,FVector& el,FVector& et,FVector& es,float& er,USplineMeshComponent*& sm,UProceduralMeshComponent*& pm) {
void ArdActor::BreakSplineInstanceData(FrdSplineInstanceData o,FVector& sl,FVector& st,FVector& ss,float& sr,FVector& el,FVector& et,FVector& es,float& er,USplineMeshComponent*& sm) {
	sl=o.startLocation; st=o.startTangent; ss=o.startScale; sr=o.startRoll; el=o.endLocation; et=o.endTangent; es=o.endScale; er=o.endRoll; sm=o.smc; //pm=o.pmc;
}

//FrdSplineInstanceData ArdActor::MakeSplineInstanceData(const FVector& sl,const FVector& st,const FVector& ss,float sr,const FVector& el,const FVector& et,const FVector& es,float er,USplineMeshComponent* sm,UProceduralMeshComponent* pm) {
FrdSplineInstanceData ArdActor::MakeSplineInstanceData(const FVector& sl,const FVector& st,const FVector& ss,float sr,const FVector& el,const FVector& et,const FVector& es,float er,USplineMeshComponent* sm) {
	return FrdSplineInstanceData(sl,st,ss,sr,el,et,es,er,sm);//,pm);
}

// rdPickup
void ArdActor::BreakPickup(FrdPickup o,TEnumAsByte<rdPickupType>& put,TEnumAsByte<rdPickupHighlightType>& pht,TEnumAsByte<rdPickupHighlightStyle>& hs,int32& i,float& hd,float& pd,float& th,FLinearColor& c1,FLinearColor& c2,float& ss,float& es,bool& fg,bool& fd,float& st,bool& sl,float& rd,bool& upp) {
	put=o.type; pht=o.highlightType; hs=o.highlightStyle; i=o.id; hd=o.highlightDistance; pd=o.pickupDistance; th=o.thickness; c1=o.color1; c2=o.color2; ss=o.strobeSpeed; es=o.ebbSpeed; fg=o.fadeToGround; fd=o.fadeInDistance; st=o.respawnTime; sl=o.respawnLonely; rd=o.respawnDistance; upp=o.useStencilBuffer;
}

FrdPickup ArdActor::MakePickup(TEnumAsByte<rdPickupType> put,TEnumAsByte<rdPickupHighlightType> pht,TEnumAsByte<rdPickupHighlightStyle> hs,int32 i,float hd,float pd,float th,FLinearColor c1,FLinearColor c2,float ss,float es,bool fg,bool fd,float st,bool sl,float rd,bool upp) {
	return FrdPickup(put,pht,hs,i,hd,pd,th,c1,c2,ss,es,fg,fd,st,sl,rd,upp);
}

// rdProxyItem
void ArdActor::BreakProxyItem(FrdProxyItem o,FName& sd,UInstancedStaticMeshComponent*& isc,FName& dm,UClass*& dp,FTransform& dof,int32& ii,AActor*& pf,UClass*& ac,int32& pii,UInstancedStaticMeshComponent*& pic,TSoftObjectPtr<UDataLayerAsset>& pdl,FName& psd,float& d,FTransform& t,AActor*& ia,float& pto,TEnumAsByte<rdProxyType>& pt,bool& ud,bool& p,bool& sp,bool& dr,bool& dh,AActor*& iv,bool& dy,UrdStateDataAsset*& ss,bool& cse,FrdPickup& pu) {
	sd=o.sid; isc=o.ismc; dm=o.destroyedSid; dp=o.destroyedPrefab; dof=o.destroyedOffset; ii=o.instanceIndex; pf=o.prefab; ac=o.actorClass; d=o.distance; 
	t=o.transform; 
	ia=o.proxyActor; pto=o.proxyPhysicsTimeout; pdl=o.proxyDataLayer; psd=o.proxyStaticMeshSid; pii=o.proxyInstanceIndex; pic=o.proxyInstanceComponent;
	pt=o.proxyType; ud=o.bUseWithDestruction; p=o.bPooled; sp=o.bSimplePool; dr=o.bDontRemove; dh=o.bDontHide; iv=o.instanceVolume; dy=o.bDestroyed; ss=o.savedState;
	cse=o.bCallSwapEvent; pu=o.pickup;
}

FrdProxyItem ArdActor::MakeProxyItem(const FName sd,UInstancedStaticMeshComponent* isc,const FName dm,UClass* dp,const FTransform dof,int32 ii,AActor* pf,UClass* ac,int32 pii,UInstancedStaticMeshComponent* pic,TSoftObjectPtr<UDataLayerAsset> pdl,const FName psm,float d,const FTransform& t,AActor* ia,float pto,TEnumAsByte<rdProxyType> pt,bool ud,bool p,bool sp,bool dr,bool dh,AActor* iv,bool dy,UrdStateDataAsset* ss,bool cse,FrdPickup pu) {
	return FrdProxyItem(sd,isc,dm,dp,dof,ii,pf,ac,d,t,ia,pto,pii,pic,pdl,psm,pt,ud,p,sp,dr,dh,iv,dy,ss,cse,pu);
}

void ArdActor::BreakProxySetup(FrdProxySetup o,TSoftClassPtr<AActor>& pa,TSoftObjectPtr<UDataLayerAsset>& pdl,TSoftObjectPtr<UStaticMesh>& psm,TArray<TSoftObjectPtr<UMaterialInterface>>& smm,TEnumAsByte<ErdCollision>& smCol,TSoftObjectPtr<UStaticMesh>& dm,TArray<TSoftObjectPtr<UMaterialInterface>>& dmm,TEnumAsByte<ErdCollision>& dmCol,TSoftClassPtr<AActor>& dp,FTransform& dof,TEnumAsByte<rdProxyType>& pt,float& sd,float& pto,bool& ud,bool& p,bool& sp,int32& pam,bool& dr,bool& dh,bool& em,UrdStateDataAsset*& ss,bool& cse,FrdPickup& pu) {
	pa=o.proxyActor; pdl=o.proxyDataLayer; psm=o.proxyStaticMesh; smm=o.proxyMeshMaterials; smCol=o.proxyMeshCollision;
	dm=o.destroyedMesh; dmm=o.destroyedMeshMaterials; dmCol=o.destroyedMeshCollision; dp=o.destroyedPrefab; dof=o.destroyedOffset; 
	pt=o.proxyType; sd=o.scanDistance; pto=o.proxyPhysicsTimeout; ud=o.bUseWithDestruction; p=o.bPooled; sp=o.bSimplePool; 
	pam=o.pooledAmount; dr=o.bDontRemove; dh=o.bDontHide;
	em=o.bEntireMeshProxy; ss=o.savedState; cse=o.bCallSwapEvent; pu=o.pickup;
}

// FrdProxySetup
FrdProxySetup ArdActor::MakeProxySetup(TEnumAsByte<rdProxyType> pt,TSoftClassPtr<AActor> pa,TSoftObjectPtr<UDataLayerAsset> pdl,TSoftObjectPtr<UStaticMesh> psm,const TArray<TSoftObjectPtr<UMaterialInterface>>& smm,TEnumAsByte<ErdCollision> smCol,TSoftObjectPtr<UStaticMesh> dm,const TArray<TSoftObjectPtr<UMaterialInterface>>& dmm,TEnumAsByte<ErdCollision> dmCol,TSoftClassPtr<AActor> dp,const FTransform dof,float ld,float pto,bool ud,bool p,bool sp,int32 pam,bool dr,bool dh,bool em,UrdStateDataAsset* ss,bool cse,const FrdPickup& pu) {
	return FrdProxySetup(pt,pa,pdl,psm,smm,smCol,dm,dmm,dmCol,dp,dof,ld,pto,ud,p,sp,pam,dr,dh,em,ss,cse,pu);
}

void ArdActor::BreakInstanceItems(FrdInstanceItems o,FName& s,TArray<int32>& i,TArray<FTransform>& t) {
	s=o.sid; i=o.instances; t=o.transforms;
}

FrdInstanceItems ArdActor::MakeInstanceItems(const FName s,const TArray<int32>& i,const TArray<FTransform>& t) {
	return FrdInstanceItems(s,i,t);
}

void ArdActor::BreakProxyActorItem(FrdProxyActorItem o,AActor*& pa,int32& pii,UInstancedStaticMeshComponent*& pic,FrdProxyItem& pi,int32& ps,int32& ii,FTransform& t,int32& ic,bool& fd,bool& fld,bool& fi,bool& dr,bool& dh) {
	pa=o.proxyActor; pii=o.proxyInstanceIndex; pic=o.proxyInstanceComponent; pi=o.proxyItem; ps=o.proxySource; ii=o.instanceIndex; ic=o.idleCountdown;
	fd=o.forDestruction; fld=o.forLongDistance; fi=o.forImpact; dr=o.bDontRemove; dh=o.bDontHide;
}

FrdProxyActorItem ArdActor::MakeProxyActorItem(AActor* pa,int32 pii,UInstancedStaticMeshComponent* pic,const FrdProxyItem& pi,int32 ps,int32 ii,const FTransform& t,int32 ic,bool fd,bool fld,bool fi,bool dr,bool dh) {
	return FrdProxyActorItem(pa,pii,pic,pi,ps,ii,t,ic,fd,fld,fi,dr,dh);
}

//............................................................................
