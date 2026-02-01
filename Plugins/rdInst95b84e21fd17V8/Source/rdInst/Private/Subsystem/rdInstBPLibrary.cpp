//
// rdInstBPLibrary.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.00
//
// Creation Date: 1st October 2022
// Last Modified: 9th November 2022
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBPLibrary.h"
#include "EngineUtils.h"
#include "rdInst.h"

//----------------------------------------------------------------------------------------------------------------
// GetRandomizeUserData
//
// This is the AssetUserData that contains the Randomization Settings for Actors. This is set into either the Actors
// in your level or the ChildActorComponents in your Blueprints.
// rdBPtools contains a settings window from where these settings can be easily changed.
// Or you can manually add the AssetUserData to the StaticMesh from the StaticMeshEditor, and change the settings in there.
//
//----------------------------------------------------------------------------------------------------------------
UrdRandomizeAssetUserData* rdInstLib::GetRandomizeUserData(AActor* actor,bool make) {
	
	if(!actor) return nullptr;
	USceneComponent* sc=actor->GetRootComponent();
	if(!sc) return nullptr;

	UrdRandomizeAssetUserData* tag=(UrdRandomizeAssetUserData*)sc->GetAssetUserDataOfClass(UrdRandomizeAssetUserData::StaticClass());
	if(!tag && make) {
		tag=NewObject<UrdRandomizeAssetUserData>(sc);
		tag->SetName(TEXT("rdRandomize"));
		sc->AddAssetUserData(tag);
	}

	return tag;
}

void rdInstLib::RemoveRandomizeUserData(AActor* actor) {

	USceneComponent* sc=actor->GetRootComponent();
	if(!sc) return;
	sc->RemoveUserDataOfClass(UrdRandomizeAssetUserData::StaticClass());
}

//----------------------------------------------------------------------------------------------------------------
// GetRandomizeUserData
//
// This is the AssetUserData that contains the Randomization Settings for Components. This is set into either the Actors
// in your level or the ChildActorComponents in your Blueprints.
// rdBPtools contains a settings window from where these settings can be easily changed.
// Or you can manually add the AssetUserData to the StaticMesh from the StaticMeshEditor, and change the settings in there.
//
//----------------------------------------------------------------------------------------------------------------
bool getThreeRotators(const FString& str,FRotator& r1,FRotator& r2,FVector& v) {

	TArray<FString> nums;
	int32 num=str.ParseIntoArray(nums,TEXT(","));
	
	if(num==9) { // 2 rotators and 1 vector

		r1.Roll=FCString::Atof(*nums[0]);
		r1.Pitch=FCString::Atof(*nums[1]);
		r1.Yaw=FCString::Atof(*nums[2]);

		r2.Roll=FCString::Atof(*nums[3]);
		r2.Pitch=FCString::Atof(*nums[4]);
		r2.Yaw=FCString::Atof(*nums[5]);

		v.X=FCString::Atof(*nums[6]);
		v.Y=FCString::Atof(*nums[7]);
		v.Z=FCString::Atof(*nums[8]);

		return true;
	}

	return false;
}

bool getTwoRotators(const FString& str,FRotator& r1,FRotator& r2) {

	TArray<FString> nums;
	int32 num=str.ParseIntoArray(nums,TEXT(","));
	
	if(num==6) { // 2 rotators

		r1.Roll=FCString::Atof(*nums[0]);
		r1.Pitch=FCString::Atof(*nums[1]);
		r1.Yaw=FCString::Atof(*nums[2]);

		r2.Roll=FCString::Atof(*nums[3]);
		r2.Pitch=FCString::Atof(*nums[4]);
		r2.Yaw=FCString::Atof(*nums[5]);

		return true;
	}

	return false;
}

bool getTwoVectors(const FString& str,FVector& v1,FVector& v2) {

	TArray<FString> nums;
	int32 num=str.ParseIntoArray(nums,TEXT(","));

	if(num==6) { // 2 vectors

		v1.X=FCString::Atof(*nums[0]);
		v1.Y=FCString::Atof(*nums[1]);
		v1.Z=FCString::Atof(*nums[2]);

		v2.X=FCString::Atof(*nums[3]);
		v2.Y=FCString::Atof(*nums[4]);
		v2.Z=FCString::Atof(*nums[5]);

		return true;
	}

	return false;
}

