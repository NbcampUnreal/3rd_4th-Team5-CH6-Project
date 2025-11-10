//
// rdInst_ProxySettings.cpp
//
// Copyright (c) 2024 Recourse Design ltd. All rights reserved.
//
// Creation Date: 7th March 2025
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
// ProxySettingsFromSelection
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ProxySettingsFromSelection() {

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());
	FillFromOptionDefaults(TEXT("ProxySettings"),rdInstOptions);

	FrdProxySetup proxy;
	bool firstSet=false;
	TArray<AActor*> selectedMeshActors;
	for(FSelectionIterator it(*GEditor->GetSelectedActors());it;++it) {
		AStaticMeshActor* ma=Cast<AStaticMeshActor>(*it);
		if(ma) {
			selectedMeshActors.Add(ma);
			UStaticMesh* mesh=ma->GetStaticMeshComponent()->GetStaticMesh();
			if(mesh) {
				fillProxySetupFromAssetData(mesh,proxy);
			}
			for(FName tag:ma->Tags) {
				FString stag=tag.ToString();
				if(stag.StartsWith(TEXT("rdProxy="))) {
					proxy.FromString(stag.RightChop(8));
					if(!firstSet) {
						setSettingsFromProxySetup(proxy);
						firstSet=true;
					} else {
						MergeProxySetupData(proxy);
					}
					break;
				}
			}
		}
	}

	if(ShowProxySettings()) {
		ChangeProxySettingsForActors(selectedMeshActors);
	}
}

//----------------------------------------------------------------------------------------------------------------
// ProxySettingsFromSelectedAssets
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ProxySettingsFromSelectedAssets() {

	int32 numSel=selectedAssets.Num();
	if(numSel<=0) return;

	rdInstOptions=NewObject<UrdInstOptions>(GetTransientPackage());
	FillFromOptionDefaults(TEXT("ProxySettings"),rdInstOptions);

	FrdProxySetup proxy;
	bool firstSet=false;
	for(auto& asset:selectedAssets) {

		UPackage* package=rdGetPackage(asset.PackageName.ToString());
		if(package) {
			UStaticMesh* mesh=FindObject<UStaticMesh>(package,*asset.AssetName.ToString(),true);
			if(mesh) {
				UrdInstProxyAssetUserData* pad=(UrdInstProxyAssetUserData*)mesh->GetAssetUserDataOfClass(UrdInstProxyAssetUserData::StaticClass());
				if(pad) {
					proxy=pad->proxy;
					if(!firstSet) {
						setSettingsFromProxySetup(proxy);
						firstSet=true;
					} else {
						MergeProxySetupData(proxy);
					}
				}

			}
		}
	}

	if(ShowProxySettings()) {
		ChangeProxySettingsForAssets();
	}
}

