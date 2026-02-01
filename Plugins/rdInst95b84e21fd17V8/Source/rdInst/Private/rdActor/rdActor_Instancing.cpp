//
// rdActor_Instancing.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 22nd June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "Kismet/KismetMathLibrary.h"

//.............................................................................
//  rdUpdateISMTransforms
//
// Updates this Actors Instance Transforms to reflect any change in Actor Transform
//
//.............................................................................
void ArdActor::rdUpdateISMTransforms() {

	if(!rdGetBaseActor()) return;

#if WITH_EDITOR
	rdBaseActor->dontDoModify++;
#endif
	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		UInstancedStaticMeshComponent* imsc=rdBaseActor->FindISMCforMesh(sid);
		if(imsc) {
			FrdInstanceSettingsArray& i=it.Value;
			for(FrdInstanceFastSettings& j:i.settingsFast) {

				FTransform transform(j.transform);

				FVector scale=GetActorScale3D();
				FRotator rot=GetActorRotation();
				FVector loc=rot.RotateVector(transform.GetTranslation())*scale;
				transform.SetTranslation(loc);
				FTransform transform2(rot,GetActorLocation(),scale);
				transform.Accumulate(transform2);

//				UE_LOG(LogTemp,Display,TEXT("ArdActor(%x)::Updating instance %d to %f,%f"),this,j.index,transform.GetTranslation().X,transform.GetTranslation().Y);//@@
				imsc->UpdateInstanceTransform(j.index,transform,rdBaseActor->bWorldSpaceInstancing,false,true);
			}
			for(FrdInstanceRandomSettings& j:i.settingsRandom) {

				FTransform transform(j.actualTransform);

				FVector scale=GetActorScale3D();
				FRotator rot=GetActorRotation();
				FVector loc=rot.RotateVector(transform.GetTranslation())*scale;
				transform.SetTranslation(loc);
				FTransform transform2(rot,GetActorLocation(),scale);
				transform.Accumulate(transform2);

				imsc->UpdateInstanceTransform(j.index,transform,rdBaseActor->bWorldSpaceInstancing,false,true);
			}
		}
	}
#if WITH_EDITOR
	rdBaseActor->dontDoModify--;
#endif
}

//.............................................................................
//  rdRemoveInstances
//.............................................................................
void ArdActor::rdRemoveInstances() {

	if(!rdGetBaseActor()) return;
	rdBaseActor->rdRemInstancesForOwner(this);
}

//.............................................................................
//  rdResetAllInstances
//.............................................................................
void ArdActor::rdResetAllInstances() {

	if(!rdGetBaseActor()) return;
	rdBaseActor->rdRemAllInstances();
}

//.............................................................................
// rdGetInstanceGen
//
// Use this method to directly get the ISMC used for creating Instances for the specified StaticMesh
// Then you can add as many Instances directly to that ISMC. This is the fastest way of using ArdActors.
// Note: These Instances will Not be managed by ArdActor
//
//.............................................................................
UInstancedStaticMeshComponent* ArdActor::rdGetInstanceGen(const UStaticMesh* mesh,bool create) {
	return rdGetInstanceGenX(rdGetSMsid(mesh),create);
}
UInstancedStaticMeshComponent* ArdActor::rdGetInstanceGenX(const FName sid,bool create) {

	if(!rdGetBaseActor()) return nullptr;
	return rdBaseActor->rdGetInstanceGenX(sid,create);
}

//.............................................................................
// rdGetHInstanceGen
//
// Use this method to directly get the HISMC used for creating Instances for the specified StaticMesh
// Then you can add as many Instances directly to that HISMC. This is the fastest way of using ArdActors.
// Note: These Instances will Not be managed by ArdActor
//
//.............................................................................
UHierarchicalInstancedStaticMeshComponent* ArdActor::rdGetHInstanceGen(UStaticMesh* mesh,bool create) {
	return rdGetHInstanceGenX(rdGetSMsid(mesh),create);
}
UHierarchicalInstancedStaticMeshComponent* ArdActor::rdGetHInstanceGenX(const FName sid,bool create) {

	if(!rdGetBaseActor()) return nullptr;
	return rdBaseActor->rdGetHInstanceGenX(sid,create);
}

//.............................................................................
// rdGetPreferedInstanceGen
//.............................................................................
UInstancedStaticMeshComponent* ArdActor::rdGetPreferredInstanceGenX(const FName sid,bool create) {

	if(!rdGetBaseActor()) return nullptr;
	return rdBaseActor->rdGetPreferredInstanceGenX(sid,create);
}

//.............................................................................
// rdAddInstance
//
// Adds an Instance of the specified StaticMesh at the specified Transform
// Returns the index of the newly created Instance or -1 if failed.
// Note: These Instances will Not be managed by ArdActor
//
//.............................................................................
int32 ArdActor::rdAddInstance(UStaticMesh* mesh,const FTransform& transform) {
	return rdAddInstanceX(rdGetSMsid(mesh),transform);
}
int32 ArdActor::rdAddInstanceX(const FName sid,const FTransform& transform) {

	if(!rdGetBaseActor()) return -1;
	return rdBaseActor->rdAddInstanceX(this,sid,transform);
}

//.............................................................................
// rdInstToWorld
//.............................................................................
FTransform ArdActor::rdInstToWorld(const FTransform& tran) {

	FTransform transform=tran;
	FVector scale=GetActorScale3D();
	FRotator rot=GetActorRotation();
	FVector loc=rot.RotateVector(transform.GetTranslation())*scale;
	transform.SetTranslation(loc);
	FTransform transform2(rot,GetActorLocation(),scale);
	transform.Accumulate(transform2);
	if(rdGetBaseActor()) {
		transform.Accumulate(rdBaseActor->GetActorTransform().Inverse());
	}

	return transform;
}

