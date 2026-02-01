//
// rdInst_InstanceSettings.cpp
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 18th August 2024
// Last Modified: 16th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "rdInstances.h"
#include "rdInstOptions.h"
#include "rdInstSubsystem.h"
#include "Interfaces/IMainFrameModule.h"
#include "Editor/UnrealEd/Public/ScopedTransaction.h"
#include "Runtime/Core/Public/Misc/ScopedSlowTask.h"
#include "EditorFramework/AssetImportData.h"
#include "Misc/OutputDeviceNull.h"
#include "FileHelpers.h"
#include "Engine/Selection.h"
#include "Components/InstancedStaticMeshComponent.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//----------------------------------------------------------------------------------------------------------------
// ChangeInstanceSettings
//----------------------------------------------------------------------------------------------------------------
bool FrdInstEditorModule::ChangeInstanceSettings() {

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());

	int32 numSel=selectedAssets.Num();
	if(numSel<=0) return false;
	auto firstAsset=selectedAssets[0];
	UPackage* package=rdGetPackage(firstAsset.PackageName.ToString());
	if(package) {
		UStaticMesh* mesh=FindObject<UStaticMesh>(package,*firstAsset.AssetName.ToString(),true);
		if(mesh) {
			SetInstanceDataFromTag(mesh);
		}
	}

	if(numSel==1) {

		return ShowStaticMeshInstanceSettings(false);
	}

	for(auto asset:selectedAssets) {
		package=rdGetPackage(asset.PackageName.ToString());
		if(package) {
			UStaticMesh* mesh=FindObject<UStaticMesh>(package,*asset.AssetName.ToString(),true);
			if(mesh) {
				MergeInstanceDataFromTag(mesh);
			}
		}
	}

	return ShowStaticMeshInstanceSettings(true);
}

