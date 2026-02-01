//
// rdInst_ConvertPrefabInstanceData.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Version 1.51
//
// Creation Date: 21st June 2025
// Last Modified: 21st June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstEditor.h"
#include "ScopedTransaction.h"
#include "Runtime/Core/Public/Misc/ScopedSlowTask.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Engine/Selection.h"
#include "rdActor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/CompilerResultsLog.h"
#include "FileHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FrdInstEditorModule"

//----------------------------------------------------------------------------------------------------------------
// ConvertSelectedAssetPrefabsToArrays
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ConvertSelectedAssetPrefabsToArrays() {

	int32 numSel=selectedAssets.Num();
	if(numSel<=0) return;

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing Prefab Data to Arrays..."));
#endif
	{
		FScopedSlowTask SlowTask((float)GEditor->GetSelectedActors()->Num(),LOCTEXT("ChangingPrefabsToArrays","Changing Prefab Data to Arrays..."));
		SlowTask.MakeDialog();

		for(auto& asset:selectedAssets) {

			UPackage* package=rdGetPackage(asset.PackageName.ToString());
			if(package) {
				UBlueprint* bp=FindObject<UBlueprint>(package,*(asset.AssetName.ToString()),true);

				FCompilerResultsLog resultsLog;
				FKismetEditorUtilities::CompileBlueprint(bp,EBlueprintCompileOptions::None,&resultsLog);

				AActor* bpActor=(AActor*)bp->GeneratedClass->GetDefaultObject();
				UBlueprintGeneratedClass* bpClass=Cast<UBlueprintGeneratedClass>(bp->GeneratedClass);
				ArdActor* prefab=Cast<ArdActor>(bpActor);
				if(prefab) {
					prefab->rdMoveTablesToArrays();

					FKismetEditorUtilities::CompileBlueprint(bp,EBlueprintCompileOptions::None,&resultsLog);
					FAssetRegistryModule::AssetCreated(bp);

					//if(saveAsset) {
						package->FullyLoad();
						package->SetDirtyFlag(true);
						FEditorFileUtils::PromptForCheckoutAndSave({package},false,false);
					//}
				}
			}
		}
	}

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
// ConvertSelectedAssetPrefabsToTables
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ConvertSelectedAssetPrefabsToTables() {

	int32 numSel=selectedAssets.Num();
	if(numSel<=0) return;

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing Asset Prefab Data to Tables..."));
#endif
	{
		FScopedSlowTask SlowTask((float)GEditor->GetSelectedActors()->Num(),LOCTEXT("ChangingPrefabsToTables","Changing Prefab Data to Tables..."));
		SlowTask.MakeDialog();

		for(auto& asset:selectedAssets) {

			UPackage* package=rdGetPackage(asset.PackageName.ToString());
			if(package) {
				UBlueprint* bp=FindObject<UBlueprint>(package,*(asset.AssetName.ToString()),true);

				FCompilerResultsLog resultsLog;
				FKismetEditorUtilities::CompileBlueprint(bp,EBlueprintCompileOptions::None,&resultsLog);

				AActor* bpActor=(AActor*)bp->GeneratedClass->GetDefaultObject();
				UBlueprintGeneratedClass* bpClass=Cast<UBlueprintGeneratedClass>(bp->GeneratedClass);
				ArdActor* prefab=Cast<ArdActor>(bpActor);
				if(prefab) {
					prefab->rdMoveArraysToTables();

					FKismetEditorUtilities::CompileBlueprint(bp,EBlueprintCompileOptions::None,&resultsLog);
					FAssetRegistryModule::AssetCreated(bp);

					//if(saveAsset) {
						package->FullyLoad();
						package->SetDirtyFlag(true);
						FEditorFileUtils::PromptForCheckoutAndSave({package},false,false);
					//}
				}
			}
		}
	}

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
// ConvertSelectedActorPrefabsToArrays
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ConvertSelectedActorPrefabsToArrays() {

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing Prefab Data to Arrays..."));
#endif
	{
		FScopedSlowTask SlowTask((float)GEditor->GetSelectedActors()->Num(),LOCTEXT("ChangingPrefabsToArrays","Changing Prefab Data to Arrays..."));
		SlowTask.MakeDialog();

		for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

			ArdActor* prefab=Cast<ArdActor>(*it);
			if(prefab) {
				prefab->rdMoveTablesToArrays();
			}

			SlowTask.EnterProgressFrame(1.0f);
		}
	}
#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
// ConvertSelectedActorPrefabsToTables
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ConvertSelectedActorPrefabsToTables() {

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing Prefab Data to Tables..."));
#endif
	{
		FScopedSlowTask SlowTask((float)GEditor->GetSelectedActors()->Num(),LOCTEXT("ChangingPrefabsToTables","Changing Prefab Data to Tables..."));
		SlowTask.MakeDialog();

		for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {

			ArdActor* prefab=Cast<ArdActor>(*it);
			if(prefab) {
				prefab->rdMoveArraysToTables();
			}

			SlowTask.EnterProgressFrame(1.0f);
		}
	}
#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