//----------------------------------------------------------------------------------------------------------------
// ChangeProxySettingsForAssets
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeProxySettingsForAssets() {

	for(auto& asset:selectedAssets) {

		UPackage* package=rdGetPackage(asset.PackageName.ToString());
		if(package) {
			UStaticMesh* mesh=FindObject<UStaticMesh>(package,*asset.AssetName.ToString(),true);
			if(mesh) {
				FrdProxySetup proxy;
				UrdInstProxyAssetUserData* pad=(UrdInstProxyAssetUserData*)mesh->GetAssetUserDataOfClass(UrdInstProxyAssetUserData::StaticClass());
				if(pad) {
					proxy=pad->proxy;
				}
				setProxySetupFromSettings(proxy);
				mesh->Modify();
				if(!pad) {
					pad=NewObject<UrdInstProxyAssetUserData>(mesh);
					mesh->AddAssetUserData(pad);
				}
				pad->proxy=proxy;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------
// ChangeProxySettingsForActors
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeProxySettingsForActors(TArray<AActor*>& selectedMeshActors) {

	for(AActor* actor:selectedMeshActors) {

		FrdProxySetup proxy;
		AStaticMeshActor* ma=Cast<AStaticMeshActor>(actor);
		if(ma) {
			UStaticMesh* mesh=ma->GetStaticMeshComponent()->GetStaticMesh();
			if(mesh) {
				fillProxySetupFromAssetData(mesh,proxy);
			}
		}
		for(FName tag:actor->Tags) {
			FString stag=tag.ToString();
			if(stag.StartsWith(TEXT("rdProxy="))) {
				proxy.FromString(stag.RightChop(8));
				actor->Tags.Remove(tag);
				break;
			}
		}
		setProxySetupFromSettings(proxy);
		actor->Modify();
		actor->Tags.Add(*FString::Printf(TEXT("rdProxy=%s"),*proxy.ToString()));
	}
}

//----------------------------------------------------------------------------------------------------------------
// fillProxySetupFromAssetData
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::fillProxySetupFromAssetData(UStaticMesh* mesh,FrdProxySetup& proxy) {

	UrdInstProxyAssetUserData* pad=(UrdInstProxyAssetUserData*)mesh->GetAssetUserDataOfClass(UrdInstProxyAssetUserData::StaticClass());
	if(pad) {
		proxy=pad->proxy;
	}
}

//----------------------------------------------------------------------------------------------------------------
// setProxySetupFromSettings
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::setProxySetupFromSettings(FrdProxySetup& proxy) {

	if(rdInstOptions->defProxyType) proxy.proxyType=(TEnumAsByte<rdProxyType>)rdInstOptions->proxyType;
	if(rdInstOptions->defProxyActor) proxy.proxyActor=rdInstOptions->proxyActor;
	if(rdInstOptions->defProxyDataLayerName) proxy.proxyDataLayer=rdInstOptions->ProxyDataLayer;
	if(rdInstOptions->defProxyStaticMesh) proxy.proxyStaticMesh=rdInstOptions->ProxyStaticMesh;
	if(rdInstOptions->defProxyMeshMaterials) proxy.proxyMeshMaterials=rdInstOptions->ProxyStaticMeshMaterials;
	if(rdInstOptions->defProxyMeshCollision) proxy.proxyMeshCollision=(TEnumAsByte<ErdCollision>)rdInstOptions->ProxyStaticMeshCollision;
	if(rdInstOptions->defProxyDestroyedMesh) proxy.destroyedMesh=rdInstOptions->ProxyDestroyedMesh;
	if(rdInstOptions->defProxyDestroyedMaterials) proxy.destroyedMeshMaterials=rdInstOptions->ProxyDestroyedMeshMaterials;
	if(rdInstOptions->defProxyDestroyedCollision) proxy.destroyedMeshCollision=(TEnumAsByte<ErdCollision>)rdInstOptions->ProxyDestroyedMeshCollision;
	if(rdInstOptions->defProxyDestroyedPrefab) proxy.destroyedPrefab=rdInstOptions->ProxyDestroyedPrefab;

	FVector origV=proxy.destroyedOffset.GetTranslation();
	FVector newV=rdInstOptions->ProxyDestroyedOffset.GetTranslation();
	if(rdInstOptions->defProxyDestroyedOffsetX) origV.X=newV.X;
	if(rdInstOptions->defProxyDestroyedOffsetY) origV.Y=newV.Y;
	if(rdInstOptions->defProxyDestroyedOffsetZ) origV.Z=newV.Z;
	proxy.destroyedOffset.SetTranslation(origV);
	if(rdInstOptions->defProxyLongDistance) proxy.scanDistance=rdInstOptions->ProxyLongDistance;
	if(rdInstOptions->defProxyPhysicsTimeout) proxy.proxyPhysicsTimeout=rdInstOptions->ProxyPhysicsTimeout;
	if(rdInstOptions->defProxyUseDestruction) proxy.bUseWithDestruction=rdInstOptions->bProxyUseDestruction;
	if(rdInstOptions->defProxyPool) proxy.bPooled=rdInstOptions->bProxyPool;
	if(rdInstOptions->defProxySimplePool) proxy.bSimplePool=rdInstOptions->bProxySimplePool;
	if(rdInstOptions->defProxyNumToPool) proxy.pooledAmount=rdInstOptions->ProxyNumToPool;
	if(rdInstOptions->defProxyDontRemove) proxy.bDontRemove=rdInstOptions->bProxyDontRemove;
	if(rdInstOptions->defProxyDontHide) proxy.bDontHide=rdInstOptions->bProxyDontHide;
	if(rdInstOptions->defProxyAllMesh) proxy.bEntireMeshProxy=rdInstOptions->bProxyAllMesh;
//	if(rdInstOptions->def) proxy.savedState=rdInstOptions->;
	if(rdInstOptions->defCallSwapEvent) proxy.bCallSwapEvent=rdInstOptions->ProxyCallSwapEvent;

	if(rdInstOptions->defPickupType) proxy.pickup.type=(TEnumAsByte<rdPickupType>)rdInstOptions->pickupType;
	if(rdInstOptions->defPickupHighlightType) proxy.pickup.highlightType=(TEnumAsByte<rdPickupHighlightType>)rdInstOptions->pickupHighlightType;
	if(rdInstOptions->defPickupHighlightStyle) proxy.pickup.highlightStyle=(TEnumAsByte<rdPickupHighlightStyle>)rdInstOptions->pickupHighlightStyle;
	if(rdInstOptions->defPickupID) proxy.pickup.id=rdInstOptions->pickupID;
	if(rdInstOptions->defPickupHighlightThickness) proxy.pickup.thickness=rdInstOptions->pickupHighlightThickness;
	if(rdInstOptions->defPickupHighlightColor1) proxy.pickup.color1=rdInstOptions->pickupHighlightColor1;
	if(rdInstOptions->defPickupHighlightColor2) proxy.pickup.color2=rdInstOptions->pickupHighlightColor2;
	if(rdInstOptions->defPickupStrobeSpeed) proxy.pickup.strobeSpeed=rdInstOptions->pickupStrobeSpeed;
	if(rdInstOptions->defPickupEbbSpeed) proxy.pickup.ebbSpeed=rdInstOptions->pickupEbbSpeed;
	if(rdInstOptions->defPickupFadeHighlight) proxy.pickup.fadeToGround=rdInstOptions->bPickupFadeHighlight;
	if(rdInstOptions->defPickupRespawnTime) proxy.pickup.respawnTime=rdInstOptions->pickupRespawnTime;
	if(rdInstOptions->defPickupRespawnWhenLonely) proxy.pickup.respawnLonely=rdInstOptions->bPickupRespawnWhenLonely;

	if(rdInstOptions->defPickupHighlightDistance) proxy.pickup.highlightDistance=rdInstOptions->pickupHighlightDistance;
	if(rdInstOptions->defPickupPickupDistance) proxy.pickup.pickupDistance=rdInstOptions->pickupPickupDistance;
	if(rdInstOptions->defPickupRespawnDistance) proxy.pickup.respawnDistance=rdInstOptions->pickupRespawnDistance;
	if(rdInstOptions->defPickupDistanceFadeHighlight) proxy.pickup.fadeInDistance=rdInstOptions->bPickupDistanceFadeHighlight;
	if(rdInstOptions->defPickupUseStencilBuffer) proxy.pickup.useStencilBuffer=rdInstOptions->bPickupUseStencilBuffer;
}

//----------------------------------------------------------------------------------------------------------------
// setSettingsFromProxySetup
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::setSettingsFromProxySetup(FrdProxySetup& proxy) {

	rdInstOptions->SetAllDefault();

	rdInstOptions->proxyType=(int32)proxy.proxyType;
	rdInstOptions->proxyActor=proxy.proxyActor;
	rdInstOptions->ProxyDataLayer=proxy.proxyDataLayer;
	rdInstOptions->ProxyStaticMesh=proxy.proxyStaticMesh;
	rdInstOptions->ProxyStaticMeshMaterials=proxy.proxyMeshMaterials;
	rdInstOptions->materialList.Empty();
	for(auto& m:rdInstOptions->ProxyStaticMeshMaterials) {
		rdInstOptions->materialList.Add(m.Get());
	}
	rdInstOptions->ProxyStaticMeshCollision=(int32)proxy.proxyMeshCollision;
	rdInstOptions->ProxyDestroyedMesh=proxy.destroyedMesh;
	rdInstOptions->ProxyDestroyedMeshMaterials=proxy.destroyedMeshMaterials;
	rdInstOptions->materialList2.Empty();
	for(auto& m:rdInstOptions->ProxyDestroyedMeshMaterials) {
		rdInstOptions->materialList2.Add(m.Get());
	}
	rdInstOptions->ProxyDestroyedMeshCollision=(int32)proxy.destroyedMeshCollision;
	rdInstOptions->ProxyDestroyedPrefab=proxy.destroyedPrefab;
	rdInstOptions->ProxyDestroyedOffset=proxy.destroyedOffset;
	rdInstOptions->pivotOffset=proxy.destroyedOffset.GetTranslation(); //@ for now we're just focusing on the translation, rot and scale later
	rdInstOptions->ProxyLongDistance=proxy.scanDistance;
	rdInstOptions->ProxyPhysicsTimeout=proxy.proxyPhysicsTimeout;
	rdInstOptions->bProxyUseDestruction=proxy.bUseWithDestruction;
	rdInstOptions->bProxyPool=proxy.bPooled;
	rdInstOptions->bProxySimplePool=proxy.bSimplePool;
	rdInstOptions->ProxyNumToPool=proxy.pooledAmount;
	rdInstOptions->bProxyDontRemove=proxy.bDontRemove;
	rdInstOptions->bProxyDontHide=proxy.bDontHide;
	rdInstOptions->bProxyAllMesh=proxy.bEntireMeshProxy;
//	rdInstOptions->=proxy.savedState;
	rdInstOptions->ProxyCallSwapEvent=proxy.bCallSwapEvent;

	rdInstOptions->pickupType=(int32)proxy.pickup.type;
	rdInstOptions->pickupHighlightType=(int32)proxy.pickup.highlightType;
	rdInstOptions->pickupHighlightStyle=(int32)proxy.pickup.highlightStyle;
	rdInstOptions->pickupID=proxy.pickup.id;
	rdInstOptions->pickupHighlightThickness=proxy.pickup.thickness;
	rdInstOptions->pickupHighlightColor1=proxy.pickup.color1;
	rdInstOptions->pickupHighlightColor2=proxy.pickup.color2;
	rdInstOptions->pickupStrobeSpeed=proxy.pickup.strobeSpeed;
	rdInstOptions->pickupEbbSpeed=proxy.pickup.ebbSpeed;
	rdInstOptions->bPickupFadeHighlight=proxy.pickup.fadeToGround;
	
	rdInstOptions->pickupRespawnTime=proxy.pickup.respawnTime;
	rdInstOptions->bPickupRespawnWhenLonely=proxy.pickup.respawnLonely;

	rdInstOptions->pickupHighlightDistance=proxy.pickup.highlightDistance;
	rdInstOptions->pickupPickupDistance=proxy.pickup.pickupDistance;
	rdInstOptions->pickupRespawnDistance=proxy.pickup.respawnDistance;
	rdInstOptions->bPickupDistanceFadeHighlight=proxy.pickup.fadeInDistance;
	rdInstOptions->bPickupUseStencilBuffer=proxy.pickup.useStencilBuffer;
}

//----------------------------------------------------------------------------------------------------------------
// MergeProxySetupData
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::MergeProxySetupData(FrdProxySetup& proxy) {

	if(proxy.proxyType!=rdInstOptions->proxyType) rdInstOptions->defProxyType=false;
	if(proxy.proxyActor!=rdInstOptions->proxyActor) rdInstOptions->defProxyActor=false;
	if(proxy.bEntireMeshProxy!=rdInstOptions->bProxyAllMesh) rdInstOptions->defProxyAllMesh=false;
	if(proxy.bPooled!=rdInstOptions->bProxyPool) rdInstOptions->defProxyPool=false;
	if(proxy.bSimplePool!=rdInstOptions->bProxySimplePool) rdInstOptions->defProxySimplePool=false;
	if(proxy.pooledAmount!=rdInstOptions->ProxyNumToPool) rdInstOptions->defProxyNumToPool=false;
	if(proxy.proxyDataLayer!=rdInstOptions->ProxyDataLayer) rdInstOptions->defProxyDataLayerName=false;

	if(proxy.proxyStaticMesh!=rdInstOptions->ProxyStaticMesh) rdInstOptions->defProxyStaticMesh=false;
	if(!rdInstOptions->defProxyStaticMesh || proxy.proxyMeshMaterials!=rdInstOptions->ProxyStaticMeshMaterials) {
		rdInstOptions->defProxyMeshMaterials=false;
		rdInstOptions->materialList.Empty();
	}
	if(proxy.proxyMeshCollision!=rdInstOptions->ProxyStaticMeshCollision) rdInstOptions->defProxyMeshCollision=false;

	if(proxy.bDontRemove!=rdInstOptions->bProxyDontRemove) rdInstOptions->defProxyDontRemove=false;
	if(proxy.bDontHide!=rdInstOptions->bProxyDontHide) rdInstOptions->defProxyDontHide=false;
	if(proxy.proxyPhysicsTimeout!=rdInstOptions->ProxyPhysicsTimeout) rdInstOptions->defProxyPhysicsTimeout=false;
	if(proxy.scanDistance!=rdInstOptions->ProxyLongDistance) rdInstOptions->defProxyLongDistance=false;
	if(proxy.bUseWithDestruction!=rdInstOptions->bProxyUseDestruction) rdInstOptions->defProxyUseDestruction=false;

	if(proxy.destroyedMesh!=rdInstOptions->ProxyDestroyedMesh) rdInstOptions->defProxyDestroyedMesh=false;
	if(proxy.destroyedPrefab!=rdInstOptions->ProxyDestroyedPrefab) rdInstOptions->defProxyDestroyedPrefab=false;
	FVector v1=proxy.destroyedOffset.GetTranslation();
	FVector v2=rdInstOptions->ProxyDestroyedOffset.GetTranslation();
	if(v1.X!=v2.X) rdInstOptions->defProxyDestroyedOffsetX=false;
	if(v1.Y!=v2.Y) rdInstOptions->defProxyDestroyedOffsetY=false;
	if(v1.Z!=v2.Z) rdInstOptions->defProxyDestroyedOffsetZ=false;
	if(proxy.proxyMeshCollision!=rdInstOptions->ProxyStaticMeshCollision) rdInstOptions->defProxyMeshCollision=false;
	if(!rdInstOptions->defProxyDestroyedMesh || proxy.destroyedMeshMaterials!=rdInstOptions->ProxyDestroyedMeshMaterials) {
		rdInstOptions->materialList2.Empty();
		rdInstOptions->defProxyDestroyedMaterials=false;
	}
	if(proxy.destroyedMeshCollision!=rdInstOptions->ProxyDestroyedMeshCollision) rdInstOptions->defProxyDestroyedCollision=false;

	//if(proxy.savedState!=rdInstOptions->) rdInstOptions->=false;
	if(proxy.bCallSwapEvent!=rdInstOptions->ProxyCallSwapEvent) rdInstOptions->defCallSwapEvent=false;
	if(proxy.pickup.type!=rdInstOptions->pickupType) rdInstOptions->defPickupType=false;
	if(proxy.pickup.highlightType!=rdInstOptions->pickupHighlightType) rdInstOptions->defPickupHighlightType=false;
	if(proxy.pickup.highlightStyle!=rdInstOptions->pickupHighlightStyle) rdInstOptions->defPickupHighlightStyle=false;
	if(proxy.pickup.id!=rdInstOptions->pickupID) rdInstOptions->defPickupID=false;
	if(proxy.pickup.thickness!=rdInstOptions->pickupHighlightThickness) rdInstOptions->defPickupHighlightThickness=false;
	if(proxy.pickup.strobeSpeed!=rdInstOptions->pickupStrobeSpeed) rdInstOptions->defPickupStrobeSpeed=false;
	if(proxy.pickup.ebbSpeed!=rdInstOptions->pickupEbbSpeed) rdInstOptions->defPickupEbbSpeed=false;
	if(proxy.pickup.fadeToGround!=rdInstOptions->bPickupFadeHighlight) rdInstOptions->defPickupFadeHighlight=false;
	if(proxy.pickup.color1!=rdInstOptions->pickupHighlightColor1) rdInstOptions->defPickupHighlightColor1=false;
	if(proxy.pickup.color2!=rdInstOptions->pickupHighlightColor2) rdInstOptions->defPickupHighlightColor2=false;
	if(proxy.pickup.respawnTime!=rdInstOptions->pickupRespawnTime) rdInstOptions->defPickupRespawnTime=false;
	if(proxy.pickup.respawnLonely!=rdInstOptions->bPickupRespawnWhenLonely) rdInstOptions->defPickupRespawnWhenLonely=false;

	if(proxy.pickup.highlightDistance!=rdInstOptions->pickupHighlightDistance) rdInstOptions->defPickupHighlightDistance=false;
	if(proxy.pickup.pickupDistance!=rdInstOptions->pickupPickupDistance) rdInstOptions->defPickupPickupDistance=false;
	if(proxy.pickup.respawnDistance!=rdInstOptions->pickupRespawnDistance) rdInstOptions->defPickupRespawnDistance=false;
	if(proxy.pickup.fadeInDistance!=rdInstOptions->bPickupDistanceFadeHighlight) rdInstOptions->defPickupDistanceFadeHighlight=false;
	if(proxy.pickup.useStencilBuffer!=rdInstOptions->bPickupUseStencilBuffer) rdInstOptions->defPickupUseStencilBuffer=false;
}

//----------------------------------------------------------------------------------------------------------------
// ChangeProxySettingsForSelectedFiles
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeProxySettingsForSelectedFiles() {

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Changing Instance Proxy Settings..."));
#endif

	{
		FScopedSlowTask SlowTask((float)selectedAssets.Num(),LOCTEXT("ChangingProxySettings","Changing Instance Proxy Settings..."));
		SlowTask.MakeDialog();

		// and change them...
		for(auto asset:selectedAssets) {
			UPackage* package=rdGetPackage(asset.PackageName.ToString());
			if(package) {
				UStaticMesh* mesh=FindObject<UStaticMesh>(package,*asset.AssetName.ToString(),true);
				if(mesh) {
					ChangeProxySettingsForStaticMesh(mesh,&SlowTask);
					package->FullyLoad();
					package->SetDirtyFlag(true);
					FEditorFileUtils::PromptForCheckoutAndSave({package},false,false);
				}
			}

			SlowTask.EnterProgressFrame(1.0f);
		}
	}

#ifdef _INCLUDE_DEBUGGING_STUFF
	UE_LOG(LogTemp, Display, TEXT("Done."));
#endif
}

//----------------------------------------------------------------------------------------------------------------
// ChangeProxySettingsForStaticMesh
//----------------------------------------------------------------------------------------------------------------
void FrdInstEditorModule::ChangeProxySettingsForStaticMesh(UStaticMesh* mesh,FScopedSlowTask* SlowTask) {

	UrdInstData* tag=GetInstUserData(mesh,true);
	if(!tag) return;
	
	FrdProxySetup& prox=tag->proxySettings;

	prox.proxyType=(rdProxyType)rdInstOptions->proxyType;
	prox.proxyActor=rdInstOptions->proxyActor?rdInstOptions->proxyActor->GetClass():nullptr;
	prox.proxyDataLayer=rdInstOptions->ProxyDataLayer;
	prox.proxyStaticMesh=rdInstOptions->ProxyStaticMesh;
	prox.proxyMeshMaterials=rdInstOptions->ProxyStaticMeshMaterials;
	prox.proxyMeshCollision=(TEnumAsByte<ErdCollision>)rdInstOptions->ProxyStaticMeshCollision;
	prox.bUseWithDestruction=rdInstOptions->bProxyUseDestruction;
	prox.destroyedMesh=rdInstOptions->ProxyDestroyedMesh;
	prox.destroyedMeshMaterials=rdInstOptions->ProxyDestroyedMeshMaterials;
	prox.destroyedMeshCollision=(TEnumAsByte<ErdCollision>)rdInstOptions->ProxyDestroyedMeshCollision;
	prox.destroyedPrefab=rdInstOptions->ProxyDestroyedPrefab.Get();
	prox.destroyedOffset=rdInstOptions->ProxyDestroyedOffset;
	prox.scanDistance=rdInstOptions->ProxyLongDistance;
	prox.proxyPhysicsTimeout=rdInstOptions->ProxyPhysicsTimeout;
	prox.bPooled=rdInstOptions->bProxyPool;
	prox.bSimplePool=rdInstOptions->bProxySimplePool;
	prox.pooledAmount=rdInstOptions->ProxyNumToPool;
	prox.bDontRemove=rdInstOptions->bProxyDontRemove;
	prox.bDontHide=rdInstOptions->bProxyDontHide;
	prox.bEntireMeshProxy=rdInstOptions->bProxyAllMesh;
	prox.savedState=nullptr;
	prox.bCallSwapEvent=rdInstOptions->ProxyCallSwapEvent;
/*
	int32				pickupType;
	int32				pickupHighlightType;
	int32				pickupHighlightStyle;
	FLinearColor		pickupHighlightColor1;
	FLinearColor		pickupHighlightColor2;
	float				proxyStrobeSpeed;
	float				proxyEbbSpeed;
	bool				bPickupFadeHighlight;
	float				proxyRespawnTime;
	bool				bPickupRespawnWhenLonely;
*/
//	prox.pickUp.=
	SetInstUserData(mesh,tag);
}

//----------------------------------------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