//----------------------------------------------------------------------------------------------------------------
// ChangeInstanceSettingsForStaticMesh
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeInstanceSettingsForStaticMesh(UStaticMesh* mesh,FScopedSlowTask* SlowTask) {

	UrdInstData* tag=GetInstUserData(mesh,true);
	if(!tag) return;

	if(rdInstOptions->defStartCull) tag->startCull=rdInstOptions->startCull;
	if(rdInstOptions->defEndCull) tag->endCull=rdInstOptions->endCull;
	if(rdInstOptions->defMobility) tag->mobility=rdInstOptions->mobility;
	if(rdInstOptions->defCollision) tag->collision=rdInstOptions->collision;
	if(rdInstOptions->defMinLOD) tag->minLOD=rdInstOptions->minLOD;
	if(rdInstOptions->defNumPerInstCustData) tag->numPerInstCustData=rdInstOptions->numPerInstCustData;
	if(rdInstOptions->defWPODisableDistance) tag->WPODisableDistance=rdInstOptions->WPODisableDistance;
	if(rdInstOptions->defRandomSeed) tag->randomSeed=rdInstOptions->randomSeed;
	if(rdInstOptions->defShadowInvalidation) tag->shadowInvalidation=rdInstOptions->shadowInvalidation;
	if(rdInstOptions->defCollisionProfile) tag->collisionProfile=rdInstOptions->collisionProfile;
	if(rdInstOptions->defMinDrawDistance) tag->minDrawDistance=rdInstOptions->minDrawDistance;
	if(rdInstOptions->defNaniteProgDistance) tag->naniteProgDistance=rdInstOptions->naniteProgDistance;

	if((rdInstOptions->defLightChannel&0x0001)) { if(rdInstOptions->lightChannel&0x0001) tag->lightChannel|=0x0001; else tag->lightChannel&=~0x0001; }
	if((rdInstOptions->defLightChannel&0x0002)) { if(rdInstOptions->lightChannel&0x0002) tag->lightChannel|=0x0002; else tag->lightChannel&=~0x0002; }
	if((rdInstOptions->defLightChannel&0x0004)) { if(rdInstOptions->lightChannel&0x0004) tag->lightChannel|=0x0004; else tag->lightChannel&=~0x0004; }

	if((rdInstOptions->defInstFlags&0x0001)) { if(rdInstOptions->instFlags&0x0001) tag->instFlags|=0x0001; else tag->instFlags&=~0x0001; }
	if((rdInstOptions->defInstFlags&0x0002)) { if(rdInstOptions->instFlags&0x0002) tag->instFlags|=0x0002; else tag->instFlags&=~0x0002; }
	if((rdInstOptions->defInstFlags&0x0004)) { if(rdInstOptions->instFlags&0x0004) tag->instFlags|=0x0004; else tag->instFlags&=~0x0004; }
	if((rdInstOptions->defInstFlags&0x0008)) { if(rdInstOptions->instFlags&0x0008) tag->instFlags|=0x0008; else tag->instFlags&=~0x0008; }
	if((rdInstOptions->defInstFlags&0x0010)) { if(rdInstOptions->instFlags&0x0010) tag->instFlags|=0x0010; else tag->instFlags&=~0x0010; }
	if((rdInstOptions->defInstFlags&0x0020)) { if(rdInstOptions->instFlags&0x0020) tag->instFlags|=0x0020; else tag->instFlags&=~0x0020; }
	if((rdInstOptions->defInstFlags&0x0040)) { if(rdInstOptions->instFlags&0x0040) tag->instFlags|=0x0040; else tag->instFlags&=~0x0040; }
	if((rdInstOptions->defInstFlags&0x0080)) { if(rdInstOptions->instFlags&0x0080) tag->instFlags|=0x0080; else tag->instFlags&=~0x0080; }
	if((rdInstOptions->defInstFlags&0x0100)) { if(rdInstOptions->instFlags&0x0100) tag->instFlags|=0x0100; else tag->instFlags&=~0x0100; }
	if((rdInstOptions->defInstFlags&0x0200)) { if(rdInstOptions->instFlags&0x0200) tag->instFlags|=0x0200; else tag->instFlags&=~0x0200; }
	if((rdInstOptions->defInstFlags&0x0400)) { if(rdInstOptions->instFlags&0x0400) tag->instFlags|=0x0400; else tag->instFlags&=~0x0400; }
	if((rdInstOptions->defInstFlags&0x0800)) { if(rdInstOptions->instFlags&0x0800) tag->instFlags|=0x0800; else tag->instFlags&=~0x0800; }
	if((rdInstOptions->defInstFlags&0x1000)) { if(rdInstOptions->instFlags&0x1000) tag->instFlags|=0x1000; else tag->instFlags&=~0x1000; }
	if((rdInstOptions->defInstFlags&0x2000)) { if(rdInstOptions->instFlags&0x2000) tag->instFlags|=0x2000; else tag->instFlags&=~0x2000; }
	if((rdInstOptions->defInstFlags&0x4000)) { if(rdInstOptions->instFlags&0x4000) tag->instFlags|=0x4000; else tag->instFlags&=~0x4000; }
	if((rdInstOptions->defInstFlags&0x8000)) { if(rdInstOptions->instFlags&0x8000) tag->instFlags|=0x8000; else tag->instFlags&=~0x8000; }
	if((rdInstOptions->defInstFlags&0x10000)) { if(rdInstOptions->instFlags&0x10000) tag->instFlags|=0x10000; else tag->instFlags&=~0x10000; }
	if((rdInstOptions->defInstFlags&0x20000)) { if(rdInstOptions->instFlags&0x20000) tag->instFlags|=0x20000; else tag->instFlags&=~0x20000; }
	if((rdInstOptions->defInstFlags&0x40000)) { if(rdInstOptions->instFlags&0x40000) tag->instFlags|=0x40000; else tag->instFlags&=~0x40000; }
	if((rdInstOptions->defInstFlags&0x80000)) { if(rdInstOptions->instFlags&0x80000) tag->instFlags|=0x80000; else tag->instFlags&=~0x80000; }
	if((rdInstOptions->defInstFlags&0x100000)) { if(rdInstOptions->instFlags&0x100000) tag->instFlags|=0x100000; else tag->instFlags&=~0x100000; }
	if((rdInstOptions->defInstFlags&0x200000)) { if(rdInstOptions->instFlags&0x200000) tag->instFlags|=0x200000; else tag->instFlags&=~0x200000; }
	if((rdInstOptions->defInstFlags&0x400000)) { if(rdInstOptions->instFlags&0x400000) tag->instFlags|=0x400000; else tag->instFlags&=~0x400000; }

	SetInstUserData(mesh,tag);
}