//..................................................................................................
// GetRandomizeUserDataFromTags
//..................................................................................................
UrdRandomizeAssetUserData* rdInstLib::GetRandomizeUserDataFromTags(TArray<FName>& tags) {

	UrdRandomizeAssetUserData* tag=NewObject<UrdRandomizeAssetUserData>();
	tag->SetName(TEXT("rdRandomize"));

	tag->location1=FVector(0,0,0);
	tag->location2=FVector(0,0,0);
	tag->rotation1=FRotator(0,0,0);
	tag->rotation2=FRotator(0,0,0);
	tag->flipProbability=FVector(0,0,0);
	tag->lockAxis=0;
	tag->scale1=FVector(1,1,1);
	tag->scale2=FVector(1,1,1);
	tag->showProbability=1.0f;

	for(auto stag:tags) {

		FString st=stag.ToString();

		if(st.StartsWith(TEXT("rdLoc="))) {
			FVector v1,v2;
			if(getTwoVectors(st.RightChop(6),v1,v2)) {

				tag->location1=v1;
				tag->location2=v2;
			}

		} else if(st.StartsWith(TEXT("rdRot="))) {
			FRotator v1,v2;
			FVector v3;
			if(getThreeRotators(st.RightChop(6),v1,v2,v3)) {

				tag->rotation1=v1;
				tag->rotation2=v2;
				tag->flipProbability=v3;

			} else if(getTwoRotators(st.RightChop(6),v1,v2)) {

				tag->rotation1=v1;
				tag->rotation2=v2;
				tag->flipProbability=FVector(0,0,0);
			}

		} else if(st.StartsWith(TEXT("rdScale="))) {
			FVector v1=FVector(),v2=FVector();

			TArray<FString> nums;
			int32 num=st.RightChop(8).ParseIntoArray(nums,TEXT(","));

			if(num==6) { // 2 vectors (no locked axis)

				v1.X=FCString::Atof(*nums[0]);
				v1.Y=FCString::Atof(*nums[1]);
				v1.Z=FCString::Atof(*nums[2]);

				v2.X=FCString::Atof(*nums[3]);
				v2.Y=FCString::Atof(*nums[4]);
				v2.Z=FCString::Atof(*nums[5]);

				tag->lockAxis=0;

			} else if(num==4) { // 2 vectors (locked X&Y axis)

				v1.X=v1.Y=FCString::Atof(*nums[0]);
				v1.Z=FCString::Atof(*nums[1]);

				v2.X=v2.Y=FCString::Atof(*nums[2]);
				v2.Z=FCString::Atof(*nums[3]);

				tag->lockAxis=1;

			} else if(num==2) { // 2 vectors (locked All axis)

				v1.X=v1.Y=v1.Z=FCString::Atof(*nums[0]);
				v2.X=v2.Y=v2.Z=FCString::Atof(*nums[1]);

				tag->lockAxis=2;
			}

			tag->scale1=v1;
			tag->scale2=v2;

		} else if(st.StartsWith(TEXT("rdShow="))) {
			if(st.Find(TEXT("|"))!=INDEX_NONE) {
				TArray<FString> valList;
				st.RightChop(7).ParseIntoArray(valList,TEXT("|"));
				if(valList.Num()==2) {
					tag->showProbability=FCString::Atof(*valList[0]);
					tag->themes=valList[1];
				}
			} else {
				tag->showProbability=FCString::Atof(*st.RightChop(7));
			}

		} else if(st.StartsWith(TEXT("rdRely="))) {
			tag->relyActorName=st.RightChop(7);
		}
	}

	return tag;
}

UrdRandomizeAssetUserData* rdInstLib::GetRandomizeUserData(UActorComponent* actor,bool make) {

	UrdRandomizeAssetUserData* tag=(UrdRandomizeAssetUserData*)actor->GetAssetUserDataOfClass(UrdRandomizeAssetUserData::StaticClass());
	if(!tag && make) {
		tag=NewObject<UrdRandomizeAssetUserData>(actor);
		tag->SetName(TEXT("rdRandomize"));

		tag->location1=FVector(0,0,0);
		tag->location2=FVector(0,0,0);
		tag->rotation1=FRotator(0,0,0);
		tag->rotation2=FRotator(0,0,0);
		tag->flipProbability=FVector(0,0,0);
		tag->lockAxis=0;
		tag->scale1=FVector(1,1,1);
		tag->scale2=FVector(1,1,1);
		tag->showProbability=1.0f;

		for(auto stag:actor->ComponentTags) {

			FString st=stag.ToString();

			if(st.StartsWith(TEXT("rdLoc="))) {
				FVector v1,v2;
				if(getTwoVectors(st.RightChop(6),v1,v2)) {

					tag->location1=v1;
					tag->location2=v2;
				}

			} else if(st.StartsWith(TEXT("rdRot="))) {
				FRotator v1,v2;
				FVector v3;
				if(getThreeRotators(st.RightChop(6),v1,v2,v3)) {

					tag->rotation1=v1;
					tag->rotation2=v2;
					tag->flipProbability=v3;

				} else if(getTwoRotators(st.RightChop(6),v1,v2)) {

					tag->rotation1=v1;
					tag->rotation2=v2;
					tag->flipProbability=FVector(0,0,0);
				}

			} else if(st.StartsWith(TEXT("rdScale="))) {
				FVector v1=FVector(),v2=FVector();

				TArray<FString> nums;
				int32 num=st.RightChop(8).ParseIntoArray(nums,TEXT(","));

				if(num==6) { // 2 vectors (no locked axis)

					v1.X=FCString::Atof(*nums[0]);
					v1.Y=FCString::Atof(*nums[1]);
					v1.Z=FCString::Atof(*nums[2]);

					v2.X=FCString::Atof(*nums[3]);
					v2.Y=FCString::Atof(*nums[4]);
					v2.Z=FCString::Atof(*nums[5]);

					tag->lockAxis=0;

				} else if(num==4) { // 2 vectors (locked X&Y axis)

					v1.X=v1.Y=FCString::Atof(*nums[0]);
					v1.Z=FCString::Atof(*nums[1]);

					v2.X=v2.Y=FCString::Atof(*nums[2]);
					v2.Z=FCString::Atof(*nums[3]);

					tag->lockAxis=1;

				} else if(num==2) { // 2 vectors (locked All axis)

					v1.X=v1.Y=v1.Z=FCString::Atof(*nums[0]);
					v2.X=v2.Y=v2.Z=FCString::Atof(*nums[1]);

					tag->lockAxis=2;
				}

				tag->scale1=v1;
				tag->scale2=v2;

			} else if(st.StartsWith(TEXT("rdShow="))) {
				if(st.Find(TEXT("|"))!=INDEX_NONE) {
					TArray<FString> valList;
					st.RightChop(7).ParseIntoArray(valList,TEXT("|"));
					if(valList.Num()==2) {
						tag->showProbability=FCString::Atof(*valList[0]);
						tag->themes=valList[1];
					}
				} else {
					tag->showProbability=FCString::Atof(*st.RightChop(7));
				}

			} else if(st.StartsWith(TEXT("rdRely="))) {
				tag->relyActorName=st.RightChop(7);
			}
		}

		actor->AddAssetUserData(tag);
	}

	return tag;
}

