//
// rdActor_Sid.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 3rd December 2024
// Last Modified: 20th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"

//.............................................................................
// rdPackMaterialName
//.............................................................................
FString ArdActor::rdPackMaterialName(const FString& nm) {

	FString str=nm.Replace(TEXT("/Game/"),TEXT("%1")).Replace(TEXT("Materials/"),TEXT("%2")).Replace(TEXT("Meshes/"),TEXT("%3"));
	return str;
}

//.............................................................................
// rdUnpackMaterialName
//.............................................................................
FString ArdActor::rdUnpackMaterialName(const FString& nm) {

	FString str=nm.Replace(TEXT("%1"),TEXT("/Game/")).Replace(TEXT("%2"),TEXT("Materials/")).Replace(TEXT("%3"),TEXT("Meshes/"));
	return str;
}

//.............................................................................
//  rdGetSMsid
//.............................................................................
FName ArdActor::rdGetSMsid(const UStaticMesh* m,TEnumAsByte<ErdSpawnType> type,int32 id,const FName groupName) {
	if(!m) return FName();
	if((type==ErdSpawnType::UseDefaultSpawn && (!rdGetBaseActor() || rdBaseActor->bUseHISMs)) || type==ErdSpawnType::HISM) {
		if(m && rdBaseActor->rdIsMeshNanite(m)) {
			type=ErdSpawnType::ISM;
		}
	}
	FString str=FString::Printf(TEXT("sid|%d|%d|%s|%d|%s"),RD_SID_VER,(int32)type,*m->GetPathName(),id,groupName.IsNone()?TEXT(""):*groupName.ToString());
	FName sid=FName(*str);
	return sid;
}

//.............................................................................
//  rdGetSMXsid
//.............................................................................
FName ArdActor::rdGetSMXsid(TEnumAsByte<ErdSpawnType> type,const UStaticMesh* mesh,TArray<TSoftObjectPtr<UMaterialInterface>> mats,bool bReverseCulling,ErdCollision collision,float startCull,float endCull,int32 id,const FName groupName) {
	if(!mesh) return FName();
	if((type==ErdSpawnType::UseDefaultSpawn && (!rdGetBaseActor() || rdBaseActor->bUseHISMs)) || type==ErdSpawnType::HISM) {
		if(mesh && rdBaseActor->rdIsMeshNanite(mesh)) {
			type=ErdSpawnType::ISM;
		}
	}

	FString str=FString::Printf(TEXT("sid|%d|%d|%s|%d"),RD_SID_VER,(int32)type,*mesh->GetPathName(),(int32)collision);

	if(bReverseCulling || mats.Num()>0 || startCull>=0.0f || endCull>=0.0f || id>0 || !groupName.IsNone()) {
		str+=FString::Printf(TEXT("|%d|%s|%s|%d|%s"),bReverseCulling,*FString::SanitizeFloat(startCull),*FString::SanitizeFloat(endCull),id,groupName.IsNone()?TEXT(""):*groupName.ToString());
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
	return sid;
}

//.............................................................................
//  rdGetSMCsid
//.............................................................................
FName ArdActor::rdGetSMCsid(const UStaticMeshComponent* smc,TEnumAsByte<ErdSpawnType> type,float overrideStartCull,float overrideEndCull,int32 overrideID,const FName overrideGroupName) {

	if(!smc) return FName();
	UStaticMesh* mesh=smc->GetStaticMesh();
	if(!mesh) return FName();
	if((type==ErdSpawnType::UseDefaultSpawn && (!rdGetBaseActor() || rdBaseActor->bUseHISMs)) || type==ErdSpawnType::HISM) {
		if(mesh && rdBaseActor->rdIsMeshNanite(mesh)) {
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

		if(!revCull && (startCull==0 || startCull==400000.0f) && (endCull==0 || endCull==startCull) && overrideGroupName.IsNone()) {
			str=FString::Printf(TEXT("sid|%d|%d|%s|%d"),RD_SID_VER,(int32)type,*mesh->GetPathName(),(int32)smc->BodyInstance.GetCollisionEnabled()+1);
		} else {
			str=FString::Printf(TEXT("sid|%d|%d|%s|%d|%d|%s|%s|%d|%s"),RD_SID_VER,(int32)type,*mesh->GetPathName(),(int32)smc->BodyInstance.GetCollisionEnabled()+1,revCull,*FString::SanitizeFloat(startCull),*FString::SanitizeFloat(endCull),overrideID,overrideGroupName.IsNone()?TEXT(""):*overrideGroupName.ToString());
		}
		return FName(*str);
	}

	str=FString::Printf(TEXT("sid|%d|%d|%s|%d|%d|%s|%s|%d|%s|"),RD_SID_VER,(int32)type,*mesh->GetPathName(),(int32)smc->BodyInstance.GetCollisionEnabled()+1,revCull,*FString::SanitizeFloat(startCull),*FString::SanitizeFloat(endCull),overrideID,overrideGroupName.IsNone()?TEXT(""):*overrideGroupName.ToString());
	FString lastMat;
	for(auto mi:omats) {
		if(mi) {
			TSoftObjectPtr<UMaterialInterface> sp=mi;
			FString pnm=sp.ToString();
			if(pnm==lastMat) pnm=TEXT("+");
			else lastMat=pnm;
			str+=pnm+TEXT("|");
		} else {
			str+=TEXT("|");
		}
	}

	FName sid=FName(*str);

	return sid;
}

//.............................................................................
//  rdGetMeshFromSid
//.............................................................................
UStaticMesh* ArdActor::rdGetMeshFromSid(const FName sid) {

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
	FSoftObjectPath sop(fname);

	UStaticMesh* mesh=Cast<UStaticMesh>(sop.ResolveObject());
	if(!mesh) {
		mesh=Cast<UStaticMesh>(sop.TryLoad());
	}
	
	return mesh;
}

//.............................................................................
// rdGetSoftMeshFromSid - Returns the UStaticMesh SoftObjectPtr from the sid
//.............................................................................
TSoftObjectPtr<UStaticMesh> ArdActor::rdGetSoftMeshFromSid(const FName sid) {

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
TArray<TSoftObjectPtr<UMaterialInterface>> ArdActor::rdGetMaterialsFromSid(const FName sid) {
	TArray<TSoftObjectPtr<UMaterialInterface>> ary;
	if(!rdGetBaseActor()) return ary;
	return rdBaseActor->rdGetMaterialsFromSid(sid);
}

//.............................................................................
// rdGetMeshNameFromSid
//.............................................................................
FString ArdActor::rdGetMeshNameFromSid(const FName sid) {
	FString str=sid.ToString();
	int32 pos=str.Find(TEXT("|"),ESearchCase::CaseSensitive,ESearchDir::FromStart);
	FString fname=str.Left(pos);
	int32 i=fname.Find(TEXT("."),ESearchCase::CaseSensitive,ESearchDir::FromEnd);
	return fname.RightChop(i+1);
}

//.............................................................................
