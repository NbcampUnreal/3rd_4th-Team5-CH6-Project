//
// rdInstBaseActor_CustomData.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 29th June 2024 (moved from rdInstBaseActor_Instancing.cpp)
// Last Modified: 26th April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "Engine/StaticMesh.h"
#include "Components/InstancedStaticMeshComponent.h"

//.............................................................................
// rdSetNumInstCustomData
//.............................................................................
void ArdInstBaseActor::rdSetNumInstCustomData(UStaticMesh* mesh,int32 numData) {
	return rdSetNumInstCustomDataX(rdGetSMsid(mesh),numData);
}
void ArdInstBaseActor::rdSetNumInstCustomDataX(const FName sid,int32 numData) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) ismc->NumCustomDataFloats=numData;
}

//.............................................................................
// rdSetNumInstCustomDataFast
//.............................................................................
void ArdInstBaseActor::rdSetNumInstCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 numData) {

	if(!ismc) return;
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	ismc->SetNumCustomDataFloats(numData);
#else
	ismc->NumCustomDataFloats=numData;
#endif
}

//.............................................................................
// rdGetCustomDataPtr
//.............................................................................
TArray<float>& ArdInstBaseActor::rdGetCustomDataPtr(UStaticMesh* mesh,int32& numData) {
	return rdGetCustomDataPtrX(rdGetSMsid(mesh),numData);
}
TArray<float>& ArdInstBaseActor::rdGetCustomDataPtrX(const FName sid,int32& numData) {

	static TArray<float> empty;
	numData=0;

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc && ismc->NumCustomDataFloats>0) {

		numData=ismc->NumCustomDataFloats;
		return ismc->PerInstanceSMCustomData;
	}

	return empty;
}

//.............................................................................
// rdGetCustomDataPtrFast
//.............................................................................
TArray<float>& ArdInstBaseActor::rdGetCustomDataPtrFast(UInstancedStaticMeshComponent* ismc,int32& numData) {

	static TArray<float> empty;
	numData=0;

	if(ismc && ismc->NumCustomDataFloats>0) {

		numData=ismc->NumCustomDataFloats;
		return ismc->PerInstanceSMCustomData;
	}

	return empty;
}

//.............................................................................
// rdGetCustomCol3Data
//.............................................................................
FLinearColor ArdInstBaseActor::rdGetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex) {
	return rdGetCustomCol3DataX(rdGetSMsid(mesh),instanceIndex,dataIndex);
}
FLinearColor ArdInstBaseActor::rdGetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+3) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to get CustomData index greater than NumCustomData"));
			return FLinearColor();
		}
		int32 n=instanceIndex*ismc->NumCustomDataFloats+dataIndex;
		return FLinearColor(ismc->PerInstanceSMCustomData[n],ismc->PerInstanceSMCustomData[n+1],ismc->PerInstanceSMCustomData[n+2]);
	}
	return FLinearColor();
}

//.............................................................................
// rdGetCustomCol3DataFast
//.............................................................................
FLinearColor ArdInstBaseActor::rdGetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex) {

	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+3) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to get CustomData index greater than NumCustomData"));
			return FLinearColor();
		}
		int32 n=instanceIndex*ismc->NumCustomDataFloats+dataIndex;
		return FLinearColor(ismc->PerInstanceSMCustomData[n],ismc->PerInstanceSMCustomData[n+1],ismc->PerInstanceSMCustomData[n+2]);
	}
	return FLinearColor();
}

//.............................................................................
// rdGetCustomCol4Data
//.............................................................................
FLinearColor ArdInstBaseActor::rdGetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex) {
	return rdGetCustomCol4DataX(rdGetSMsid(mesh),instanceIndex,dataIndex);
}
FLinearColor ArdInstBaseActor::rdGetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+4) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to get CustomData index greater than NumCustomData"));
			return FLinearColor();
		}
		int32 n=instanceIndex*ismc->NumCustomDataFloats+dataIndex;
		return FLinearColor(ismc->PerInstanceSMCustomData[n],ismc->PerInstanceSMCustomData[n+1],ismc->PerInstanceSMCustomData[n+2],ismc->PerInstanceSMCustomData[n+3]);
	}
	return FLinearColor();
}