//.............................................................................
// rdWorldToInst
//.............................................................................
FTransform ArdActor::rdWorldToInst(const FTransform& tran) {

	return UKismetMathLibrary::MakeRelativeTransform(tran,GetActorTransform());
}

//.............................................................................
// rdAddInstanceWorld
//
// Adds an Instance of the specified StaticMesh at the specified relative Transform
// Returns the index of the newly created Instance or -1 if failed.
// Note: These Instances will Not be managed by ArdActor
//
//.............................................................................
int32 ArdActor::rdAddInstanceWorld(UStaticMesh* mesh,const FTransform& tran) {
	return rdAddInstanceWorldX(rdGetSMsid(mesh),tran);
}
int32 ArdActor::rdAddInstanceWorldX(const FName sid,const FTransform& tran) {

	if(!rdGetBaseActor()) return -1;
	return rdBaseActor->rdAddInstanceX(this,sid,rdInstToWorld(tran));
}

//.............................................................................
// rdAddInstanceFast
//
// Adds an Instance using the specified HISMC at the specified Transform
// Returns the index of the newly created Instance or -1 if failed.
// Note: These Instances will Not be managed by ArdActor
//
//.............................................................................
int32 ArdActor::rdAddInstanceFast(UInstancedStaticMeshComponent* instGen,const FTransform& transform) {

	if(!rdGetBaseActor()) return -1;

	return rdBaseActor->rdAddOwnedInstance(this,instGen,transform);
}

//.............................................................................
// rdAddInstanceFastWorld
//
// Adds an Instance using the specified HISMC at the specified Transform
// Returns the index of the newly created Instance or -1 if failed.
// Note: These Instances will Not be managed by ArdActor
//
//.............................................................................
int32 ArdActor::rdAddInstanceFastWorld(UInstancedStaticMeshComponent* instGen,const FTransform& tran) {

	if(!rdGetBaseActor()) return -1;
	return rdBaseActor->rdAddOwnedInstance(this,instGen,rdInstToWorld(tran));
}

//.............................................................................
// rdClearInstanceList
//
// Now we get to the Managed routines, where each Instance is assigned to our
// Actor and we can control them as a group.
//
//.............................................................................
void ArdActor::rdClearInstanceList() {

	rdRemoveInstances();
	InstanceDataX.Empty();
}

//.............................................................................
// rdAddInstanceToList
//
// Adds a new entry in our Managed Instance List of the specified StaticMesh and Transform
//
//.............................................................................
void ArdActor::rdAddInstanceToList(UStaticMesh* mesh,const FTransform& transform) {
	return rdAddInstanceToListX(rdGetSMsid(mesh),transform);
}
void ArdActor::rdAddInstanceToListX(const FName sid,const FTransform& transform) {

	FrdInstanceFastSettings is;
	is.transform=transform;
	is.bVisible=true;
	FrdInstanceSettingsArray& isa=InstanceDataX.FindOrAdd(sid);
	isa.settingsFast.Add(MoveTemp(is));
}

//.............................................................................
// rdAddInstanceToList
//
// Adds a new entry in our Managed Instance List of the specified StaticMesh, 
//  Transform and Folder. optional Component which can contain Randomization settings
//
//.............................................................................
void ArdActor::rdAddAdvInstanceToList(UStaticMesh* mesh,const FTransform& transform,const FString& folder,UActorComponent* comp) {
	return rdAddAdvInstanceToListX(rdGetSMsid(mesh),transform,folder,comp);
}
void ArdActor::rdAddAdvInstanceToListX(const FName sid,const FTransform& transform,const FString& folder,UActorComponent* comp) {

	FrdInstanceRandomSettings is;
	is.origTransform=transform;
	is.actualTransform=transform;
	is.folder=folder;
	if(comp) {
		UrdRandomizeAssetUserData* ftag=rdInstLib::GetRandomizeUserData(comp,false);
		if(ftag) {
			is.randomSettings.location1=ftag->location1;
			is.randomSettings.location2=ftag->location2;
			is.randomSettings.rotation1=ftag->rotation1;
			is.randomSettings.rotation2=ftag->rotation2;
			is.randomSettings.scale1=ftag->scale1;
			is.randomSettings.scale2=ftag->scale2;
			is.randomSettings.flipProbability=ftag->flipProbability;
			is.randomSettings.showProbability=ftag->showProbability;
			is.randomSettings.lockAxis=ftag->lockAxis;
			is.randomSettings.relyActorName=ftag->relyActorName;
			is.randomSettings.themes=ftag->themes;
		}
	}
	FrdInstanceSettingsArray& isa=InstanceDataX.FindOrAdd(sid);
	isa.settingsRandom.Add(MoveTemp(is));
}

//.............................................................................
// rdGetLastInstanceIndex
//
// Returns the index of the last created Instance of the StaticMesh by this Actor
//
//.............................................................................
int32 ArdActor::rdGetLastInstanceIndex(UStaticMesh* mesh) {
	return rdGetLastInstanceIndexX(rdGetSMsid(mesh));
}
int32 ArdActor::rdGetLastInstanceIndexX(const FName sid) {

	if(!rdGetBaseActor()) return -1;
	return rdBaseActor->rdGetLastInstanceIndexX(this,sid);
}

