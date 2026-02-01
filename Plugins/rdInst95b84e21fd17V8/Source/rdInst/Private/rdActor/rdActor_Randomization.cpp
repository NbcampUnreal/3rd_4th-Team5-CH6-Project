//
// rdActor_Randomization.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 14th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "Kismet/KismetMathLibrary.h"

//.............................................................................
//  rdRandomize
//.............................................................................
void ArdActor::rdRandomize() {

	ArdActor* rdActor=Cast<ArdActor>(GetParentActor());
	if(!bOverrideRandom) {
		randomSeed=FDateTime::Now().GetTicks();
		randomStream.Initialize(randomSeed);
	}

	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(true,comps);
	for(auto c:comps) {
		UChildActorComponent* ca=Cast<UChildActorComponent>(c);
		if(ca) {
			ArdActor* rdAct=Cast<ArdActor>(ca->GetChildActor());
			if(rdAct) {
				rdAct->rdRandomize();
			}
		}
	}
}

//.............................................................................
// rdRandomVectorInRange
//.............................................................................
FVector ArdActor::rdRandomVectorInRange(const FVector& v1,const FVector& v2) {
	FVector v;
	v.X=rdGetRandomStream().FRandRange(v1.X,v2.X);
	v.Y=rdGetRandomStream().FRandRange(v1.Y,v2.Y);
	v.Z=rdGetRandomStream().FRandRange(v1.Z,v2.Z);
	return v;
}

//.............................................................................
// rdRandomRotatorInRange
//.............................................................................
FRotator ArdActor::rdRandomRotatorInRange(const FRotator& r1,const FRotator& r2,const FVector& flipProb) {

	FRotator r;
	r.Roll=rdGetRandomStream().FRandRange(r1.Roll,r2.Roll);
	r.Pitch=rdGetRandomStream().FRandRange(r1.Pitch,r2.Pitch);
	r.Yaw=rdGetRandomStream().FRandRange(r1.Yaw,r2.Yaw);

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	if(UKismetMathLibrary::RandomBoolWithWeightFromStream(rdGetRandomStream(),flipProb.X)) { r.Roll+=180.0f; }
	if(UKismetMathLibrary::RandomBoolWithWeightFromStream(rdGetRandomStream(),flipProb.Y)) { r.Pitch+=180.0f; }
	if(UKismetMathLibrary::RandomBoolWithWeightFromStream(rdGetRandomStream(),flipProb.Z)) { r.Yaw+=180.0f; }
#else
	if(UKismetMathLibrary::RandomBoolWithWeightFromStream(flipProb.X,rdGetRandomStream())) { r.Roll+=180.0f; }
	if(UKismetMathLibrary::RandomBoolWithWeightFromStream(flipProb.Y,rdGetRandomStream())) { r.Pitch+=180.0f; }
	if(UKismetMathLibrary::RandomBoolWithWeightFromStream(flipProb.Z,rdGetRandomStream())) { r.Yaw+=180.0f; }
#endif
	return r;
}

bool ArdActor::rdRelianceChainVisible(const FString& name) {

	if(name.IsEmpty()) return true; // not relying on any other item

	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		FrdInstanceSettingsArray& i=it.Value;

		for(FrdInstanceFastSettings& j:i.settingsFast) {
			if(j.name==name) {
				return !j.bNoVis;
			}
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
			if(j.name==name) {
				return (j.bVisible && rdRelianceChainVisible(j.randomSettings.relyActorName));
			}
		}
	}

	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(true,comps);
	for(auto c:comps) {
		if(c->GetName()==name) {
			UrdTransformAssetUserData* ftag=rdInstLib::GetTransformUserData(c,true);
			UChildActorComponent* cac=Cast<UChildActorComponent>(c);
			AActor* actor=nullptr;
			if(cac) actor=(AActor*)cac->GetChildActor();
			if(actor) return !actor->IsHidden();
			else c->IsVisible();
		}
	}

	return true; // maybe return false to hide if the rely actor is not found
}

