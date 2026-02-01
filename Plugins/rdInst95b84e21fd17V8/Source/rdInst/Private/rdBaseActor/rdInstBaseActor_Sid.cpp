//
// rdInstBaseActor_Sid.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 3rd December 2024 (moved from rdInstBaseActor_Utilities.cpp)
// Last Modified: 29th May 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
// A sid is a string ID which can be used to reference StaticMeshes, Instanced StaticMeshes, Niaga Meshes and VFX, Decals, Lights, BPs and any Actors. 
// The sid contains the objects instance data and can be used as a type of simple, fast and memory friendly serialized entities.
//
// The format of sids is as follows:
//
// sid|<version>|<type>|
//
// <type> can be: (ErdSpawnType)
//        0: Default
//        1: ISM
//        2: HISM
//        3: SMC
//        4: Niagara Mesh
//        5: rdMeshlet (not currently implemented)
//        6: Componentless Mesh (Scene Proxy) (not currently implemented)
//		  7: Actor
//		  8: DataLayer
//
// ISM sids:
//
//     sid|<version>|<type>|<soft object (packed)>|<collision type>|<Reverse Culling>|<Start Cull>|<End Cull>|<id>|<optional material override 1 (packed)>|<optional material override 2 (packed) etc>
//
// Actor sids:
//
//     sid|<version>|<type>|<soft class (packed)>|<actor instance data>
//
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "rdActorStrings.h"
#include "NiagaraActor.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/DecalActor.h"
#include "Engine/SpotLight.h"
#include "Engine/RectLight.h"
#include "Engine/PointLight.h"
#include "Engine/TextRenderActor.h"
#include "Components/ShapeComponent.h"
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
#include "Engine/LocalFogVolume.h"
#endif
//.............................................................................
// rdPackMaterialName
//.............................................................................
FString ArdInstBaseActor::rdPackMaterialName(const FString& nm) {

	FString str=nm.Replace(TEXT("/Game/"),TEXT("%1")).Replace(TEXT("Materials/"),TEXT("%2")).Replace(TEXT("Meshes/"),TEXT("%3"));
	return str;
}

//.............................................................................
// rdUnpackMaterialName
//.............................................................................
FString ArdInstBaseActor::rdUnpackMaterialName(const FString& nm) {

	FString str=nm.Replace(TEXT("%1"),TEXT("/Game/")).Replace(TEXT("%2"),TEXT("Materials/")).Replace(TEXT("%3"),TEXT("Meshes/"));
	return str;
}

//.............................................................................
// rdGetSMsid - just the mesh fullname for sid
//.............................................................................
FName ArdInstBaseActor::rdGetSMsid(const TSoftObjectPtr<UStaticMesh> m,ErdSpawnType type,const FName groupName) {
	//if(!m.IsValid()) return FName();

	if((type==ErdSpawnType::UseDefaultSpawn && bUseHISMs) || type==ErdSpawnType::HISM) {
		UStaticMesh* mesh=m.Get();
		if(mesh && rdIsMeshNanite(mesh)) {
			type=ErdSpawnType::ISM;
		}
	}

	FString str=FString::Printf(TEXT("sid|%d|%d|%s|0"),RD_SID_VER,type,*rdPackMaterialName(m->GetPathName()));
	FName sid=FName(*str);
	//if(m.Get() && !instList.Find(sid)) instList.Add(sid,FrdInstList(sid,(UStaticMesh*)m.Get()));
	return sid;
}

