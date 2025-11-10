//
// rdInstBaseActor_Splines.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 25th November 2023
// Last Modified: 27th January 2024
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "Misc/OutputDeviceNull.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "rdActor.h"

//----------------------------------------------------------------------------------------------------------------
// rdSplitSplineInto
//----------------------------------------------------------------------------------------------------------------
TArray<AActor*> ArdInstBaseActor::rdSplitSplineInto(AActor* actor,USplineComponent* spline,int32 num,bool callBPfunctions) {

	TArray<AActor*> actors;
	if(!actor || !spline) return actors;

	actors.Add(actor); // main spline

	int meshIndex=0;
	float meshOffset=0;
	FOutputDeviceNull ar;
	if(callBPfunctions) {
		actor->CallFunctionByNameWithArguments(TEXT("rdClearSpline"),ar,NULL,true);
		meshOffset=BuildBPSplineSection(actor,0.0f,meshIndex);
	}

	int32 numPoints=spline->GetNumberOfSplinePoints();
	if(num<=(numPoints/3)) { // only split splines with num*3 points or greater

		int32 numSplits=num;
		int32 pntsPerSplit=numPoints/numSplits;
		int32 splitInd=pntsPerSplit;
		for(int32 i=0;i<numSplits-1;i++) {

			AActor* newActor=DuplicateActor(actor);

			FString nm=FString::Printf(TEXT("split%d"),i);
			newActor->Tags.Add(FName(*nm));
			actors.Add(newActor);

			USplineComponent* spline2=nullptr;
			TArray<UActorComponent*> comps2;
			newActor->GetComponents(USplineComponent::StaticClass(),comps2);
			for(auto comp2:comps2) {
				if(comp2->GetName()==spline->GetName()) {
					spline2=Cast<USplineComponent>(comp2);
					break;
				}
			}
			if(spline2) {

				spline2->ClearSplinePoints(false);
				rdCopySpline(spline,spline2,splitInd,(i==(numSplits-2))?spline->GetNumberOfSplinePoints():splitInd+pntsPerSplit+1);
				splitInd+=pntsPerSplit;
			}

			if(callBPfunctions) meshOffset=BuildBPSplineSection(newActor,meshOffset,meshIndex);

		}
		rdTrimSpline(spline,0,pntsPerSplit);
	}

	return actors;
}

//.............................................................................
// rdCopySpline
//.............................................................................
void ArdInstBaseActor::rdCopySpline(USplineComponent* spline1,USplineComponent* spline2,int32 start,int32 end) {

	if(end>spline1->GetNumberOfSplinePoints()) return;

	FSplinePoint pt1=FillSplinePoint(spline1,start);
	FSplinePoint pt2=FillSplinePoint(spline1,end-1);

	for(int32 i=start;i<end;i++) {
		FSplinePoint spt=FillSplinePoint(spline1,i);
		spt.InputKey-=pt1.InputKey;
		spline2->AddPoint(spt);
	}

	spline2->UpdateSpline();

	// sets the outer points to Curve_user, so tangents need to be updated whenever the points are moved
	spline2->SetTangentsAtSplinePoint(0,pt1.ArriveTangent,pt1.LeaveTangent,ESplineCoordinateSpace::Local,true);
	spline2->SetTangentsAtSplinePoint(spline2->GetNumberOfSplinePoints()-1,pt2.ArriveTangent,pt2.LeaveTangent,ESplineCoordinateSpace::Local,true);
}

//.............................................................................
// rdTrimSpline
//.............................................................................
void ArdInstBaseActor::rdTrimSpline(USplineComponent* spline,int32 start,int32 len) {

	if(start+len>spline->GetNumberOfSplinePoints()) return;

	FSplinePoint pt1=FillSplinePoint(spline,start);

	int32 end=(len==0)?spline->GetNumberOfSplinePoints()-1:start+len;
	FSplinePoint pt2=FillSplinePoint(spline,end);

	// remove from 0 to start
	for(int32 i=0;i<start;i++) {
		spline->RemoveSplinePoint(0,false);
	}

	// remove left overs
	int32 left=(len==0)?0:spline->GetNumberOfSplinePoints()-len;
	for(int32 i=0;i<left;i++) {
		spline->RemoveSplinePoint(len+1,false);
	}

	spline->UpdateSpline();

	spline->SetTangentsAtSplinePoint(0,pt1.ArriveTangent,pt1.LeaveTangent,ESplineCoordinateSpace::Local,true);
	spline->SetTangentsAtSplinePoint(spline->GetNumberOfSplinePoints()-1,pt2.ArriveTangent,pt2.LeaveTangent,ESplineCoordinateSpace::Local,true);
}

//----------------------------------------------------------------------------------------------------------------
// rdJoinSplines
//----------------------------------------------------------------------------------------------------------------
void ArdInstBaseActor::rdJoinSplines(AActor* actor,bool callBPfunctions) {

	if(!actor) return;
	TArray<AActor*> actorList;
	actor->GetAttachedActors(actorList);
	actorList.Add(actor);

	// remove actors that are sub splines attached to parents in the selected list
	TArray<AActor*> aList;
	for(auto a:actorList) {
		for(auto a2:actorList) {
			if(a2->GetAttachParentActor()==a) {
				aList.Add(a2);
			}
		}
	}
	for(auto a:aList) {
		actorList.Remove(a);
	}

	// Now merge attach actors subsplines to their parents (only one level deep to preserve sub editing)
	for(auto a:actorList) {
		rdMergeActorsSubSplines(a,callBPfunctions);
	}
}