//.............................................................................
// rdSetInstanceData
//
// Sets our Managed Instance List to each of the specified StaticMesh and Transforms 
// from the passed in Arrays (Arrays must be the same length)
//
//.............................................................................
bool ArdActor::rdSetInstanceData(const TArray<UStaticMesh*>& meshList,const TArray<FTransform>& transformList) {
	TArray<FName> sidList;
	for(auto m:meshList) sidList.Add(rdGetSMsid(m));
	return rdSetInstanceDataX(sidList,transformList);
}
bool ArdActor::rdSetInstanceDataX(const TArray<FName>& sidList,const TArray<FTransform>& transformList) {

	int32 num=sidList.Num();
	if(num!=transformList.Num()) return false;

	InstanceDataX.Empty();

	for(int32 i=0;i<num;i++) {

		FrdInstanceFastSettings is;
		is.transform=transformList[i];
		FrdInstanceSettingsArray& isa=InstanceDataX.FindOrAdd(sidList[i]);
		isa.settingsFast.Add(MoveTemp(is));
	}

	return true;
}

//.............................................................................
// rdAddInstances
//
// Adds new entries in our Managed Instance List of each of the transforms for 
// the specified StaticMesh 
//
//.............................................................................
bool ArdActor::rdAddInstances(UStaticMesh* mesh,const TArray<FTransform>& transforms) {
	return rdAddInstancesX(rdGetSMsid(mesh),transforms);
}
bool ArdActor::rdAddInstancesX(const FName sid,const TArray<FTransform>& transforms) {

	int32 num=transforms.Num();
	FrdInstanceSettingsArray& isa=InstanceDataX.FindOrAdd(sid);
	for(int32 i=0;i<num;i++) {

		FrdInstanceFastSettings is;
		is.transform=transforms[i];
		isa.settingsFast.Add(MoveTemp(is));
	}
	return true;
}

//.............................................................................
// rdAddInstancesFast
//
// Adds new entries in our Managed Instance List of each of the transforms for 
// the specified StaticMesh 
//
//.............................................................................
void ArdActor::rdAddInstancesFast(UStaticMesh* mesh,const TArray<FTransform>& transforms) {
	rdAddInstancesFastX(rdGetSMsid(mesh),transforms);
}
void ArdActor::rdAddInstancesFastX(const FName sid,const TArray<FTransform>& transforms) {

	if(!rdGetBaseActor()) return;
	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(!ismc) return;
	return rdBaseActor->rdAddInstancesFast(ismc,transforms);
}

//.............................................................................
// rdGetOrigTransform
//
// Returns the original Transform of the specified USceneComponent (including 
// ChildActorComponents etc). 
// This is the Transform before any Randomization is applied.
//
//.............................................................................
FTransform ArdActor::rdGetOrigTransform(USceneComponent* cac) {

	FTransform transform(FRotator(0,0,0),FVector(0,0,0),FVector(1,1,1));
	//while(cac && cac->GetAttachParent()) {
		
		TArray<FName>& tags=cac->ComponentTags;

		FVector loc(0,0,0),scale(1,1,1);
		FRotator rot(0,0,0);

		// Try getting it from the UAssetUserData first
		UrdTransformAssetUserData* ftag=rdInstLib::GetTransformUserData(cac,false);
		if(ftag) {

			transform.Accumulate(ftag->origTransform);

		} else {

			// the UAssetUserData can't exist yet, create one from this Components Tags
			for(auto& tag:tags) {
				FString st=tag.ToString();
				if(st.StartsWith(TEXT("loc="))) {
					getOneVector(st.RightChop(4),loc);
				} else if(st.StartsWith(TEXT("rot="))) {
					getOneRotator(st.RightChop(4),rot);
				} else if(st.StartsWith(TEXT("scale="))) {
					getOneVector(st.RightChop(6),scale);
				}
			}
			FTransform transform2(rot,loc,scale);

			ftag=rdInstLib::GetTransformUserData(cac,true);
			ftag->origTransform=transform2;

			transform.Accumulate(transform2);
		}

		//cac=nullptr;//cac->GetAttachParent();
	//}
	return transform;

}

//.............................................................................
// rdGetInstanceTranform
//
//   Not particularly fast, but makes things like swinging instanced doors open very easy.
// As there's only 1 or 2 going on at a time, it's not going to make any difference in the scheme of things.
//
//.............................................................................
bool ArdActor::rdGetInstanceTransform(UStaticMesh* mesh,int32 index,FTransform& stransform) {
	return rdGetInstanceTransformX(rdGetSMsid(mesh),index,stransform);
}
bool ArdActor::rdGetInstanceTransformX(const FName sid,int32 index,FTransform& stransform) {

	if(!rdGetBaseActor()) return false;
	return rdBaseActor->rdGetInstanceTransformX(sid,index,stransform);
}