//.............................................................................
// rdGetSMXsid - mesh fullname, materials and negative check
//.............................................................................
FName ArdInstBaseActor::rdGetSMXsid(ErdSpawnType type,const TSoftObjectPtr<UStaticMesh> mesh,TArray<TSoftObjectPtr<UMaterialInterface>> mats,bool bReverseCulling,ErdCollision collision,float startCull,float endCull,int32 id,const FName groupName) {

	if(!mesh) return FName();

	if((type==ErdSpawnType::UseDefaultSpawn && bUseHISMs) || type==ErdSpawnType::HISM) {
		UStaticMesh* m=mesh.Get();
		if(m && rdIsMeshNanite(m)) {
			type=ErdSpawnType::ISM;
		}
	}

	FString str=FString::Printf(TEXT("sid|%d|%d|%s|%d"),RD_SID_VER,type,*rdPackMaterialName(mesh->GetPathName()),(int32)collision);

	if(bReverseCulling || mats.Num()>0 || startCull>=0.0f || endCull>=0.0f || id>0) {

		if(startCull<0.0f || endCull<0.0f) {
			UrdInstData* uid=GetInstUserData(mesh.Get(),false);
			if(uid) {
				startCull=uid->startCull;
				endCull=uid->endCull;
			} else {
				startCull=endCull=0.0f;
			}
		}
		if((startCull<=0.1f || startCull==400000.0f) && (endCull<=0.1f || endCull==startCull) && mats.Num()==0 && id==0) {
		} else {
			str+=FString::Printf(TEXT("|%d|%s|%s|%d|%s"),bReverseCulling,*FString::SanitizeFloat(startCull),*FString::SanitizeFloat(endCull),id,groupName.IsNone()?TEXT(""):*groupName.ToString());
		}
		if(mats.Num()>0) {
			str+=TEXT("|");
			FString lastMat;
			for(auto m:mats) {
				FString pnm=rdPackMaterialName(m.ToString());
				if(pnm==lastMat) pnm=TEXT("+");
				else lastMat=pnm;
				str+=pnm+TEXT("|");
			}
		}
	}
	FName sid=FName(*str);
	//if(!instList.Find(sid)) instList.Add(sid,FrdInstList(sid,(UStaticMesh*)mesh.Get()));
	return sid;
}

//.............................................................................
// rdGetInstsid - sid from the InstanceSetup struct
//.............................................................................
FName ArdInstBaseActor::rdGetInstSid(const FrdInstanceSetup& m) {

	if(!m.mesh) return FName();

	ErdSpawnType type=m.type;
	if((type==ErdSpawnType::UseDefaultSpawn && bUseHISMs) || type==ErdSpawnType::HISM) {
		UStaticMesh* mesh=m.mesh.Get();

		if(mesh && rdIsMeshNanite(mesh)) {
			type=ErdSpawnType::ISM;
		}
	}

	FString str=FString::Printf(TEXT("sid|%d|%d|%s|%d"),RD_SID_VER,(int32)type,*rdPackMaterialName(m.mesh->GetPathName()),(int32)m.collision);

	if(m.materials.Num()>0 || m.startCull>=0.0f || m.endCull>=0.0f || m.id>0) {

		float startCull=m.startCull,endCull=m.endCull;
		if(startCull<0.0f || endCull<0.0f) {
			UrdInstData* uid=GetInstUserData(m.mesh.Get(),false);
			if(uid) {
				startCull=uid->startCull;
				endCull=uid->endCull;
			} else {
				startCull=endCull=0.0f;
			}
		}
		if((startCull<=0.1f || startCull==400000.0f) && (endCull<=0.1f || endCull==startCull) && m.materials.Num()==0 && m.id==0 && m.groupName.IsNone()) {
		} else {
			str+=FString::Printf(TEXT("|0|%s|%s|%d|%s"),*FString::SanitizeFloat(startCull),*FString::SanitizeFloat(endCull),m.id,m.groupName.IsNone()?TEXT(""):*m.groupName.ToString());
		}
		if(m.materials.Num()>0) {
			str+=TEXT("|");
			FString lastMat;
			for(auto mt:m.materials) {
				FString pnm=rdPackMaterialName(mt.ToString());
				if(pnm==lastMat) pnm=TEXT("+");
				else lastMat=pnm;
				str+=pnm+TEXT("|");
			}
		}
	}
	FName sid=FName(*str);
	//if(!instList.Find(sid)) instList.Add(sid,FrdInstList(sid,m.mesh));
	return sid;
}