//----------------------------------------------------------------------------------------------------------------
// rdMergeActorsSubSplines
//----------------------------------------------------------------------------------------------------------------
void ArdInstBaseActor::rdMergeActorsSubSplines(AActor* actor,bool callBPfunctions) {

	TArray<AActor*> aList;
	actor->GetAttachedActors(aList,true);

	TArray<AActor*> sortedList;
	sortedList.SetNum(aList.Num());
	for(auto subActor:aList) {
		if(subActor->Tags.Contains(FName(TEXT("SubSpline")))) {
			int32 ind=GetSubSplineIndex(subActor);
			if(ind>=0) sortedList[ind]=subActor;
		}
	}

	USplineComponent* spline=GetSpline(actor);
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	float key=spline->GetInputKeyValueAtDistanceAlongSpline(spline->GetDistanceAlongSplineAtSplinePoint(spline->GetNumberOfSplinePoints()-1));
#else
	float key=spline->SplineCurves.ReparamTable.Eval((spline->GetDistanceAlongSplineAtSplinePoint(spline->GetNumberOfSplinePoints()-1)),0.0f);
#endif
	for(auto a:sortedList) {
		if(IsValid(a)) {
			rdMergeActorsSubSplines(a);

			USplineComponent* sub=GetSpline(a);

			if(callBPfunctions) {
				UFunction* func=actor->FindFunction(FName(TEXT("rdMergeSubSplineData")));
				if(func) {

					struct cnvParms {
						AActor* subspline;
						int32	startIndex;
						int32	endIndex;
					};

					cnvParms parms;
					parms.subspline=a;
					parms.startIndex=(int32)key;
					parms.endIndex=(int32)key+sub->GetNumberOfSplinePoints()-2;
					actor->ProcessEvent(func,(void*)&parms);
				}
			}

			for(int32 i=1;i<sub->GetNumberOfSplinePoints();i++) {

				FSplinePoint pt=FillSplinePoint(sub,i,key);
				spline->AddPoint(pt,true);
			}
#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
			key+=sub->GetInputKeyValueAtDistanceAlongSpline(sub->GetDistanceAlongSplineAtSplinePoint(sub->GetNumberOfSplinePoints()-1));
#else
			key+=sub->SplineCurves.ReparamTable.Eval((sub->GetDistanceAlongSplineAtSplinePoint(sub->GetNumberOfSplinePoints()-1)),0.0f);
#endif

			a->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			a->Destroy();
		}
	}

	if(callBPfunctions) {
		// Reset the main spline
		UFunction* func=actor->FindFunction(FName(TEXT("rdMergeSubSplineData")));
		if(func) {

			struct cnvParms {
				AActor* subspline;
				int32	startIndex;
				int32	endIndex;
			};

			cnvParms parms;
			parms.subspline=nullptr;
			parms.startIndex=0;
			parms.endIndex=0;
			actor->ProcessEvent(func,(void*)&parms);
		}
	}
}