//.............................................................................
// rdSetInstanceTranform
//
//   Not particularly fast, but makes things like swinging instanced doors open very easy.
// As there's only 1 or 2 going on at a time, it's not going to make any difference in the scheme of things.
//
//.............................................................................
void ArdActor::rdSetInstanceTransform(UStaticMesh* mesh,int32 index,const FTransform& stransform) {
	return rdSetInstanceTransformX(rdGetSMsid(mesh),index,stransform);
}
void ArdActor::rdSetInstanceTransformX(const FName sid,int32 index,const FTransform& stransform) {

	if(sid.IsNone() || !rdGetBaseActor() || index<0) return;
	UInstancedStaticMeshComponent* imsc=rdBaseActor->FindISMCforMesh(sid);
	if(!imsc) return;

	FTransform transform(stransform);

	FVector scale=GetActorScale3D();
	FRotator rot=GetActorRotation();
	FVector loc=rot.RotateVector(transform.GetTranslation())*scale;
	transform.SetTranslation(loc);
	FTransform transform2(rot,GetActorLocation(),scale);
	transform.Accumulate(transform2);

#if WITH_EDITOR
	rdBaseActor->dontDoModify++;
#endif
	imsc->UpdateInstanceTransform(index,transform,rdBaseActor->bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
	rdBaseActor->dontDoModify--;
#endif
}

//.............................................................................
// rdApplyRemoveInstance
//
// Removes the specified Instance Index belonging to the specified StaticMesh,
// storing it as an applied operation, not affecting the source data
//
//.............................................................................
void ArdActor::rdApplySetInstanceTransform(UStaticMesh* mesh,int32 index,const FTransform& stransform) {
	rdApplySetInstanceTransformX(rdGetSMsid(mesh),index,stransform);
}
void ArdActor::rdApplySetInstanceTransformX(const FName sid,int32 index,const FTransform& stransform) {
	FrdInstanceSettingsArray* i=InstanceDataX.Find(sid);
	if(!i) return;
	const FVector l=stransform.GetTranslation();
	const FVector s=stransform.GetScale3D();
	const FRotator r=stransform.Rotator();
	alterations.Add(FString::Printf(TEXT("ST:%s:%d:%f:%f:%f:%f:%f:%f:%f:%f:%f"),*sid.ToString(),index,l.X,l.Y,l.Z,s.X,s.Y,s.Z,r.Roll,r.Pitch,r.Yaw));

	rdSetInstanceTransformX(sid,index,stransform);
}

//.............................................................................
// rdRemoveInstance
//
// Removes the specified Instance Index belonging to the specified StaticMesh
//
//.............................................................................
void ArdActor::rdRemoveInstance(UStaticMesh* mesh,int32 index) {
	rdRemoveInstanceX(rdGetSMsid(mesh),index);
}
void ArdActor::rdRemoveInstanceX(const FName sid,int32 index) {

	if(sid.IsNone() || !rdGetBaseActor() || index<0) return;

	FrdInstanceSettingsArray* i=InstanceDataX.Find(sid);
	if(!i) return;
	int32 ind=0;
	for(FrdInstanceFastSettings& j:i->settingsFast) {
		if(j.index==index) {
			i->settingsFast.RemoveAt(ind);
			rdBaseActor->rdRemoveInstanceX(sid,index);
			return;
		}
		ind++;
	}
	ind=0;
	for(FrdInstanceRandomSettings& j:i->settingsRandom) {
		if(j.index==index) {
			i->settingsRandom.RemoveAt(ind);
			rdBaseActor->rdRemoveInstanceX(sid,index);
			return;
		}
		ind++;
	}
}

//.............................................................................
// rdApplyRemoveInstance
//
// Removes the specified Instance Index belonging to the specified StaticMesh,
// storing it as an applied operation, not affecting the source data
//
//.............................................................................
void ArdActor::rdApplyRemoveInstance(UStaticMesh* mesh,int32 index) {
	rdApplyRemoveInstanceX(rdGetSMsid(mesh),index);
}
void ArdActor::rdApplyRemoveInstanceX(const FName sid,int32 index) {
	FrdInstanceSettingsArray* i=InstanceDataX.Find(sid);
	if(!i) return;
	alterations.Add(FString::Printf(TEXT("RM:%s:%d"),*sid.ToString(),index));
	int32 ind=0;
	for(FrdInstanceFastSettings& j:i->settingsFast) {
		if(j.index==index) {
			rdBaseActor->rdRemoveInstanceX(sid,index);
			return;
		}
		ind++;
	}
	ind=0;
	for(FrdInstanceRandomSettings& j:i->settingsRandom) {
		if(j.index==index) {
			rdBaseActor->rdRemoveInstanceX(sid,index);
			return;
		}
		ind++;
	}

}

//.............................................................................
// rdRemoveInstanceFast
//
// Removes the specified Instance Index belonging to the specified ISMC
//
//.............................................................................
void ArdActor::rdRemoveInstanceFast(UInstancedStaticMeshComponent* instGen,int32 index) {

	if(!instGen || !rdGetBaseActor() || index<0) return;
	rdBaseActor->rdRemoveOwnedInstance(this,instGen,index);
}

//.............................................................................
// rdAddInstanceTranform
//
//   Not particularly fast, but makes things like swinging instanced doors open very easy.
// As there's only 1 or 2 going on at a time, it's not going to make any difference in the scheme of things.
//
//.............................................................................
void ArdActor::rdAddInstanceTransform(UStaticMesh* mesh,int32 index,const FTransform& stransform) {
	rdAddInstanceTransformX(rdGetSMsid(mesh),index,stransform);
}
void ArdActor::rdAddInstanceTransformX(const FName sid,int32 index,const FTransform& stransform) {

	if(sid.IsNone() || !rdGetBaseActor() || index<0) return;
	UInstancedStaticMeshComponent* imsc=rdBaseActor->FindISMCforMesh(sid);
	if(!imsc) return;

	FTransform transform;
	if(!rdGetInstanceTransformX(sid,index,transform)) return;
	transform.Accumulate(stransform);

#if WITH_EDITOR
	rdBaseActor->dontDoModify++;
#endif
	imsc->UpdateInstanceTransform(index,transform,rdBaseActor->bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
	rdBaseActor->dontDoModify--;
#endif
}

//.............................................................................
// rdGetStaticMeshFromInstanceIndex
//.............................................................................
UStaticMesh* ArdActor::rdGetStaticMeshFromInstanceIndex(UPrimitiveComponent* comp) {

	UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(comp);
	if(!ismc) return nullptr;

	return ismc->GetStaticMesh();
}

//.............................................................................
// rdGetActorFromInstanceIndex
//.............................................................................
AActor* ArdActor::rdGetActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index) {

	if(comp && rdGetBaseActor()) return rdBaseActor->rdGetActorFromInstanceIndex(comp,index);
	return nullptr;
}

//.............................................................................
// rdGetrdActorFromInstanceIndex
//.............................................................................
ArdActor* ArdActor::rdGetrdActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index) {

	if(comp && rdGetBaseActor()) return rdBaseActor->rdGetrdActorFromInstanceIndex(comp,index);
	return nullptr;
}