//----------------------------------------------------------------------------------------------------------------
// ChangeInstanceSettingsForSelectedFiles
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeInstanceSettingsForSelectedFiles() {

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing Instance Settings..."));
#endif

	{
		FScopedSlowTask SlowTask((float)selectedAssets.Num(),LOCTEXT("ChangingInstanceSettings","Changing Instance Settings..."));
		SlowTask.MakeDialog();

		// and change them...
		for(auto asset:selectedAssets) {
			UPackage* package=rdGetPackage(asset.PackageName.ToString());
			if(package) {
				UStaticMesh* mesh=FindObject<UStaticMesh>(package,*asset.AssetName.ToString(),true);
				if(mesh) {
					ChangeInstanceSettingsForStaticMesh(mesh,&SlowTask);
					package->FullyLoad();
					package->SetDirtyFlag(true);
					FEditorFileUtils::PromptForCheckoutAndSave({package},false,false);
				}
			}

			SlowTask.EnterProgressFrame(1.0f);
		}

		UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
		if(rdInstSubsystem) {
			ArdInstBaseActor* rdBase=rdInstSubsystem->rdGetBase();
			if(rdBase) {
				rdBase->rdRecreateInstances();
			}
		}
	}

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//.............................................................................
// SetInstanceDataFromTag
//.............................................................................
void FrdInstEditorModule::SetInstanceDataFromTag(UStaticMesh* mesh) {

	if(!mesh) return;

	UrdInstData* tag=GetInstUserData(mesh,false);
	if(tag) {

		rdInstOptions->startCull=tag->startCull;
		rdInstOptions->endCull=tag->endCull;
		rdInstOptions->mobility=tag->mobility;
		rdInstOptions->collision=tag->collision;
		rdInstOptions->minLOD=tag->minLOD;
		rdInstOptions->randomSeed=tag->randomSeed;
		rdInstOptions->lightChannel=tag->lightChannel;
		rdInstOptions->instFlags=tag->instFlags;
		rdInstOptions->numPerInstCustData=tag->numPerInstCustData;
		rdInstOptions->WPODisableDistance=tag->WPODisableDistance;
		rdInstOptions->shadowInvalidation=tag->shadowInvalidation;
		rdInstOptions->minDrawDistance=tag->minDrawDistance;
		rdInstOptions->naniteProgDistance=tag->naniteProgDistance;
	}

}

//.............................................................................
// MergeInstanceDataFromTag
//.............................................................................
void FrdInstEditorModule::MergeInstanceDataFromTag(UStaticMesh* mesh) {

	if(!mesh) return;

	UrdInstData* tag=GetInstUserData(mesh,true);
	if(tag) {

		if(rdInstOptions->startCull!=tag->startCull) rdInstOptions->defStartCull=false;
		if(rdInstOptions->endCull!=tag->endCull) rdInstOptions->defEndCull=false;
		if(rdInstOptions->mobility!=tag->mobility) rdInstOptions->defMobility=false;
		if(rdInstOptions->collision!=tag->collision) rdInstOptions->defCollision=false;
		if(rdInstOptions->minLOD!=tag->minLOD) rdInstOptions->defMinLOD=false;
		if(rdInstOptions->randomSeed!=tag->randomSeed) rdInstOptions->defRandomSeed=false;
		if(rdInstOptions->numPerInstCustData!=tag->numPerInstCustData) rdInstOptions->defNumPerInstCustData=false;
		if(rdInstOptions->WPODisableDistance!=tag->WPODisableDistance) rdInstOptions->defWPODisableDistance=false;
		if(rdInstOptions->shadowInvalidation!=tag->shadowInvalidation) rdInstOptions->defShadowInvalidation=false;
		if(rdInstOptions->collisionProfile!=tag->collisionProfile) rdInstOptions->defCollisionProfile=false;
		if(rdInstOptions->minDrawDistance!=tag->minDrawDistance) rdInstOptions->defMinDrawDistance=false;
		if(rdInstOptions->naniteProgDistance!=tag->naniteProgDistance) rdInstOptions->defNaniteProgDistance=false;

		if((rdInstOptions->lightChannel&0x0001)!=(tag->lightChannel&0x0001)) rdInstOptions->defLightChannel&=~0x0001;
		if((rdInstOptions->lightChannel&0x0002)!=(tag->lightChannel&0x0002)) rdInstOptions->defLightChannel&=~0x0002;
		if((rdInstOptions->lightChannel&0x0004)!=(tag->lightChannel&0x0004)) rdInstOptions->defLightChannel&=~0x0004;

		if((rdInstOptions->instFlags&0x0001)!=(tag->instFlags&0x0001)) rdInstOptions->defInstFlags&=~0x0001;
		if((rdInstOptions->instFlags&0x0002)!=(tag->instFlags&0x0002)) rdInstOptions->defInstFlags&=~0x0002;
		if((rdInstOptions->instFlags&0x0004)!=(tag->instFlags&0x0004)) rdInstOptions->defInstFlags&=~0x0004;
		if((rdInstOptions->instFlags&0x0008)!=(tag->instFlags&0x0008)) rdInstOptions->defInstFlags&=~0x0008;
		if((rdInstOptions->instFlags&0x0010)!=(tag->instFlags&0x0010)) rdInstOptions->defInstFlags&=~0x0010;
		if((rdInstOptions->instFlags&0x0020)!=(tag->instFlags&0x0020)) rdInstOptions->defInstFlags&=~0x0020;
		if((rdInstOptions->instFlags&0x0040)!=(tag->instFlags&0x0040)) rdInstOptions->defInstFlags&=~0x0040;
		if((rdInstOptions->instFlags&0x0080)!=(tag->instFlags&0x0080)) rdInstOptions->defInstFlags&=~0x0080;
		if((rdInstOptions->instFlags&0x0100)!=(tag->instFlags&0x0100)) rdInstOptions->defInstFlags&=~0x0100;
		if((rdInstOptions->instFlags&0x0200)!=(tag->instFlags&0x0200)) rdInstOptions->defInstFlags&=~0x0200;
		if((rdInstOptions->instFlags&0x0400)!=(tag->instFlags&0x0400)) rdInstOptions->defInstFlags&=~0x0400;
		if((rdInstOptions->instFlags&0x0800)!=(tag->instFlags&0x0800)) rdInstOptions->defInstFlags&=~0x0800;
		if((rdInstOptions->instFlags&0x1000)!=(tag->instFlags&0x1000)) rdInstOptions->defInstFlags&=~0x1000;
		if((rdInstOptions->instFlags&0x2000)!=(tag->instFlags&0x2000)) rdInstOptions->defInstFlags&=~0x2000;
		if((rdInstOptions->instFlags&0x4000)!=(tag->instFlags&0x4000)) rdInstOptions->defInstFlags&=~0x4000;
		if((rdInstOptions->instFlags&0x8000)!=(tag->instFlags&0x8000)) rdInstOptions->defInstFlags&=~0x8000;
		if((rdInstOptions->instFlags&0x10000)!=(tag->instFlags&0x10000)) rdInstOptions->defInstFlags&=~0x10000;
		if((rdInstOptions->instFlags&0x20000)!=(tag->instFlags&0x20000)) rdInstOptions->defInstFlags&=~0x20000;
		if((rdInstOptions->instFlags&0x40000)!=(tag->instFlags&0x40000)) rdInstOptions->defInstFlags&=~0x40000;
		if((rdInstOptions->instFlags&0x80000)!=(tag->instFlags&0x80000)) rdInstOptions->defInstFlags&=~0x80000;
		if((rdInstOptions->instFlags&0x100000)!=(tag->instFlags&0x100000)) rdInstOptions->defInstFlags&=~0x100000;
		if((rdInstOptions->instFlags&0x200000)!=(tag->instFlags&0x200000)) rdInstOptions->defInstFlags&=~0x200000;
		if((rdInstOptions->instFlags&0x400000)!=(tag->instFlags&0x400000)) rdInstOptions->defInstFlags&=~0x400000;
		if((rdInstOptions->instFlags&0x800000)!=(tag->instFlags&0x800000)) rdInstOptions->defInstFlags&=~0x800000;
		if((rdInstOptions->instFlags&0x1000000)!=(tag->instFlags&0x1000000)) rdInstOptions->defInstFlags&=~0x1000000;
		if((rdInstOptions->instFlags&0x2000000)!=(tag->instFlags&0x2000000)) rdInstOptions->defInstFlags&=~0x2000000;
		if((rdInstOptions->instFlags&0x4000000)!=(tag->instFlags&0x4000000)) rdInstOptions->defInstFlags&=~0x4000000;
	}
}

//.............................................................................
// GetInstUserData
//.............................................................................
UrdInstData* FrdInstEditorModule::GetInstUserData(const UStaticMesh* mesh,bool make) {

	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(!rdInstSubsystem) return nullptr;

	return rdInstSubsystem->rdGetBase()->GetInstUserData(mesh,make);
}

//----------------------------------------------------------------------------------------------------------------
// SetInstUserData
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::SetInstUserData(const UStaticMesh* mesh,UrdInstData* instData) {
	UrdInstSubsystem* rdInstSubsystem=GEngine?GEngine->GetEngineSubsystem<UrdInstSubsystem>():nullptr;
	if(!rdInstSubsystem) return;

	rdInstSubsystem->rdGetBase()->SetInstUserData(mesh,instData);
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
