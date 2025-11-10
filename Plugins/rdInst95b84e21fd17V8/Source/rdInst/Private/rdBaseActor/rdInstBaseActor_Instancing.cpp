//
// rdInstBaseActor_Instancing.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 24th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#if WITH_EDITOR
#include "EditorFramework/AssetImportData.h"
#endif
#include "rdActor.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "rdSpawnActor.h"
#include "rdInstSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

//.............................................................................
//  rdAddInstance
//
// Adds an Instance of the specified UStaticMesh with the specified transform
// This gets called by ArdActor, don't call yourself unless from C++
//.............................................................................
int32 ArdInstBaseActor::rdAddInstance(const AActor* instOwner,UStaticMesh* mesh,const FTransform& transform) {
	return rdAddInstanceX(instOwner,rdGetSMsid(mesh),transform);
}
int32 ArdInstBaseActor::rdAddInstanceX(const AActor* instOwner,const FName sid,const FTransform& transform) {

	if(sid.IsNone()) return -1;

	UInstancedStaticMeshComponent* ismc=nullptr;
	if(bUseHISMs) {
		ismc=rdGetHInstanceGenX(sid,true);
	} else {
		ismc=rdGetInstanceGenX(sid,true);
	}
	if(!ismc) {
		instList.Remove(sid);
		freeInstanceMap.Remove(sid);
		return -1;
	}

	FrdInstList* il=instList.Find(sid);
	if(!il) {
		il=&instList.Add(sid,FrdInstList(sid,(UStaticMesh*)ismc->GetStaticMesh()));
	}

	int32 ind=-1;
	TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
	if(reuse && reuse->Num()>0) {
		int32 nm=reuse->Num()-1;
		ind=(*reuse)[nm];
		reuse->RemoveAt(nm);
#if WITH_EDITOR
		dontDoModify++;
#endif
		ismc->UpdateInstanceTransform(ind,transform,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
		dontDoModify--;
#endif
	} else {
#if ENGINE_MAJOR_VERSION>4
		ind=ismc->AddInstance(transform,bWorldSpaceInstancing);
#else
		ind=ismc->AddInstance(transform);
#endif
	}
	if(instOwner) {
		il->instItems.Add(new FrdInstItem(instOwner,ind));
	}
	return ind;
}

//.............................................................................
//  rdAddInstancesX
//
// Adds the Instances of the specified UStaticMesh with the specified transforms
//.............................................................................
void ArdInstBaseActor::rdAddInstancesX(const FName sid,const TArray<FTransform>& transforms) {

	UInstancedStaticMeshComponent* ismc=nullptr;
	if(bUseHISMs) {
		ismc=rdGetHInstanceGenX(sid,true);
	} else {
		ismc=rdGetInstanceGenX(sid,true);
	}
	if(!ismc) {
		return;
	}

	TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
	if(reuse && reuse->Num()>0) {
#if WITH_EDITOR
		dontDoModify++;
#endif
		for(auto& t:transforms) {
			int32 nm=reuse->Num()-1;
			if(nm>=0) {
				int32 ind=(*reuse)[nm];
				reuse->RemoveAt(nm);
				ismc->UpdateInstanceTransform(ind,t,bWorldSpaceInstancing,false,true);
			} else {
#if ENGINE_MAJOR_VERSION>4
				ismc->AddInstance(t,bWorldSpaceInstancing);
#else
				ismc->AddInstance(t);
#endif
			}
		}
#if WITH_EDITOR
		dontDoModify--;
#endif
	} else {
#if ENGINE_MAJOR_VERSION>4
		for(auto& t:transforms) ismc->AddInstance(t,bWorldSpaceInstancing);
#else
		for(auto& t:transforms) ismc->AddInstance(t);
#endif
	}
}

//.............................................................................
//  rdAddOwnedInstance
//
// Used in Conjunction with the above method - accepts the HISMC rather than the Static Mesh
// so you can avoid Map Lookups.
//.............................................................................
int32 ArdInstBaseActor::rdAddOwnedInstance(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,const FTransform& itransform) {

	if(!instOwner || !instGen) return -1;

	FName sid=rdGetSMCsid(instGen);
	FrdInstList* il=instList.Find(sid);
	if(!il) {
		return -1;
	}

	FTransform transform(itransform);
	transform.SetTranslation(GetActorRotation().RotateVector(transform.GetTranslation()));
	transform.Accumulate(GetActorTransform());

	int32 ind=-1;
	TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
	if(reuse && reuse->Num()>0) {
		int32 nm=reuse->Num()-1;
		ind=(*reuse)[nm];
		reuse->RemoveAt(nm);
#if WITH_EDITOR
		dontDoModify++;
#endif
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
		instGen->UpdateInstanceTransform(ind,transform,bWorldSpaceInstancing,false,true);
#else
		instGen->UpdateInstanceTransform(ind,transform,bWorldSpaceInstancing,true,true);
#endif
#if WITH_EDITOR
		dontDoModify--;
#endif
	} else {
#if ENGINE_MAJOR_VERSION>4
		ind=instGen->AddInstance(transform,bWorldSpaceInstancing);
#else
		ind=instGen->AddInstance(transform);
#endif
	}

	ArdActor* rdActor=Cast<ArdActor>((AActor*)instOwner);
	if(rdActor && rdActor->positionInfo.customData.Num()>0 && instGen->NumCustomDataFloats>=rdActor->positionInfo.customData.Num()) {
		instGen->SetCustomData(ind,rdActor->positionInfo.customData);
	}

	il->instItems.Add(new FrdInstItem(instOwner,ind));
	return ind;
}

//.............................................................................
//  rdAddOwnedInstances
//.............................................................................
int32 ArdInstBaseActor::rdAddOwnedInstances(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms) {

	if(!instOwner || !instGen) return -1;
	UStaticMesh* mesh=instGen->GetStaticMesh();
	FName sid=rdGetSMCsid(instGen);
	FrdInstList* il=instList.Find(sid);
	if(!il) {
		return -1;
	}

	TArray<int32> inds;
	int32 ind=-1;
	TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
	if(reuse && reuse->Num()>0) {
		for(auto& t:transforms) {
			int32 nm=reuse->Num()-1;
			if(nm>=0) {
				ind=(*reuse)[nm];
				reuse->RemoveAt(nm);
#if WITH_EDITOR
				dontDoModify++;
#endif
				instGen->UpdateInstanceTransform(ind,t,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
				dontDoModify--;
#endif
			} else {
#if ENGINE_MAJOR_VERSION>4
				ind=instGen->AddInstance(t,bWorldSpaceInstancing);
#else
				ind=instGen->AddInstance(t);
#endif
			}
			inds.Add(ind);
		}
	} else {
#if ENGINE_MAJOR_VERSION>4
		for(auto& t:transforms) inds.Add(instGen->AddInstance(t,bWorldSpaceInstancing));
#else
		for(auto& t:transforms) inds.Add(instGen->AddInstance(t));
#endif
	}

	ArdActor* rdActor=Cast<ArdActor>((AActor*)instOwner);
	if(rdActor && rdActor->positionInfo.customData.Num()>0 && instGen->NumCustomDataFloats>=rdActor->positionInfo.customData.Num()) {
		for(auto i:inds) {
			instGen->SetCustomData(i,rdActor->positionInfo.customData);
		}
	}

	for(int32 i:inds) {
		il->instItems.Add(new FrdInstItem(instOwner,i));
	}

	return ind;
}

//.............................................................................
//  rdAddInstanceFast
//
// Used in Conjunction with the above method - accepts the ISMC/HISMC rather than the Static Mesh
// so you can avoid Map Lookups.
//.............................................................................
int32 ArdInstBaseActor::rdAddInstanceFast(UInstancedStaticMeshComponent* instGen,const FTransform& transform) {

	int32 ind=-1;
	if(instGen) {

		UStaticMesh* mesh=instGen->GetStaticMesh();
		FName sid=rdGetSMCsid(instGen);
		TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
		if(reuse && reuse->Num()>0) {
			int32 nm=reuse->Num()-1;
			ind=(*reuse)[nm];
			reuse->RemoveAt(nm);
#if WITH_EDITOR
			dontDoModify++;
#endif
			instGen->UpdateInstanceTransform(ind,transform,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
			dontDoModify--;
#endif
		} else {
#if ENGINE_MAJOR_VERSION>4
			ind=instGen->AddInstance(transform,bWorldSpaceInstancing);
#else
			ind=instGen->AddInstance(transform);
#endif
		}
	}
	return ind;
}

//.............................................................................
//  rdAddInstanceFast
//.............................................................................
void ArdInstBaseActor::rdAddInstancesFast(UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms) {

	if(instGen) {
		UStaticMesh* mesh=instGen->GetStaticMesh();
		FName sid=rdGetSMCsid(instGen);
		TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
		if(reuse && reuse->Num()>0) {
#if WITH_EDITOR
			dontDoModify++;
#endif
			for(auto& t:transforms) {
				int32 nm=reuse->Num()-1;
				if(nm>=0) {
					int32 ind=(*reuse)[nm];
					reuse->RemoveAt(nm);
					instGen->UpdateInstanceTransform(ind,t,bWorldSpaceInstancing,false,true);
				} else {
#if ENGINE_MAJOR_VERSION>4
					instGen->AddInstance(t,bWorldSpaceInstancing);
#else
					instGen->AddInstance(t);
#endif
				}
			}
#if WITH_EDITOR
			dontDoModify--;
#endif
		} else {
#if ENGINE_MAJOR_VERSION>4
			for(auto& t:transforms) instGen->AddInstance(t,bWorldSpaceInstancing);
#else
			for(auto& t:transforms) instGen->AddInstance(t);
#endif
		}
	}
}

//.............................................................................
//  rdAddRemoveInstancesFast
//.............................................................................
void ArdInstBaseActor::rdAddRemoveInstancesFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) const TArray<int32>& delIndices,UPARAM(ref) const TArray<FTransform>& transforms) {

	int32 remInd=delIndices.Num()-1;
	for(auto& t:transforms) {
		if(remInd>=0) {
			rdUpdateTransformFast(ismc,delIndices[remInd],t);
			remInd--;
		} else {
			rdAddInstanceFast(ismc,t);
		}
	}
	for(int32 i=0;i<remInd;i++) {
		rdRemoveInstanceFast(ismc,delIndices[i]);
	}
}

//.............................................................................
//  rdAddInstanceFastWithIndexes
//.............................................................................
void ArdInstBaseActor::rdAddInstancesFastWithIndexes(UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms,TArray<int32>& indexList) {

	int32 ind=-1;
	if(instGen) {
		UStaticMesh* mesh=instGen->GetStaticMesh();
		FName sid=rdGetSMCsid(instGen);
		TArray<FTransform> newInstTransforms;
		TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
		if(reuse && reuse->Num()>0) {

#if WITH_EDITOR
			dontDoModify++;
#endif
			for(auto& t:transforms) {
				int32 nm=reuse->Num()-1;
				if(nm>=0) {
					ind=(*reuse)[nm];
					reuse->RemoveAt(nm);
					instGen->UpdateInstanceTransform(ind,t,bWorldSpaceInstancing,false,true);
					indexList.Add(ind);
				} else {
					newInstTransforms.Add(t);
				}
			}
#if WITH_EDITOR
			dontDoModify--;
#endif
		} else {
			newInstTransforms.Append(transforms);
		}

		if(newInstTransforms.Num()>0) {
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
			TArray<int32> indexes=instGen->AddInstances(newInstTransforms,true,bWorldSpaceInstancing,true);
#else
			TArray<int32> indexes=instGen->AddInstances(newInstTransforms,bWorldSpaceInstancing);
#endif
			indexList.Append(MoveTemp(indexes));
		}

//#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
//		instGen->InstanceUpdateCmdBuffer.NumEdits++;
//#endif
//		instGen->MarkRenderStateDirty();
	}
}

//.............................................................................
//  rdAddInstanceArrayFastWithIndexes
//.............................................................................
void ArdInstBaseActor::rdAddInstanceArrayFastWithIndexes(UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bso) {

	if(instGen) {
		UStaticMesh* mesh=instGen->GetStaticMesh();
		FName sid=rdGetSMCsid(instGen);

		TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
		bool worldSpace=bWorldSpaceInstancing;
		if(reuse && reuse->Num()>0) {

#if WITH_EDITOR
			dontDoModify++;
#endif
			bso.transformMap.ForEach([&reuse,&instGen,&worldSpace](FrdInstanceTransformItem& t) {
				int32 nm=reuse->Num()-1;
				if(nm>=0) {
					t.index=(*reuse)[nm];
					reuse->RemoveAt(nm);
					instGen->UpdateInstanceTransform(t.index,t.transform,worldSpace,false,true);
				} else {
#if ENGINE_MAJOR_VERSION>4
					t.index=instGen->AddInstance(t.transform,worldSpace);
#else
					t.index=instGen->AddInstance(t.transform);
#endif
				}
			});
#if WITH_EDITOR
			dontDoModify--;
#endif
		} else {
			bso.transformMap.ForEachCell([instGen,&worldSpace](TArray<FrdInstanceTransformItem>& t,int32 cell) {
				for(auto& ti:t) {
#if ENGINE_MAJOR_VERSION>4
					ti.index=instGen->AddInstance(ti.transform,worldSpace);
#else
					ti.index=instGen->AddInstance(ti.transform);
#endif
				}
			});
		}

//#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
//		instGen->InstanceUpdateCmdBuffer.NumEdits++;
//#endif
//		instGen->MarkRenderStateDirty();
	}
}

//.............................................................................
//  rdAddInstancArrayFastWithIndexesRel
//.............................................................................
void ArdInstBaseActor::rdAddInstanceArrayFastWithIndexesRel(const FTransform& rt,UInstancedStaticMeshComponent* instGen,FrdBakedSpawnObjects& bso) {

	int32 ind=-1;
	if(instGen) {
		UStaticMesh* mesh=instGen->GetStaticMesh();
		FName sid=rdGetSMCsid(instGen);

		TArray<int32>* reuse=bRecycleInstances?freeInstanceMap.Find(sid):nullptr;
		bool worldSpace=bWorldSpaceInstancing;
		if(reuse && reuse->Num()>0) {

#if WITH_EDITOR
			dontDoModify++;
#endif
			bso.transformMap.ForEach([this,&rt,&ind,&reuse,&instGen,&worldSpace](FrdInstanceTransformItem& tt) {

				FTransform t=rdRelativeToWorldNoScale(tt.transform,rt);

				int32 nm=reuse->Num()-1;
				if(nm>=0) {
					ind=(*reuse)[nm];
					reuse->RemoveAt(nm);
					instGen->UpdateInstanceTransform(ind,t,worldSpace,false,true);
					tt.index=ind;
				} else {
#if ENGINE_MAJOR_VERSION>4
					tt.index=instGen->AddInstance(t,worldSpace);
#else
					tt.index=instGen->AddInstance(t);
#endif
				}
			});
#if WITH_EDITOR
			dontDoModify--;
#endif
		} else {
			bso.transformMap.ForEachCell([&rt,this,&worldSpace,instGen](TArray<FrdInstanceTransformItem>& tl,int32 cell) {
				for(auto& t:tl) {
#if ENGINE_MAJOR_VERSION>4
					t.index=instGen->AddInstance(rdRelativeToWorld(t.transform,rt),worldSpace);
#else
					t.index=instGen->AddInstance(rdRelativeToWorld(t.transform,rt));
#endif
				}
			});
		}

//#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
//		instGen->InstanceUpdateCmdBuffer.NumEdits++;
//#endif
		//instGen->MarkRenderStateDirty();
		//instGen->MarkRenderInstancesDirty();

		//void UActorComponent::MarkRenderStateDirty()
		//void UActorComponent::MarkRenderTransformDirty()
		//void UActorComponent::MarkRenderDynamicDataDirty()
		//void UActorComponent::MarkForNeededEndOfFrameUpdate()
	}
}

//.............................................................................
//  rdAddInstanceRaw
//
// Accepts the ISMC/HISMC rather than the Static Mesh
// Calls the Super AddInstance without any processing
//.............................................................................
int32 ArdInstBaseActor::rdAddInstanceRaw(UInstancedStaticMeshComponent* instGen,const FTransform& transform) {

	if(instGen) {
#if ENGINE_MAJOR_VERSION>4
		return instGen->AddInstance(transform,bWorldSpaceInstancing);
#else
		return instGen->AddInstance(transform);
#endif
	}
	return -1;
}

//.............................................................................
//  rdAddInstanceRaw
//
// Accepts the ISMC/HISMC rather than the Static Mesh
// Calls the Super AddInstance without any processing
//.............................................................................
int32 ArdInstBaseActor::rdAddInstancesRaw(UInstancedStaticMeshComponent* instGen,const TArray<FTransform>& transforms) {

	if(instGen) {

#if ENGINE_MAJOR_VERSION>4
		TArray<int32> inds=instGen->AddInstances(transforms,true,bWorldSpaceInstancing); // bool bShouldReturnIndices, bool bWorldSpace
#else
#if ENGINE_MINOR_VERSION>25
		TArray<int32> inds=instGen->AddInstances(transforms,true);
#else
		TArray<int32> inds;
		for(auto& t:transforms) inds.Add(instGen->AddInstance(t,bWorldSpaceInstancing));
#endif
#endif
		return (inds.Num()>0)?inds[0]:-1;
	}
	return -1;
}

//.............................................................................
//  rdAddInstanceRawWithIndexes
//
// Accepts the ISMC/HISMC rather than the Static Mesh
// Calls the Super AddInstance without any processing
//.............................................................................
int32 ArdInstBaseActor::rdAddInstancesRawWithIndexes(UInstancedStaticMeshComponent* instGen,UPARAM(ref) const TArray<FTransform>& transforms,TArray<int32>& indexList) {

	if(instGen) {

#if ENGINE_MAJOR_VERSION>4
		TArray<int32> inds=instGen->AddInstances(transforms,true,bWorldSpaceInstancing); // bool bShouldReturnIndices, bool bWorldSpace

#else
#if ENGINE_MINOR_VERSION>25
		TArray<int32> inds=instGen->AddInstances(transforms,true);
#else
		TArray<int32> inds;
		for(auto& t:transforms) inds.Add(instGen->AddInstance(t,bWorldSpaceInstancing));
#endif
#endif
		if(inds.Num()<=0) return -1;
		int32 index=inds[0];
		indexList.Append(MoveTemp(inds));
		return index;
	}

	return -1;
}



//.............................................................................
// rdRemoveInstance
//
// Removes the Instance referred to by index for the specified StaticMesh
//
//.............................................................................
void ArdInstBaseActor::rdRemoveInstance(UStaticMesh* mesh,int32 index) {
	rdRemoveInstanceX(rdGetSMsid(mesh),index);
}
void ArdInstBaseActor::rdRemoveInstanceX(const FName sid,int32 index) {

	FrdInstList* il=instList.Find(sid);
	if(!il) return;
	TArray<FrdInstItem*>& items=il->instItems;
	for(int32 ii=items.Num()-1;ii>=0;ii--) {
		FrdInstItem* i=items[ii];
		if(i->index==index) {

			UInstancedStaticMeshComponent* ismc=FindISMCforMesh(sid);
			if(index<ismc->GetInstanceCount()) {

				if(bRecycleInstances) {

					if(rdSetInstanceVisibilityFast(ismc,index,false)) {
						TArray<int32>& indexes=freeInstanceMap.FindOrAdd(sid);
						indexes.Add(index);
						items.Remove(i);
					}

				} else {

					UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
					if(hismc || ismc->bSupportRemoveAtSwap) {
#else
					if(hismc) {
#endif

						ismc->RemoveInstance(index); // Swaps the last item in the list with the deleted index

						if(ii!=items.Num()-1) {

							int32 ind=items[ii]->index;
							items.RemoveAtSwap(ii);
							int32 newIndex=items[ii]->index;
							items[ii]->index=ind;
							AActor* swapOwner=(AActor*)items[ii]->owner;

							if(swapOwner) {
								ArdActor* rdActor=(ArdActor*)Cast<ArdActor>(swapOwner);
								if(rdActor) {
									FrdInstanceSettingsArray* isa=rdActor->InstanceDataX.Find(sid);
									if(isa) {
										for(FrdInstanceFastSettings& j:isa->settingsFast) {
											if(j.index==newIndex) j.index=ind;
										}
										for(FrdInstanceRandomSettings& j:isa->settingsRandom) {
											if(j.index==newIndex) j.index=ind;
										}
									}
								}
							}
						} else {
							items.RemoveAt(ii);
						}

					} else if(ismc) {
						ismc->RemoveInstance(index);
					}
				}
			}
			return;
		}
	}
}

//.............................................................................
// rdRemoveOwnedInstance
//
// Removes the specified Instance Index belonging to the specified HISMC
//
//.............................................................................
void ArdInstBaseActor::rdRemoveOwnedInstance(const AActor* instOwner,UInstancedStaticMeshComponent* instGen,int32 index) {

	if(!instGen) return;

	UStaticMesh* mesh=instGen->GetStaticMesh();
	FName sid=rdGetSMCsid(instGen);
	TArray<FrdInstItem*>& items=instList[sid].instItems;
	for(int32 ii=items.Num()-1;ii>=0;ii--) {
		FrdInstItem* i=items[ii];
		if(i->index==index) {

			if(index<instGen->GetInstanceCount()) {

				if(bRecycleInstances) {

					if(rdSetInstanceVisibilityFast(instGen,index,false)) {
						TArray<int32>& indexes=freeInstanceMap.FindOrAdd(sid);
						indexes.Add(index);
						items.Remove(i);
					}

				} else {

					UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(instGen);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
					if(hismc || instGen->bSupportRemoveAtSwap) {
#else
					if(hismc) {
#endif

						instGen->RemoveInstance(index); // Swaps the last item in the list with the deleted index

						if(ii!=items.Num()-1) {

							int32 ind=items[ii]->index;
							items.RemoveAtSwap(ii);
							int32 newIndex=items[ii]->index;
							items[ii]->index=ind;
							AActor* swapOwner=(AActor*)items[ii]->owner;

							if(swapOwner) {
								ArdActor* rdActor=(ArdActor*)Cast<ArdActor>(swapOwner);
								if(rdActor) {
									FrdInstanceSettingsArray* isa=rdActor->InstanceDataX.Find(sid);
									if(isa) {
										for(FrdInstanceFastSettings& j:isa->settingsFast) {
											if(j.index==newIndex) j.index=ind;
										}
										for(FrdInstanceRandomSettings& j:isa->settingsRandom) {
											if(j.index==newIndex) j.index=ind;
										}
									}
								}
							}
						} else {
							items.RemoveAt(ii);
						}
					} else {

						if(instGen) instGen->RemoveInstance(index);
						items.RemoveAt(ii);
					}
				}
			}
			return;
		}
	}
}

//.............................................................................
// rdRemoveInstanceFast
//
// Removes the specified Instance Index belonging to the specified HISMC
//.............................................................................
void ArdInstBaseActor::rdRemoveInstanceFast(UInstancedStaticMeshComponent* instGen,int32 index) {

	if(!instGen) return;
	if(index<instGen->GetInstanceCount()) {
		if(bRecycleInstances) {
			FTransform tran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));
#if WITH_EDITOR
			dontDoModify++;
#endif
			instGen->UpdateInstanceTransform(index,tran,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
			dontDoModify--;
#endif
			UStaticMesh* mesh=instGen->GetStaticMesh();
			FName sid=rdGetSMCsid(instGen);
			TArray<int32>& indexes=freeInstanceMap.FindOrAdd(sid);
			indexes.Add(index);

		} else {

			instGen->RemoveInstance(index);
		}
	}
}

//.............................................................................
// rdRemoveInstancesFast
//
// Removes the specified Instances in the Array of Indexes belonging to the specified HISMC
//.............................................................................
void ArdInstBaseActor::rdRemoveInstancesFast(UInstancedStaticMeshComponent* instGen,const TArray<int32>& indexes) {

	if(!instGen) return;
	FName sid=rdGetSMCsid(instGen);
	TArray<int32>& indexes2=freeInstanceMap.FindOrAdd(sid);
	FTransform tran(FRotator(0,0,0),FVector(0,0,0),FVector(0,0,0));

	for(auto index:indexes) {
		if(index<instGen->GetInstanceCount()) {

			if(bRecycleInstances) {

#if WITH_EDITOR
				dontDoModify++;
#endif
				instGen->UpdateInstanceTransform(index,tran,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
				dontDoModify--;
#endif
				indexes2.Add(index);

			} else {

				instGen->RemoveInstance(index);
			}
		}
	}
}

//.............................................................................
// rdRemoveInstanceRaw
//
// Removes the specified Instance Index belonging to the specified HISMC
//.............................................................................
void ArdInstBaseActor::rdRemoveInstanceRaw(UInstancedStaticMeshComponent* instGen,int32 index) {

	instGen->RemoveInstance(index);
}

void ArdInstBaseActor::rdRemoveInstancesX(const FName sid,UPARAM(ref) const TArray<int32>& indexes) {
	rdRemoveInstancesFast(FindISMCforMesh(sid),indexes);
}

//.............................................................................
// rdRemoveInstancesRaw
//
// Removes the specified Instances in the Array of Indexes belonging to the specified HISMC
//.............................................................................
void ArdInstBaseActor::rdRemoveInstancesRaw(UInstancedStaticMeshComponent* instGen,const TArray<int32>& indexes) {

#if ENGINE_MAJOR_VERSION>4
	instGen->RemoveInstances(indexes);
#else
	for(auto i:indexes) {
		instGen->RemoveInstance(i);
	}
#endif
}

//.............................................................................
//  rdRemInstancesForOwner
//.............................................................................
void ArdInstBaseActor::rdRemInstancesForOwner(const AActor* instOwner) {

	if(instList.Num()<=0) {
		rdRemAllHISMCs();
	}

	if(!instOwner) return;

	for(auto& i:instList) {

		FrdInstList& il=i.Value;
		TArray<FrdInstItem*>& items=il.instItems;
		FName sid=il.sid;
		UInstancedStaticMeshComponent* ismc=FindISMCforMesh(sid);
		
		for(int32 ii=items.Num()-1;ii>=0;ii--) {
			FrdInstItem* itm=items[ii];

			if(itm && itm->owner && (!IsValid(itm->owner) || itm->owner==instOwner)) { // remove old actors at the same time

				if(ismc && itm->index<ismc->GetInstanceCount()) {

					if(bRecycleInstances) {

						if(rdSetInstanceVisibilityFast(ismc,itm->index,false)) {
							TArray<int32>& indexes=freeInstanceMap.FindOrAdd(sid);
							indexes.Add(itm->index);
						}
						items.Remove(itm);

					} else {

						UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
						if(hismc || ismc->bSupportRemoveAtSwap) {
#else
						if(hismc) {
#endif

							ismc->RemoveInstance(itm->index); // Swaps the last item in the list with the deleted index

							if(ii!=items.Num()-1) {
								int32 index=items[ii]->index;
								items.RemoveAtSwap(ii);
								items[ii]->index=index;
							} else {
								items.RemoveAt(ii);
							}
						} else if(ismc) {

							ismc->RemoveInstance(itm->index); // Just removes and shifts the rest of the indexes down
							items.RemoveAt(ii);
						}
					}
				}
			}
		}
	}
}

//.............................................................................
//  rdRemAllInstances
//.............................................................................
void ArdInstBaseActor::rdRemAllInstances() {

	rdRemoveAllProxies();

	if(instList.Num()>0) {

		for(auto& i:instList) {
			if(i.Value.instItems.Num()>0) {
				UInstancedStaticMeshComponent* ismc=FindISMCforMesh(i.Value.sid);
				if(ismc) {
					ismc->ClearInstances();
				}
				i.Value.instItems.Empty();
			}
		}
		instList.Empty();
	}
	freeInstanceMap.Empty();
	rdRemAllHISMCs();
}

//.............................................................................
//  rdRemAllHISMCs
//.............................................................................
void ArdInstBaseActor::rdRemAllHISMCs() {

	TArray<UActorComponent*> list;
	GetComponents(UInstancedStaticMeshComponent::StaticClass(),list);
	for(auto comp:list) {
		UInstancedStaticMeshComponent* ismc=(UInstancedStaticMeshComponent*)comp;
		ismc->ClearInstances();
		ismc->DestroyComponent();
	}
	freeInstanceMap.Empty();
}







//.............................................................................
// rdGetTransformPtrFast
//.............................................................................
TArray<FMatrix>& ArdInstBaseActor::rdGetTransformsPtrFast(UInstancedStaticMeshComponent* ismc,int32& numTransforms) {

	static TArray<FMatrix> empty;
	numTransforms=0;
	if(ismc) {

		numTransforms=ismc->PerInstanceSMData.Num();
		return (TArray<FMatrix>&)ismc->PerInstanceSMData;
	}

	return empty;
}

//.............................................................................
// rdGetTransformPtr
//.............................................................................
TArray<FMatrix>& ArdInstBaseActor::rdGetTransformsPtr(UStaticMesh* mesh,int32& numTransforms) {
	return rdGetTransformsPtrX(rdGetSMsid(mesh),numTransforms);
}
TArray<FMatrix>& ArdInstBaseActor::rdGetTransformsPtrX(const FName sid,int32& numTransforms) {

	static TArray<FMatrix> empty;
	numTransforms=0;

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		numTransforms=ismc->PerInstanceSMData.Num();
		return (TArray<FMatrix>&)ismc->PerInstanceSMData;
	}

	return empty;
}

//.............................................................................
// rdUpdateTransformFast
//.............................................................................
void ArdInstBaseActor::rdUpdateTransformFast(UInstancedStaticMeshComponent* ismc,int32 index,const FTransform& transform) {

	if(ismc && index>=0 && index<ismc->GetInstanceCount()) {
#if WITH_EDITOR
		dontDoModify++;
#endif
		ismc->UpdateInstanceTransform(index,transform,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
		dontDoModify--;
#endif
		/*
		TArray<FTransform> transforms={transform};
		FTransform prevTransform;
		ismc->GetInstanceTransform(index,prevTransform,bWorldSpaceInstancing);
		TArray<FTransform> prevTransforms={prevTransform};
		ismc->BatchUpdateInstancesTransforms(index,transforms,prevTransforms,bWorldSpaceInstancing,true,true);
		*/
	}
}

//.............................................................................
// rdUpdateTransform
//.............................................................................
void ArdInstBaseActor::rdUpdateTransform(UStaticMesh* mesh,int32 index,const FTransform& transform) {
	rdUpdateTransformX(rdGetSMsid(mesh),index,transform);
}
void ArdInstBaseActor::rdUpdateTransformX(const FName sid,int32 index,const FTransform& transform) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc && index>=0 && index<ismc->GetInstanceCount()) {
#if WITH_EDITOR
		dontDoModify++;
#endif
		ismc->UpdateInstanceTransform(index,transform,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
		dontDoModify--;
#endif
		/*
		TArray<FTransform> transforms={transform};
		FTransform prevTransform;
		ismc->GetInstanceTransform(index,prevTransform,bWorldSpaceInstancing);
		TArray<FTransform> prevTransforms={prevTransform};
		ismc->BatchUpdateInstancesTransforms(index,transforms,prevTransforms,bWorldSpaceInstancing,true,true);
		*/
	}
}

//.............................................................................
// rdUpdateTransformsFast
//.............................................................................
void ArdInstBaseActor::rdUpdateTransformsFast(UInstancedStaticMeshComponent* imsc,int32 startIndex,const TArray<FTransform>& transforms,int32 arrayoffset) {

	if(imsc) {

		TArray<FTransform>& t2=(TArray<FTransform>&)transforms[arrayoffset];

		imsc->BatchUpdateInstancesTransforms(startIndex,transforms,true,true,true);
	}
}

//.............................................................................
// rdIncrementTransformsFast
//.............................................................................
void ArdInstBaseActor::rdIncrementTransformsFast(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indexes,const FTransform& transform) {

	if(ismc) {
		//ismc->Modify();
		int32 numIndexes=indexes.Num();
		for(int32 i=0;i<numIndexes;i++) {
			const int32 index=indexes[i];

			FInstancedStaticMeshInstanceData& iData=ismc->PerInstanceSMData[index];
			FMatrix iMat=iData.Transform;
			FTransform t;
			t.SetFromMatrix(iMat);
			t.Accumulate(transform);
			iData.Transform=t.ToMatrixWithScale();
		}

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
		ismc->InstanceUpdateCmdBuffer.NumEdits++;
		//ismc->MarkRenderStateDirty();
#endif
	}
}

//.............................................................................
// rdIncrementAllTransformsFast
//.............................................................................
void ArdInstBaseActor::rdIncrementAllTransformsFast(UInstancedStaticMeshComponent* ismc,const FTransform& transform) {

	if(ismc) {
		//ismc->Modify();
		int32 numIndexes=ismc->GetInstanceCount();
		for(int32 i=0;i<numIndexes;i++) {

			FInstancedStaticMeshInstanceData& iData=ismc->PerInstanceSMData[i];
			FMatrix iMat=iData.Transform;
			FTransform t;
			t.SetFromMatrix(iMat);
			FVector loc=t.GetTranslation();
			if(loc.Z>=-400000.0f) { // is visible
				t.Accumulate(transform);
				iData.Transform=t.ToMatrixWithScale();
			}
		}

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
		ismc->InstanceUpdateCmdBuffer.NumEdits++;
		//ismc->MarkRenderStateDirty();
#endif
	}
}

//.............................................................................
// rdUpdateTransforms
//.............................................................................
void ArdInstBaseActor::rdUpdateTransforms(UStaticMesh* mesh,int32 startIndex,const TArray<FTransform>& transforms) {
	rdUpdateTransformsX(rdGetSMsid(mesh),startIndex,transforms);
}
void ArdInstBaseActor::rdUpdateTransformsX(const FName sid,int32 startIndex,const TArray<FTransform>& transforms) {

	UInstancedStaticMeshComponent* imsc=rdGetInstanceGenX(sid);
	if(imsc) {
		imsc->BatchUpdateInstancesTransforms(startIndex,transforms,true,true,true);
	}
}

//.............................................................................
// rdUpdateTransformArrayX
//.............................................................................
void ArdInstBaseActor::rdUpdateTransformArrayX(const FName sid,const TArray<int32> indices,const TArray<FTransform>& transforms) {
	
	if(indices.Num()!=transforms.Num()) {
		return;
	}

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		int32 ti=0;
#if WITH_EDITOR
		dontDoModify++;
#endif
		for(auto& i:indices) {
			ismc->UpdateInstanceTransform(i,transforms[ti++],bWorldSpaceInstancing,false,true);
		}
#if WITH_EDITOR
		dontDoModify--;
#endif
#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
		ismc->InstanceUpdateCmdBuffer.NumEdits++;
		//ismc->MarkRenderStateDirty();
#endif
	}
}

//.............................................................................
// rdUpdateTransformTupleX
//.............................................................................
void ArdInstBaseActor::rdUpdateTransformTuplesX(const FName sid,TArray<TTuple<int32,FTransform>>& transforms) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
#if WITH_EDITOR
		dontDoModify++;
#endif
		for(auto& i:transforms) {
			ismc->UpdateInstanceTransform(i.Key,i.Value,bWorldSpaceInstancing,false,true);
		}
#if WITH_EDITOR
		dontDoModify--;
#endif
#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
		ismc->InstanceUpdateCmdBuffer.NumEdits++;
		//ismc->MarkRenderStateDirty();
#endif
	}
}

