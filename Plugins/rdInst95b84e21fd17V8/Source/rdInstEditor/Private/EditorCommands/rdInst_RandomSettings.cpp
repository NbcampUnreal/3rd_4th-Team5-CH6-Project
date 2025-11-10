//
// rdInst_RandomSettings.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 3rd Septamber 2024
// Last Modified: 14th May 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "Interfaces/IMainFrameModule.h"
#include "Editor/UnrealEd/Public/ScopedTransaction.h"
#include "Runtime/Core/Public/Misc/ScopedSlowTask.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/InheritableComponentHandler.h"
#include "Engine/SCS_Node.h"
#include "Dialogs/Dialogs.h"
#include "Misc/MessageDialog.h"
#include "Engine/Selection.h"
#include "FileHelpers.h"
#include "BlueprintEditor.h"

#include "rdInstOptions.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

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

//----------------------------------------------------------------------------------------------------------------
// SetRandomFromTags
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::SetRandomFromTags(TArray<FName>& tags) {

	FrdRandomSettings opt;
	SetRandomFromTags(tags,&opt);

	rdInstOptions->rndLocation1=opt.location1;
	rdInstOptions->rndLocation2=opt.location2;
	rdInstOptions->rndRotation1=opt.rotation1;
	rdInstOptions->rndRotation2=opt.rotation2;
	rdInstOptions->rndScale1=opt.scale1;
	rdInstOptions->rndScale2=opt.scale2;
	rdInstOptions->rndFlipProbability=opt.flipProbability;
	rdInstOptions->rndShowProbability=opt.showProbability;
	rdInstOptions->lockAxis=opt.lockAxis;
	rdInstOptions->rndRelyOnActorName=opt.relyActorName;
	rdInstOptions->themes=opt.themes;
}