//.............................................................................
// FillSplinePoint
//.............................................................................
FSplinePoint ArdInstBaseActor::FillSplinePoint(USplineComponent* spline,int32 point,float startKey) {

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>0
	return FSplinePoint(startKey+spline->GetInputKeyValueAtDistanceAlongSpline(spline->GetDistanceAlongSplineAtSplinePoint(point)),
#else
	return FSplinePoint(startKey+spline->SplineCurves.Position.Points[point].InVal,
#endif
					spline->GetLocationAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetArriveTangentAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetLeaveTangentAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetRotationAtSplinePoint(point,ESplineCoordinateSpace::Local),
					spline->GetScaleAtSplinePoint(point),
					spline->GetSplinePointType(point)
					);
}

//.............................................................................
// BuildBPSplineSection
//.............................................................................
float ArdInstBaseActor::BuildBPSplineSection(const AActor* actor,float startOffset,int32& index) {

	float endOffset=0.0f;

	UFunction* func=actor->FindFunction(FName(TEXT("rdBuildSection")));
	if(func) {

		struct cnvParms {
#if ENGINE_MAJOR_VERSION>4
			double	startMeshOffset;
#else
			float	startMeshOffset;
#endif
			int32	startIndex;
#if ENGINE_MAJOR_VERSION>4
			double	endMeshOffset;
#else
			float	endMeshOffset;
#endif
			int32	endIndex;
		};

		cnvParms parms;
		parms.startMeshOffset=startOffset;
		parms.startIndex=index;
		((AActor*)actor)->ProcessEvent(func,(void*)&parms);
		endOffset=parms.endMeshOffset;
		index=parms.endIndex;
	}

	return endOffset;
}

//.............................................................................
// DuplicateActor
//.............................................................................
AActor* ArdInstBaseActor::DuplicateActor(const AActor* actor) {

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags=RF_Transactional;
	SpawnParams.Template=(AActor*)actor;

	AActor* newActor=GetWorld()->SpawnActor<AActor>(actor->GetClass(),FTransform(FRotator(0,0,0),FVector(0,0,0),FVector(1,1,1)),SpawnParams);
	if(newActor) {

		if(newActor->GetRootComponent()==nullptr) {
			for(auto c:newActor->GetComponents()) {
				USceneComponent* sc=Cast<USceneComponent>(c);
				if(sc && (sc->GetName().StartsWith(TEXT("Default")) || sc->GetName().StartsWith(TEXT("shared")))) {
					newActor->SetRootComponent(sc);
					break;
				}
			}
		}

		// Set the edited flag to True to override construction
		for(auto c:newActor->GetComponents()) {
			USplineComponent* sc=Cast<USplineComponent>(c);
			if(sc) {
				sc->bSplineHasBeenEdited=true;
			}
		}

		newActor->AttachToActor((AActor*)actor,FAttachmentTransformRules::KeepRelativeTransform);
		newActor->SetActorRelativeTransform(FTransform(FRotator(0,0,0),FVector(0,0,0),FVector(1,1,1)));
		if(newActor->Tags.Contains(TEXT("SubSpline"))) {
			TArray<FName> delList;
			for(auto& ftag:newActor->Tags) {
				FString tag(*ftag.ToString());
				if(tag.StartsWith(TEXT("split"))) {
					delList.Add(ftag);
				}
			}
			for(auto& tag:delList) newActor->Tags.Remove(tag);

		} else {
			newActor->Tags.Add(FName(TEXT("SubSpline")));
		}
	}

	return newActor;
}

//----------------------------------------------------------------------------------------------------------------
// GetSubSplineIndex
//----------------------------------------------------------------------------------------------------------------
int32 ArdInstBaseActor::GetSubSplineIndex(const AActor* actor) {

	int32 ind=-1;
	for(auto& ftag:actor->Tags) {
		FString tag(*ftag.ToString());
		if(tag.StartsWith(TEXT("split"))) {
			ind=FCString::Atoi(*tag.RightChop(5));
			break;
		}
	}
	return ind;
}

//----------------------------------------------------------------------------------------------------------------
// CopySplinePoint
//----------------------------------------------------------------------------------------------------------------
void ArdInstBaseActor::CopySplinePoint(USplineComponent* s1,int32 i1,USplineComponent* s2,int32 i2,FVector& loc,FRotator& rot,FVector& scale) {

	s2->SetLocationAtSplinePoint(i2,s1->GetLocationAtSplinePoint(i1,ESplineCoordinateSpace::Local),ESplineCoordinateSpace::Local,false);

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	s2->SetRotationAtSplinePoint(i2,s1->GetRotationAtSplinePoint(i1,ESplineCoordinateSpace::Local),ESplineCoordinateSpace::Local,false);
	s2->SetScaleAtSplinePoint(i2,s1->GetScaleAtSplinePoint(i1),false);
#else
	if(s2->SplineCurves.Rotation.Points.IsValidIndex(i2)) {
		const FQuat quat=s1->GetRotationAtSplinePoint(i1,ESplineCoordinateSpace::Local).Quaternion();
		FVector upVec=quat.GetUpVector();
		s2->SetUpVectorAtSplinePoint(i2,upVec,ESplineCoordinateSpace::Local,false);
		FVector dir=quat.GetForwardVector();
		s2->SetTangentAtSplinePoint(i2,dir,ESplineCoordinateSpace::Local,false);
	}
	if(s2->SplineCurves.Scale.Points.IsValidIndex(i2)) {
		s2->SplineCurves.Scale.Points[i2].OutVal=s1->GetScaleAtSplinePoint(i1);
	}
#endif
	if(i1==0) {
		FVector prevLoc=s2->GetLocationAtSplinePoint(i2-1,ESplineCoordinateSpace::Local);
		FVector nextLoc=s1->GetLocationAtSplinePoint(1,ESplineCoordinateSpace::Local);
		float dist1=s2->GetDistanceAlongSplineAtSplinePoint(i2)-s2->GetDistanceAlongSplineAtSplinePoint(i2-1);
		float dist2=s1->GetDistanceAlongSplineAtSplinePoint(1);
		FVector inc=(nextLoc-prevLoc)/(dist1+dist2);
		FVector tangent=(inc*dist1*0.5f);
		s1->SetTangentsAtSplinePoint(i1,-tangent,tangent,ESplineCoordinateSpace::Local,true);
		s2->SetTangentsAtSplinePoint(i2,tangent,-tangent,ESplineCoordinateSpace::Local,true);
	} else {
		FVector prevLoc=s1->GetLocationAtSplinePoint(i1-1,ESplineCoordinateSpace::Local);
		FVector nextLoc=s2->GetLocationAtSplinePoint(1,ESplineCoordinateSpace::Local);
		float dist1=s1->GetDistanceAlongSplineAtSplinePoint(i1)-s1->GetDistanceAlongSplineAtSplinePoint(i1-1);
		float dist2=s2->GetDistanceAlongSplineAtSplinePoint(1);
		FVector inc=(nextLoc-prevLoc)/(dist1+dist2);
		FVector tangent=(inc*dist1*0.5f);
		s1->SetTangentsAtSplinePoint(i1,tangent,-tangent,ESplineCoordinateSpace::Local,true);
		s2->SetTangentsAtSplinePoint(i2,-tangent,tangent,ESplineCoordinateSpace::Local,true);
	}

}
//----------------------------------------------------------------------------------------------------------------
// GetSpline
//----------------------------------------------------------------------------------------------------------------
USplineComponent* ArdInstBaseActor::GetSpline(const AActor* actor) {

	TArray<UActorComponent*> comps;
	actor->GetComponents(USplineComponent::StaticClass(),comps);
	for(auto comp:comps) {
		USplineComponent* sc=Cast<USplineComponent>(comp);
		if(sc) return sc; // just return the first one for now
	}
	return nullptr;
}

//.............................................................................
// rdGetSplinePointPosition
//.............................................................................
void ArdInstBaseActor::rdGetSplinePointPosition(int32 point,FVector& loc,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace) {
	
	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return;

	loc=spline->GetLocationAtSplinePoint(point,cordSpace);
}

//.............................................................................
// rdSetSplinePointPosition
//.............................................................................
TArray<AActor*> ArdInstBaseActor::rdSetSplinePointPosition(int32 point,const FVector& loc,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace,bool updateSpline) {
	
	TArray<AActor*> modList;
	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return modList;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return modList;

	spline->SetLocationAtSplinePoint(point,loc,cordSpace,updateSpline);
	modList.Add((AActor*)splineActor);
	FRotator rot=spline->GetRotationAtSplinePoint(point,ESplineCoordinateSpace::Local);
	FVector scale=spline->GetScaleAtSplinePoint(point);

	if(point==0 || point==spline->GetNumberOfSplinePoints()-1) {

		int32 index=subsplines.Find((AActor*)splineActor);
		if(index!=INDEX_NONE) {

			if(point==0 && index>0) {

				USplineComponent* subspline=GetSpline(subsplines[index-1]);
				if(subspline) {
					CopySplinePoint(spline,0,subspline,subspline->GetNumberOfSplinePoints()-1,(FVector&)loc,rot,scale);
					modList.Add(subsplines[index-1]);
				}

			} else if(point>0 && index<subsplines.Num()-2) {

				USplineComponent* subspline=GetSpline(subsplines[index+1]);
				if(subspline) {
					CopySplinePoint(spline,spline->GetNumberOfSplinePoints()-1,subspline,0,(FVector&)loc,rot,scale);
					modList.Add(subsplines[index+1]);
				}
			}
		}
	}

	return modList;
}

//.............................................................................
// rdGetSplinePointRotation
//.............................................................................
void ArdInstBaseActor::rdGetSplinePointRotation(int32 point,FRotator& rot,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace) {

	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return;

	rot=spline->GetRotationAtSplinePoint(point,cordSpace);
}

//.............................................................................
// rdSetSplinePointRotation
//.............................................................................
TArray<AActor*> ArdInstBaseActor::rdSetSplinePointRotation(int32 point,const FRotator& rot,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace,bool updateSpline) {

	TArray<AActor*> modList;
	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return modList;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return modList;

	spline->SetRotationAtSplinePoint(point,rot,cordSpace,updateSpline);
	modList.Add((AActor*)splineActor);
	FVector loc=spline->GetLocationAtSplinePoint(point,ESplineCoordinateSpace::Local);
	FVector scale=spline->GetScaleAtSplinePoint(point);

	if(point==0 || point==spline->GetNumberOfSplinePoints()-1) {

		int32 index=0;
		if(subsplines.Find((AActor*)splineActor,index)) {

			if(point==0 && index>0) {

				USplineComponent* subspline=GetSpline(subsplines[index-1]);
				if(subspline) {
					CopySplinePoint(spline,0,subspline,subspline->GetNumberOfSplinePoints()-1,loc,(FRotator&)rot,scale);
					modList.Add(subsplines[index-1]);
				}

			} else if(point>0 && index<subsplines.Num()-2) {

				USplineComponent* subspline=GetSpline(subsplines[index+1]);
				if(subspline) {
					CopySplinePoint(spline,spline->GetNumberOfSplinePoints()-1,subspline,0,loc,(FRotator&)rot,scale);
					modList.Add(subsplines[index+1]);
				}
			}
		}
	}

	return modList;
}

//.............................................................................
// rdGetSplinePointScale
//.............................................................................
void ArdInstBaseActor::rdGetSplinePointScale(int32 point,FVector& scale,const TArray<AActor*>& subsplines,const AActor* splineActor) {

	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return;

	scale=spline->GetScaleAtSplinePoint(point);
}

//.............................................................................
// rdSetSplinePointScale
//.............................................................................
TArray<AActor*> ArdInstBaseActor::rdSetSplinePointScale(int32 point,const FVector& scale,const TArray<AActor*>& subsplines,const AActor* splineActor,bool updateSpline) {

	TArray<AActor*> modList;
	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return modList;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return modList;

	spline->SetScaleAtSplinePoint(point,scale,updateSpline);
	modList.Add((AActor*)splineActor);
	FVector loc=spline->GetLocationAtSplinePoint(point,ESplineCoordinateSpace::Local);
	FRotator rot=spline->GetRotationAtSplinePoint(point,ESplineCoordinateSpace::Local);

	if(point==0 || point==spline->GetNumberOfSplinePoints()-1) {

		int32 index=0;
		if(subsplines.Find((AActor*)splineActor,index)) {

			if(point==0 && index>0) {

				USplineComponent* subspline=GetSpline(subsplines[index-1]);
				if(subspline) {
					CopySplinePoint(spline,0,subspline,subspline->GetNumberOfSplinePoints()-1,loc,rot,(FVector&)scale);
					modList.Add(subsplines[index-1]);
				}

			} else if(point>0 && index<subsplines.Num()-2) {

				USplineComponent* subspline=GetSpline(subsplines[index+1]);
				if(subspline) {
					CopySplinePoint(spline,spline->GetNumberOfSplinePoints()-1,subspline,0,loc,rot,(FVector&)scale);
					modList.Add(subsplines[index+1]);
				}
			}
		}
	}

	return modList;
}

//.............................................................................
// rdGetSplinePointTangent
//.............................................................................
void ArdInstBaseActor::rdGetSplinePointTangent(int32 point,FVector& leaveTangent,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace) {

	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return;

	leaveTangent=spline->GetTangentAtSplinePoint(point,cordSpace);
}

//.............................................................................
// rdSetSplinePointTangent
//.............................................................................
TArray<AActor*> ArdInstBaseActor::rdSetSplinePointTangent(int32 point,const FVector& leaveTangent,const TArray<AActor*>& subsplines,const AActor* splineActor,const ESplineCoordinateSpace::Type cordSpace,bool updateSpline) {

	TArray<AActor*> modList;
	if(!splineActor) splineActor=rdGetSubSplineAtPoint(point,subsplines,point);
	if(!splineActor) return modList;

	USplineComponent* spline=GetSpline(splineActor);
	if(!spline) return modList;

	spline->SetTangentAtSplinePoint(point,leaveTangent,cordSpace,updateSpline);
	modList.Add((AActor*)splineActor);
	FVector loc=spline->GetLocationAtSplinePoint(point,ESplineCoordinateSpace::Local);
	FRotator rot=spline->GetRotationAtSplinePoint(point,ESplineCoordinateSpace::Local);
	FVector scale=spline->GetScaleAtSplinePoint(point);

	if(point==0 || point==spline->GetNumberOfSplinePoints()-1) {

		int32 index=0;
		if(subsplines.Find((AActor*)splineActor,index)) {

			if(point==0 && index>0) {

				USplineComponent* subspline=GetSpline(subsplines[index-1]);
				if(subspline) {
					CopySplinePoint(spline,0,subspline,subspline->GetNumberOfSplinePoints()-1,loc,rot,scale);
					modList.Add(subsplines[index-1]);
				}

			} else if(point>0 && index<subsplines.Num()-2) {

				USplineComponent* subspline=GetSpline(subsplines[index+1]);
				if(subspline) {
					CopySplinePoint(spline,spline->GetNumberOfSplinePoints()-1,subspline,0,loc,rot,scale);
					modList.Add(subsplines[index+1]);
				}
			}
		}
	}

	return modList;
}

//.............................................................................
// rdGetSubSplineAtPoint
//.............................................................................
AActor* ArdInstBaseActor::rdGetSubSplineAtPoint(int32 point,const TArray<AActor*>& subsplines,int32& subPoint) {

	int32 i=0;
	for(auto& ss:subsplines) {
		USplineComponent* spline=GetSpline(ss);
		if(spline) {
			int32 numPoints=spline->GetNumberOfSplinePoints();
			if(point<i+numPoints) {
				subPoint=point-i;
				return ss;
			}
			i+=numPoints;
		}
	}

	return nullptr;
}

//.............................................................................
// rdFindClosestSplinePoint
//.............................................................................
AActor* ArdInstBaseActor::rdFindClosestSplinePoint(const FVector& loc,const TArray<AActor*>& subsplines,int32& localPoint,int32& overallPoint,float& distance,bool& found) {

	float closestDistance=9999999.9f;
	AActor* closestActor=nullptr;
	int32 closestPoint=-1;

	for(auto& ss:subsplines) {
		USplineComponent* spline=GetSpline(ss);
		if(spline) {
			float ikey=spline->FindInputKeyClosestToWorldLocation(loc);
			FVector sloc=spline->GetLocationAtSplineInputKey(ikey,ESplineCoordinateSpace::World);
			float dist=FVector::Distance(sloc,loc);
			if(dist<closestDistance) {
				closestDistance=dist;
				closestActor=ss;
				closestPoint=(int32)(ikey+0.5f);
			}
		}
	}

	if(!closestActor) {
		localPoint=-1;
		overallPoint=-1;
		found=false;
		return nullptr;
	}

	distance=closestDistance;
	localPoint=overallPoint=closestPoint;
	int32 index=0;
	if(subsplines.Find(closestActor,index)) {

		index--;
		while(index>=0) {

			USplineComponent* subspline=GetSpline(subsplines[index]);
			if(subspline) {
				overallPoint+=subspline->GetNumberOfSplinePoints()-1;
			}
			index--;
		}
	}
	found=true;
	return closestActor;
}

//.............................................................................
// rdPopulateSplineFast
//.............................................................................
int32 ArdInstBaseActor::rdPopulateSplineFast(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,UStaticMesh* mesh,const FTransform offset,float gap,UMaterialInterface* mat,const TEnumAsByte<ESplineMeshAxis::Type> axis,bool useCollision,bool useRoll) {

	if(!actor || !spline || !mesh) return 0;
	double splineLen=spline->GetSplineLength();
	if(endDistance<=0.0f || endDistance>splineLen) endDistance=splineLen;
	if(startDistance<0.0f) startDistance=0.0f;
	if(startDistance>splineLen || startDistance>endDistance) return 0;

	float meshLength=0.0f;
	FBox mbox=mesh->GetBoundingBox();
	TArray<FrdSplineInstanceData> instData;

	switch(axis) {
		case ESplineMeshAxis::X: meshLength=(mbox.Max.X-mbox.Min.X)*offset.GetScale3D().X; break;
		case ESplineMeshAxis::Z: meshLength=(mbox.Max.Z-mbox.Min.Z)*offset.GetScale3D().Z; break;
		default: meshLength=(mbox.Max.Y-mbox.Min.Y)*offset.GetScale3D().Y; break;
	}
	if(meshLength<=0.0f) return 0;

	ArdActor* rdActor=Cast<ArdActor>(actor);

	double currDist=0.0f;
	int32 cnt=0;
	while(currDist<splineLen) {

		FrdSplineInstanceData id;
		id.startLocation=spline->GetLocationAtDistanceAlongSpline(currDist,ESplineCoordinateSpace::Local);
		id.endLocation=spline->GetLocationAtDistanceAlongSpline(currDist+meshLength,ESplineCoordinateSpace::Local);
		id.startTangent=spline->GetDirectionAtDistanceAlongSpline(currDist,ESplineCoordinateSpace::Local)*meshLength;
		id.endTangent=spline->GetDirectionAtDistanceAlongSpline(currDist+meshLength,ESplineCoordinateSpace::Local)*meshLength;
		id.startScale=spline->GetScaleAtDistanceAlongSpline(currDist);
		id.endScale=spline->GetScaleAtDistanceAlongSpline(currDist+meshLength);
		id.startRoll=spline->GetRollAtDistanceAlongSpline(currDist,ESplineCoordinateSpace::Local);
		id.endRoll=spline->GetRollAtDistanceAlongSpline(currDist+meshLength,ESplineCoordinateSpace::Local);
		USplineMeshComponent* c=nullptr;
		if(rdActor) {
			TArray<FrdSplineInstanceData>& ilst=rdActor->GetSplineInstanceData();
			if(ilst.Num()>cnt) {
				FrdSplineInstanceData& idat=ilst[cnt];
				if(idat==id) {
					c=idat.smc;
					if(c) {
						id.smc=c;
						instData.Add(id);
						cnt++;
					}
				} else {
					c=idat.smc;
					if(c) {
						c->SetStaticMesh(mesh);
						c->SetCollisionEnabled(useCollision?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);
						c->SetStartScale(FVector2D(id.startScale.X,id.startScale.Y),false);
						c->SetEndScale(FVector2D(id.endScale.X,id.endScale.Y),false);
						if(useRoll) {
							c->SetStartRoll(id.startRoll,false);
							c->SetEndRoll(id.endRoll,false);
						}
						c->SetStartAndEnd(id.startLocation+offset.GetTranslation(),id.startTangent,id.endLocation+offset.GetTranslation(),id.endTangent,true);
						id.smc=c;
						instData.Add(id);
						cnt++;
					}
				}
			}
		}
		if(!c) {
			c=(USplineMeshComponent*)actor->AddComponentByClass(USplineMeshComponent::StaticClass(),false,FTransform(),false);
			if(c) {
				c->SetMobility(EComponentMobility::Movable);
				c->SetCollisionEnabled(useCollision?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);
				c->SetForwardAxis(axis,false);
				c->SetSplineUpDir(spline->GetDefaultUpVector(ESplineCoordinateSpace::Local),false);
				c->SetStaticMesh(mesh);
				c->SetStartScale(FVector2D(id.startScale.X,id.startScale.Y),false);
				c->SetEndScale(FVector2D(id.endScale.X,id.endScale.Y),false);
				if(useRoll) {
					c->SetStartRoll(id.startRoll,false);
					c->SetEndRoll(id.endRoll,false);
				}
				c->SetStartAndEnd(id.startLocation+offset.GetTranslation(),id.startTangent,id.endLocation+offset.GetTranslation(),id.endTangent,true);
				id.smc=c;
				instData.Add(id);
				cnt++;
			}
		}
		currDist+=(meshLength+gap);
	}

	if(rdActor) {
		rdActor->SetSplineInstanceData(instData);
	}

	return cnt;
}

//.............................................................................
// rdPopulateSplinePMCFast
//.............................................................................
/*
int32 ArdInstBaseActor::rdPopulateSplinePMCFast(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,UStaticMesh* mesh,UMaterialInterface* mat,const FVector scale,const TEnumAsByte<ESplineMeshAxis::Type> axis) {

	if(!actor || !spline || !mesh) return 0;
	float splineLen=spline->GetSplineLength();
	if(endDistance<=0.0f || endDistance>splineLen) endDistance=splineLen;
	if(startDistance<0.0f) startDistance=0.0f;
	if(startDistance>splineLen || startDistance>endDistance) return 0;

	float meshLength=0.0f;
	FBox mbox=mesh->GetBoundingBox();
	FRotator meshRot=FRotator(0,0,0);
	switch(axis) {
		case ESplineMeshAxis::Y: meshLength=(mbox.Max.X-mbox.Min.X)*scale.X; meshRot.Yaw=90; break;
		case ESplineMeshAxis::Z: meshLength=(mbox.Max.Z-mbox.Min.Z)*scale.Z; meshRot.Pitch=90; break;
		default: meshLength=(mbox.Max.Y-mbox.Min.Y)*scale.Y; break;
	}
	if(meshLength<=0.0f) return 0;

	double currDist=0.0f;
	int32 cnt=0;

	TArray<FVector>				vertices;
	TArray<int32>				triangles;
	TArray<FVector>				normals;
	TArray<FVector2D>			UVs;
	TArray<FProcMeshTangent>	tangents;
	UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(mesh,0,0,vertices,triangles,normals,UVs,tangents);
	TArray<FLinearColor> vcols;
	TArray<FVector>	verts;
	verts.SetNum(vertices.Num());

	while(currDist<splineLen) {

		FVector startLoc=spline->GetLocationAtDistanceAlongSpline(currDist,ESplineCoordinateSpace::Local);
		//FVector endLoc=spline->GetLocationAtDistanceAlongSpline(currDist+meshLength,ESplineCoordinateSpace::Local);
		//FVector directInc=(endLoc-startLoc)/meshLength;
		//FVector startX=FVector(startLoc.X,0,0);
		float startX=startLoc.X;
		float startY=startLoc.Y;
		float startZ=startLoc.Z;

		UProceduralMeshComponent* p=(UProceduralMeshComponent*)actor->AddComponentByClass(UProceduralMeshComponent::StaticClass(),false,FTransform(FRotator(),startLoc,scale),false);
		if(p) {
			int32 i=0;
			TMap<float,FTransform> tranMap;
			for(auto& v:vertices) {
				if(!tranMap.Contains(v.X)) {
					FTransform t=spline->GetTransformAtDistanceAlongSpline(currDist+v.X,ESplineCoordinateSpace::Local);
					tranMap.Add(v.X,t);
				}
			}

			for(auto& v:vertices) {
				FTransform& t=tranMap[v.X];
				//FVector test=t.GetTranslation()-startLoc;//@@
				FVector vv(t.GetTranslation().X-startX,0,0);//t.GetTranslation().Z-startZ);
				//vv=t.Rotator().RotateVector(vv);
				verts[i++]=t.Rotator().RotateVector(v+vv);
			}

/			
			switch(axis) {
				case ESplineMeshAxis::X: {
					for(auto& v:vertices) {
						locInc=spline->GetLocationAtDistanceAlongSpline(currDist+v.Y,ESplineCoordinateSpace::Local)-startLoc;
						rot=spline->GetRotationAtDistanceAlongSpline(currDist+v.Y,ESplineCoordinateSpace::Local);
						verts[i++]=rot.RotateVector(v+locInc);
					}
					break; }
				case ESplineMeshAxis::Y: {
					//locInc.Y=locInc.Z=0.0f;
					for(auto& v:vertices) {
						//locInc=spline->GetLocationAtDistanceAlongSpline(currDist+v.Y,ESplineCoordinateSpace::Local)-startLoc;
						//locInc.X=spline->GetLocationAtDistanceAlongSpline(currDist+v.X,ESplineCoordinateSpace::Local).X-startLoc.X;
						rot=spline->GetRotationAtDistanceAlongSpline(currDist+v.X,ESplineCoordinateSpace::Local);
						verts[i++]=rot.RotateVector(v);//-locInc);
					}
					break; }
				case ESplineMeshAxis::Z: {
					for(auto& v:vertices) {
						locInc=spline->GetLocationAtDistanceAlongSpline(currDist+v.Z,ESplineCoordinateSpace::Local)-startLoc;
						rot=spline->GetRotationAtDistanceAlongSpline(currDist+v.Z,ESplineCoordinateSpace::Local);
						verts[i++]=rot.RotateVector(v+locInc);
					}
					break; }
			}
/
			p->CreateMeshSection_LinearColor(0,verts,triangles,normals,UVs,vcols,tangents,true);
		}

//		UKismetProceduralMeshLibrary::CreateGridMeshTriangles(int32 NumX, int32 NumY, bool bWinding, TArray<int32>& Triangles)
//		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents)

		currDist+=meshLength;
	}

	return cnt;
}
*/
//.............................................................................
// rdPopulateSpline
//.............................................................................
int32 ArdInstBaseActor::rdPopulateSpline(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,const TArray<FrdSplinePopulateData>& data,TArray<FrdSplineInstanceData>& instData,bool random,bool useCollision,bool useRoll) {

	if(!actor || !spline || data.Num()<=0) return 0;
	float splineLen=spline->GetSplineLength();
	if(endDistance<=0.0f || endDistance>splineLen) endDistance=splineLen;
	if(startDistance<0.0f) startDistance=0.0f;
	if(startDistance>splineLen || startDistance>endDistance) return 0;

	ArdActor* rdActor=Cast<ArdActor>(actor);
	
	double currDist=0.0f;
	int32 cnt=0,datLen=data.Num();
	FRandomStream rand;
	rand.Initialize(FDateTime::Now().GetTicks());
	while(currDist<splineLen) {
		FrdSplinePopulateData& dat=(FrdSplinePopulateData&)data[random?rand.RandRange(0,datLen-1):cnt%datLen];
		float meshLength=0.0f;
		FBox mbox=dat.mesh->GetBoundingBox();

		switch(dat.axis) {
			case ESplineMeshAxis::X: meshLength=(mbox.Max.X-mbox.Min.X)*dat.offset.GetScale3D().X; break;
			case ESplineMeshAxis::Z: meshLength=(mbox.Max.Z-mbox.Min.Z)*dat.offset.GetScale3D().Z; break;
			default: meshLength=(mbox.Max.Y-mbox.Min.Y)*dat.offset.GetScale3D().Y; break;
		}
		if(meshLength>0.0f) {

			FrdSplineInstanceData id;
			id.startLocation=spline->GetLocationAtDistanceAlongSpline(currDist,ESplineCoordinateSpace::Local);
			id.endLocation=spline->GetLocationAtDistanceAlongSpline(currDist+meshLength,ESplineCoordinateSpace::Local);
			id.startTangent=spline->GetDirectionAtDistanceAlongSpline(currDist,ESplineCoordinateSpace::Local)*meshLength;
			id.endTangent=spline->GetDirectionAtDistanceAlongSpline(currDist+meshLength,ESplineCoordinateSpace::Local)*meshLength;
			id.startScale=spline->GetScaleAtDistanceAlongSpline(currDist);
			id.endScale=spline->GetScaleAtDistanceAlongSpline(currDist+meshLength);
			id.startRoll=spline->GetRollAtDistanceAlongSpline(currDist,ESplineCoordinateSpace::Local);
			id.endRoll=spline->GetRollAtDistanceAlongSpline(currDist+meshLength,ESplineCoordinateSpace::Local);
			USplineMeshComponent* c=nullptr;
			if(rdActor) {
				TArray<FrdSplineInstanceData>& ilst=rdActor->GetSplineInstanceData();
				if(ilst.Num()>cnt) {
					FrdSplineInstanceData& idat=ilst[cnt];
					if(idat==id) {
						c=idat.smc;
						if(c) {
							id.smc=c;
							instData.Add(id);
							cnt++;
						}
					} else {
						c=idat.smc;
						if(c) {
							c->SetStaticMesh(dat.mesh);
							c->SetCollisionEnabled(useCollision?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);
							c->SetStartScale(FVector2D(id.startScale.X,id.startScale.Y),false);
							c->SetEndScale(FVector2D(id.endScale.X,id.endScale.Y),false);
							if(useRoll) {
								c->SetStartRoll(id.startRoll,false);
								c->SetEndRoll(id.endRoll,false);
							}
							c->SetStartAndEnd(id.startLocation+dat.offset.GetTranslation(),id.startTangent,id.endLocation+dat.offset.GetTranslation(),id.endTangent,true);
							int32 i=0;
							for(auto f:dat.customData) c->SetCustomPrimitiveDataFloat(i++,f);
							id.smc=c;
							instData.Add(id);
							cnt++;
						}
					}
				}
			}
			if(!c) {
				c=(USplineMeshComponent*)actor->AddComponentByClass(USplineMeshComponent::StaticClass(),false,FTransform(),false);
				if(c) {
					c->SetMobility(EComponentMobility::Movable);
					c->SetCollisionEnabled(useCollision?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);
					c->SetForwardAxis(dat.axis,false);
					c->SetSplineUpDir(spline->GetDefaultUpVector(ESplineCoordinateSpace::Local),false);
					c->SetStaticMesh(dat.mesh);
					c->SetStartScale(FVector2D(id.startScale.X,id.startScale.Y),false);
					c->SetEndScale(FVector2D(id.endScale.X,id.endScale.Y),false);
					if(useRoll) {
						c->SetStartRoll(id.startRoll,false);
						c->SetEndRoll(id.endRoll,false);
					}
					c->SetStartAndEnd(id.startLocation+dat.offset.GetTranslation(),id.startTangent,id.endLocation+dat.offset.GetTranslation(),id.endTangent,true);
					int32 i=0;
					for(auto f:dat.customData) c->SetCustomPrimitiveDataFloat(i++,f);
					id.smc=c;
					instData.Add(id);
					cnt++;
				}
			}
		} else {
			meshLength=50; // to stop an infinite loop
		}

		currDist+=(meshLength+dat.gap);
	}

	if(rdActor) {
		rdActor->SetSplineInstanceData(instData);
	}

	return cnt;
}

//.............................................................................
// rdPopulateSplinePMC
//.............................................................................
/*
int32 ArdInstBaseActor::rdPopulateSplinePMC(AActor* actor,USplineComponent* spline,float startDistance,float endDistance,const TArray<FrdSplinePopulateData>& data,TArray<FrdSplineInstanceData>& instData,bool random) {

	if(!actor || !spline || data.Num()<=0) return 0;
	float splineLen=spline->GetSplineLength();
	if(endDistance<=0.0f || endDistance>splineLen) endDistance=splineLen;
	if(startDistance<0.0f) startDistance=0.0f;
	if(startDistance>splineLen || startDistance>endDistance) return 0;
		

	return 0;
}
*/
//.............................................................................
// rdRemoveSplineSMCs
//.............................................................................
void ArdInstBaseActor::rdRemoveSplineSMCs(AActor* actor,USplineComponent* spline) {

	if(!actor || !spline) return;

	TArray<UActorComponent*> cac;
	actor->GetComponents(USplineMeshComponent::StaticClass(),cac,false);
	for(auto c:cac)	{
		c->DestroyComponent();
	}
}

//.............................................................................
// rdRemoveSplinePMCs
//.............................................................................
/*
void ArdInstBaseActor::rdRemoveSplinePMCs(AActor* actor,USplineComponent* spline) {

	if(!actor || !spline) return;

	TArray<UActorComponent*> cac;
	actor->GetComponents(UProceduralMeshComponent::StaticClass(),cac,false);
	for(auto c:cac)	{
		c->DestroyComponent();
	}
}
*/
//.............................................................................