//.............................................................................
// rdIncrementTransforms
//.............................................................................
void ArdInstBaseActor::rdIncrementTransforms(UStaticMesh* mesh,const TArray<int32>& indexes,const FTransform& transform) {
	rdIncrementTransformsX(rdGetSMsid(mesh),indexes,transform);
}
void ArdInstBaseActor::rdIncrementTransformsX(const FName sid,const TArray<int32>& indexes,const FTransform& transform) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		//ismc->Modify();
		int32 numIndexes=indexes.Num();
		for(int32 i=0;i<numIndexes;i++) {
			const int32 index=indexes[i];

			FInstancedStaticMeshInstanceData& iData=ismc->PerInstanceSMData[index];
			FMatrix iMat=iData.Transform;
			FTransform t;
			t.SetFromMatrix(iMat);
			t.Accumulate(transform);
			iData.Transform=t.ToMatrixWithScale();
		}

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
		ismc->InstanceUpdateCmdBuffer.NumEdits++;
		//ismc->MarkRenderStateDirty();
#endif
	}
}

//.............................................................................
//  rdChangeInstanceTransformsForOwner
//.............................................................................
void ArdInstBaseActor::rdChangeInstanceTransformsForOwner(const AActor* instOwner,const FTransform& transform,bool worldSpace,bool markDirty,bool teleport) {

#if WITH_EDITOR
	dontDoModify++;
#endif
	for(auto& i:instList) {
		TArray<FrdInstItem*>& items=i.Value.instItems;
		for(int32 ii=items.Num()-1;ii>=0;ii--) {
			FrdInstItem* itm=items[ii];
			if(itm && itm->owner==instOwner) {
				UInstancedStaticMeshComponent* ismc=FindISMCforMesh(i.Value.sid);
				if(ismc) ismc->UpdateInstanceTransform(itm->index,transform,worldSpace,markDirty,teleport);
			}
		}
	}
#if WITH_EDITOR
	dontDoModify--;
#endif
}