//.............................................................................
// rdGetCustomCol4DataFast
//.............................................................................
FLinearColor ArdInstBaseActor::rdGetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex) {

	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+4) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to get CustomData index greater than NumCustomData"));
			return FLinearColor();
		}
		int32 n=instanceIndex*ismc->NumCustomDataFloats+dataIndex;
		return FLinearColor(ismc->PerInstanceSMCustomData[n],ismc->PerInstanceSMCustomData[n+1],ismc->PerInstanceSMCustomData[n+2],ismc->PerInstanceSMCustomData[n+3]);
	}
	return FLinearColor();
}

//.............................................................................
// rdSetCustomData
//.............................................................................
void ArdInstBaseActor::rdSetCustomData(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,float value,bool batch) {
	rdSetCustomDataX(rdGetSMsid(mesh),instanceIndex,dataIndex,value,batch);
}
void ArdInstBaseActor::rdSetCustomDataX(const FName sid,int32 instanceIndex,int32 dataIndex,float value,bool batch) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to set CustomData index greater than NumCustomData"));
			return;
		}
		if(batch) {
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)]=value;
		} else {
			ismc->SetCustomDataValue(instanceIndex,dataIndex,value,true);
		}
	}
}

//.............................................................................
// rdSetCustomDataFast
//.............................................................................
void ArdInstBaseActor::rdSetCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,float value,bool batch) {

	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to set CustomData index greater than NumCustomData"));
			return;
		}
		if(batch) {
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)]=value;
		} else {
			ismc->SetCustomDataValue(instanceIndex,dataIndex,value,true);
		}
	}
}

//.............................................................................
// rdSetCustomCol3Data
//.............................................................................
void ArdInstBaseActor::rdSetCustomCol3Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	rdSetCustomCol3DataX(rdGetSMsid(mesh),instanceIndex,dataIndex,col,batch);
}
void ArdInstBaseActor::rdSetCustomCol3DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+3) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to set CustomData index greater than NumCustomData"));
			return;
		}
		if(batch) {
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)]=col.R;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+1]=col.G;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+2]=col.B;
		} else {
			ismc->SetCustomDataValue(instanceIndex,dataIndex,col.R,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+1,col.G,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+2,col.B,true);
		}
	}
}

//.............................................................................
// rdSetCustomCol3DataFast
//.............................................................................
void ArdInstBaseActor::rdSetCustomCol3DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+3) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to set CustomData index greater than NumCustomData"));
			return;
		}
		if(batch) {
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)]=col.R;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+1]=col.G;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+2]=col.B;
		} else {
			ismc->SetCustomDataValue(instanceIndex,dataIndex,col.R,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+1,col.G,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+2,col.B,true);
		}
	}
}

//.............................................................................
// rdSetCustomCol4Data
//.............................................................................
void ArdInstBaseActor::rdSetCustomCol4Data(UStaticMesh* mesh,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	rdSetCustomCol4DataX(rdGetSMsid(mesh),instanceIndex,dataIndex,col,batch);
}
void ArdInstBaseActor::rdSetCustomCol4DataX(const FName sid,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+4) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to set CustomData index greater than NumCustomData"));
			return;
		}
		if(batch) {
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)]=col.R;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+1]=col.G;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+2]=col.B;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+3]=col.A;
		} else {
			ismc->SetCustomDataValue(instanceIndex,dataIndex,col.R,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+1,col.G,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+2,col.B,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+3,col.A,true);
		}
	}
}