//.............................................................................
// rdGetSMCsid - mesh fullname, materials and attributes for sid
//.............................................................................
FName ArdInstBaseActor::rdGetSMCsid(const UStaticMeshComponent* smc,ErdSpawnType type,float overrideStartCull,float overrideEndCull,int32 overrideID,const FName overrideGroupName) {

	if(!smc) return FName();
	UStaticMesh* mesh=smc->GetStaticMesh();
	if(!mesh) return FName();

	if((type==ErdSpawnType::UseDefaultSpawn && bUseHISMs) || type==ErdSpawnType::HISM) {
		if(rdIsMeshNanite(mesh)) {
			type=ErdSpawnType::ISM;
		}
	}

	if(type==0 && overrideStartCull<0.0f && overrideEndCull<0.0f && overrideID==0 && smc->ComponentTags.Num()==1 && smc->ComponentTags[0].ToString().StartsWith(TEXT("sid|"))) {
		return smc->ComponentTags[0];
	}

	FString	str;
	float startCull=0.0f,endCull=0.0f;
	const UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(smc);
	if(overrideStartCull>=0.0f && overrideEndCull>=overrideStartCull) {
		startCull=overrideStartCull;
		endCull=overrideEndCull;
	} else {
		if(ismc) {
			startCull=ismc->InstanceStartCullDistance;
			endCull=ismc->InstanceEndCullDistance;
		}
	}

	bool revCull=ismc?ismc->bReverseCulling:smc->GetComponentTransform().ToMatrixWithScale().Determinant()<0.0f;
	int32 numMats=smc->GetNumMaterials();
	TArray<UMaterialInterface*> omats=smc->OverrideMaterials;
	bool matsAreSame=false;
	if(omats.Num()==0) {
		matsAreSame=true;
	} else if(numMats==omats.Num()) {
		int32 i=0;
		for(auto mi:omats) {
			if(mi->GetPathName()!=mesh->GetStaticMaterials()[i++].MaterialInterface->GetPathName()) {
				i=0;
				break;
			}
		}
		if(i==numMats) { // the overridden materials are the same as the default ones
			matsAreSame=true;
		}
	}

	if(matsAreSame) {

		if(!revCull && (startCull<=0.1f || startCull==400000.0f) && (endCull<=0.1f || endCull==startCull) && overrideGroupName.IsNone()) {
			str=FString::Printf(TEXT("sid|%d|%d|%s|%d"),RD_SID_VER,(int32)type,*rdPackMaterialName(mesh->GetPathName()),(int32)smc->BodyInstance.GetCollisionEnabled()+1);
		} else {
			str=FString::Printf(TEXT("sid|%d|%d|%s|%d|%d|%s|%s|%d|%s"),RD_SID_VER,(int32)type,*rdPackMaterialName(mesh->GetPathName()),(int32)smc->BodyInstance.GetCollisionEnabled()+1,revCull,*FString::SanitizeFloat(startCull),*FString::SanitizeFloat(endCull),overrideID,overrideGroupName.IsNone()?TEXT(""):*overrideGroupName.ToString());
		}
		return FName(*str);
	}

	str=FString::Printf(TEXT("sid|%d|%d|%s|%d|%d|%s|%s|%d|%s|"),RD_SID_VER,(int32)type,*rdPackMaterialName(mesh->GetPathName()),(int32)smc->BodyInstance.GetCollisionEnabled()+1,revCull,*FString::SanitizeFloat(startCull),*FString::SanitizeFloat(endCull),overrideID,overrideGroupName.IsNone()?TEXT(""):*overrideGroupName.ToString());
	FString lastMat;
	for(auto mi:omats) {
		if(mi) {
			TSoftObjectPtr<UMaterialInterface> sp=mi;
			FString pnm=rdPackMaterialName(sp.ToString());
			if(pnm==lastMat) pnm=TEXT("+");
			else lastMat=pnm;
			str+=pnm+TEXT("|");
		} else {
			str+=TEXT("|");
		}
	}

	FName sid=FName(*str);

	//if(!instList.Find(sid)) instList.Add(sid,FrdInstList(sid,(UStaticMesh*)smc->GetStaticMesh()));
	return sid;
}