//.............................................................................
//  rdGetLastInstanceIndex
//.............................................................................
int32 ArdInstBaseActor::rdGetLastInstanceIndex(const AActor* instOwner,UStaticMesh* mesh) {
	return rdGetLastInstanceIndexX(instOwner,rdGetSMsid(mesh));
}
int32 ArdInstBaseActor::rdGetLastInstanceIndexX(const AActor* instOwner,const FName sid) {

	FrdInstList* il=instList.Find(sid);
	if(!il || il->instItems.Num()<1) return -1;
	
	int32 ind=il->instItems.Num()-1;
	while(ind>=0 && il->instItems[ind]->owner!=instOwner) ind--;
	
	if(ind<0) return -1;

	return il->instItems[ind]->index;
}

//.............................................................................
//  rdGetInstanceGen
//
// Returns the ISMC that is used to create Instanced for the specified UStaticMesh*
// This is the more optimal way to add Instances, add all instances for a specified mesh
// from the returned component. That way you only do one Map lookup per StaticMesh.
//.............................................................................
UInstancedStaticMeshComponent* ArdInstBaseActor::rdGetInstanceGen(const UStaticMesh* mesh,bool create) {
	return rdGetInstanceGenX(rdGetSMsid((UStaticMesh*)mesh));
}

UInstancedStaticMeshComponent* ArdInstBaseActor::rdGetInstanceGenX(const FName sid,bool create) {

	if(sid.IsNone()) return nullptr;

	int32 ver=0;
	TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn;
	TSoftObjectPtr<UStaticMesh> mesh=nullptr;
	TArray<TSoftObjectPtr<UMaterialInterface>> materials;
	bool bReverseCulling=false;
	TEnumAsByte<ErdCollision> collision;
	float startCull,endCull;
	int32 id;
	FName groupID;
	rdGetSidDetails(sid,ver,type,mesh,materials,bReverseCulling,collision,startCull,endCull,id,groupID);
	if(!mesh) {
		return nullptr;
	}

	UInstancedStaticMeshComponent* ismc=FindISMCforMesh(sid);
	if(ismc) {
		return ismc;
	}

	freeInstanceMap.Remove(sid);
	if(!create) return nullptr;

	TArray<UMaterialInterface*> mats;
	for(auto m:materials) {
		mats.Add(m.Get());
	}


#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	if(!GetRootComponent()) {
		USceneComponent* root=(USceneComponent*)AddComponentByClass(USceneComponent::StaticClass(),false,FTransform(),false);
		//root->SetMobility(EComponentMobility::Static);
	}
	if(bUseHISMs) {
		ismc=(UInstancedStaticMeshComponent*)AddComponentByClass(UrdHierarchicalInstancedStaticMeshComponent::StaticClass(),true,FTransform(),false);
	} else {
		ismc=(UInstancedStaticMeshComponent*)AddComponentByClass(UrdInstancedStaticMeshComponent::StaticClass(),true,FTransform(),false);
	}