//.............................................................................
// rdGetInstanceIndexesForMesh
//.............................................................................
int32 ArdActor::rdGetInstanceIndexesForMesh(const UStaticMesh* mesh,TArray<int32>& indexes) {
	return rdGetInstanceIndexesForMeshX(rdGetSMsid(mesh),indexes);
}
int32 ArdActor::rdGetInstanceIndexesForMeshX(const FName sid,TArray<int32>& indexes) {

	FrdInstanceSettingsArray* dat=InstanceDataX.Find(sid);
	if(!dat) return 0;

	indexes.Empty();

	for(FrdInstanceRandomSettings& rs:dat->settingsRandom) {
		indexes.Add(rs.index);
	}

	for(FrdInstanceFastSettings& fs:dat->settingsFast) {
		indexes.Add(fs.index);
	}

	return indexes.Num();
}

//.............................................................................
// rdSetPerInstanceDataValueForMesh
//.............................................................................
int32 ArdActor::rdSetPerInstanceDataValueForMesh(const UStaticMesh* mesh,int32 index,float data) {
	return rdSetPerInstanceDataValueForMeshX(rdGetSMsid(mesh),index,data);
}
int32 ArdActor::rdSetPerInstanceDataValueForMeshX(const FName sid,int32 index,float data) {

	if(!rdGetBaseActor()) {
		return -1; // couldn't create an rdBaseActor
	}

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(!ismc) { // no ISMC found for this mesh
		return -2;
	}
	
	if(index>=ismc->PerInstanceSMCustomData.Num()) { // Instances have less PerInstance data values that the "data" array
		return -3;
	}

	FrdInstanceSettingsArray* dat=InstanceDataX.Find(sid);
	if(!dat) return -4;

	for(FrdInstanceRandomSettings& rs:dat->settingsRandom) {
		
		ismc->SetCustomDataValue(rs.index,index,data,true);
	}

	for(FrdInstanceFastSettings& fs:dat->settingsFast) {
		
		ismc->SetCustomDataValue(fs.index,index,data,true);
	}

	return 0;
}

//.............................................................................
// rdSetPerInstanceDataForMesh
//.............................................................................
int32 ArdActor::rdSetPerInstanceDataForMesh(const UStaticMesh* mesh,TArray<float> data) {
	return rdSetPerInstanceDataForMeshX(rdGetSMsid(mesh),data);
}
int32 ArdActor::rdSetPerInstanceDataForMeshX(const FName sid,TArray<float> data) {

	if(!rdGetBaseActor()) {
		return -1; // couldn't create an rdBaseActor
	}

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(!ismc) { // no ISMC found for this mesh
		return -2;
	}
	
	if(ismc->PerInstanceSMCustomData.Num()<data.Num()) { // Instances have less PerInstance data values that the "data" array
		return -3;
	}

	FrdInstanceSettingsArray* dat=InstanceDataX.Find(sid);
	if(!dat) return -4;

	for(FrdInstanceRandomSettings& rs:dat->settingsRandom) {
		
		ismc->SetCustomData(rs.index,data,true);
	}

	for(FrdInstanceFastSettings& fs:dat->settingsFast) {
		
		ismc->SetCustomData(fs.index,data,true);
	}

	return 0;
}

//.............................................................................
// rdSetPerInstanceDataValueForMeshID
//.............................................................................
int32 ArdActor::rdSetPerInstanceDataValueForMeshID(const UStaticMesh* mesh,float ID,int32 index,float data) {
	return rdSetPerInstanceDataValueForMeshIDX(rdGetSMsid(mesh),ID,index,data);
}
int32 ArdActor::rdSetPerInstanceDataValueForMeshIDX(const FName sid,float ID,int32 index,float data) {

	if(!rdGetBaseActor()) {
		return -1; // couldn't create an rdBaseActor
	}

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(!ismc) { // no ISMC found for this mesh
		return -2;
	}
	
	int32 stride=ismc->PerInstanceSMCustomData.Num();
	if(index>=stride) { // Instances have less PerInstance data values that the "data" array
		return -3;
	}

	FrdInstanceSettingsArray* dat=InstanceDataX.Find(sid);
	if(!dat) return -4;

	TArray<float>& allData=ismc->PerInstanceSMCustomData;

	for(FrdInstanceRandomSettings& rs:dat->settingsRandom) {

		if(allData[rs.index*stride]==ID) {
			ismc->SetCustomDataValue(rs.index,index,data,true);
		}
	}

	for(FrdInstanceFastSettings& fs:dat->settingsFast) {
		
		if(allData[fs.index*stride]==ID) {
			ismc->SetCustomDataValue(fs.index,index,data,true);
		}
	}

	return 0;
}

