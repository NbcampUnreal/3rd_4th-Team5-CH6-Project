//
// rdInst_UpdateSpawnActorFromSelection.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 10th Aril 2025 (moved from CreateSpawnActorFromSelection.cpp)
// Last Modified: 10th April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "ScopedTransaction.h"
#include "Runtime/Core/Public/Misc/ScopedSlowTask.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Engine/Selection.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "rdInstOptions.h"
#include "rdSpawnActor.h"
#include "rdInstSubsystem.h"
#include "Editor/GroupActor.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//----------------------------------------------------------------------------------------------------------------
// UpdateSpawnActorFromSelection
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::UpdateSpawnActorFromSelection() {

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(!rdInstSubsystem) {
		return;
	}
	ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Opening UpdateSpawnActorFromSelection Window..."));
#endif

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());

	bool ret=ShowUpdateSpawnActorSettings();
	if(!ret) {
#ifdef _INCLUDE_DEBUGGING_STUFF
		UE_LOG(LogTemp, Display, TEXT("rdInst UpdateSpawnActorFromSelection cancelled."));
#endif
		return;
	}

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdInst","CreateSpawnActorFromSelection"));

	TArray<ArdSpawnActor*> spawnActors;
	TMap<UClass*,TArray<FrdSetObjectItem>> actors;
	TMap<FName,TArray<FrdSetObjectItem>> instances;
	FTransform transform;
	FBox box(FVector(99999,99999,99999),FVector(-99999,-99999,-99999));

	// Create a list of the objects for the SpawnActor
	for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

		ArdSpawnActor* spawnActor=Cast<ArdSpawnActor>(*it);
		if(spawnActor) {
			spawnActors.Add(spawnActor);
			continue;
		}

		AActor* actor=Cast<AActor>(*it);
		if(actor) {
			
			TArray<FrdSetObjectItem>* spawnItems=nullptr;
			FrdSetObjectItem* spawnDat=nullptr;

			AStaticMeshActor* sma=Cast<AStaticMeshActor>(actor);
			if(sma) {

				FName sid=rdBase->rdGetSMCsid(sma->GetStaticMeshComponent());
				if(!sid.IsNone()) {
					spawnItems=instances.Find(sid);
					if(!spawnItems) {
						spawnItems=&instances.Add(sid);
						spawnItems->Add(FrdSetObjectItem(sid));
					}
					spawnDat=&((*spawnItems)[0]);
				}
			} else {

				//if(spawnDat.proxyType==0) spawnDat.proxyType=-1; // no instancing support for non-StaticMeshes
				spawnItems=actors.Find(actor->GetClass());
				if(!spawnItems) {
					spawnItems=&actors.Add(actor->GetClass());
					spawnItems->Add(FrdSetObjectItem(actor->GetClass(),actor,false,0));
				}
				spawnDat=&((*spawnItems)[0]);
			}
			

			FBox bnds=actor->GetComponentsBoundingBox(true,true);
			if(bnds.Min.X<box.Min.X) box.Min.X=bnds.Min.X;
			if(bnds.Min.Y<box.Min.Y) box.Min.Y=bnds.Min.Y;
			if(bnds.Min.Z<box.Min.Z) box.Min.Z=bnds.Min.Z;
			if(bnds.Max.X>box.Max.X) box.Max.X=bnds.Max.X;
			if(bnds.Max.Y>box.Max.Y) box.Max.Y=bnds.Max.Y;
			if(bnds.Max.Z>box.Max.Z) box.Max.Z=bnds.Max.Z;

			spawnDat->transforms.Add(actor->GetActorTransform());
		}
	}

	float totalWidth=box.Max.X-box.Min.X;
	float totalDepth=box.Max.Y-box.Min.Y;
	float totalHeight=box.Max.Z-box.Min.Z;

//	transform.SetTranslation(FVector(box.Min.X+(totalWidth/2.0f),box.Min.Y+(totalDepth/2.0f),box.Min.Z));
	transform.SetTranslation(FVector(box.Min.X,box.Min.Y,box.Min.Z));
	transform.SetScale3D(FVector(totalWidth/32.0f,totalDepth/32.0f,totalHeight/32.0f));

	UrdSetObjectsList* objectList=NewObject<UrdSetObjectsList>();
	for(auto i:instances) {
		for(auto v:i.Value) {
			objectList->objects.Add(v);
		}
	}
	for(auto a:actors) {
		for(auto v:a.Value) {
			objectList->objects.Add(v);
		}
	}

//	rdBase->rdUpdateSpawnActorFromObjectList(objectList,transform,rdInstOptions);//->spawnDistance,rdInstOptions->distFrames,rdInstOptions->spatial,(TEnumAsByte<rdSpawnMode>)rdInstOptions->spawnMode,rdInstOptions->harvestInstances);

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