#else
	if(!GetRootComponent()) {
		USceneComponent* root=NewObject<USceneComponent>(this,USceneComponent::StaticClass());
        root->RegisterComponent();
		//root->SetMobility(EComponentMobility::Static);
	}
	if(bUseHISMs) {
		ismc=NewObject<UInstancedStaticMeshComponent>(this,UrdHierarchicalInstancedStaticMeshComponent::StaticClass());
	} else {
		ismc=NewObject<UInstancedStaticMeshComponent>(this,UrdInstancedStaticMeshComponent::StaticClass());
	}
    ismc->RegisterComponent();
#endif
	if(ismc) {

		ismc->SetFlags(RF_Transient);

		ismc->SetStaticMesh(mesh.Get());
		ismc->OverrideMaterials=mats;

		SetHISMCdata(mesh.Get(),ismc);
		if(collision!=ErdCollision::UseDefault) {
			switch(collision) {
				case ErdCollision::NoCollision: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision,false); break;
				case ErdCollision::QueryOnly: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly,false); break;
				case ErdCollision::PhysicsOnly: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::PhysicsOnly,false); break;
				case ErdCollision::QueryAndPhysics: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics,false); break;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
				case ErdCollision::ProbeOnly: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::ProbeOnly,false); break;
				case ErdCollision::QueryAndProbe: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndProbe,false); break;