//.............................................................................
// rdGetMeshFromSid - Returns the UStaticMesh referenced in the sid
//.............................................................................
UStaticMesh* ArdInstBaseActor::rdGetMeshFromSid(const FName sid) {

	if(sid.IsNone()) {
		return nullptr;
	}
	FrdInstList* il=instList.Find(sid);
	if(!il || !il->mesh.Get()) {

		TArray<FString> vals;
		FString strSid=((FName&)sid).ToString();
		int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);

		if(num<4) {
			return nullptr;
		}

		FString fname=rdUnpackMaterialName(vals[3]);
		FSoftObjectPath sop(fname);

		UStaticMesh* mesh=Cast<UStaticMesh>(sop.ResolveObject());
		if(!mesh) {
			mesh=Cast<UStaticMesh>(sop.TryLoad());
		}

		if(mesh) {
			if(!il) {
				il=&instList.Add(sid,FrdInstList(sid,mesh));
			} else {
				il->mesh=mesh;
			}
		}
	}
	
	return il?il->mesh.Get():nullptr;
}

//.............................................................................
// rdGetSoftMeshFromSid - Returns the UStaticMesh SoftObjectPtr from the sid
//.............................................................................
TSoftObjectPtr<UStaticMesh> ArdInstBaseActor::rdGetSoftMeshFromSid(const FName sid) {

	if(sid.IsNone()) {
		return nullptr;
	}

	TArray<FString> vals;
	FString strSid=((FName&)sid).ToString();
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);

	if(num<4) {
		return nullptr;
	}

	FString fname=rdUnpackMaterialName(vals[3]);
	FSoftObjectPath objectPath(fname);
	TSoftObjectPtr<UStaticMesh> sm(objectPath);
	return sm;
}