//.............................................................................
// rdSetPerInstanceDataForMeshID
//.............................................................................
int32 ArdActor::rdSetPerInstanceDataForMeshID(const UStaticMesh* mesh,float ID,TArray<float> data) {
	return rdSetPerInstanceDataForMeshIDX(rdGetSMsid(mesh),ID,data);
}
int32 ArdActor::rdSetPerInstanceDataForMeshIDX(const FName sid,float ID,TArray<float> data) {

	if(data.Num()<2) {
		return -5;
	}

	if(!rdGetBaseActor()) {
		return -1; // couldn't create an rdBaseActor
	}

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(!ismc) { // no ISMC found for this mesh
		return -2;
	}
	
	int32 stride=ismc->PerInstanceSMCustomData.Num();
	if(stride<data.Num()) { // Instances have less PerInstance data values that the "data" array
		return -3;
	}

	FrdInstanceSettingsArray* dat=InstanceDataX.Find(sid);
	if(!dat) return -4;

	data[0]=ID;
	TArray<float>& allData=ismc->PerInstanceSMCustomData;

	for(FrdInstanceRandomSettings& rs:dat->settingsRandom) {

		if(allData[rs.index*stride]==ID) {
			ismc->SetCustomData(rs.index,data,true);
		}
	}

	for(FrdInstanceFastSettings& fs:dat->settingsFast) {
		
		if(allData[fs.index*stride]==ID) {
			ismc->SetCustomData(fs.index,data,true);
		}
	}

	return 0;
}

//.............................................................................
// rdGetInstancesBounds
//.............................................................................
void ArdActor::rdGetInstancesBounds(const FVector& inMin,const FVector& inMax,FVector& outMin,FVector& outMax) {

	outMin=inMin;
	outMax=inMax;

	for(auto& it:InstanceData) {

		UStaticMesh* sm=it.Key;
		FrdInstanceSettingsArray& i=InstanceData[sm];
		for(FrdInstanceFastSettings& j:i.settingsFast) {

			FTransform& transform=j.transform;
			FVector loc=transform.GetTranslation();
			//TODO: scale to current actors scale
			if(loc.X<outMin.X) outMin.X=loc.X;
			if(loc.X>outMax.X) outMax.X=loc.X;
			if(loc.Y<outMin.Y) outMin.Y=loc.Y;
			if(loc.Y>outMax.Y) outMax.Y=loc.Y;
			if(loc.Z<outMin.Z) outMin.Z=loc.Z;
			if(loc.Z>outMax.Z) outMax.Z=loc.Z;
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
				
			FTransform& transform=j.origTransform;
			FVector loc=transform.GetTranslation();
			//TODO: scale to current actors scale
			if(loc.X<outMin.X) outMin.X=loc.X;
			if(loc.X>outMax.X) outMax.X=loc.X;
			if(loc.Y<outMin.Y) outMin.Y=loc.Y;
			if(loc.Y>outMax.Y) outMax.Y=loc.Y;
			if(loc.Z<outMin.Z) outMin.Z=loc.Z;
			if(loc.Z>outMax.Z) outMax.Z=loc.Z;
		}
	}
}

//.............................................................................
// rdMirrorInstanceLocations
//.............................................................................
void ArdActor::rdMirrorInstanceLocations(int32 axis,const FVector& center) {

	FVector vec((axis&1)?-1.0f:1.0f,(axis&2)?-1.0f:1.0f,(axis&4)?-1.0f:1.0f);
	for(auto& it:InstanceDataX) {

		FName sid=it.Key;
		FrdInstanceSettingsArray& i=it.Value;
		for(FrdInstanceFastSettings& j:i.settingsFast) {

			FTransform& transform=j.transform;
			FVector loc=(transform.GetTranslation()-center)*vec+center;
			transform.SetTranslation(loc);
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
				
			FTransform& transform=j.origTransform;
			FVector loc=(transform.GetTranslation()-center)*vec+center;
			transform.SetTranslation(loc);
		}
	}
	rdBuild();
}

//.............................................................................
// rdPlaceOnGround
//.............................................................................
void ArdActor::rdPlaceOnGround(bool smartPlacement) {

	FHitResult hit;
	FCollisionQueryParams queryParams;
	FVector actorLoc=GetActorLocation();

	for(auto& it:InstanceDataX) {

		FName sid=it.Key;
		FrdInstanceSettingsArray& i=it.Value;
		for(FrdInstanceFastSettings& j:i.settingsFast) {

			FTransform& transform=j.transform;
			FVector loc=transform.GetTranslation();
			GetWorld()->LineTraceSingleByChannel(hit,FVector(actorLoc.X,actorLoc.Y,actorLoc.Z-5000),FVector(actorLoc.X,actorLoc.Y,actorLoc.Z+5000),ECollisionChannel::ECC_Visibility,queryParams);
			if(hit.bBlockingHit) {
				loc.Z=hit.ImpactPoint.Z-GetActorLocation().Z;
				if(smartPlacement) {
				}
				transform.SetTranslation(loc);
			}
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
				
			FTransform& transform=j.origTransform;
			FVector loc=transform.GetTranslation();
			GetWorld()->LineTraceSingleByChannel(hit,FVector(actorLoc.X,actorLoc.Y,actorLoc.Z-5000),FVector(actorLoc.X,actorLoc.Y,actorLoc.Z+5000),ECollisionChannel::ECC_Visibility,queryParams);
			if(hit.bBlockingHit) {
				loc.Z=hit.ImpactPoint.Z-GetActorLocation().Z;
				transform.SetTranslation(loc);
			}
		}
	}

	rdBuild();
}