#endif
			}
			ismc->RecreatePhysicsState();
		}
		ismc->SetMobility(EComponentMobility::Movable);
//		ismc->SetMobility(EComponentMobility::Static);
		if(startCull>0.0f && endCull>=startCull) {
			ismc->InstanceStartCullDistance=startCull;
			ismc->InstanceEndCullDistance=endCull;
		}
		ismc->bReverseCulling=bReverseCulling;

		//ismc->SetHasPerInstancePrevTransforms(true);

		//ismc->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		ismc->ComponentTags.Add(sid);

		instList.Add(sid,FrdInstList(sid,mesh));
	}

	return ismc;
}

//.............................................................................
//  rdGetInstanceGen
//
// Returns the HISMC that is used to create Instanced for the specified UStaticMesh*
// This is the more optimal way to add Instances, add all instances for a specified mesh
// from the returned component. That way you only do one Map lookup per StaticMesh.
//.............................................................................
UHierarchicalInstancedStaticMeshComponent* ArdInstBaseActor::rdGetHInstanceGen(const UStaticMesh* mesh,bool create) {
	return rdGetHInstanceGenX(rdGetSMsid((UStaticMesh*)mesh));
}

UHierarchicalInstancedStaticMeshComponent* ArdInstBaseActor::rdGetHInstanceGenX(const FName sid,bool create) {

	if(sid.IsNone()) return nullptr;

	int32 ver=0;
	TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn;
	TSoftObjectPtr<UStaticMesh> mesh=nullptr;
	TArray<TSoftObjectPtr<UMaterialInterface>> materials;
	bool bReverseCulling=false;
	TEnumAsByte<ErdCollision> collision;
	float startCull,endCull;
	int32 id;
	FName groupID;
	rdGetSidDetails(sid,ver,type,mesh,materials,bReverseCulling,collision,startCull,endCull,id,groupID);
	if(!mesh) {
		return nullptr;
	}
		
	UHierarchicalInstancedStaticMeshComponent* ismc=FindHISMCforMesh(sid);
	if(ismc) {
		if(ismc->PerInstanceSMData.Num()==0) { // has been reset, reset the recycle list
			freeInstanceMap.Remove(sid);
		}
		return ismc;
	}

	freeInstanceMap.Remove(sid);
	if(!create) return nullptr;

	TArray<UMaterialInterface*> mats;
	for(auto m:materials) {
		mats.Add(m.Get());
	}

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	if(!GetRootComponent()) {
		USceneComponent* root=(USceneComponent*)AddComponentByClass(USceneComponent::StaticClass(),false,FTransform(),false);
		//root->SetMobility(EComponentMobility::Static);
	}
	ismc=(UHierarchicalInstancedStaticMeshComponent*)AddComponentByClass(UrdHierarchicalInstancedStaticMeshComponent::StaticClass(),true,FTransform(),false);
#else
	if(!GetRootComponent()) {
		USceneComponent* root=NewObject<USceneComponent>(this,USceneComponent::StaticClass());
        root->RegisterComponent();
		//root->SetMobility(EComponentMobility::Static);
	}
	ismc=NewObject<UHierarchicalInstancedStaticMeshComponent>(this,UrdHierarchicalInstancedStaticMeshComponent::StaticClass());

#endif
	if(ismc) {

		ismc->SetFlags(RF_Transient);

		ismc->SetStaticMesh((UStaticMesh*)mesh.Get());
		ismc->OverrideMaterials=mats;

		SetHISMCdata((UStaticMesh*)mesh.Get(),ismc);
		if(collision!=ErdCollision::UseDefault) {
			switch(collision) {
				case ErdCollision::NoCollision: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision,false); break;
				case ErdCollision::QueryOnly: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly,false); break;
				case ErdCollision::PhysicsOnly: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::PhysicsOnly,false); break;
				case ErdCollision::QueryAndPhysics: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics,false); break;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
				case ErdCollision::ProbeOnly: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::ProbeOnly,false); break;
				case ErdCollision::QueryAndProbe: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndProbe,false); break;
#endif
			}
			ismc->RecreatePhysicsState();
		}
		//ismc->SetMobility(EComponentMobility::Static);
		ismc->SetMobility(EComponentMobility::Movable);
		if(startCull>0.0f && endCull>=startCull) {
			ismc->InstanceStartCullDistance=startCull;
			ismc->InstanceEndCullDistance=endCull;
		}
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		ismc->SetReverseCulling(bReverseCulling);
#else
		ismc->bReverseCulling=bReverseCulling;
#endif

		//ismc->SetHasPerInstancePrevTransforms(true);

		ismc->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		ismc->ComponentTags.Add(sid);

		instList.Add(sid,FrdInstList(sid,mesh));
	}

	return ismc;
}

//.............................................................................
// rdGetPreferredInstanceGenX
//.............................................................................
UInstancedStaticMeshComponent* ArdInstBaseActor::rdGetPreferredInstanceGenX(const FName sid,bool create) {

	if(bUseHISMs) {
		if(!bUseISMsForNanite || !rdMeshIsNaniteX(sid)) {
			return rdGetHInstanceGenX(sid,create);
		}
	}
	return rdGetInstanceGenX(sid,create);
}

//.............................................................................
// FindISMCforMesh
//.............................................................................
UInstancedStaticMeshComponent* ArdInstBaseActor::FindISMCforMesh(const FName& sid) {

	UInstancedStaticMeshComponent* ismc=nullptr;
	TArray<UActorComponent*> list=GetComponentsByTag(UInstancedStaticMeshComponent::StaticClass(),sid);
	if(list.Num()>0) {
		ismc=(UInstancedStaticMeshComponent*)list[0];
		if(ismc->PerInstanceSMData.Num()==0) { // has been reset, reset the recycle list
			freeInstanceMap.Remove(sid);
		}
	}
	return ismc;
}

//.............................................................................
// FindISMCforMeshInVolume
//.............................................................................
UInstancedStaticMeshComponent* ArdInstBaseActor::FindISMCforMeshInVolume(AActor* volume,const UStaticMesh* mesh) {
	return FindISMCforMeshInVolumeX(volume,rdGetSMsid((UStaticMesh*)mesh));
}
UInstancedStaticMeshComponent* ArdInstBaseActor::FindISMCforMeshInVolumeX(AActor* volume,const FName sid) {

	if(!volume || sid.IsNone()) return nullptr;

	TArray<UActorComponent*> comps;
	volume->GetComponents(UInstancedStaticMeshComponent::StaticClass(),comps);
	for(auto& c:comps) {
		UInstancedStaticMeshComponent* smc=(UInstancedStaticMeshComponent*)c;
		FName csid=rdGetSMCsid(smc);
		if(sid==csid) {
			if(smc->PerInstanceSMData.Num()==0) { // has been reset, reset the recycle list
				freeInstanceMap.Remove(sid);
			}
			return smc;
		}
	}
	return nullptr;
}

//.............................................................................
// FindHISMCforMesh
//.............................................................................
UHierarchicalInstancedStaticMeshComponent* ArdInstBaseActor::FindHISMCforMesh(const FName& sid) {

	UHierarchicalInstancedStaticMeshComponent* ismc=nullptr;
	TArray<UActorComponent*> list=GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(),sid);
	if(list.Num()>0) {
		ismc=(UHierarchicalInstancedStaticMeshComponent*)list[0];
		if(ismc && ismc->PerInstanceSMData.Num()==0) { // has been reset, reset the recycle list
			//FName sid=rdGetSMCsid(ismc);
			freeInstanceMap.Remove(sid);
		}
	}
	return ismc;
}

//.............................................................................
// rdCalcInstanceCounts
//.............................................................................
void ArdInstBaseActor::rdCalcInstanceCounts() {

	numInstances=0;
	numRawInstances=0;
	numComponents=0;
	numRawComponents=0;
	numRecycleInstances=0;
	numRecycleComponents=0;
	numAutoInstances=0;
	numAutoComponents=0;

	// Managed Instances
	for(auto& i:instList) {
		TArray<FrdInstItem*>& items=i.Value.instItems;
		if(items.Num()>0) {
			numInstances+=items.Num();
			numComponents++;
		}
	}

	// Raw Instances
	TArray<UActorComponent*> list;
	GetComponents(UInstancedStaticMeshComponent::StaticClass(),list);
	for(auto comp:list) {
		UInstancedStaticMeshComponent* ismc=(UInstancedStaticMeshComponent*)comp;
		numRawInstances+=ismc->GetInstanceCount();
		numRawComponents++;
	}
	if(numRawInstances>=numInstances) numRawInstances-=numInstances;
	if(numRawComponents>=numComponents) numRawComponents-=numComponents;

	// Recycle Instances
	for(auto& it:freeInstanceMap) {
		numRecycleInstances+=it.Value.Num();
		numRecycleComponents++;
	}
	if(numInstances>=numRecycleInstances) numInstances-=numRecycleInstances;
	if(numRawInstances>=numRecycleInstances) numRawInstances-=numRecycleInstances;

	// Auto Instances
	numAutoComponents=autoInstSavedIndexes.Num();
	for(auto& ary:autoInstSavedIndexes) {
		numAutoInstances+=ary.Value.Num();
	}
	if(numInstances>=numAutoInstances) numInstances-=numAutoInstances;
	if(numRawInstances>=numAutoInstances) numRawInstances-=numAutoInstances;
	if(numComponents>=numAutoComponents) numComponents-=numAutoComponents;
	if(numRawComponents>=numAutoComponents) numRawComponents-=numAutoComponents;
}