//.............................................................................
// rdGetMaterialsFromSid
//.............................................................................
TArray<TSoftObjectPtr<UMaterialInterface>> ArdInstBaseActor::rdGetMaterialsFromSid(const FName sid) {

	TArray<TSoftObjectPtr<UMaterialInterface>> mil;
	TArray<FString> vals;
	FString strSid=((FName&)sid).ToString();
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	int32 ver=FCString::Atoi(*vals[1]);
	if(num<((ver>1)?10:9)) {
		return mil;
	}
	FString lastMat;
	for(int32 ind=((ver==1)?9:10);ind<num;ind++) {
//	for(int32 ind=((ver==1)?8:9);ind<num;ind++) {

		FString fname;
		if(vals[ind]==TEXT("+")) fname=lastMat;
		else fname=rdUnpackMaterialName(vals[ind]);
		lastMat=fname;
		int32 i=fname.Find(TEXT("."),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
		FString path,name;
		if(i>=0) {
			path=fname.Left(i);
			name=fname.RightChop(i+1);
		} else {
			i=fname.Find(TEXT("/"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
			if(i<0) i=fname.Find(TEXT("\\"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
			if(i>=0) {
				path=fname;
				name=fname.RightChop(i+1);
			}
		}

		FSoftObjectPath sop(path);
		UMaterialInterface* mat=Cast<UMaterialInterface>(sop.ResolveObject());
		if(!mat) {
			mat=Cast<UMaterialInterface>(sop.TryLoad());
		}
		if(mat) {
			mil.Add(MoveTemp(mat));
		} else {
			mil.Add(nullptr);
		}
	}

	return mil;
}

//.............................................................................
// rdGetMeshNameFromSid
//.............................................................................
FString ArdInstBaseActor::rdGetMeshNameFromSid(const FName sid) {

	TArray<FString> vals;
	FString strSid=((FName&)sid).ToString();
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	if(num<4) {
		return TEXT("");
	}

	FString fname=rdUnpackMaterialName(vals[3]);
	int32 i=fname.Find(TEXT("."),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
	return fname.RightChop(i+1);
}

//.............................................................................
// rdGetSpawnTypeFromSid
//.............................................................................
ErdSpawnType ArdInstBaseActor::rdGetSpawnTypeFromSid(const FName sid) {
	if(sid.IsNone()) {
		return ErdSpawnType::Actor;
	}

	FString st=sid.ToString();
	return (ErdSpawnType)(st[6]-L'0');
}

//.............................................................................
// rdGetSidDetails
//.............................................................................
void ArdInstBaseActor::rdGetSidDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,TSoftObjectPtr<UStaticMesh>& mesh,TArray<TSoftObjectPtr<UMaterialInterface>>& materials,bool& bReverseCulling,TEnumAsByte<ErdCollision>& collision,float& startCull,float& endCull,int32& id,FName& groupID) {

	mesh=rdGetMeshFromSid(sid);
	materials=rdGetMaterialsFromSid(sid);

	TArray<FString> vals;
	FString strSid=((FName&)sid).ToString();
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	ver=FCString::Atoi(*vals[1]);
	type=(ErdSpawnType)((num>2)?FCString::Atoi(*vals[2]):0);
	collision=(TEnumAsByte<ErdCollision>)((num>4)?(FCString::Atoi(*vals[4])):ErdCollision::QueryAndPhysics);
	bReverseCulling=(num>5)?vals[5].ToBool():false;
	startCull=(num>6)?FCString::Atof(*vals[6]):0.0f;
	endCull=(num>7)?FCString::Atof(*vals[7]):startCull;
	id=(num>8)?FCString::Atoi(*vals[8]):0;
	if(ver>1) {
		groupID=(num>9)?*vals[9]:TEXT("");
	}
}

//.............................................................................
// rdGetActorSid
//.............................................................................
FName ArdInstBaseActor::rdGetActorSid(UClass* aclass,const FString& aStr,const FName groupName) {
	
	TSoftClassPtr<UClass> sc(aclass);
	FString str=FString::Printf(TEXT("sid|%d|7|%s|"),RDSTRING_VERSION,*rdPackMaterialName(sc.ToString()));
	if(!groupName.IsNone()) str+=groupName.ToString();
	if(!aStr.IsEmpty()) str+=TEXT("|")+aStr;
	return FName(*str);
}

FName ArdInstBaseActor::rdGetActorSid(ADecalActor* actor) { return rdGetActorSid(ADecalActor::StaticClass(),FrdString::ToString(actor)); }
FName ArdInstBaseActor::rdGetActorSid(ASpotLight* actor) { return rdGetActorSid(ASpotLight::StaticClass(),FrdString::ToString(actor)); }
FName ArdInstBaseActor::rdGetActorSid(ARectLight* actor) { return rdGetActorSid(ARectLight::StaticClass(),FrdString::ToString(actor)); }
FName ArdInstBaseActor::rdGetActorSid(APointLight* actor) { return rdGetActorSid(APointLight::StaticClass(),FrdString::ToString(actor)); }
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
FName ArdInstBaseActor::rdGetActorSid(ALocalFogVolume* actor) { return rdGetActorSid(ALocalFogVolume::StaticClass(),FrdString::ToString(actor)); }
#endif
FName ArdInstBaseActor::rdGetActorSid(APostProcessVolume* actor) { return rdGetActorSid(APostProcessVolume::StaticClass(),FrdString::ToString(actor)); }
FName ArdInstBaseActor::rdGetActorSid(ANiagaraActor* actor) { return rdGetActorSid(ANiagaraActor::StaticClass(),FrdString::ToString(actor)); }
FName ArdInstBaseActor::rdGetActorSid(ATextRenderActor* actor) { return rdGetActorSid(ATextRenderActor::StaticClass(),FrdString::ToString(actor)); }

FName ArdInstBaseActor::rdGetActorSid(AActor* actor) { 
	
	UClass* aclass=actor->GetClass();
	if(aclass==ADecalActor::StaticClass()) return rdGetActorSid(ADecalActor::StaticClass(),FrdString::ToString(((ADecalActor*)actor))); 
	if(aclass==ASpotLight::StaticClass()) return rdGetActorSid(ASpotLight::StaticClass(),FrdString::ToString(((ASpotLight*)actor))); 
	if(aclass==ARectLight::StaticClass()) return rdGetActorSid(ARectLight::StaticClass(),FrdString::ToString(((ARectLight*)actor))); 
	if(aclass==APointLight::StaticClass()) return rdGetActorSid(APointLight::StaticClass(),FrdString::ToString(((APointLight*)actor))); 
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
	if(aclass==ALocalFogVolume::StaticClass()) return rdGetActorSid(ALocalFogVolume::StaticClass(),FrdString::ToString(((ALocalFogVolume*)actor)));
#endif
	if(aclass==APostProcessVolume::StaticClass()) return rdGetActorSid(APostProcessVolume::StaticClass(),FrdString::ToString(((APostProcessVolume*)actor)));
	if(aclass==ANiagaraActor::StaticClass()) return rdGetActorSid(ANiagaraActor::StaticClass(),FrdString::ToString(((ANiagaraActor*)actor)));
	if(aclass==ATextRenderActor::StaticClass()) return rdGetActorSid(ATextRenderActor::StaticClass(),FrdString::ToString(((ATextRenderActor*)actor)));

	// Check for BP rdToString for custom properties
	FString str=FrdString::ToString(actor);
	UFunction* func=actor->FindFunction(FName(TEXT("rdToString")));
	if(func) {
		struct cnvParms { FString str; };
		cnvParms parms;
		actor->ProcessEvent(func,(void*)&parms);
		if(!parms.str.IsEmpty()) {
			str+=TEXT("|")+parms.str;
		}
	}
	return rdGetActorSid(actor->GetClass(),str);
}

//.............................................................................
// rdGetActorClassFromSid
//.............................................................................
TSoftClassPtr<UObject> ArdInstBaseActor::rdGetActorClassFromSid(const FName sid) {

	FString strSid=((FName&)sid).ToString();
	TArray<FString> vals;
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	if(num<4) return nullptr;

	ErdSpawnType spawnType=(ErdSpawnType)FCString::Atoi(*vals[2]);
	if(spawnType!=ErdSpawnType::Actor) return nullptr;

	FString classname=rdUnpackMaterialName(vals[3]);
	TSoftClassPtr<UObject> actorClass=TSoftClassPtr<UObject>(FSoftClassPath(classname));
	return actorClass;
}

//.............................................................................
// rdGetSidActorDetails
//.............................................................................
void ArdInstBaseActor::rdGetSidActorDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,UClass*& uclass,FString& propStr,FName& groupName) {

	FString strSid=((FName&)sid).ToString();
	TArray<FString> vals;
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	if(num<5) return;

	type=(ErdSpawnType)FCString::Atoi(*vals[2]);
	if(type!=ErdSpawnType::Actor) return;

	FString classname=rdUnpackMaterialName(vals[3]);
	TSoftClassPtr<UObject> actorClass=TSoftClassPtr<UObject>(FSoftClassPath(classname));
	uclass=actorClass.Get();

	int32 i=4;
	ver=FCString::Atoi(*vals[1]);
	groupName=*vals[i++];

	for(;i<num;i++) propStr+=vals[i]+TEXT("|");
}

//.............................................................................
// rdSpawnActorFromSid
//.............................................................................
AActor* ArdInstBaseActor::rdSpawnActorFromSid(const FName sid,const FTransform& transform) {

	AActor* actor=nullptr;
	FString strSid=((FName&)sid).ToString();
	TArray<FString> vals;
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	if(num<5) return nullptr;

	int32 ver=FCString::Atoi(*vals[1]);
	ErdSpawnType spawnType=(ErdSpawnType)FCString::Atoi(*vals[2]);
	if(spawnType!=ErdSpawnType::Actor) return nullptr;

	FString classname=rdUnpackMaterialName(vals[3]);
	TSoftClassPtr<UObject> actorClass=TSoftClassPtr<UObject>(FSoftClassPath(classname));
	UClass* uclass=actorClass.Get();

	FName groupName=*vals[4];

	actor=rdSpawnActor(uclass,transform);
	if(!actor) {
		return nullptr;
	}

	FString str=TEXT("");
	for(int32 i=5;i<num;i++) {
		str+=vals[i]+TEXT("|");
	}
	rdApplyStringProps(actor,str,ver);
	
	return actor;
}

//.............................................................................
// rdSpawnActorsFromSid
//.............................................................................
int32 ArdInstBaseActor::rdSpawnActorsFromSid(const FName sid,const TArray<FTransform>& transforms,TArray<AActor*>& outActors) {

	for(auto& t:transforms) {
		outActors.Add(rdSpawnActorFromSid(sid,t));
	}

	return outActors.Num();
}

//.............................................................................
// rdSpawnActorsAndProxiesFromSid
//.............................................................................
void ArdInstBaseActor::rdSpawnActorsAndProxiesFromSid(const FName sid,TArray<FAddProxyQueueItem>& proxyQueue) {

	for(FAddProxyQueueItem& pqi:proxyQueue) {

		FrdProxyActorItem& item=pqi.actorItem;
		FTransform& t=pqi.transform;
		FHitResult& hit=pqi.hit;

		AActor* actor=rdSpawnActorFromSid(sid,t);
		if(!actor) {
			continue;
		}

		item.proxyActor=actor;
		proxyActorMap.Add(actor,item);

		ArdActor* rdActor=Cast<ArdActor>(actor);
		if(rdActor) {
			rdActor->proxyInstanceIndex=item.instanceIndex; // store the instance index in the proxy actor if based on rdActor
			//UInstancedStaticMeshComponent* ismc=item.instanceVolume?FindISMCforMeshInVolumeX(item.instanceVolume,item.sid):FindISMCforMesh(item.sid);
			rdActor->proxyInstanceComponent=item.ismc;
			rdActor->rdBuild();
		}

		if(item.proxyItem.bCallSwapEvent) {
			rdQueueProxySwapInDelegate(item.proxyActor,item.sid,item.instanceIndex,item.ismc,item.proxyItem.savedState);
		}

		if(item.forImpact) {
			TArray<UActorComponent*> comps;
			actor->GetComponents(UShapeComponent::StaticClass(),comps,false);
			for(auto comp:comps) {
				UShapeComponent* actComp=Cast<UShapeComponent>(comp);
				if(actComp) {
					FVector loc=t.GetTranslation(),imp(0,0,0);
					rdQueueProxyDamage((UPrimitiveComponent*)comp,actor,loc,imp,hit);
				}
			}
		}
	}
}

//.............................................................................
// rdGetDataLayerSid
//.............................................................................
FName ArdInstBaseActor::rdGetDataLayerSid(TSoftObjectPtr<UDataLayerAsset>& dl,const FName groupName) {
	
	FString str=FString::Printf(TEXT("sid|%d|8|%s|%s"),RDSTRING_VERSION,*rdPackMaterialName(dl.ToString()),*groupName.ToString());
	return FName(*str);
}

//.............................................................................
// rdGetDataLayerFromSid
//.............................................................................
TSoftClassPtr<UDataLayerAsset> ArdInstBaseActor::rdGetDataLayerFromSid(const FName sid) {

	FString strSid=((FName&)sid).ToString();
	TArray<FString> vals;
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	if(num<4) return TSoftClassPtr<UDataLayerAsset>();

	ErdSpawnType type=(ErdSpawnType)FCString::Atoi(*vals[2]);
	if(type!=ErdSpawnType::DataLayer) return TSoftClassPtr<UDataLayerAsset>();

	FString dlName=rdUnpackMaterialName(vals[3]);
	TSoftClassPtr<UDataLayerAsset> dl=TSoftClassPtr<UDataLayerAsset>(FSoftClassPath(dlName));
	
	return dl;
}

//.............................................................................
// rdGetSidDataLayerDetails
//.............................................................................
 void ArdInstBaseActor::rdGetSidDataLayerDetails(const FName sid,int32& ver,TEnumAsByte<ErdSpawnType>& type,TSoftClassPtr<UDataLayerAsset>& dl,FName& groupName) {

	FString strSid=((FName&)sid).ToString();
	TArray<FString> vals;
	int32 num=strSid.ParseIntoArray(vals,TEXT("|"),false);
	if(num<4) return;

	ver=FCString::Atoi(*vals[1]);
	type=(ErdSpawnType)FCString::Atoi(*vals[2]);
	if(type!=ErdSpawnType::DataLayer) return;

	FString dlName=rdUnpackMaterialName(vals[3]);
	dl=TSoftClassPtr<UDataLayerAsset>(FSoftClassPath(dlName));

	if(num>3) {
		groupName=*vals[4];
	}
}

//.............................................................................