//.............................................................................
// rdSetCustomCol4DataFast
//.............................................................................
void ArdInstBaseActor::rdSetCustomCol4DataFast(UInstancedStaticMeshComponent* ismc,int32 instanceIndex,int32 dataIndex,const FLinearColor& col,bool batch) {
	if(ismc) {
		if(ismc->NumCustomDataFloats<=dataIndex+4) {
			UE_LOG(LogTemp,Display,TEXT("rdInst Error: Tried to set CustomData index greater than NumCustomData"));
			return;
		}
		if(batch) {
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)]=col.R;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+1]=col.G;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+2]=col.B;
			ismc->PerInstanceSMCustomData[dataIndex+(instanceIndex*ismc->NumCustomDataFloats)+3]=col.A;
		} else {
			ismc->SetCustomDataValue(instanceIndex,dataIndex,col.R,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+1,col.G,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+2,col.B,true);
			ismc->SetCustomDataValue(instanceIndex,dataIndex+3,col.A,true);
		}
	}
}

//.............................................................................
// rdSetAllCustomData
//.............................................................................
void ArdInstBaseActor::rdSetAllCustomData(UStaticMesh* mesh,UPARAM(ref) TArray<float>& data,bool update) {
	rdSetAllCustomDataX(rdGetSMsid(mesh),data,update);
}
void ArdInstBaseActor::rdSetAllCustomDataX(const FName sid,UPARAM(ref) TArray<float>& data,bool update) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc && data.Num()==ismc->NumCustomDataFloats*ismc->GetInstanceCount()) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3

		int32 num=ismc->GetInstanceCount(),len=ismc->NumCustomDataFloats;
		TArray<float>* ptr=&data;
		for(int32 i=0;i<num;i++) {
			ismc->SetCustomData(i,*ptr,update);
			ptr+=(sizeof(float)*len);
		}
#else
		ismc->PerInstanceSMCustomData=data;
		if(update) {

#if ENGINE_MAJOR_VERSION>5
			//ismc->MarkRenderStateDirty();
#else
			UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
			if(hismc) {
				hismc->BuildTreeIfOutdated(true,true);
			//} else {
				//ismc->MarkRenderStateDirty();
			}
#endif

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
			ismc->InstanceUpdateCmdBuffer.NumEdits++;
			//ismc->MarkRenderStateDirty();
#endif
		}
#endif
	}
}

//.............................................................................
// rdSetAllCustomDataFast
//.............................................................................
void ArdInstBaseActor::rdSetAllCustomDataFast(UInstancedStaticMeshComponent* ismc,UPARAM(ref) TArray<float>& data,bool update) {

	if(ismc && data.Num()==ismc->NumCustomDataFloats*ismc->GetInstanceCount()) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3

		int32 num=ismc->GetInstanceCount(),len=ismc->NumCustomDataFloats;
		TArray<float>* ptr=&data;
		for(int32 i=0;i<num;i++) {
			ismc->SetCustomData(i,*ptr,update);
			ptr+=(sizeof(float)*len);
		}
#else

		ismc->PerInstanceSMCustomData=data;
		if(update) {

			//ismc->Modify();

#if ENGINE_MAJOR_VERSION>5
			//ismc->MarkRenderStateDirty();
#else
			UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
			if(hismc) {
				hismc->BuildTreeIfOutdated(true,true);
			//} else {
				//ismc->MarkRenderStateDirty();
			}
#endif

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
			ismc->InstanceUpdateCmdBuffer.NumEdits++;
			//ismc->MarkRenderStateDirty();
#endif
		}
#endif
	}
}

//.............................................................................
// rdUpdateCustomData
//.............................................................................
void ArdInstBaseActor::rdUpdateCustomData(UStaticMesh* mesh) {
	rdUpdateCustomDataX(rdGetSMsid(mesh));
}
void ArdInstBaseActor::rdUpdateCustomDataX(const FName sid) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		//ismc->Modify();

#if ENGINE_MAJOR_VERSION>5
		//ismc->MarkRenderStateDirty();
#else
		UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
		if(hismc) {
			hismc->BuildTreeIfOutdated(true,true);
		//} else {
			//ismc->MarkRenderStateDirty();
		}
#endif

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
		ismc->InstanceUpdateCmdBuffer.NumEdits++;
		//ismc->MarkRenderStateDirty();
#endif
	}
}