//.............................................................................
// rdRecreateInstances
//.............................................................................
void ArdInstBaseActor::rdRecreateInstances() {

	rdRemAllInstances();

	FWorldContext* world=GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	UWorld* World=world?world->World():GetWorld();

	// Thanks to yllstar on Discord for the fix below (was crashing during the building of HLODs)
	if(!World || !IsValid(World) || World->bIsTearingDown) {
		UE_LOG(LogTemp, Warning, TEXT("rdRecreateInstances aborted: Invalid or tearing down world."));
		return;
	}

	for(TActorIterator<AActor> actor(World);actor;++actor) {
		ArdActor* rdActor=Cast<ArdActor>(*actor);
		if(rdActor) {
			rdActor->rdRecreateBPInstances();
		}
		ArdSpawnActor* spawnActor=Cast<ArdSpawnActor>(*actor);
		if(spawnActor) {
			spawnActor->rdSpawn();
		}
	}
}

//.............................................................................
// rdRemoveAllInstances
//.............................................................................
void ArdInstBaseActor::rdRemoveAllInstances() {

	rdRemAllInstances();
}

//.............................................................................
// SetHISMCdata
//.............................................................................
void ArdInstBaseActor::SetHISMCdataX(const FName sid,UInstancedStaticMeshComponent* ismc) {
	SetHISMCdata(rdGetMeshFromSid(sid),ismc);
}
void ArdInstBaseActor::SetHISMCdata(UStaticMesh* mesh,UInstancedStaticMeshComponent* ismc) {

	if(!mesh) return;
	UrdInstData* tag=GetInstUserData(mesh,true);

	while(tag->randomSeed==0) {
		tag->randomSeed=FDateTime::Now().GetTicks();
	}

	ismc->InstanceStartCullDistance=tag->startCull;
	ismc->InstanceEndCullDistance=tag->endCull;
	if(ismc->InstanceStartCullDistance<=0.01f || ismc->InstanceStartCullDistance>maxCullDistance) ismc->InstanceStartCullDistance=maxCullDistance;
	if(ismc->InstanceEndCullDistance<=0.01f || ismc->InstanceEndCullDistance>maxCullDistance) ismc->InstanceEndCullDistance=maxCullDistance;

	ismc->MinDrawDistance=tag->minDrawDistance;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>4
	ismc->NanitePixelProgrammableDistance=tag->naniteProgDistance;
#endif

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	ismc->SetNumCustomDataFloats(tag->numPerInstCustData);
#else
	ismc->NumCustomDataFloats=tag->numPerInstCustData;
#endif

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	ismc->WorldPositionOffsetDisableDistance=tag->WPODisableDistance;
#endif
	switch(tag->mobility) {
		case 0: ismc->Mobility=EComponentMobility::Static; break;
		case 1: ismc->Mobility=EComponentMobility::Stationary; break;
		case 2: ismc->Mobility=EComponentMobility::Movable; break;
	}

	switch(tag->collision) {
		case 0: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision,false); break;
		case 1: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly,false); break;
		case 2: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::PhysicsOnly,false); break;
		case 3: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics,false); break;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
		case 4: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::ProbeOnly,false); break;
		case 5: ismc->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndProbe,false); break;
#endif
	}

	ismc->MinLOD=tag->minLOD;
	ismc->InstancingRandomSeed=tag->randomSeed;

	FLightingChannels lc;
	if(tag->lightChannel&0x1) lc.bChannel0=true;
	if(tag->lightChannel&0x2) lc.bChannel1=true;
	if(tag->lightChannel&0x4) lc.bChannel2=true;
	ismc->LightingChannels=lc;

	ismc->CastShadow=!!(tag->instFlags&rdInst_CastShadow);
	ismc->bCastStaticShadow=!!(tag->instFlags&rdInst_CastStaticShadow);
	ismc->bCastDynamicShadow=!!(tag->instFlags&rdInst_CastDynamicShadow);
	ismc->bCastFarShadow=!!(tag->instFlags&rdInst_CastFarShadow);
	ismc->bCastInsetShadow=!!(tag->instFlags&rdInst_CastInsetShadow);
	ismc->bCastHiddenShadow=!!(tag->instFlags&rdInst_CastHiddenShadow);
	ismc->bCastShadowAsTwoSided=!!(tag->instFlags&rdInst_CastShadowAsTwoSided);
	ismc->bCastVolumetricTranslucentShadow=!!(tag->instFlags&rdInst_CastVolumetrixTranslucentShadow);
	ismc->bReceiveMobileCSMShadows=!!(tag->instFlags&rdInst_ReceiveMobileCSMShadow);
	ismc->bSelfShadowOnly=!!(tag->instFlags&rdInst_SelfShadowOnly);
	ismc->bCastContactShadow=!!(tag->instFlags&rdInst_CastContactShadow);

	ismc->bRenderInMainPass=!!(tag->instFlags&rdInst_RenderInMainPass);
	ismc->bRenderInDepthPass=!!(tag->instFlags&rdInst_RenderInDepthPass);
	ismc->bEvaluateWorldPositionOffset=!!(tag->instFlags&rdInst_EvaluateWPO);

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	ismc->ShadowCacheInvalidationBehavior=(EShadowCacheInvalidationBehavior)tag->shadowInvalidation;
#endif
	ismc->bReceivesDecals=!!(tag->instFlags&rdInst_ReceivesDecals);

	UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
	if(hismc) {
		hismc->bDisableCollision=!!(tag->instFlags&rdInst_DisableCollision);
	}

	if(tag->instFlags&rdInst_CanEverAffectNavigation) {
		ismc->bNavigationRelevant=true;
		ismc->SetCanEverAffectNavigation(true);
	} else {
		ismc->bNavigationRelevant=false;
		ismc->SetCanEverAffectNavigation(false);
	}
	ismc->bFillCollisionUnderneathForNavmesh=!!(tag->instFlags&rdInst_FillUnderForNav);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>5
	ismc->bRasterizeAsFilledConvexVolume=!!(tag->instFlags&rdInst_FillNavConvex);
#endif
	ismc->bHasPerInstanceHitProxies=!!(tag->instFlags&rdInst_HasPerInstanceHitProxies);
#if ENGINE_MAJOR_VERSION>4
	ismc->bRayTracingFarField=!!(tag->instFlags&rdInst_RayTracingFarField);
#endif
	ismc->bAffectDynamicIndirectLighting=!!(tag->instFlags&rdInst_AffectDynamicIndirectLighting);
	ismc->bAffectDistanceFieldLighting=!!(tag->instFlags&rdInst_AffectDistanceFieldLighting);
	ismc->bSelectable=!!(tag->instFlags&rdInst_Selectable); // let the ISMC be selected in the outliner

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	ismc->WorldPositionOffsetDisableDistance=tag->WPODisableDistance;
#endif
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	ismc->bWorldPositionOffsetWritesVelocity=!!(tag->instFlags&rdInst_WPOWritesVelocity);
#endif

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	ismc->bSupportRemoveAtSwap=true; // New to U5.3 - Same behavior as HIMS when removing instances
#endif

	ismc->bVisibleInRayTracing=true;

	if(tag->collisionProfile.Len()>0) ismc->SetCollisionProfileName(FName(tag->collisionProfile));

	ismc->RecreatePhysicsState();

}

//.............................................................................
// rdGetActorFromInstanceIndex
//.............................................................................
AActor* ArdInstBaseActor::rdGetActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index) {

	UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(comp);
	if(!ismc) return nullptr;
	FName sid=rdGetSMCsid(ismc);
	FrdInstList* inst=instList.Find(sid);
	if(!inst) return nullptr;

	for(FrdInstItem* i:inst->instItems) {
		if(i->index==index) return (AActor*)i->owner;
	}

	return nullptr;
}

