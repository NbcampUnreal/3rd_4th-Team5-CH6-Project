//
// rdActorFactory.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.50
//
// Creation Date: 29th April 2025 (moved from rdInst.cpp)
// Last Modified: 19th May 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActorFactory.h"
#include "rdSpawnActor.h"
#include "rdProceduralActor.h"
#include "rdInstEditor.h"
#include "rdInstSubsystem.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//.............................................................................
// Handlers for dragging rd DataAssets into the level
//
// These classes are used so you can drag DataAssets into the level, and they 
//   spawn either a SpawnActor or a ProceduralActor.
//
//.............................................................................

// UrdSpawnActorFactory
bool UrdSpawnActorFactory::CanCreateActorFrom(const FAssetData& a,FText& msg) { 
	if(!a.IsValid() || !a.GetClass()->IsChildOf(UrdBakedDataAsset::StaticClass())) {
		return false;
	}
	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			rdBase->lastDraggedSpawnActorDataAsset=(FAssetData*)&a;
		}
	}
	return true;
}

#if ENGINE_MAJOR_VERSION>4
UClass* UrdSpawnActorFactory::GetDefaultActorClass(const FAssetData& a) { 
	return ArdSpawnActor::StaticClass(); 
}

bool UrdSpawnActorFactory::CanPlaceElementsFromAssetData(const FAssetData& a) { 
	if(!a.IsValid() || !a.GetClass()->IsChildOf(UrdBakedDataAsset::StaticClass())) {
		return false;
	}
	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			rdBase->lastDraggedSpawnActorDataAsset=(FAssetData*)&a;
		}
	}
	return true;
}
#endif

// UrdProceduralActorFactory
bool UrdProceduralActorFactory::CanCreateActorFrom(const FAssetData& a,FText& msg) { 
	if(!a.IsValid() || !a.GetClass()->IsChildOf(UrdPlacementDataAsset::StaticClass())) {
		return false;
	}
	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			rdBase->lastDraggedProceduralActorDataAsset=(FAssetData*)&a;
		}
	}
	return true;
}

#if ENGINE_MAJOR_VERSION>4
UClass* UrdProceduralActorFactory::GetDefaultActorClass(const FAssetData& a) { 
	return ArdProceduralActor::StaticClass(); 
}

bool UrdProceduralActorFactory::CanPlaceElementsFromAssetData(const FAssetData& a) { 
	if(!a.IsValid() || !a.GetClass()->IsChildOf(UrdPlacementDataAsset::StaticClass())) {
		return false;
	}
	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			rdBase->lastDraggedProceduralActorDataAsset=(FAssetData*)&a;
		}
	}
	return true;
}
#endif

// UrdMultiProceduralActorFactory
bool UrdMultiProceduralActorFactory::CanCreateActorFrom(const FAssetData& a, FText& msg) { 
	if(!a.IsValid() || !a.GetClass()->IsChildOf(UrdMultiPlacementDataAsset::StaticClass())) {
		return false;
	}
	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			rdBase->lastDraggedProceduralActorDataAsset=(FAssetData*)&a;
		}
	}
	return true;
}

#if ENGINE_MAJOR_VERSION>4
UClass* UrdMultiProceduralActorFactory::GetDefaultActorClass(const FAssetData& a) { 
	return ArdProceduralActor::StaticClass(); 
}

bool UrdMultiProceduralActorFactory::CanPlaceElementsFromAssetData(const FAssetData& a) { 
	if(!a.IsValid() || !a.GetClass()->IsChildOf(UrdMultiPlacementDataAsset::StaticClass())) {
		return false;
	}
	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(rdInstSubsystem) {
		ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
		if(rdBase) {
			rdBase->lastDraggedProceduralActorDataAsset=(FAssetData*)&a;
		}
	}
	return true;
}
#endif


//.............................................................................
#undef LOCTEXT_NAMESPACE
	