//.............................................................................
// rdUpdateCustomDataFast
//.............................................................................
void ArdInstBaseActor::rdUpdateCustomDataFast(UInstancedStaticMeshComponent* ismc) {

	if(ismc) {
		//ismc->Modify();

#if ENGINE_MAJOR_VERSION>5
		//ismc->MarkRenderStateDirty();
#else
		UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
		if(hismc) {
			hismc->BuildTreeIfOutdated(true,true);
		//} else {
			//ismc->MarkRenderStateDirty();
		}
#endif

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
		ismc->InstanceUpdateCmdBuffer.NumEdits++;
		//ismc->MarkRenderStateDirty();
#endif
	}
}

//.............................................................................
// rdFillCustomData
//.............................................................................
void ArdInstBaseActor::rdFillCustomData(UStaticMesh* mesh,int32 offset,int32 stride,float baseValue,float randomVariance,int32 granularity,float inc,float incRandomVariance) {
	rdFillCustomDataX(rdGetSMsid(mesh),offset,stride,baseValue,randomVariance,granularity,inc,incRandomVariance);
}
void ArdInstBaseActor::rdFillCustomDataX(const FName sid,int32 offset,int32 stride,float baseValue,float randomVariance,int32 granularity,float inc,float incRandomVariance) {

	UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
	if(ismc) {
		rdFillCustomDataFast(ismc,offset,stride,baseValue,randomVariance,granularity,inc,incRandomVariance);
	}
}

//.............................................................................
// rdFillCustomDataFast
//.............................................................................
void ArdInstBaseActor::rdFillCustomDataFast(UInstancedStaticMeshComponent* ismc,int32 offset,int32 stride,float baseValue,float randomVariance,int32 granularity,float inc,float incRandomVariance) {

	if(!ismc) return;
	//ismc->Modify();

	TArray<float>& data=ismc->PerInstanceSMCustomData;
	int32 num=data.Num(),i=offset;
	float val=baseValue;

	FRandomStream rand;
	rand.Initialize(FDateTime::Now().GetTicks());

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>3

		int32 len=ismc->NumCustomDataFloats;
		while(i<num) {

			float v=val+rand.FRandRange(-randomVariance,randomVariance);
			ismc->SetCustomDataValue(i/len,i-((int32)(i/len)*len),round(v*pow(10,granularity-1))/pow(10,granularity-1),true);
			i+=stride;
			val+=(inc+rand.FRandRange(-incRandomVariance,incRandomVariance));
		}
#else

	while(i<num) {

		float v=val+rand.FRandRange(-randomVariance,randomVariance);
		data[i]=round(v*pow(10,granularity-1))/pow(10,granularity-1);
		i+=stride;
		val+=(inc+rand.FRandRange(-incRandomVariance,incRandomVariance));
	}


#if ENGINE_MAJOR_VERSION>5
	//ismc->MarkRenderStateDirty();
#else
	UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(ismc);
	if(hismc) {
		hismc->BuildTreeIfOutdated(true,true);
	//} else {
	//	ismc->MarkRenderStateDirty();
	}
#endif

#if ENGINE_MAJOR_VERSION<5 || ENGINE_MINOR_VERSION<4
	ismc->InstanceUpdateCmdBuffer.NumEdits++;
	//ismc->MarkRenderStateDirty();
#endif
#endif
}