//.............................................................................
// rdAddInstanceLong
//.............................................................................
void ArdActor::rdAddInstanceLong(UStaticMesh* mesh,USceneComponent* comp,const FTransform& transform,const FString& name,const FString& folder) {
	rdAddInstanceLongX(rdGetSMsid(mesh),comp,transform,name,folder);
}
void ArdActor::rdAddInstanceLongX(const FName sid,USceneComponent* comp,const FTransform& transform,const FString& name,const FString& folder) {

	FName csid=sid;
	if(csid.IsNone() && comp) {
		UChildActorComponent* cac=Cast<UChildActorComponent>(comp);
		if(cac) {
			AStaticMeshActor* smActor=Cast<AStaticMeshActor>(cac->GetChildActorTemplate());
			if(smActor)	{
				csid=rdGetSMCsid(smActor->GetStaticMeshComponent());
			}
		} else {
			UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(comp);
			if(smc) {
				csid=rdGetSMCsid(smc);
			}
		}
	}
	if(csid.IsNone()) return;
	bCreateFromArrays=false;

	FrdInstanceSettingsArray& settingsArray=InstanceDataX.FindOrAdd(csid);

	FrdInstanceRandomSettings rs;
	if(comp && rdSetRandomFromTags(comp->ComponentTags,&rs.randomSettings)) {

		rs.origTransform=transform;
		rs.name=name;
		rs.folder=folder;

		settingsArray.settingsRandom.Add(MoveTemp(rs));

	} else {

		FrdInstanceFastSettings rsf;
		rsf.transform=transform;
		rsf.name=name;
		rsf.folder=folder;

		settingsArray.settingsFast.Add(MoveTemp(rsf));
	}
}

//.............................................................................
// rdDuplicateInstancesLongX
//.............................................................................
void ArdActor::rdDuplicateInstancesLongX(FrdSidInstancesMap sidMap) {
/*
	for(auto& it:sidMap.instances) {

		FName sid=it.Key;
		FrdInstanceIndexes& ii=it.Value;

		for(int32 i:ii.indexes) {

			FTransform t=rdGetInstanceTransformX(sid,i)

			int32 index=rdAddInstanceX(sid,t);
			rdAddInstanceLongX(sid,nullptr,t,"","",index);
		}
	}
*/
}

//.............................................................................
// rdRemoveInstancesLongX
//.............................................................................
void ArdActor::rdRemoveInstancesLongX(FrdSidInstancesMap sidMap) {
/*
	for(auto& it:sidMap.instances) {

		FName sid=it.Key;
		FrdInstanceIndexes& ii=it.Value;

		for(int32 i:ii.indexes) {
			rdRemInstanceLongX(sid,nullptr,i);
		}
	}
*/
}

//.............................................................................
// rdReplaceInstancesLongX
//.............................................................................
void ArdActor::rdReplaceInstancesLongX(FrdSidInstancesMap sidMap,FName newSid) {
/*
	for(auto& it:sidMap.instances) {

		FName sid=it.Key;
		FrdInstanceIndexes& ii=it.Value;

		for(int32 i:ii.indexes) {
			FTransform t=rdGetInstanceTransformX(sid,i)
			rdRemInstanceLongX(sid,nullptr,i);
			int32 index=rdAddInstanceX(newSid,t);
			rdAddInstanceLongX(newSid,nullptr,t,"","",index);
		}
	}
*/
}

//.............................................................................
// rdUpdateTransform
//.............................................................................
void ArdActor::rdUpdateTransform(UStaticMesh* mesh,int32 index,const FTransform& transform) {
	rdUpdateTransformX(rdGetSMsid(mesh),index,transform);
}
void ArdActor::rdUpdateTransformX(const FName sid,int32 index,const FTransform& transform) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdUpdateTransformX(sid,index,transform);
}

//.............................................................................
// rdUpdateTransforms
//.............................................................................
void ArdActor::rdUpdateTransforms(UStaticMesh* mesh,int32 startIndex,const TArray<FTransform>& transforms) {
	rdUpdateTransformsX(rdGetSMsid(mesh),startIndex,transforms);
}
void ArdActor::rdUpdateTransformsX(const FName sid,int32 startIndex,const TArray<FTransform>& transforms) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdUpdateTransformsX(sid,startIndex,transforms);
}

//.............................................................................
// rdIncrementTransforms
//.............................................................................
void ArdActor::rdIncrementTransforms(UStaticMesh* mesh,const TArray<int32>& indexes,const FTransform& transform) {
	rdIncrementTransformsX(rdGetSMsid(mesh),indexes,transform);
}
void ArdActor::rdIncrementTransformsX(const FName sid,const TArray<int32>& indexes,const FTransform& transform) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdIncrementTransformsX(sid,indexes,transform);
}

//.............................................................................
// rdGetTransformsPtr
//.............................................................................
TArray<FMatrix>& ArdActor::rdGetTransformsPtr(UStaticMesh* mesh,int32& numTransforms) {
	return rdGetTransformsPtrX(rdGetSMsid(mesh),numTransforms);
}
TArray<FMatrix>& ArdActor::rdGetTransformsPtrX(const FName sid,int32& numTransforms) {

	static TArray<FMatrix> empty;
	if(!rdGetBaseActor()) {
		return empty;
	}
	return rdGetBase()->rdGetTransformsPtrX(sid,numTransforms);
}

//.............................................................................
// rdSetNumInstCustomData
//.............................................................................
void ArdActor::rdSetNumInstCustomData(UStaticMesh* mesh,int32 numData) {
	rdSetNumInstCustomDataX(rdGetSMsid(mesh),numData);
}
void ArdActor::rdSetNumInstCustomDataX(const FName sid,int32 numData) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetNumInstCustomDataX(sid,numData);
}

//.............................................................................
// rdSetNumInstCustomDataFast
//.............................................................................
void ArdActor::rdSetNumInstCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 numData) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetNumInstCustomDataFast(ismc,numData);
}