//.............................................................................
// rdAddRandomization
//
// This routine Applies any Randomization settings to the passed in Transform.
// See rdInstBPLibrary.cpp for a description of what it does.
//
//.............................................................................
bool ArdActor::rdAddRandomization(FTransform& outTransform,FrdRandomSettings* rnd) {

	if(!rnd) return true;

#if ENGINE_MAJOR_VERSION>4 && ENGINE_MINOR_VERSION>1
	bool isVis=UKismetMathLibrary::RandomBoolWithWeightFromStream(rdGetRandomStream(),rnd->showProbability);
#else
	bool isVis=UKismetMathLibrary::RandomBoolWithWeightFromStream(rnd->showProbability,rdGetRandomStream());
#endif
	if(!isVis) {
		return false;
	}

	FString theme=rdGetTheme();
	if(!theme.IsEmpty() && !rnd->themes.IsEmpty()) {
		TArray<FString> sthemes;
		theme.ParseIntoArray(sthemes,TEXT(","));
		TArray<FString> themes;
		int32 num=rnd->themes.ParseIntoArray(themes,TEXT(","));
		if(themes.Num()>0) { 
			bool found=false;
			for(auto t:sthemes) {
				if(themes.Contains(t)) {
					found=true;
					break;
				}
			}
			if(!found) return false;
		}
	}

	FVector loc=rdRandomVectorInRange(rnd->location1,rnd->location2);
	FRotator rot=rdRandomRotatorInRange(rnd->rotation1,rnd->rotation2,rnd->flipProbability);
	FVector scale=rdRandomVectorInRange(rnd->scale1,rnd->scale2);
	if(rnd->lockAxis==2) scale.Y=scale.Z=scale.X;
	else if(rnd->lockAxis==1) scale.Y=scale.X;

	FTransform transform(rot,loc,scale);
	outTransform.Accumulate(transform);

	return true;
}

//.............................................................................
// rdHasRandomTags
//.............................................................................
bool ArdActor::rdHasRandomTags(AActor* actor) {

	TArray<FName>& tags=actor->Tags;
	for(auto& tag:tags) {
		FString st=tag.ToString();
		if(st.StartsWith(TEXT("rdLoc="))) return true;
		if(st.StartsWith(TEXT("rdRot="))) return true;
		if(st.StartsWith(TEXT("rdScale="))) return true;
		if(st.StartsWith(TEXT("rdShow="))) return true;
		if(st.StartsWith(TEXT("rdRely="))) return true;
	}
	return false;
}

bool ArdActor::rdHasRandomTags(UActorComponent* comp) {

	TArray<FName>& tags=comp->ComponentTags;
	for(auto& tag:tags) {
		FString st=tag.ToString();
		if(st.StartsWith(TEXT("rdLoc="))) return true;
		if(st.StartsWith(TEXT("rdRot="))) return true;
		if(st.StartsWith(TEXT("rdScale="))) return true;
		if(st.StartsWith(TEXT("rdShow="))) return true;
		if(st.StartsWith(TEXT("rdRely="))) return true;
	}
	return false;
}

