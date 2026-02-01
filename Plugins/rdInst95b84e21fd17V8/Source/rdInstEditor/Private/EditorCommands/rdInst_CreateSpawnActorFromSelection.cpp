//
// rdInst_CreateSpawnActorFromSelection.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.51
//
// Creation Date: 2nd March 2024
// Last Modified: 29th June 2025
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
#include "Engine/TextRenderActor.h"
#include "rdInstances.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//----------------------------------------------------------------------------------------------------------------
// CreateSpawnActorFromSelection
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::CreateSpawnActorFromSelection() {

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(!rdInstSubsystem) {
		return;
	}
	ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Opening CreateSpawnActorFromSelection Window..."));
#endif

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());

	bool ret=ShowConvertToSpawnActorSettings();
	if(!ret) {
#ifdef _INCLUDE_DEBUGGING_STUFF
		UE_LOG(LogTemp, Display, TEXT("rdInst CreateSpawnActorFromSelection cancelled."));
#endif
		return;
	}

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdInst","CreateSpawnActorFromSelection"));

	TArray<FrdSetObjectItem> actors;
	TMap<FName,TArray<FrdSetObjectItem>> instances;
	FTransform transform;
	FBox box(FVector(99999,99999,99999),FVector(-99999,-99999,-99999));
	TArray<AActor*> remList;

	// Create a list of the objects for the SpawnActor
	for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

		AActor* actor=Cast<AActor>(*it);
		if(actor) {

			AGroupActor* group=Cast<AGroupActor>(actor);
			if(group) continue;

			remList.Add(actor);
			
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

					rdBase->rdGetProxySettings(sma,sma->GetStaticMeshComponent()->GetStaticMesh(),(*spawnItems)[spawnItems->Num()-1].proxy);

					spawnDat=&((*spawnItems)[0]);
					//spawnDat->transforms.Add(actor->GetActorTransform());
				}
			} else {

				//if(spawnDat.proxyType==0) spawnDat.proxyType=-1; // no instancing support for non-StaticMeshes
				//spawnItems=actors.Find(actor->GetClass());
				//if(!spawnItems) {
					//spawnItems=&actors.Add(FrdSetObjectItem);//actor->GetClass());
				actors.Add(FrdSetObjectItem(actor->GetClass(),actor,false,0));
				spawnDat=&actors[actors.Num()-1];
					
				//}
				//spawnDat=&((*spawnItems)[0]);
				//spawnDat->transforms.Add(actor->ActorToWorld());
			}
			
			if(!spawnDat) continue;

			FTransform tran=actor->GetActorTransform();

			FBox bnds=actor->GetComponentsBoundingBox(true,true);
			if(actor->GetClass()==ATextRenderActor::StaticClass()) {
				bnds.Min=bnds.Max=tran.GetTranslation(); // TextRenderActors have really strange bounds!
			}
			if(bnds.Min.X<box.Min.X) box.Min.X=bnds.Min.X;
			if(bnds.Min.Y<box.Min.Y) box.Min.Y=bnds.Min.Y;
			if(bnds.Min.Z<box.Min.Z) box.Min.Z=bnds.Min.Z;
			if(bnds.Max.X>box.Max.X) box.Max.X=bnds.Max.X;
			if(bnds.Max.Y>box.Max.Y) box.Max.Y=bnds.Max.Y;
			if(bnds.Max.Z>box.Max.Z) box.Max.Z=bnds.Max.Z;

			spawnDat->transforms.Add(tran);
		}
	}

	float totalWidth=box.Max.X-box.Min.X;
	float totalDepth=box.Max.Y-box.Min.Y;
	float totalHeight=box.Max.Z-box.Min.Z;

	transform.SetTranslation(FVector(box.Min.X,box.Min.Y,box.Min.Z));
	transform.SetScale3D(FVector(totalWidth,totalDepth,totalHeight));

	UrdSetObjectsList* objectList=NewObject<UrdSetObjectsList>();
	for(auto i:instances) {
		for(auto v:i.Value) {
			objectList->objects.Add(v);
		}
	}
	for(auto a:actors) {
//		for(auto v:a.Value) {
			objectList->objects.Add(a);//v);
//		}
	}

	if(rdInstOptions->dataAssetFilename.IsEmpty()) {
		rdInstOptions->dataAssetFilename=ShowDataAssetFilename();
	}

	ArdSpawnActor* sa=rdBase->rdCreateSpawnActorFromObjectList(objectList,transform,rdInstOptions->dataAssetFolder+rdInstOptions->dataAssetFilename+TEXT(".")+rdInstOptions->dataAssetFilename,rdInstOptions->spawnDistance,rdInstOptions->distFrames,rdInstOptions->spatial,(TEnumAsByte<rdSpawnMode>)rdInstOptions->spawnMode,rdInstOptions->harvestInstances,rdInstOptions->spawnActorGridX,rdInstOptions->spawnActorGridY);
	if(!sa) {
		return;
	}
	sa->SetActorLabel(rdInstOptions->dataAssetFilename);
	sa->bRelativeSpawning=rdInstOptions->bRelativeSpawning;
	sa->bSpawnOnStaticMeshes=rdInstOptions->bSpawnOnStaticMeshes;
	sa->defaultStreamDistance=rdInstOptions->spawnActorStreamDistance;
	sa->bStreamCalcDistancesBySize=rdInstOptions->calcSpawnDistanceBySize;
	sa->bStreamDeepCalcDistances=rdInstOptions->calcSpawnDistanceDeepScan;
	sa->localRadius=rdInstOptions->localRadius;
	sa->viewFrustumDistance=rdInstOptions->viewFrustumDistance;
	sa->viewFrustumExtend=rdInstOptions->viewFrustumExtend;

	if(rdInstOptions->replace) {

		for(auto a:remList) {
			a->Destroy();
		}
	}

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
// CreateSpawnActorsFromSelectedLevelAssets
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::CreateSpawnActorsFromSelectedLevelAssets() {

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(!rdInstSubsystem) {
		return;
	}
	ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Opening CreateSpawnActorFromSelectedLevelActors Window..."));
#endif

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());

	bool ret=ShowConvertToSpawnActorSettings();
	if(!ret) {
#ifdef _INCLUDE_DEBUGGING_STUFF
		UE_LOG(LogTemp, Display, TEXT("rdInst CreateSpawnActorFromSelectLevelActors cancelled."));
#endif
		return;
	}

	// Add Undo point
	const FScopedTransaction Transaction(LOCTEXT("rdInst","CreateSpawnActorFromSelectedLevelAssets"));

}

//----------------------------------------------------------------------------------------------------------------
// UpdateSpawnActorsFromSelectedLevelAssets
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::UpdateSpawnActorsFromSelectedLevelAssets() {

}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