//.............................................................................
// rdGetrdActorFromInstanceIndex
//.............................................................................
ArdActor* ArdInstBaseActor::rdGetrdActorFromInstanceIndex(UPrimitiveComponent* comp,int32 index) {

	UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(comp);
	if(!ismc) return nullptr;
	FName sid=rdGetSMCsid(ismc);
	FrdInstList* inst=instList.Find(sid);
	if(!inst) return nullptr;

	for(FrdInstItem* i:inst->instItems) {
		if(i->index==index && i->owner && !i->owner->IsA(ArdInstBaseActor::StaticClass())) return (ArdActor*)i->owner;
	}

	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------
// GetInstUserData
//
// Data is stored in a UserAssetData class (UrdInstAssetUserData)
//----------------------------------------------------------------------------------------------------------------
UrdInstData* ArdInstBaseActor::GetInstUserDataX(const FName sid,bool make) {
	return GetInstUserData(rdGetMeshFromSid(sid),make);
}
UrdInstData* ArdInstBaseActor::GetInstUserData(const UStaticMesh* mesh,bool make) {

	if(!mesh) {
		return nullptr;
	}
	UrdInstAssetUserData* tag=(UrdInstAssetUserData*)((UStaticMesh*)mesh)->GetAssetUserDataOfClass(UrdInstAssetUserData::StaticClass());
	if(!tag) {
		if(!make) {
			return nullptr;
		}
		UrdInstData* data=NewObject<UrdInstData>();
		tag=NewObject<UrdInstAssetUserData>((UStaticMesh*)mesh);
		tag->SetName(TEXT("rdInst"));
		tag->startCull=data->startCull;
		tag->endCull=data->endCull;
		tag->mobility=data->mobility;
		tag->collision=data->collision;
		tag->collisionProfile=data->collisionProfile;
		tag->minLOD=data->minLOD;
		tag->lightChannel=data->lightChannel;
		tag->randomSeed=data->randomSeed;
		tag->instFlags=data->instFlags;
		tag->numPerInstCustData=data->numPerInstCustData;
		tag->WPODisableDistance=data->WPODisableDistance;
		tag->shadowInvalidation=data->shadowInvalidation;
		tag->proxySettings=data->proxySettings;
		tag->minDrawDistance=data->minDrawDistance;
		tag->naniteProgDistance=data->naniteProgDistance;
		((UStaticMesh*)mesh)->AddAssetUserData(tag);
		return data;
	}

	UrdInstData* data=NewObject<UrdInstData>();
	data->startCull=tag->startCull;
	data->endCull=tag->endCull;
	data->mobility=tag->mobility;
	data->collision=tag->collision;
	data->collisionProfile=tag->collisionProfile;
	data->minLOD=tag->minLOD;
	data->lightChannel=tag->lightChannel;
	data->randomSeed=tag->randomSeed;
//	data->instFlags=tag->instFlags;
	data->instFlags=tag->instFlags|0x00300000; // for now blast in these flags
	data->numPerInstCustData=tag->numPerInstCustData;
	data->WPODisableDistance=tag->WPODisableDistance;
	data->shadowInvalidation=tag->shadowInvalidation;
	data->proxySettings=tag->proxySettings;
	data->minDrawDistance=tag->minDrawDistance;
	data->naniteProgDistance=tag->naniteProgDistance;

	return data;
}

//----------------------------------------------------------------------------------------------------------------
// SetInstUserData
//----------------------------------------------------------------------------------------------------------------
void ArdInstBaseActor::SetInstUserDataX(const FName sid,UrdInstData* instData) {
	SetInstUserData(rdGetMeshFromSid(sid),instData);
}
void ArdInstBaseActor::SetInstUserData(const UStaticMesh* mesh,UrdInstData* data) {

	UrdInstAssetUserData* tag=(UrdInstAssetUserData*)((UStaticMesh*)mesh)->GetAssetUserDataOfClass(UrdInstAssetUserData::StaticClass());
	if(!tag) {
		tag=NewObject<UrdInstAssetUserData>((UStaticMesh*)mesh);
		tag->SetName(TEXT("rdInst"));
		((UStaticMesh*)mesh)->AddAssetUserData(tag);
	}
	tag->startCull=data->startCull;
	tag->endCull=data->endCull;
	tag->mobility=data->mobility;
	tag->collision=data->collision;
	tag->collisionProfile=data->collisionProfile;
	tag->minLOD=data->minLOD;
	tag->lightChannel=data->lightChannel;
	tag->randomSeed=data->randomSeed;
	tag->instFlags=data->instFlags;
	tag->numPerInstCustData=data->numPerInstCustData;
	tag->WPODisableDistance=data->WPODisableDistance;
	tag->shadowInvalidation=data->shadowInvalidation;
	tag->proxySettings=data->proxySettings;
	tag->minDrawDistance=data->minDrawDistance;
	tag->naniteProgDistance=data->naniteProgDistance;
}

//.............................................................................
// rdGetInstanceTranform
//
//   Not particularly fast, but makes things like swinging instanced doors open very easy.
// As there's only 1 or 2 going on at a time, it's not going to make any difference in the scheme of things.
//
//.............................................................................
bool ArdInstBaseActor::rdGetInstanceTransform(const UStaticMesh* mesh,int32 index,FTransform& stransform) {
	return rdGetInstanceTransformX(rdGetSMsid((UStaticMesh*)mesh),index,stransform);
}
bool ArdInstBaseActor::rdGetInstanceTransformX(const FName sid,int32 index,FTransform& stransform) {

	if(sid.IsNone() || index<0) return false;
	UInstancedStaticMeshComponent* ismc=FindISMCforMesh(sid);
	if(!ismc) return false;

	return ismc->GetInstanceTransform(index,stransform,true);
}

//.............................................................................
// rdGetInstanceLocation
//.............................................................................
bool ArdInstBaseActor::rdGetInstanceLocation(UStaticMesh* mesh,int32 index,FVector& sloc) {
	return rdGetInstanceLocationX(rdGetSMsid(mesh),index,sloc);
}
bool ArdInstBaseActor::rdGetInstanceLocationX(const FName sid,int32 index,FVector& sloc) {

	if(sid.IsNone() || index<0) return false;
	UInstancedStaticMeshComponent* ismc=FindISMCforMesh(sid);
	if(!ismc || index>=ismc->GetInstanceCount()) return false;

	FMatrix& t=ismc->PerInstanceSMData[index].Transform;
	sloc.X=t.M[3][0];
	sloc.Y=t.M[3][1];
	sloc.Z=t.M[3][2];

	return true;
}

//.............................................................................
// rdGetInstanceTranformFast
//.............................................................................
bool ArdInstBaseActor::rdGetInstanceTransformFast(UInstancedStaticMeshComponent* ismc,int32 index,FTransform& stransform) {

	if(!ismc || index<0 ) return false;

	return ismc->GetInstanceTransform(index,stransform,true);
}

//.............................................................................
// rdGetInstanceLocationFast
//.............................................................................
bool ArdInstBaseActor::rdGetInstanceLocationFast(UInstancedStaticMeshComponent* ismc,int32 index,FVector& sloc) {

	if(!ismc || index>=ismc->GetInstanceCount()) return false;

	FMatrix& t=ismc->PerInstanceSMData[index].Transform;
	sloc.X=t.M[3][0];
	sloc.Y=t.M[3][1];
	sloc.Z=t.M[3][2];

	return true;
}

//.............................................................................
// rdSetInstanceVisibility
//.............................................................................
bool ArdInstBaseActor::rdSetInstanceVisibility(UStaticMesh* mesh,int32 index,bool vis) {
	return rdSetInstanceVisibilityX(rdGetSMsid(mesh),index,vis);
}
bool ArdInstBaseActor::rdSetInstanceVisibilityX(const FName sid,int32 index,bool vis) {

	if(sid.IsNone() || index<0) return false;
	UInstancedStaticMeshComponent* ismc=FindISMCforMesh(sid);
	if(!ismc) return false;

	int32 numTransforms=0;
	FTransform tran;
	ismc->GetInstanceTransform(index,tran,false);
	FVector loc=tran.GetTranslation();

	if(vis) {
		if(loc.Z<=-400000.0f) { // is hidden
			tran.AddToTranslation(FVector(0.0f,0.0f,500000.0f));
		} else return false;
	} else {
		if(loc.Z>=-400000.0f) { // is visible
			tran.AddToTranslation(FVector(0.0f,0.0f,-500000.0f));
		} else return false;
	}
#if WITH_EDITOR
	dontDoModify++;
#endif
	ismc->UpdateInstanceTransform(index,tran,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
	dontDoModify--;
#endif
	return true;
}

//.............................................................................
// rdSetInstanceVisibilityFast
//.............................................................................
bool ArdInstBaseActor::rdSetInstanceVisibilityFast(UInstancedStaticMeshComponent* ismc,int32 index,bool vis) {

	if(!ismc || index<0) return false;

	FTransform tran;
	ismc->GetInstanceTransform(index,tran,false);
	FVector loc=tran.GetTranslation();

	if(vis) {
		if(loc.Z<=-400000.0f) { // is hidden
			tran.AddToTranslation(FVector(0.0f,0.0f,500000.0f));
		} else return false;
	} else {
		if(loc.Z>=-400000.0f) { // is visible
			tran.AddToTranslation(FVector(0.0f,0.0f,-500000.0f));
		} else return false;
	}
#if WITH_EDITOR
	dontDoModify++;
#endif
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3
	ismc->UpdateInstanceTransform(index,tran,bWorldSpaceInstancing,false,true);
#else
	ismc->UpdateInstanceTransform(index,tran,bWorldSpaceInstancing,true,true);
#endif
#if WITH_EDITOR
	dontDoModify--;
#endif
	return true;
}

//.............................................................................
// rdSetInstancesVisibility
//.............................................................................
void ArdInstBaseActor::rdSetInstancesVisibility(UStaticMesh* mesh,const TArray<int32>& indexes,bool vis) {
	rdSetInstancesVisibilityX(rdGetSMsid(mesh),indexes,vis);
}
void ArdInstBaseActor::rdSetInstancesVisibilityX(const FName sid,const TArray<int32>& indexes,bool vis) {

	if(sid.IsNone()) return;
	UInstancedStaticMeshComponent* ismc=FindISMCforMesh(sid);
	if(!ismc) return;
	rdSetInstancesVisibilityFast(ismc,indexes,vis);
}

//.............................................................................
// rdSetInstancesVisibilityFast
//.............................................................................
void ArdInstBaseActor::rdSetInstancesVisibilityFast(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indexes,bool vis) {

	if(!ismc || ismc->PerInstanceSMData.Num()==0) return;

	//ismc->Modify();
	FTransform t;

	for(int32 i=0;i<indexes.Num();i++) {
		if(indexes[i]>=ismc->PerInstanceSMData.Num()) continue;

		int32 index=indexes[i];
		if(index<0) continue;

		FInstancedStaticMeshInstanceData& instanceData=ismc->PerInstanceSMData[index];
		t=FTransform(instanceData.Transform);
		FVector loc=t.GetTranslation();

		if(vis) {
			if(loc.Z<=-400000.0f) { // is hidden
				t.AddToTranslation(FVector(0.0f,0.0f,500000.0f));
			}
		} else {
			if(loc.Z>=-400000.0f) { // is visible
				t.AddToTranslation(FVector(0.0f,0.0f,-500000.0f));
			}
		}
#if WITH_EDITOR
		dontDoModify++;
#endif
		ismc->UpdateInstanceTransform(index,t,bWorldSpaceInstancing,false,true);
#if WITH_EDITOR
		dontDoModify--;
#endif
		//instanceData.Transform=t.ToMatrixWithScale();
	}

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
	ismc->InstanceUpdateCmdBuffer.NumEdits++;
	//ismc->MarkRenderStateDirty();
#endif
}

//.............................................................................
// rdGetTotalNumberOfInstancesInRecycleCache
//.............................................................................
int32 ArdInstBaseActor::rdGetTotalNumberOfInstancesInRecycleCache() {

	int32 cnt=0;
	for(auto& it:freeInstanceMap) {
		cnt+=it.Value.Num();
	}
	return cnt;
}

//.............................................................................
// rdGetNumberOfInstancesInRecycleCache
//.............................................................................
int32 ArdInstBaseActor::rdGetNumberOfInstancesInRecycleCache(UStaticMesh* mesh) {
	return rdGetNumberOfInstancesInRecycleCacheX(rdGetSMsid(mesh));
}
int32 ArdInstBaseActor::rdGetNumberOfInstancesInRecycleCacheX(const FName sid) {

	TArray<int32>* ary=freeInstanceMap.Find(sid);
	if(ary) return ary->Num();
	return 0;
}

//.............................................................................
// rdRelativeToWorld
//.............................................................................
FTransform ArdInstBaseActor::rdRelativeToWorld(const FTransform& t1,const FTransform& t2) { 

	FTransform transform=t1;
	FVector scale=t2.GetScale3D();
	FRotator rot=t2.Rotator();
	FVector loc=rot.RotateVector(transform.GetTranslation())*scale;
	transform.SetTranslation(loc);
	FTransform transform2(rot,t2.GetTranslation(),scale);
	transform.Accumulate(transform2);
	transform.Accumulate(GetActorTransform().Inverse());
	return transform;
}

//.............................................................................
// rdWorldToRelative
//.............................................................................
FTransform ArdInstBaseActor::rdWorldToRelative(const FTransform& t1,const FTransform& t2) { 
	
	return t1.GetRelativeTransform(t2);// UKismetMathLibrary::MakeRelativeTransform(t1,t2);
}

//.............................................................................
// rdRelativeToWorldNoScale
//.............................................................................
FTransform ArdInstBaseActor::rdRelativeToWorldNoScale(const FTransform& t1,const FTransform& t2) { 

	FTransform transform=t1;
	FVector scale(1,1,1);
	FRotator rot=t2.Rotator();
	FVector loc=rot.RotateVector(transform.GetTranslation())*scale;
	transform.SetTranslation(loc);
	FTransform transform2(rot,t2.GetTranslation(),scale);
	transform.Accumulate(transform2);
	transform.Accumulate(GetActorTransform().Inverse());
	return transform;
}

//.............................................................................
// rdWorldToRelativeNoScale
//.............................................................................
FTransform ArdInstBaseActor::rdWorldToRelativeNoScale(const FTransform& t1,const FTransform& t2) { 
	
	FTransform t3(t2);
	t3.SetScale3D(FVector(1,1,1));
	return t1.GetRelativeTransform(t3);
}

//.............................................................................
// ISMC classes
//.............................................................................


#if ENGINE_MAJOR_VERSION>4
//bool UrdInstancedStaticMeshComponent::CanEditSMInstance(const FSMInstanceId& InstanceId) {}
//bool UrdInstancedStaticMeshComponent::CanMoveSMInstance(const FSMInstanceId& InstanceId, const ETypedElementWorldType InWorldType) {}
//bool UrdInstancedStaticMeshComponent::GetSMInstanceTransform(const FSMInstanceId& InstanceId,FTransform& OutInstanceTransform,bool bWorldSpace) {}
//bool UrdInstancedStaticMeshComponent::SetSMInstanceTransform(const FSMInstanceId& InstanceId,const FTransform& InstanceTransform,bool bWorldSpace,bool bMarkRenderStateDirty,bool bTeleport) {}
//void UrdInstancedStaticMeshComponent::NotifySMInstanceMovementStarted(const FSMInstanceId& InstanceId) {}
//void UrdInstancedStaticMeshComponent::NotifySMInstanceMovementOngoing(const FSMInstanceId& InstanceId) {}
//void UrdInstancedStaticMeshComponent::NotifySMInstanceMovementEnded(const FSMInstanceId& InstanceId) {}
/*
void UrdInstancedStaticMeshComponent::NotifySMInstanceSelectionChanged(const FSMInstanceId& InstanceId, const bool bIsSelected) {

//	UE_LOG(LogTemp, Display, TEXT("ISMC instance selection changed"));
#if WITH_EDITOR
	FrdInstEditorModule::lastSelectedISMC=InstanceId.ISMComponent;
	FrdInstEditorModule::lastSelectedInstanceIndex=InstanceId.InstanceIndex;
	FrdInstEditorModule::lastSelectedPrefab=nullptr;

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	bool beingEdited=false;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			FrdInstEditorModule::lastSelectedPrefab=rdBase->rdGetActorFromInstanceIndex(FrdInstModule::lastSelectedISMC,FrdInstModule::lastSelectedInstanceIndex);
			FrdInstEditorModule::wasSelected=FrdInstModule::lastSelectedPrefab?FrdInstModule::lastSelectedPrefab->IsSelectedInEditor():false;
		}
	}

#endif
	Super::NotifySMInstanceSelectionChanged(InstanceId,bIsSelected);
}
*/
//bool UrdInstancedStaticMeshComponent::DeleteSMInstances(TArrayView<const FSMInstanceId> InstanceIds) {}
//bool UrdInstancedStaticMeshComponent::DuplicateSMInstances(TArrayView<const FSMInstanceId> InstanceIds, TArray<FSMInstanceId>& OutNewInstanceIds) {}



//bool UrdHierarchicalInstancedStaticMeshComponent::CanEditSMInstance(const FSMInstanceId& InstanceId) {}
//bool UrdHierarchicalInstancedStaticMeshComponent::CanMoveSMInstance(const FSMInstanceId& InstanceId, const ETypedElementWorldType InWorldType) {}
//bool UrdHierarchicalInstancedStaticMeshComponent::GetSMInstanceTransform(const FSMInstanceId& InstanceId,FTransform& OutInstanceTransform,bool bWorldSpace) {}
//bool UrdHierarchicalInstancedStaticMeshComponent::SetSMInstanceTransform(const FSMInstanceId& InstanceId,const FTransform& InstanceTransform,bool bWorldSpace,bool bMarkRenderStateDirty,bool bTeleport) {}
//void UrdHierarchicalInstancedStaticMeshComponent::NotifySMInstanceMovementStarted(const FSMInstanceId& InstanceId) {}
//void UrdHierarchicalInstancedStaticMeshComponent::NotifySMInstanceMovementOngoing(const FSMInstanceId& InstanceId) {}
//void UrdHierarchicalInstancedStaticMeshComponent::NotifySMInstanceMovementEnded(const FSMInstanceId& InstanceId) {}
/*
void UrdHierarchicalInstancedStaticMeshComponent::NotifySMInstanceSelectionChanged(const FSMInstanceId& InstanceId, const bool bIsSelected) {

//	UE_LOG(LogTemp, Display, TEXT("HISMC instance selection changed"));

#if WITH_EDITOR

	FrdInstModule::lastSelectedISMC=InstanceId.ISMComponent;
	FrdInstModule::lastSelectedInstanceIndex=InstanceId.InstanceIndex;
	FrdInstModule::lastSelectedPrefab=nullptr;

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	bool beingEdited=false;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			FrdInstModule::lastSelectedPrefab=rdBase->rdGetActorFromInstanceIndex(FrdInstModule::lastSelectedISMC,FrdInstModule::lastSelectedInstanceIndex);
			FrdInstModule::wasSelected=FrdInstModule::lastSelectedPrefab?FrdInstModule::lastSelectedPrefab->IsSelectedInEditor():false;
		}
	}

#endif
	Super::NotifySMInstanceSelectionChanged(InstanceId,bIsSelected);
}
*/
//bool UrdHierarchicalInstancedStaticMeshComponent::DeleteSMInstances(TArrayView<const FSMInstanceId> InstanceIds) {}
//bool UrdHierarchicalInstancedStaticMeshComponent::DuplicateSMInstances(TArrayView<const FSMInstanceId> InstanceIds, TArray<FSMInstanceId>& OutNewInstanceIds) {}

#endif

//.............................................................................
// rdBatchUpdateInstancesTransforms
//.............................................................................
void ArdInstBaseActor::rdBatchUpdateInstancesTransforms(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indices,const TArray<FTransform>& transforms,bool bWorldSpace,bool bMarkRenderStateDirty,bool bTeleport) {

	//ismc->Modify();

	// Navigation update required if the component is relevant to navigation and registered.
	//const bool bNavigationUpdateRequired = ismc->bNavigationRelevant && ismc->IsRegistered();
	//const bool bPartialNavigationUpdateRequired=bNavigationUpdateRequired && ismc->SupportsPartialNavigationUpdate();

	//TArray<FTransform> NavigationUpdateTransforms;
	//if(bPartialNavigationUpdateRequired) {
		// Reserve enough space for previous and new transforms
	//	NavigationUpdateTransforms.Reserve(NewInstancesTransforms.Num() * 2);
	//}

	int32 ind=0;
	for(auto& t:transforms) {
		int32 InstanceIndex=indices[ind++];
		FInstancedStaticMeshInstanceData& InstanceData=ismc->PerInstanceSMData[InstanceIndex];

		// Append instance's previous and new transforms to dirty both areas
//		if(bPartialNavigationUpdateRequired) {
//			NavigationUpdateTransforms.Append({FTransform(InstanceData.Transform)*GetComponentTransform(),(bWorldSpace?NewInstanceTransform:NewInstanceTransform*GetComponentTransform())});
//		}
		
		// Render data uses local transform of the instance
		FTransform LocalTransform=bWorldSpace?t.GetRelativeTransform(ismc->GetComponentTransform()):t;
		InstanceData.Transform=LocalTransform.ToMatrixWithScale();

//		((UrdInstancedStaticMeshComponent*)ismc)->rdTransformChanged(InstanceIndex);
		//ismc->PrimitiveInstanceDataManager.TransformChanged(InstanceIndex);

		//if(bPhysicsStateCreated) {
			// Physics uses world transform of the instance
		//	FTransform WorldTransform = bWorldSpace ? NewInstanceTransform : (LocalTransform * GetComponentTransform());
		//	UpdateInstanceBodyTransform(InstanceIndex, WorldTransform, bTeleport);
		//}
	}

//	if(bNavigationUpdateRequired) {
//		if(bPartialNavigationUpdateRequired) {
//			PartialNavigationUpdates(NavigationUpdateTransforms);
//		} else {
//			FullNavigationUpdate();
//		}
//	}

//	if(bMarkRenderStateDirty) {
//		MarkRenderStateDirty();
//	}
}

//.............................................................................
// rdQueueUpdateInstance
//.............................................................................
void ArdInstBaseActor::rdQueueUpdateInstanceTransform(UInstancedStaticMeshComponent* ismc,int32 index,const FTransform& transform) {
	rdUpdateTransformFast(ismc,index,transform);
}

//.............................................................................
// rdQueueSpawnInstance
//.............................................................................
void ArdInstBaseActor::rdQueueSpawnInstance(UInstancedStaticMeshComponent* ismc,const FTransform& transform) {

}

//.............................................................................
// rdQueueSetInstanceVisibility
//.............................................................................
void ArdInstBaseActor::rdQueueSetInstanceVisibility(UInstancedStaticMeshComponent* ismc,int32 index,bool bVis) {
	rdSetInstanceVisibilityFast(ismc,index,bVis);
}

void ArdInstBaseActor::rdQueueSetInstanceVisibilityX(const FName sid,int32 index,bool bVis) {
	rdSetInstanceVisibilityX(sid,index,bVis);
}

//.............................................................................