//.............................................................................
// rdSetRandomFromTags
//
// Fills in the Randomization settings from the specifed Actor or Component.
// These settings can be manually added and edited, or you can use rdBPtools
// to easily change them.
//
//.............................................................................
void ArdActor::rdSetRandomFromJustTags(TArray<FName>& tags,FrdRandomSettings* options) {

	// Try getting it from the UAssetUserData first
	UrdRandomizeAssetUserData* ftag=rdInstLib::GetRandomizeUserDataFromTags(tags);
	if(ftag) {

		options->location1=ftag->location1;
		options->location2=ftag->location2;
		options->rotation1=ftag->rotation1;
		options->rotation2=ftag->rotation2;
		options->scale1=ftag->scale1;
		options->scale2=ftag->scale2;
		options->flipProbability=ftag->flipProbability;
		options->showProbability=ftag->showProbability;
		options->lockAxis=ftag->lockAxis;
		options->relyActorName=ftag->relyActorName;
		options->themes=ftag->themes;

		return;
	}

	// the UAssetUserData can't exist yet, create one from this Components Tags
	for(auto& tag:tags) {

		FString st=tag.ToString();

		if(st.StartsWith(TEXT("rdLoc="))) {
			FVector v1,v2;
			if(getTwoVectors(st.RightChop(6),v1,v2)) {

				options->location1=v1;
				options->location2=v2;
			}

		} else if(st.StartsWith(TEXT("rdRot="))) {
			FRotator v1=FRotator(),v2=FRotator();
			FVector v3=FVector();
			if(getThreeRotators(st.RightChop(6),v1,v2,v3)) {

				options->rotation1=v1;
				options->rotation2=v2;
				options->flipProbability=v3;

			} else if(getTwoRotators(st.RightChop(6),v1,v2)) {

				options->rotation1=v1;
				options->rotation2=v2;
				options->flipProbability=FVector(0,0,0);
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

				options->lockAxis=0;

			} else if(num==4) { // 2 vectors (locked X&Y axis)

				v1.X=v1.Y=FCString::Atof(*nums[0]);
				v1.Z=FCString::Atof(*nums[1]);

				v2.X=v2.Y=FCString::Atof(*nums[2]);
				v2.Z=FCString::Atof(*nums[3]);

				options->lockAxis=1;

			} else if(num==2) { // 2 vectors (locked All axis)

				v1.X=v1.Y=v1.Z=FCString::Atof(*nums[0]);
				v2.X=v2.Y=v2.Z=FCString::Atof(*nums[1]);

				options->lockAxis=2;
			}

			options->scale1=v1;
			options->scale2=v2;

		} else if(st.StartsWith(TEXT("rdShow="))) {
			if(st.Find(TEXT("|"))!=INDEX_NONE) {
				TArray<FString> valList;
				st.RightChop(7).ParseIntoArray(valList,TEXT("|"));
				if(valList.Num()==2) {
					options->showProbability=FCString::Atof(*valList[0]);
					options->themes=valList[1];
				}
			} else {
				options->showProbability=FCString::Atof(*st.RightChop(7));
			}

		} else if(st.StartsWith(TEXT("rdRely="))) {
			options->relyActorName=st.RightChop(7);
		}
	}
}

void ArdActor::rdSetRandomFromActor(AActor* actor,FrdRandomSettings* options) {
	rdSetRandomFromJustTags(actor->Tags,options);
}

void ArdActor::rdSetRandomFromComponent(UActorComponent* comp,FrdRandomSettings* options) {
	rdSetRandomFromJustTags(comp->ComponentTags,options);
}

//.................................................................................................

