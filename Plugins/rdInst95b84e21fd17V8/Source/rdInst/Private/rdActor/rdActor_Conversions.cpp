//
// rdActor_Conversions.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 25th February 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"

//.............................................................................
// rdConvertInstanceToChildActor
//
// Converts the Instance to a ChildActorComponent
//
//.............................................................................
UChildActorComponent* ArdActor::rdConvertInstanceToChildActor(UStaticMesh* mesh,int32 index) {
	return rdConvertInstanceToChildActorX(rdGetSMsid(mesh),index);
}
UChildActorComponent* ArdActor::rdConvertInstanceToChildActorX(const FName sid,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {
		UChildActorComponent* cac=rdAddChildComponent(AStaticMeshActor::StaticClass(),transform);
		if(cac) {
			AStaticMeshActor* meshActor=(AStaticMeshActor*)cac->GetChildActor();
			if(!meshActor) meshActor=(AStaticMeshActor*)cac->GetChildActorTemplate();
			if(meshActor) {
				meshActor->GetStaticMeshComponent()->SetStaticMesh(rdGetMeshFromSid(sid));
				rdRemoveInstanceX(sid,index);
				return cac;
			}
		}
	}

	return nullptr;
}
#ifdef sdkfjh
//.............................................................................
// rdConvertInstanceToLevelActor
//
// Converts the Instance to a new Actor in the level (spawn)
//
//.............................................................................
AActor* ArdActor::rdConvertInstanceToLevelActor(UStaticMesh* mesh,int32 index) {
	return rdConvertInstanceToLevelActorX(rdGetSMsid(mesh),index);
}
AActor* ArdActor::rdConvertInstanceToLevelActorX(const FName sid,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AStaticMeshActor* actor=(AStaticMeshActor*)ArdActor::rdSpawnActor(AStaticMeshActor::StaticClass(),transform);
		if(actor) {
			actor->GetStaticMeshComponent()->SetStaticMesh(rdGetMeshFromSid(sid));
			rdRemoveInstanceX(sid,index);
			return (AActor*)actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstanceToActor
//.............................................................................
AActor* ArdActor::rdConvertInstanceToActor(UStaticMesh* mesh,UClass* actorClass,int32 index) {
	return rdConvertInstanceToActorX(rdGetSMsid(mesh),actorClass,index);
}
AActor* ArdActor::rdConvertInstanceToActorX(const FName sid,UClass* actorClass,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AActor* actor=ArdActor::rdSpawnActor(actorClass,transform);
		if(actor) {
			rdRemoveInstanceX(sid,index);
			return actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstanceToActorFromPool
//.............................................................................
AActor* ArdActor::rdConvertInstanceToActorFromPool(UStaticMesh* mesh,UClass* actorClass,int32 index) {
	return rdConvertInstanceToActorFromPoolX(rdGetSMsid(mesh),actorClass,index);
}
AActor* ArdActor::rdConvertInstanceToActorFromPoolX(const FName sid,UClass* actorClass,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AActor* actor=rdGetBase()->rdGetActorFromPool(actorClass,transform);
		if(actor) {
			rdRemoveInstanceX(sid,index);
			return (AActor*)actor;
		}
	}
	return nullptr;
}
#endif
//.............................................................................
// rdConvertInstanceToLevelActor
//
// Converts the Instance to a new Actor in the level (spawn)
//
//.............................................................................
AActor* ArdActor::rdConvertInstanceToLevelActor(UStaticMesh* mesh,int32 index) {
	return rdConvertInstanceToLevelActorX(rdGetSMsid(mesh),index);
}
AActor* ArdActor::rdConvertInstanceToLevelActorX(const FName sid,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AStaticMeshActor* actor=(AStaticMeshActor*)ArdActor::rdSpawnActor(AStaticMeshActor::StaticClass(),transform);
		if(actor) {
			actor->GetStaticMeshComponent()->SetStaticMesh(rdGetMeshFromSid(sid));
			rdRemoveInstanceX(sid,index);
			return (AActor*)actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstanceToActor
//.............................................................................
AActor* ArdActor::rdConvertInstanceToActor(UStaticMesh* mesh,UClass* actorClass,int32 index) {
	return rdConvertInstanceToActorX(rdGetSMsid(mesh),actorClass,index);
}
AActor* ArdActor::rdConvertInstanceToActorX(const FName sid,UClass* actorClass,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AActor* actor=ArdActor::rdSpawnActor(actorClass,transform);
		if(actor) {
			rdRemoveInstanceX(sid,index);
			return actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstanceToActorFromPool
//.............................................................................
AActor* ArdActor::rdConvertInstanceToActorFromPool(UStaticMesh* mesh,UClass* actorClass,int32 index) {
	return rdConvertInstanceToActorFromPoolX(rdGetSMsid(mesh),actorClass,index);
}
AActor* ArdActor::rdConvertInstanceToActorFromPoolX(const FName sid,UClass* actorClass,int32 index) {

	FTransform transform;
	if(rdGetInstanceTransformX(sid,index,transform)) {

		AActor* actor=rdGetBase()->rdGetActorFromPool(actorClass,transform);
		if(actor) {
			rdRemoveInstanceX(sid,index);
			return (AActor*)actor;
		}
	}
	return nullptr;
}

//.............................................................................
// rdConvertInstancesToActors
//.............................................................................
void ArdActor::rdConvertInstancesToActors(bool stripFolders,bool useRandom,bool group,const FTransform& destTransform,const FVector& centerLocation,const FString& baseFolder,TArray<AActor*>& addedActors) {
#if WITH_EDITOR
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner=nullptr;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.NameMode=FActorSpawnParameters::ESpawnActorNameMode::Requested;
	SpawnParams.ObjectFlags=RF_Transactional;//RF_Public

	for(auto& it:InstanceData) {

		UStaticMesh* sm=it.Key; // Static Mesh to Add to level
		FrdInstanceSettingsArray& i=InstanceData[sm];
		for(FrdInstanceFastSettings& j:i.settingsFast) {

			if(useRandom && !j.bVisible) continue;

			FTransform transform=j.transform;
			SpawnParams.Template=nullptr;
			FString sname=j.name;
			if(sname.IsEmpty()) sname=sm->GetName();
			SpawnParams.Name=FName(sname);

			transform.Accumulate(destTransform);
			AStaticMeshActor* meshActor=GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),transform,SpawnParams);
			check(meshActor);
			meshActor->GetStaticMeshComponent()->SetStaticMesh(sm);
			meshActor->SetActorLabel(sname);

			if(!stripFolders) {
				FString fld=baseFolder+TEXT("/")+j.folder;
				meshActor->SetFolderPath_Recursively(FName(*fld));
			}

			if(group) {
			//	groupList.Add(meshActor);
			}

			meshActor->InvalidateLightingCache();
			meshActor->PostEditMove(true);
			addedActors.Add(meshActor);
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
				
			if(useRandom && !j.bVisible) continue;
				
			FTransform transform=j.origTransform;
			if(useRandom) {
				rdAddRandomization(transform,&j.randomSettings);
			}

			SpawnParams.Template=nullptr;
			FString sname=j.name;
			if(sname.IsEmpty()) sname=sm->GetName();
			SpawnParams.Name=FName(sname);

			transform.Accumulate(destTransform);
			AStaticMeshActor* meshActor=GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),transform,SpawnParams);
			check(meshActor);
			meshActor->GetStaticMeshComponent()->SetStaticMesh(sm);

			if(!stripFolders) {
				FString fld=baseFolder+TEXT("/")+j.folder;
				meshActor->SetFolderPath_Recursively(FName(*fld));
			}

			meshActor->SetActorLabel(j.name);
			rdCreateTagsFromRandom(meshActor->Tags,j.randomSettings);

			if(group) {
			//	groupList.Add(meshActor);
			}

			meshActor->InvalidateLightingCache();
			meshActor->PostEditMove(true);
			addedActors.Add(meshActor);
		}
	}
#endif
}

//.............................................................................
// rdHarvestInstances
//.............................................................................
void ArdActor::rdHarvestInstances(bool incRandom,TArray<UStaticMesh*>& meshList,TArray<FTransform>& transformList,TArray<FString>& nameList,TArray<FString>& folderList) {
	TArray<FName> sids;
	for(auto m:meshList) {
		sids.Add(rdGetSMsid(m));
	}
	rdHarvestInstancesX(incRandom,sids,transformList,nameList,folderList);
}
void ArdActor::rdHarvestInstancesX(bool incRandom,TArray<FName>& sidList,TArray<FTransform>& transformList,TArray<FString>& nameList,TArray<FString>& folderList) {

	if(!bCanBeAssimilated) return;

	// Instances
	for(auto& it:InstanceDataX) {

		FName sid=it.Key;
		//UStaticMesh* sm=rdGetMeshFromSid(sid);
		FrdInstanceSettingsArray& i=it.Value;//InstanceData[sm];
		int cnt=1;
		for(FrdInstanceFastSettings& j:i.settingsFast) {

			sidList.Add(sid);
			transformList.Add(j.transform);
			if(j.name.Len()>0) nameList.Add(j.name);
			else nameList.Add(FString::Printf(TEXT("%s%d"),*rdGetMeshNameFromSid(sid),cnt++));
			folderList.Add(j.folder);
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {

			if(incRandom && !j.bVisible) continue;

			sidList.Add(sid);
			if(incRandom) {
				transformList.Add(j.actualTransform);
			} else {
				transformList.Add(j.origTransform);
			}
			if(j.name.Len()>0) nameList.Add(j.name);
			else nameList.Add(FString::Printf(TEXT("%s%d"),*rdGetMeshNameFromSid(sid),cnt++));
			folderList.Add(j.folder);
		}
	}
}

//.............................................................................
// rdHarvestActors
//.............................................................................
int32 ArdActor::rdHarvestActors(TArray<AActor*> sactorList,bool harvest) {

	if(!rdGetBaseActor()) return 0;

	int32 num=0;
	TArray<FrdInstItemX> meshList;
	TArray<FrdActorItemX> actorList;
	TArray<AActor*> actors;

	for(auto a:sactorList) {

		AStaticMeshActor* sma=Cast<AStaticMeshActor>(a); // StaticMeshes
		if(sma) {

			FName sid=rdGetSMCsid(sma->GetStaticMeshComponent(),sma->Tags.Contains(TEXT("UseSM"))?ErdSpawnType::SMC:ErdSpawnType::UseDefaultSpawn);
			FrdInstItemX ix;
			ix.sid=sid;
			ix.transforms.Add(sma->GetActorTransform());
			meshList.Add(MoveTemp(ix));
			num++;

		} else {

			if(harvest) {
				// harvest
				TArray<AActor*> addedActors;
				TArray<AActor*> alist={a};
				TMap<FName,FrdInstItemX> scrapeMap;
				rdBaseActor->rdHarvestInstancesFromActorList(alist,false,addedActors,scrapeMap);
				for(auto& ix:scrapeMap) {
					meshList.Add(ix.Value);
					num++;
				}
				for(auto aa:addedActors) {
					UClass* aclass=aa->GetClass();
					bool found=false;
					for(auto& ia:actorList) {
						if(ia.actorClass==aclass) {
							found=true;
							ia.transforms.Add(aa->GetActorTransform());
							num++;
						}
					}
					if(!found) {
						FrdActorItemX ia;
						ia.actorClass=aclass;
						ia.transforms.Add(aa->GetActorTransform());
						ia.actors.Add(aa);
						actorList.Add(MoveTemp(ia));
					}
				}

			} else {

				UClass* aclass=a->GetClass();
				bool found=false;
				for(auto& ia:actorList) {
					if(ia.actorClass==aclass) {
						found=true;
						ia.transforms.Add(a->GetActorTransform());
						num++;
					}
				}
				if(!found) {
					FrdActorItemX ia;
					ia.actorClass=aclass;
					ia.transforms.Add(a->GetActorTransform());
					ia.actors.Add(a);
					actorList.Add(MoveTemp(ia));
				}
			}
		}
	}

	bool ret=rdUpdatePrefabWithActorList(this,meshList,actorList,true);

	return num;
}

//.............................................................................