bool FrdInstEditorModule::SetRandomFromTags(TArray<FName>& tags,FrdRandomSettings* options) {

	bool hasTags=false;

	for(auto tag:tags) {

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

//----------------------------------------------------------------------------------------------------------------
// MergeRandomFromTags
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::MergeRandomFromTags(TArray<FName>& tags) {

	FrdRandomSettings opt;
	SetRandomFromTags(tags,&opt);

	if(opt.location1.X!=rdInstOptions->rndLocation1.X) rdInstOptions->defLocX1=false;
	if(opt.location1.Y!=rdInstOptions->rndLocation1.Y) rdInstOptions->defLocY1=false;
	if(opt.location1.Z!=rdInstOptions->rndLocation1.Z) rdInstOptions->defLocZ1=false;
	if(opt.location2.X!=rdInstOptions->rndLocation2.X) rdInstOptions->defLocX2=false;
	if(opt.location2.Y!=rdInstOptions->rndLocation2.Y) rdInstOptions->defLocY2=false;
	if(opt.location2.Z!=rdInstOptions->rndLocation2.Z) rdInstOptions->defLocZ2=false;

	if(opt.rotation1.Roll!=rdInstOptions->rndRotation1.Roll) rdInstOptions->defRotX1=false;
	if(opt.rotation1.Pitch!=rdInstOptions->rndRotation1.Pitch) rdInstOptions->defRotY1=false;
	if(opt.rotation1.Yaw!=rdInstOptions->rndRotation1.Yaw) rdInstOptions->defRotZ1=false;
	if(opt.rotation2.Roll!=rdInstOptions->rndRotation2.Roll) rdInstOptions->defRotX2=false;
	if(opt.rotation2.Pitch!=rdInstOptions->rndRotation2.Pitch) rdInstOptions->defRotY2=false;
	if(opt.rotation2.Yaw!=rdInstOptions->rndRotation2.Yaw) rdInstOptions->defRotZ2=false;

	if(opt.lockAxis!=rdInstOptions->lockAxis) rdInstOptions->defLockedAxis=false;

	if(opt.flipProbability.X!=rdInstOptions->rndFlipProbability.X) rdInstOptions->defFlipX=false;
	if(opt.flipProbability.Y!=rdInstOptions->rndFlipProbability.Y) rdInstOptions->defFlipY=false;
	if(opt.flipProbability.Z!=rdInstOptions->rndFlipProbability.Z) rdInstOptions->defFlipZ=false;

	if(opt.showProbability!=rdInstOptions->rndShowProbability) rdInstOptions->defShow=false;
	if(opt.themes!=rdInstOptions->themes) rdInstOptions->defThemes=false;

	if(opt.scale1.X!=rdInstOptions->rndScale1.X) rdInstOptions->defScaleX1=false;
	if(opt.scale1.Y!=rdInstOptions->rndScale1.Y) rdInstOptions->defScaleY1=false;
	if(opt.scale1.Z!=rdInstOptions->rndScale1.Z) rdInstOptions->defScaleZ1=false;
	if(opt.scale2.X!=rdInstOptions->rndScale2.X) rdInstOptions->defScaleX2=false;
	if(opt.scale2.Y!=rdInstOptions->rndScale2.Y) rdInstOptions->defScaleY2=false;
	if(opt.scale2.Z!=rdInstOptions->rndScale2.Z) rdInstOptions->defScaleZ2=false;

	if(opt.relyActorName!=rdInstOptions->rndRelyOnActorName) rdInstOptions->defRely=false;
}

//----------------------------------------------------------------------------------------------------------------
// CreateTagsFromRandom
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::CreateTagsFromRandom(TArray<FName>& tags,const FrdRandomSettings& random) {

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

//----------------------------------------------------------------------------------------------------------------
// RemoveTagsThatStartWith
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::RemoveTagsThatStartWith(TArray<FName>& tags,const FString& tagStart) {

	TArray<FName> toRemove;
	for(auto tag:tags) {
		FString st=tag.ToString();
		if(st.StartsWith(tagStart)) {
			toRemove.Add(tag);
		}
	}

	for(auto tag:toRemove) {
		tags.Remove(tag);
	}
}

//----------------------------------------------------------------------------------------------------------------
// RemoveRandomizeTags
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::RemoveRandomizeTags(TArray<FName>& tags) {

	TArray<FName> toRemove;
	for(auto tag:tags) {
		FString st=tag.ToString();
		if(	st.StartsWith(TEXT("rdLoc=")) ||
			st.StartsWith(TEXT("rdRot=")) ||
			st.StartsWith(TEXT("rdScale=")) ||
			st.StartsWith(TEXT("rdShow=")) ||
			st.StartsWith(TEXT("rdRely=")) 
			) {
				toRemove.Add(tag);
			}
	}

	for(auto tag:toRemove) {
		tags.Remove(tag);
	}
}

//----------------------------------------------------------------------------------------------------------------
// SetValidFieldsForTags
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::SetValidFieldsForTags(TArray<FName>& tags) {

	// Get values, and remove them from the tag list
	FrdRandomSettings opt;
	SetRandomFromTags(tags,&opt);

	RemoveRandomizeTags(tags);

	// Now Add any pertinent ones back
	FVector zeroVec(0.0f,0.0f,0.0f);
	FRotator zeroRot(0.0f,0.0f,0.0f);
	FVector oneVec(1.0f,1.0f,1.0f);

	// Location
	FVector loc1(0,0,0),loc2(0,0,0);
	loc1.X=rdInstOptions->defLocX1?rdInstOptions->rndLocation1.X:opt.location1.X;
	loc1.Y=rdInstOptions->defLocY1?rdInstOptions->rndLocation1.Y:opt.location1.Y;
	loc1.Z=rdInstOptions->defLocZ1?rdInstOptions->rndLocation1.Z:opt.location1.Z;
	loc2.X=rdInstOptions->defLocX2?rdInstOptions->rndLocation2.X:opt.location2.X;
	loc2.Y=rdInstOptions->defLocY2?rdInstOptions->rndLocation2.Y:opt.location2.Y;
	loc2.Z=rdInstOptions->defLocZ2?rdInstOptions->rndLocation2.Z:opt.location2.Z;

	if(loc1!=zeroVec || loc2!=zeroVec) {
		tags.Add(FName(*FString::Printf(TEXT("rdLoc=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),loc1.X,loc1.Y,loc1.Z,loc2.X,loc2.Y,loc2.Z)));
	}

	// Rotation and Flip
	FRotator rot1(0,0,0),rot2(0,0,0);
	FVector flip(0,0,0);
	rot1.Roll=rdInstOptions->defRotX1?rdInstOptions->rndRotation1.Roll:opt.rotation1.Roll;
	rot1.Pitch=rdInstOptions->defRotY1?rdInstOptions->rndRotation1.Pitch:opt.rotation1.Pitch;
	rot1.Yaw=rdInstOptions->defRotZ1?rdInstOptions->rndRotation1.Yaw:opt.rotation1.Yaw;
	rot2.Roll=rdInstOptions->defRotX2?rdInstOptions->rndRotation2.Roll:opt.rotation2.Roll;
	rot2.Pitch=rdInstOptions->defRotY2?rdInstOptions->rndRotation2.Pitch:opt.rotation2.Pitch;
	rot2.Yaw=rdInstOptions->defRotZ2?rdInstOptions->rndRotation2.Yaw:opt.rotation2.Yaw;
	flip.X=rdInstOptions->defFlipX?rdInstOptions->rndFlipProbability.X:opt.flipProbability.X;
	flip.Y=rdInstOptions->defFlipY?rdInstOptions->rndFlipProbability.Y:opt.flipProbability.Y;
	flip.Z=rdInstOptions->defFlipZ?rdInstOptions->rndFlipProbability.Z:opt.flipProbability.Z;
	
	if(rot1!=zeroRot || rot2!=zeroRot || flip!=zeroVec) {

		if(flip!=zeroVec) {
			tags.Add(FName(*FString::Printf(TEXT("rdRot=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),rot1.Roll,rot1.Pitch,rot1.Yaw,rot2.Roll,rot2.Pitch,rot2.Yaw,flip.X,flip.Y,flip.Z)));
		} else {
			tags.Add(FName(*FString::Printf(TEXT("rdRot=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),rot1.Roll,rot1.Pitch,rot1.Yaw,rot2.Roll,rot2.Pitch,rot2.Yaw)));
		}
	}

	// Scale
	FVector scale1(1,1,1),scale2(1,1,1);
	scale1.X=rdInstOptions->defScaleX1?rdInstOptions->rndScale1.X:opt.scale1.X;
	scale1.Y=rdInstOptions->defScaleY1?rdInstOptions->rndScale1.Y:opt.scale1.Y;
	scale1.Z=rdInstOptions->defScaleZ1?rdInstOptions->rndScale1.Z:opt.scale1.Z;
	scale2.X=rdInstOptions->defScaleX2?rdInstOptions->rndScale2.X:opt.scale2.X;
	scale2.Y=rdInstOptions->defScaleY2?rdInstOptions->rndScale2.Y:opt.scale2.Y;
	scale2.Z=rdInstOptions->defScaleZ2?rdInstOptions->rndScale2.Z:opt.scale2.Z;

	if(scale1!=oneVec || scale2!=oneVec) {
		if(rdInstOptions->lockAxis==2) tags.Add(FName(*FString::Printf(TEXT("rdScale=%.6g,%.6g"),scale1.X,scale2.X)));
		else if(rdInstOptions->lockAxis==1) tags.Add(FName(*FString::Printf(TEXT("rdScale=%.6g,%.6g,%.6g,%.6g"),scale1.X,scale1.Z,scale2.X,scale2.Z)));
		else tags.Add(FName(*FString::Printf(TEXT("rdScale=%.6g,%.6g,%.6g,%.6g,%.6g,%.6g"),scale1.X,scale1.Y,scale1.Z,scale2.X,scale2.Y,scale2.Z)));
	}

	// Show Probability and Themes
	float show=rdInstOptions->defShow?rdInstOptions->rndShowProbability:opt.showProbability;
	FString themes=rdInstOptions->defThemes?rdInstOptions->themes:opt.themes;

	if(show<1.0f || !themes.IsEmpty()) {

		if(!themes.IsEmpty()) {
			tags.Add(FName(*FString::Printf(TEXT("rdShow=%.6g|%s"),show,*themes)));
		} else {
			tags.Add(FName(*FString::Printf(TEXT("rdShow=%.6g"),show)));
		}
	}

	// Rely on Actor
	FString rely=rdInstOptions->defRely?rdInstOptions->rndRelyOnActorName:opt.relyActorName;

	if(!rely.IsEmpty()) {
		tags.Add(FName(*FString::Printf(TEXT("rdRely=%s"),*rely)));
	}
}

//----------------------------------------------------------------------------------------------------------------
// GetTransformFromTags
//----------------------------------------------------------------------------------------------------------------
FTransform FrdInstEditorModule::GetTransformFromTags(TArray<FName>& tags,bool& exists) {

	FVector loc(0,0,0),scale(1,1,1);
	FRotator rot(0,0,0);

	for(auto tag:tags) {
		FString st=tag.ToString();
		if(st.StartsWith(TEXT("loc="))) {
			getOneVector(st.RightChop(4),loc);
			exists=true;
		} else if(st.StartsWith(TEXT("rot="))) {
			getOneRotator(st.RightChop(4),rot);
			exists=true;
		} else if(st.StartsWith(TEXT("scale="))) {
			getOneVector(st.RightChop(6),scale);
			exists=true;
		}
	}

	return FTransform(rot,loc,scale);

}

//----------------------------------------------------------------------------------------------------------------
// ChangeRandomActorSettings
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeRandomActorSettings() {

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());

	int32 numSel=GEditor->GetSelectedActors()->Num();
	if(numSel<=0) return;
	FSelectionIterator it(*GEditor->GetSelectedActors());
	AActor* actor=Cast<AActor>(*it);
	if(actor) {
		SetRandomFromTags(actor->Tags);
	}

	if(numSel==1) {

		ShowRandomSettings(false);

	} else {

		for(FSelectionIterator it2(*GEditor->GetSelectedActors());it2;++it2) {

			actor=Cast<AActor>(*it2);
			if(actor) {
				MergeRandomFromTags(actor->Tags);
			}
		}
		ShowRandomSettings(true);
	}
}

//----------------------------------------------------------------------------------------------------------------
// ChangeRandomSettingsForActor
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeRandomSettingsForActor(AActor* actor) {

	TArray<FName>& tags=actor->Tags;
	SetValidFieldsForTags(tags);
	actor->MarkPackageDirty();
}

//----------------------------------------------------------------------------------------------------------------
// ChangeRandomSettingsForSelectedActors
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeRandomSettingsForSelectedActors() {

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing Randomize Settings..."));
#endif
	{
		FScopedSlowTask SlowTask((float)GEditor->GetSelectedActors()->Num(),LOCTEXT("ChangingRandomSettings","Changing Randomize Settings..."));
		SlowTask.MakeDialog();

		// and change them...
		for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

			selectedLevelActor=Cast<AActor>(*it);
			if(selectedLevelActor) {
				ChangeRandomSettingsForActor(selectedLevelActor);
				selectedLevelActor=nullptr;
			}

			SlowTask.EnterProgressFrame(1.0f);
		}
	}

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
// ChangeRandomSettings - for defaults on Assets (may implement later if potential for usefulness)
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeRandomSettings() {

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());

//	if(actor) {
//		SetRandomFromTags(actor->Tags);
//	}

//	if(numSel==1) {

		ShowRandomSettings(false);
/*
	} else {

		for(FSelectionIterator it2(*GEditor->GetSelectedActors());it2;++it2) {

			actor=Cast<AActor>(*it2);
			if(actor) {
				MergeRandomFromTags(actor->Tags);
			}
		}
		ShowRandomSettings(true);
	}
*/
}

//----------------------------------------------------------------------------------------------------------------
// ChangeRandomSettingsForSelectedAssets
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeRandomSettingsForSelectedAssets() {
}

//----------------------------------------------------------------------------------------------------------------
// ChangeRelyOnActor
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeRelyOnActor(AActor* actor1,AActor* actor2) {

	TArray<FName>& tags=actor1->Tags;
	RemoveTagsThatStartWith(tags,TEXT("rdRely="));

	tags.Add(FName(*FString::Printf(TEXT("rdRely=%s"),*actor2->GetName())));
	actor1->MarkPackageDirty();
}

//----------------------------------------------------------------------------------------------------------------
// ChangeRelyOnActorsForSelectedActors
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeRelyOnActorsForSelectedActors() {

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing RelyOnActor..."));
#endif
	{
		FScopedSlowTask SlowTask((float)GEditor->GetSelectedActors()->Num(),LOCTEXT("ChangingRelyOnActor","Changing Rely On Actor..."));
		SlowTask.MakeDialog();

		AActor* relyActor=nullptr;
		for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

			selectedLevelActor=Cast<AActor>(*it);

			if(relyActor) {
				ChangeRelyOnActor(selectedLevelActor,relyActor);
			} 
			relyActor=selectedLevelActor;

			SlowTask.EnterProgressFrame(1.0f);
		}
	}

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
// RemoveRandomSettingsForActor
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::RemoveRandomSettingsForActor(AActor* actor) {

	TArray<FName>& tags=actor->Tags;
	RemoveRandomizeTags(tags);
	actor->MarkPackageDirty();
}

//----------------------------------------------------------------------------------------------------------------
// RemoveRandomSettingsForSelectedActors
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::RemoveRandomSettingsForSelectedActors() {

	if(FMessageDialog::Open(EAppMsgType::YesNo,EAppReturnType::Yes,LOCTEXT("RemoveRandom","Are you sure you want remove the Random Settings?"))==EAppReturnType::No) {
		return;
	}

	// No need for a slow task or anything...
	for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

		selectedLevelActor=Cast<AActor>(*it);
		if(selectedLevelActor) {
			RemoveRandomSettingsForActor(selectedLevelActor);
			selectedLevelActor=nullptr;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