void rdInstLib::RemoveRandomizeUserData(UActorComponent* actor) {

	actor->RemoveUserDataOfClass(UrdRandomizeAssetUserData::StaticClass());
}

//----------------------------------------------------------------------------------------------------------------
// GetTransformUserData
//
// This AssetUserData gets set into the Child Actors in your Blueprints. 
// It contains the original location of the actor (before any Randomization) and flags indicating visibility.
//
//----------------------------------------------------------------------------------------------------------------
bool getOneRotator(const FString& str,FRotator& r1) {

	TArray<FString> nums;
	int32 num=str.ParseIntoArray(nums,TEXT(","));
	
	if(num==3) { // 1 rotator

		r1.Roll=FCString::Atof(*nums[0]);
		r1.Pitch=FCString::Atof(*nums[1]);
		r1.Yaw=FCString::Atof(*nums[2]);

		return true;
	}

	return false;
}
bool getOneVector(const FString& str,FVector& v1) {

	TArray<FString> nums;
	int32 num=str.ParseIntoArray(nums,TEXT(","));

	if(num==3) { // 2 vector

		v1.X=FCString::Atof(*nums[0]);
		v1.Y=FCString::Atof(*nums[1]);
		v1.Z=FCString::Atof(*nums[2]);

		return true;
	}

	return false;
}

UrdTransformAssetUserData* rdInstLib::GetTransformUserData(UActorComponent* comp,bool make) {

	TArray<FName>& tags=comp->ComponentTags;
	UrdTransformAssetUserData* ftag=(UrdTransformAssetUserData*)comp->GetAssetUserDataOfClass(UrdTransformAssetUserData::StaticClass());
	if(!ftag && make) {

		// the UAssetUserData does't exist yet, create one from this Components Tags
		FVector loc(0,0,0),scale(1,1,1);
		FRotator rot(0,0,0);
		bool noVis=false;
		for(auto tag:tags) {
			FString st=tag.ToString();
			if(st.StartsWith(TEXT("loc="))) {
				getOneVector(st.RightChop(4),loc);
			} else if(st.StartsWith(TEXT("rot="))) {
				getOneRotator(st.RightChop(4),rot);
			} else if(st.StartsWith(TEXT("scale="))) {
				getOneVector(st.RightChop(6),scale);
			} else if(st==TEXT("rdNoVis")) {
				noVis=true;
			}
		}

		ftag=NewObject<UrdTransformAssetUserData>(comp);
		ftag->SetName(TEXT("rdTransform"));
		ftag->bNoVis=noVis;
		ftag->origTransform=FTransform(rot,loc,scale);
		comp->AddAssetUserData(ftag);
	}

	return ftag;
}

void rdInstLib::RemoveTransformUserData(UActorComponent* actor) {

	actor->RemoveUserDataOfClass(UrdTransformAssetUserData::StaticClass());
}

//.............................................................................
//  CopyFromOther (called from CopyConstructor, used in Maps
//.............................................................................
void FrdRandomSettings::CopyFromOther(const FrdRandomSettings& other) {

	location1=other.location1;
	location2=other.location2;
	rotation1=other.rotation1;
	rotation2=other.rotation2;
	flipProbability=other.flipProbability;
	lockAxis=other.lockAxis;
	scale1=other.scale1;
	scale2=other.scale2;
	showProbability=other.showProbability;
	relyActorName=other.relyActorName;
	themes=other.themes;
}

//.............................................................................