// Helper to extract 2 FRotators and a Vector from the String (used by the RandomRotationAndFlip)
bool ArdActor::getThreeRotators(const FString& str,FRotator& r1,FRotator& r2,FVector& v) {

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

// Helper to extract 2 FRotators from the string
bool ArdActor::getTwoRotators(const FString& str,FRotator& r1,FRotator& r2) {

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

// Helper to extract 1 FRotator from the string
bool ArdActor::getOneRotator(const FString& str,FRotator& r1) {

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

// Helper to extract 2 FVectors from the string
bool ArdActor::getTwoVectors(const FString& str,FVector& v1,FVector& v2) {

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

// Helper to extract 1 FVector from the string
bool ArdActor::getOneVector(const FString& str,FVector& v1) {

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

//.............................................................................
// rdGetRandomStream
//
// Returns the Eldest Parents RandomStream - Sharing the RandomStream keeps
// all ancestors producing the same random numbers each time (via the seed)
//
//.............................................................................
FRandomStream& ArdActor::rdGetRandomStream() {

	if(!bOverrideRandom) {
		ArdActor* rdActor=Cast<ArdActor>(GetParentActor());
		if(!rdActor) rdActor=Cast<ArdActor>(GetOwner());
		if(rdActor) return rdActor->rdGetRandomStream();
	}
	return randomStream;
}

//.............................................................................
// rdSetRandomFromTags
//.............................................................................
bool ArdActor::rdSetRandomFromTags(TArray<FName>& tags,FrdRandomSettings* options) {

	bool hasTags=false;

	for(auto& tag:tags) {

		FString st=tag.ToString();

		if(st.StartsWith(TEXT("rdLoc="))) {
			hasTags=true;
			FVector v1,v2;
			if(getTwoVectors(st.RightChop(6),v1,v2)) {

				options->location1=v1;
				options->location2=v2;
			}

		} else if(st.StartsWith(TEXT("rdRot="))) {
			hasTags=true;
			FRotator v1,v2;
			FVector v3;
			if(getThreeRotators(st.RightChop(6),v1,v2,v3)) {

				options->rotation1=v1;
				options->rotation2=v2;
				options->flipProbability=v3;

			} else if(getTwoRotators(st.RightChop(6),v1,v2)) {

				options->rotation1=v1;
				options->rotation2=v2;
				options->flipProbability=FVector(0,0,0);
			}

		} else if(st.StartsWith(TEXT("rdScale="))) {
			hasTags=true;
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

				options->lockAxis=0;

			} else if(num==4) { // 2 vectors (locked X&Y axis)

				v1.X=v1.Y=FCString::Atof(*nums[0]);
				v1.Z=FCString::Atof(*nums[1]);

				v2.X=v2.Y=FCString::Atof(*nums[2]);
				v2.Z=FCString::Atof(*nums[3]);

				options->lockAxis=1;

			} else if(num==2) { // 2 vectors (locked All axis)

				v1.X=v1.Y=v1.Z=FCString::Atof(*nums[0]);
				v2.X=v2.Y=v2.Z=FCString::Atof(*nums[1]);

				options->lockAxis=2;
			}

			options->scale1=v1;
			options->scale2=v2;

		} else if(st.StartsWith(TEXT("rdShow="))) {
			hasTags=true;
			if(st.Find(TEXT("|"))!=INDEX_NONE) {
				TArray<FString> valList;
				st.RightChop(7).ParseIntoArray(valList,TEXT("|"));
				if(valList.Num()==2) {
					options->showProbability=FCString::Atof(*valList[0]);
					options->themes=valList[1];
				}
			} else {
				options->showProbability=FCString::Atof(*st.RightChop(7));
			}

		} else if(st.StartsWith(TEXT("rdRely="))) {
			hasTags=true;
			options->relyActorName=st.RightChop(7);
		}
	}
	return hasTags;
}

void ArdActor::rdCreateTagsFromRandom(TArray<FName>& tags,const FrdRandomSettings& random) {

	FVector vZ(0,0,0),vOne(1,1,1);
	FRotator rZ(0,0,0);

	const FVector& loc1=random.location1;
	const FVector& loc2=random.location2;
	if(loc1!=vZ || loc2!=vZ) {
		tags.Add(FName(*FString::Printf(TEXT("rdLoc=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),loc1.X,loc1.Y,loc1.Z,loc2.X,loc2.Y,loc2.Z)));
	}

	const FRotator& rot1=random.rotation1;
	const FRotator& rot2=random.rotation2;
	const FVector& flip=random.flipProbability;
	if(rot1!=rZ || rot2!=rZ || flip!=vZ) {
		if(flip!=vZ) {
			tags.Add(FName(*FString::Printf(TEXT("rdRot=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),rot1.Roll,rot1.Pitch,rot1.Yaw,rot2.Roll,rot2.Pitch,rot2.Yaw,flip.X,flip.Y,flip.Z)));
		} else {
			tags.Add(FName(*FString::Printf(TEXT("rdRot=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),rot1.Roll,rot1.Pitch,rot1.Yaw,rot2.Roll,rot2.Pitch,rot2.Yaw)));
		}
	}

	const FVector& scale1=random.scale1;
	const FVector& scale2=random.scale2;
	if(scale1!=vOne || scale2!=vOne) {
		if(random.lockAxis==2) tags.Add(FName(*FString::Printf(TEXT("rdScale=%.6g,%.6g"),scale1.X,scale2.X)));
		else if(random.lockAxis==1) tags.Add(FName(*FString::Printf(TEXT("rdScale=%.6g,%.6g,%.6g,%.6g"),scale1.X,scale1.Z,scale2.X,scale2.Z)));
		else tags.Add(FName(*FString::Printf(TEXT("rdScale=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),scale1.X,scale1.Y,scale1.Z,scale2.X,scale2.Y,scale2.Z)));
	}

	// Show Probability and Themes
	float show=random.showProbability;
	const FString& themes=random.themes;

	if(show<1.0f || !themes.IsEmpty()) {

		if(!themes.IsEmpty()) {
			tags.Add(FName(*FString::Printf(TEXT("rdShow=%.6g|%s"),show,*themes)));
		} else {
			tags.Add(FName(*FString::Printf(TEXT("rdShow=%.6g"),show)));
		}
	}

	// Rely on Actor
	const FString& rely=random.relyActorName;
	if(!rely.IsEmpty()) {
		tags.Add(FName(*FString::Printf(TEXT("rdRely=%s"),*rely)));
	}
}

//.............................................................................