//.............................................................................
// rdFillCustomDataInArea
//.............................................................................
int32 ArdInstBaseActor::rdFillCustomDataInArea(const TArray<UInstancedStaticMeshComponent*> ismcs,const FVector& loc,float radius,const FVector& box,int32 index,float value) {
	
	int32 cnt=0;
/*
	TArray<FHitResult> hits;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FindProxies),false,nullptr);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>2
	QueryParams.bTraceIntoSubComponents=false;
#endif
	const ECollisionChannel CollisionChannel=ECC_Visibility;
	FCollisionResponseParams ResponseParams(ECR_Overlap);
	
	bool ret=GetWorld()->SweepMultiByChannel(hits,loc,loc,FQuat::Identity,CollisionChannel,FCollisionShape::MakeSphere(radius),QueryParams);
	
	if(ret && hits.Num()>0) {

		for(auto& h:hits) {
			UInstancedStaticMeshComponent* hitISM=Cast<UInstancedStaticMeshComponent>(h.GetComponent());
			if(hitISM && ismcs.Contains(hitISM)) {
				hitISM->SetCustomDataValue(h.Item,index,value,false);
				//hitISM->MarkRenderStateDirty();
			}
		}
	}
*/
/*
	TArray<AActor*> actors;
	TArray<FrdInstanceItems> instances;
	TArray<AActor*> ignoreActors;

	rdSphereTrace(loc,radius,actors,instances,ignoreActors);

	for(auto& ii:instances) {

	}
*/

	FBox lbox(loc,loc);
	for(auto ismc:ismcs) {

//		FBox bnds=ismc->Bounds.GetBox();
//		FVector& minB=bnds.Min;
//		FVector& maxB=bnds.Max;
//		if(loc.X<minB.X || loc.X>maxB.X || loc.Y<minB.Y || loc.Y>maxB.Y || loc.Z<minB.Z || loc.Z>maxB.Z) continue;

		TArray<float>& data=ismc->PerInstanceSMCustomData;
		int32 numPerInst=ismc->NumCustomDataFloats;
		if(index>=numPerInst) continue;

		int32 numInst=ismc->GetInstanceCount();
		TArray<FInstancedStaticMeshInstanceData>& instData=ismc->PerInstanceSMData;
		FVector iloc;
		bool dirty=false;
		if(radius>0.0f) {

			for(int32 i=0;i<numInst;i++) {

				FMatrix& t=instData[i].Transform;
				iloc.X=t.M[3][0];
				iloc.Y=t.M[3][1];
				iloc.Z=t.M[3][2];
				if(FVector::Distance(loc,iloc)<radius && data[i*numPerInst+index]!=value) {
					ismc->SetCustomDataValue(i,index,value,false);
					cnt++;
					dirty=true;
				}
			}

		} else {

			for(int32 i=0;i<numInst;i++) {

				FMatrix& t=instData[i].Transform;
				iloc.X=t.M[3][0];
				iloc.Y=t.M[3][1];
				iloc.Z=t.M[3][2];
				if(iloc.X>=loc.X && iloc.X<=(loc.X+box.X) && iloc.Y>=loc.Y && iloc.Y<=(loc.Y+box.Y) && iloc.Z>=loc.Z && iloc.Z<(loc.Z+box.Z) && data[i*numPerInst+index]!=value) {
					ismc->SetCustomDataValue(i,index,value,false);
					cnt++;
					dirty=true;
				}
			}
		}

		if(dirty) ismc->MarkRenderStateDirty();
	}

	return cnt;
}

//.............................................................................
// rdFillCustomDataInAreaMulti
//.............................................................................
int32 ArdInstBaseActor::rdFillCustomDataInAreaMulti(const TArray<UInstancedStaticMeshComponent*> ismcs,const FVector& loc,float radius,const FVector& box,int32 index,TArray<float> values) {

	int32 cnt=0;

	for(auto ismc:ismcs) {

		int32 numInst=ismc->GetInstanceCount();
		FTransform tran;
		FVector iloc;

		if(radius>0.0f) {

			for(int32 i=0;i<numInst;i++) {
				ismc->GetInstanceTransform(i,tran,true);
				iloc=tran.GetTranslation();
				if(FVector::Distance(loc,iloc)<radius) {
					int32 vi=0;
					for(auto v:values) {
						ismc->SetCustomDataValue(i,index+vi++,v,false);
					}
					cnt++;
				}
			}

		} else {

			for(int32 i=0;i<numInst;i++) {
				ismc->GetInstanceTransform(i,tran,true);
				iloc=tran.GetTranslation();
				if(iloc.X>=loc.X && iloc.X<=(loc.X+box.X) && iloc.Y>=loc.Y && iloc.Y<=(loc.Y+box.Y) && iloc.Z>=loc.Z && iloc.Z<(loc.Z+box.Z)) {
					int32 vi=0;
					for(auto v:values) {
						ismc->SetCustomDataValue(i,index+vi++,v,false);
					}
					cnt++;
				}
			}
		}
	}

	return cnt;
}

//.............................................................................