//.............................................................................
// rdGetCustomDataPtr
//.............................................................................
TArray<float>& ArdActor::rdGetCustomDataPtr(UStaticMesh* mesh,int32& numData) {
	return rdGetCustomDataPtrX(rdGetSMsid(mesh),numData);
}
TArray<float>& ArdActor::rdGetCustomDataPtrX(const FName sid,int32& numData) {

	static TArray<float> empty;
	if(!rdGetBaseActor()) {
		return empty;
	}
	return rdGetBase()->rdGetCustomDataPtrX(sid,numData);
}

//.............................................................................
// rdGetCustomDataPtrFast
//.............................................................................
TArray<float>& ArdActor::rdGetCustomDataPtrFast(UInstancedStaticMeshComponent* ismc,int32& numData) {

	static TArray<float> empty;
	if(!rdGetBaseActor()) {
		return empty;
	}
	return rdGetBase()->rdGetCustomDataPtrFast(ismc,numData);
}

//.............................................................................
// rdGetCustomCol3Data
//.............................................................................
FLinearColor ArdActor::rdGetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex) {
	
	if(!rdGetBaseActor()) return FLinearColor();
	return rdGetBase()->rdGetCustomCol3Data(mesh,instanceIndex,dataIndex);
}

//.............................................................................
// rdGetCustomCol3DataX
//.............................................................................
FLinearColor ArdActor::rdGetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex) {
	
	if(!rdGetBaseActor()) return FLinearColor();
	return rdGetBase()->rdGetCustomCol3DataX(sid,instanceIndex,dataIndex);
}

//.............................................................................
// rdGetCustomCol3DataFast
//.............................................................................
FLinearColor ArdActor::rdGetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex) {
	
	if(!rdGetBaseActor()) return FLinearColor();
	return rdGetBase()->rdGetCustomCol3DataFast(ismc,instanceIndex,dataIndex);
}

//.............................................................................
// rdGetCustomCol4Data
//.............................................................................
FLinearColor ArdActor::rdGetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex) {
	
	if(!rdGetBaseActor()) return FLinearColor();
	return rdGetBase()->rdGetCustomCol4Data(mesh,instanceIndex,dataIndex);
}

//.............................................................................
// rdGetCustomCol4DataX
//.............................................................................
FLinearColor ArdActor::rdGetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex) {
	
	if(!rdGetBaseActor()) return FLinearColor();
	return rdGetBase()->rdGetCustomCol4DataX(sid,instanceIndex,dataIndex);
}

//.............................................................................
// rdGetCustomCol4DataFast
//.............................................................................
FLinearColor ArdActor::rdGetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex) {
	
	if(!rdGetBaseActor()) return FLinearColor();
	return rdGetBase()->rdGetCustomCol4DataFast(ismc,instanceIndex,dataIndex);
}

//.............................................................................
// rdSetCustomData
//.............................................................................
void ArdActor::rdSetCustomData(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,float value,bool batch) {
	rdSetCustomDataX(rdGetSMsid(mesh),instanceIndex,dataIndex,value,batch);
}
void ArdActor::rdSetCustomDataX(const FName sid,int32 instanceIndex,int32 dataIndex,float value,bool batch) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetCustomDataX(sid,instanceIndex,dataIndex,value,batch);
}

//.............................................................................
// rdSetCustomDataFast
//.............................................................................
void ArdActor::rdSetCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,float value,bool batch) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetCustomDataFast(ismc,instanceIndex,dataIndex,value,batch);
}

//.............................................................................
// rdSetCustomCol3Data
//.............................................................................
void ArdActor::rdSetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	rdSetCustomCol3DataX(rdGetSMsid(mesh),instanceIndex,dataIndex,col,batch);
}
void ArdActor::rdSetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetCustomCol3DataX(sid,instanceIndex,dataIndex,col,batch);
}

//.............................................................................
// rdSetCustomCol3DataFast
//.............................................................................
void ArdActor::rdSetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetCustomCol3DataFast(ismc,instanceIndex,dataIndex,col,batch);
}

//.............................................................................
// rdSetCustomCol4Data
//.............................................................................
void ArdActor::rdSetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	rdSetCustomCol4DataX(rdGetSMsid(mesh),instanceIndex,dataIndex,col,batch);
}
void ArdActor::rdSetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetCustomCol4DataX(sid,instanceIndex,dataIndex,col,batch);
}

//.............................................................................
// rdSetCustomCol4DataFast
//.............................................................................
void ArdActor::rdSetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetCustomCol4DataFast(ismc,instanceIndex,dataIndex,col,batch);
}

//.............................................................................
// rdSetAllCustomData
//.............................................................................
void ArdActor::rdSetAllCustomData(UStaticMesh* mesh,UPARAM(ref) TArray<float>& data,bool update) {
	rdSetAllCustomDataX(rdGetSMsid(mesh),data,update);
}
void ArdActor::rdSetAllCustomDataX(const FName sid,UPARAM(ref) TArray<float>& data,bool update) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetAllCustomDataX(sid,data,update);
}

//.............................................................................
// rdSetAllCustomDataFast
//.............................................................................
void ArdActor::rdSetAllCustomDataFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) TArray<float>& data,bool update) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdSetAllCustomDataFast(ismc,data,update);
}

//.............................................................................
// rdUpdateCustomData
//.............................................................................
void ArdActor::rdUpdateCustomData(UStaticMesh* mesh) {
	rdUpdateCustomDataX(rdGetSMsid(mesh));
}
void ArdActor::rdUpdateCustomDataX(const FName sid) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdUpdateCustomDataX(sid);
}

//.............................................................................
// rdUpdateCustomDataFast
//.............................................................................
void ArdActor::rdUpdateCustomDataFast(UInstancedStaticMeshComponent* ismc) {

	if(!rdGetBaseActor()) return;
	rdGetBase()->rdUpdateCustomDataFast(ismc);
}

//.............................................................................
