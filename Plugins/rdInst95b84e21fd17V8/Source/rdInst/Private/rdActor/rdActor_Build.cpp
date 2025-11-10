//
// rdActor_Build.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 24th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"

//.............................................................................
//  rdBuild - Applies the Randomizations for this Actor and Children and creates the ISMs
//.............................................................................
void ArdActor::rdBuild() {

	UChildActorComponent* parCAC=GetParentComponent();
	if(IsHidden() || (parCAC && !parCAC->IsVisible())) {
		rdRemoveInstances();
		return;
	}

	if(!bStaticMeshMode && bCreateFromArrays && (InstanceFastArray.Num()>0 || InstanceRandomizedArray.Num()>0)) {

		if(!rdGetBaseActor()) return;

		InstanceDataX.Empty();

		for(auto& i:InstanceFastArray) {

			if(!i.meshSetup.mesh && i.mesh) i.meshSetup.mesh=i.mesh;
			if(!i.meshSetup.mesh.LoadSynchronous()) continue;
			if(i.meshSetup.sid.IsNone()) {
				i.meshSetup.sid=rdBaseActor->rdGetInstSid(i.meshSetup);
				if(i.meshSetup.sid.IsNone()) {
					continue;
				}
			}
			
			FrdInstanceSettingsArray& ia=InstanceDataX.FindOrAdd(i.meshSetup.sid);
			TArray<FrdInstanceFastSettings>& isa=ia.settingsFast;
			for(FTransform& tt:i.transforms) {
				isa.Add(FrdInstanceFastSettings(tt));
			}
		}

		for(auto& i:InstanceRandomizedArray) {

			if(!i.meshSetup.mesh && i.mesh) i.meshSetup.mesh=i.mesh;
			if(!i.meshSetup.mesh.LoadSynchronous()) continue;
			if(i.meshSetup.sid.IsNone()) {
				i.meshSetup.sid=rdBaseActor->rdGetInstSid(i.meshSetup);
				if(i.meshSetup.sid.IsNone()) {
					continue;
				}
			}
			FrdInstanceSettingsArray& ia=InstanceDataX.FindOrAdd(i.meshSetup.sid);
			TArray<FrdInstanceRandomSettings>& isa=ia.settingsRandom;
			for(FrdInstanceRandomSettings& tt:i.settings) {
				isa.Add(tt);
			}
		}
	}

	switch(actorMode) {

		case RDACTORMODE_ONLYONE: // Shows only one Folder (SceneComponent attached to RootComponent) of ChildActors at a time
			rdThereCanBeOnlyOne();
			break;

		case RDACTORMODE_RANDOMIZE: // Applies random ranged movement/rotation/scale and hides on probabilites or dependance of other actors
			rdBuildRandomizedComponents();
			rdBuildRandomizedISMs();
			break;

		case RDACTORMODE_BUILD: // Sends the BuildEvent to the Blueprint
			rdBuildEvent();
			break;

		case RDACTORMODE_BUILDINSTANCES: // Just Builds the Instances
			rdRecreateBPInstances();
			break;
	}
}

//.............................................................................
//  rdBuildRandomizedComponent
//.............................................................................
void ArdActor::rdBuildRandomizedComponent(USceneComponent* comp) {

	UrdTransformAssetUserData* ftag=nullptr;
	UChildActorComponent* cac=Cast<UChildActorComponent>(comp);
	AActor* actor=nullptr;
	ArdActor* rdActor=nullptr;

	if(cac) {
		actor=(AActor*)cac->GetChildActor();
		rdActor=Cast<ArdActor>(actor);
	}

	ftag=rdInstLib::GetTransformUserData(comp,true);

	if(ftag->bNoVis || ftag->bHide) {

		comp->SetVisibility(false,bRecurseVisibilityChanges);
		comp->bHiddenInGame=true;
		if(actor) actor->SetHidden(true);
		ftag->bHide=true;
		return;

	} else {

		if(!rdHasRandomTags(comp)) {

			if(ftag->bHide) {
				comp->SetVisibility(false,bRecurseVisibilityChanges);
				comp->bHiddenInGame=true;
				if(rdActor) rdActor->rdBuildAsHidden();
				else if(actor) actor->SetHidden(true);
				return;
			} else {
				comp->SetVisibility(true,bRecurseVisibilityChanges);
				comp->bHiddenInGame=false;
				if(actor) actor->SetHidden(false);
				ftag->bHide=false;
				if(rdActor) rdActor->rdBuild();
			}

		} else {

			FTransform transform=rdGetOrigTransform(comp);
			FrdRandomSettings randomSettings;
			rdSetRandomFromTags(comp->ComponentTags,&randomSettings);

			if(rdAddRandomization(transform,&randomSettings)) {

				comp->SetRelativeTransform(transform);

				comp->SetVisibility(true,bRecurseVisibilityChanges);
				comp->bHiddenInGame=false;
				if(actor) actor->SetHidden(false);
				ftag->bHide=false;
				if(rdActor) rdActor->rdBuild();
				ftag->bHide=false;

			} else {

				comp->SetVisibility(false,bRecurseVisibilityChanges);
				comp->bHiddenInGame=true;
				if(rdActor) rdActor->rdBuildAsHidden();
				else if(actor) actor->SetHidden(true);
				ftag->bHide=true;
				return;
			}
		}
	}

	TArray<USceneComponent*> comps;
	comp->GetChildrenComponents(false,comps);
	for(auto c:comps) {
		rdBuildRandomizedComponent(c);
	}
}

//.............................................................................
// rdFindComponentByName
//.............................................................................
USceneComponent* ArdActor::rdFindComponentByName(const FString& name) {

	TArray<USceneComponent*> comps,rootComps,toRemove;
	RootComponent->GetChildrenComponents(true,comps);
	for(auto c:comps) {
		if(c->GetName()==name) {
			return c;
		}
	}
	return nullptr;
}

//.............................................................................
// rdRelianceChainVisibleComponent
//.............................................................................
bool ArdActor::rdRelianceChainVisibleComponent(const FString& name) {

	USceneComponent* comp=rdFindComponentByName(name);
	if(!comp) return false;

	if(rdHasRandomTags(comp)) {

		FrdRandomSettings randomSettings;
		rdSetRandomFromComponent(comp,&randomSettings);

		UrdTransformAssetUserData* ftag=rdInstLib::GetTransformUserData(comp,true);
		if(ftag->bHide) {
			return false;
		}

		if(randomSettings.relyActorName.IsEmpty()) {
			return true;
		}

		return rdRelianceChainVisibleComponent(randomSettings.relyActorName);	
	}

	return true;
}

//.............................................................................
//  rdBuildRandomizedComponents
//
// This routine goes through all the attached ChildActorComponents in this Blueprint
// and applies their Randomization settings (if any).
// 
//.............................................................................
void ArdActor::rdBuildRandomizedComponents() {

	if(!RootComponent) return;

	TArray<USceneComponent*> comps,rootComps,toRemove;
	RootComponent->GetChildrenComponents(true,comps);
	for(auto c:comps) {
		UrdTransformAssetUserData* ftag=rdInstLib::GetTransformUserData(c,true);
		ftag->bProcessed=false;
		ftag->bHide=false;
	}

	RootComponent->GetChildrenComponents(false,rootComps);
	for(USceneComponent* c:rootComps) {
		rdBuildRandomizedComponent(c);
	}

	for(USceneComponent* c:rootComps) {

		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		UrdTransformAssetUserData* ftag=rdInstLib::GetTransformUserData(c,true);

		if(rdRelianceChainVisibleComponent(c->GetName())) {
			c->SetVisibility(true,bRecurseVisibilityChanges);
			c->bHiddenInGame=false;
			AActor* actor=cac?(AActor*)cac->GetChildActor():nullptr;
			ArdActor* rdActor=Cast<ArdActor>(actor);
			if(actor) actor->SetHidden(false);
			if(rdActor) rdActor->rdBuild();
		} else {
			c->SetVisibility(false,bRecurseVisibilityChanges);
			c->bHiddenInGame=true;
			AActor* actor=cac?(AActor*)cac->GetChildActor():nullptr;
			ArdActor* rdActor=Cast<ArdActor>(actor);
			if(actor) actor->SetHidden(true);
			if(rdActor) rdActor->rdHideAllChildren();
		}
	}
}

//.............................................................................
//  rdBuildRandomizedISMs
//
// This is similar to the rdBuildRandomizedComponents, it Applies the Randomizations
// set in the Randomization Settings, and adds Instanced Static Meshes/Actors for them.
//
//.............................................................................
void ArdActor::rdBuildRandomizedISMs() {

	if(!rdGetBaseActor()) return;

	rdBaseActor->rdRemInstancesForOwner(this);

	if(IsHidden()) {
		return;
	}

	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		FrdInstanceSettingsArray& i=it.Value;
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
			j.bVisible=false;
			j.bProcessed=false;
		}
	}

	TArray<FString> hiddenFolders;
	if(RootComponent) {
		TArray<USceneComponent*> comps;
		RootComponent->GetChildrenComponents(false,comps);
		for(auto c:comps) {
			UChildActorComponent* ca=Cast<UChildActorComponent>(c);
			if(!ca) {
				if(c->ComponentTags.Contains(TEXT("rdNoVis")) || !rdFolderIncludedWithCurrentTheme(c)) {
					hiddenFolders.Add(c->GetName());
				}
			}
		}
	}

	// Process the non-random instances first
	for(auto& it:InstanceDataX) {

		FName sid=it.Key;
		FrdInstanceSettingsArray& i=it.Value;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);

		for(FrdInstanceFastSettings& j:i.settingsFast) {
			if(!j.bNoVis && (j.folder.IsEmpty() || !hiddenFolders.Contains(j.folder))) {
				j.index=rdAddInstanceFastWorld(ismc,j.transform);
				j.bVisible=true;
			} else {
				j.index=-1;
				j.bVisible=false;
			}
		}
	}

	for(auto& it:InstanceDataX) {

		FrdInstanceSettingsArray& i=InstanceDataX[it.Key];
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {

			if(j.bNoVis || (!j.folder.IsEmpty() && hiddenFolders.Contains(j.folder))) {
				j.bVisible=false;
				j.index=-1;
				continue;
			}

			j.actualTransform=j.origTransform;
			j.bVisible=rdAddRandomization(j.actualTransform,&j.randomSettings);
		}
	}

	for(auto& it:InstanceDataX) {

		FrdInstanceSettingsArray& i=InstanceDataX[it.Key];
		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);

		for(FrdInstanceRandomSettings& j:i.settingsRandom) {

			if(j.bVisible && rdRelianceChainVisible(j.randomSettings.relyActorName)) {
				j.index=rdAddInstanceFastWorld(ismc,j.actualTransform);
				j.bVisible=true;
			} else {
				j.bVisible=false;
				j.index=-1;
			}
		}
	}
}

//.............................................................................
// rdSetFastArrayItemTransform
//.............................................................................
void ArdActor::rdSetFastArrayItemTransform(int32 index,int32 tindex,const FTransform transform) {

	if(index>=InstanceFastArray.Num()) return;
	InstanceFastArray[index].transforms[tindex]=transform;
	rdBuild();
}

//.............................................................................
// rdGetFastArrayItemTransform
//.............................................................................
FTransform ArdActor::rdGetFastArrayItemTransform(int32 index,int32 tindex) {

	if(index>=InstanceFastArray.Num()) return FTransform();
	return InstanceFastArray[index].transforms[tindex];
}

//.............................................................................
// rdSetRandomizedArrayItemTransform
//.............................................................................
void ArdActor::rdSetRandomizedArrayItemTransform(int32 index,int32 tindex,const FTransform transform) {

	if(index>=InstanceRandomizedArray.Num()) return;
	InstanceRandomizedArray[index].settings[tindex].actualTransform=transform;
	rdBuild();
}

//.............................................................................
// rdGetRandomizedArrayItemTransform
//.............................................................................
FTransform ArdActor::rdGetRandomizedArrayItemTransform(int32 index,int32 tindex) {

	if(index>=InstanceRandomizedArray.Num()) return FTransform();
	return InstanceRandomizedArray[index].settings[tindex].actualTransform;
}

//.............................................................................
//  rdRecreateBPInstances (leaves the components as they are)
//.............................................................................
void ArdActor::rdRecreateBPInstances() {

	if(rdBaseActor) rdBaseActor->rdRemInstancesForOwner(this);
	rdBaseActor=nullptr;
	if(!rdGetBaseActor()) return;
	if(rdBaseActor) rdBaseActor->rdRemInstancesForOwner(this);
	if(IsHidden()) return;

	if(!bStaticMeshMode && bCreateFromArrays && (InstanceFastArray.Num()>0 || InstanceRandomizedArray.Num()>0)) {

		InstanceDataX.Empty();

		for(auto& i:InstanceFastArray) {

			if(!i.meshSetup.mesh && i.mesh) i.meshSetup.mesh=i.mesh;
			FrdInstanceSettingsArray& ia=InstanceDataX.FindOrAdd(rdBaseActor->rdGetInstSid(i.meshSetup));
			TArray<FrdInstanceFastSettings>& isa=ia.settingsFast;
			for(FTransform& tt:i.transforms) {
				isa.Add(FrdInstanceFastSettings(tt));
			}
		}

		for(auto& i:InstanceRandomizedArray) {

			if(!i.meshSetup.mesh && i.mesh) i.meshSetup.mesh=i.mesh;
			FrdInstanceSettingsArray& ia=InstanceDataX.FindOrAdd(rdBaseActor->rdGetInstSid(i.meshSetup));
			TArray<FrdInstanceRandomSettings>& isa=ia.settingsRandom;
			for(FrdInstanceRandomSettings& tt:i.settings) {
				isa.Add(tt);
			}
		}
	}

	for(auto& it:InstanceDataX) {

		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
		FrdInstanceSettingsArray& i=it.Value;

		for(FrdInstanceFastSettings& j:i.settingsFast) {
			if(j.bVisible) {
				j.index=rdAddInstanceFastWorld(ismc,j.transform);
			}
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
			if(j.bVisible) {
				j.index=rdAddInstanceFastWorld(ismc,j.actualTransform);
			}
		}
	}
}

//.............................................................................
//  rdBuildShowOnlyOneISMs
//
// Each of the ISMs has a Folder assigned to them, which relates to the Folder
// USceneComponents attached to our RootComponent.
//
// When this Actor is in ShowOnlyOne mode, the FString "showFolder" contains the
// name of the folder to show (calculated previously).
//
// We just enumerate through our Instances List, and set bHide to True for
// Instances not belonging to that Folder.
//
//.............................................................................
void ArdActor::rdBuildShowOnlyOneISMs_Folder() {

	for(auto& it:InstanceDataX) {

		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
		FrdInstanceSettingsArray& i=it.Value;

		for(FrdInstanceFastSettings& j:i.settingsFast) {

			FString instFolder=GetName()+TEXT("/")+j.folder;
			if(j.folder.IsEmpty() || instFolder==showFolder) {
				j.index=rdAddInstanceFastWorld(ismc,j.transform);
				j.bNoVis=false;
				j.bVisible=true;
			} else {
				j.bVisible=false;
			} 
		}

		FString theme=rdGetTheme();
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {

			FString instFolder=GetName()+TEXT("/")+j.folder;
			if(j.folder.IsEmpty() || instFolder==showFolder) {

				bool found=true;
				FString& iThemes=j.randomSettings.themes;
				if(!theme.IsEmpty() && !iThemes.IsEmpty()) {
					TArray<FString> sthemes;
					theme.ParseIntoArray(sthemes,TEXT(","));
					TArray<FString> themes;
					int32 num=iThemes.ParseIntoArray(themes,TEXT(","));
					if(themes.Num()>0) { 
						found=false;
						for(auto t:sthemes) {
							if(themes.Contains(t)) {
								found=true;
								break;
							}
						}
					}
				}
				if(found) {
					j.index=rdAddInstanceFastWorld(ismc,j.origTransform);
					j.bNoVis=false;
					j.bVisible=true;
				}
			} else {
				j.bVisible=false;
			}
		}
	}
}

//.............................................................................
// rdBuildFolder
//.............................................................................
void ArdActor::rdBuildFolder(USceneComponent* comp) {

	comp->SetVisibility(true,false);
	comp->bHiddenInGame=false;
	comp->ComponentTags.Remove(TEXT("rdNoVis"));

	UrdTransformAssetUserData* ftag=(UrdTransformAssetUserData*)comp->GetAssetUserDataOfClass(UrdTransformAssetUserData::StaticClass());
	if(ftag) {
		ftag->bHide=false;
		ftag->bNoVis=false;
	}

	TArray<USceneComponent*> comps;
	comp->GetChildrenComponents(false,comps);

	for(auto c:comps) {
		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		if(cac) {
			cac->SetVisibility(true,bRecurseVisibilityChanges);
			cac->bHiddenInGame=false;
			AActor* actor=cac->GetChildActor();
			if(!actor) actor=cac->GetChildActorTemplate();
			ArdActor* rdActor=Cast<ArdActor>(actor);
			if(rdActor) {
				rdActor->Tags.Remove(TEXT("rdNoVis"));
				rdActor->SetHidden(false);
				rdActor->rdBuild();
			}
		}
	}

	for(auto c:comp->GetAttachChildren()) {
		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		if(cac) {
			cac->SetVisibility(true,bRecurseVisibilityChanges);
			cac->bHiddenInGame=false;
			AActor* actor=cac->GetChildActor();
			if(!actor) actor=cac->GetChildActorTemplate();
			ArdActor* rdActor=Cast<ArdActor>(actor);
			if(rdActor) {
				rdActor->Tags.Remove(TEXT("rdNoVis"));
				rdActor->SetHidden(false);
				rdActor->rdBuild();
			}
		}
	}
}

//.............................................................................
// rdThereCanBeOnlyOne
//.............................................................................
void ArdActor::rdThereCanBeOnlyOne() {

	int32 numFolders=0;
	TArray<FString> folders;

	// Count SceneComponents (not subclasses of)
	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(false,comps);
	for(auto c:comps) {

#if WITH_EDITOR
		if(c->GetName().StartsWith(TEXT("BillboardComponent"))) {
			c->SetVisibility(true,true);
			continue;
		}
#endif
		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
		UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(c);
//		UHierarchicalInstancedStaticMeshComponent* hismc=Cast<UHierarchicalInstancedStaticMeshComponent>(c);
//		if(!cac && !ismc && !hismc && c->GetNumChildrenComponents()>0) {
		if(!cac && !ismc && !smc && c->GetNumChildrenComponents()>0) {
			folders.AddUnique(c->GetName());
		}
	}

	// Count folders in instances list
	for(auto& it:InstanceDataX) {
		FrdInstanceSettingsArray& isa=it.Value;
		for(FrdInstanceFastSettings& fs:isa.settingsFast) {
			if(!fs.folder.IsEmpty()) folders.AddUnique(fs.folder);
		}
	}

	numFolders=folders.Num();

	if(numFolders<2) { // there are no scene components (excluding root)
		rdThereCanBeOnlyOne_Asset();
	} else {
		rdThereCanBeOnlyOne_Folder();
	}
}

//.............................................................................
// rdThereCanBeOnlyOne_Asset
//
// This is the main routine for showing only one Folder (USceneComponent) of Children
// for the Actor at any one time.
// A Folder is randomly chosen from all USceneComponents directly attached to the RootComponent.
// If the blueprint has no folders, a random asset is chosen
//
//.............................................................................
void ArdActor::rdThereCanBeOnlyOne_Asset() {

	if(!rdGetBaseActor()) return;
	rdBaseActor->rdRemInstancesForOwner(this);

	if(IsHidden() || Tags.Contains(TEXT("rdNoVis"))) {
		rdHideAllChildren();
		return;
	}

	if(!RootComponent) return;

	rdHideAllChildren();

	TArray<USceneComponent*> compList;
	TArray<FName> sidList;
	TArray<FName> rsidList;
	TArray<int32> indexList;

	// Components
	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(false,comps);
	for(auto c:comps) {

#if WITH_EDITOR
		if(c->GetName().StartsWith(TEXT("BillboardComponent"))) {
			c->SetVisibility(true,true);
			continue;
		}
#endif

		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(c);
		if(cac || smc) {
			compList.Add(c);
			sidList.Add(FName());
			rsidList.Add(FName());
			indexList.Add(0);
		}
	}

	// Instances
	FString theme=rdGetTheme();
	TArray<FString> sthemes;
	theme.ParseIntoArray(sthemes,TEXT(","));

	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		FrdInstanceSettingsArray& isa=it.Value;
		int32 ind=0;
		for(FrdInstanceFastSettings& fs:isa.settingsFast) {

			bool found=true;
			if(!theme.IsEmpty() && !fs.theme.IsEmpty()) {
				TArray<FString> themes;
				int32 num=fs.theme.ParseIntoArray(themes,TEXT(","));
				if(themes.Num()>0) { 
					found=false;
					for(auto t:sthemes) {
						if(themes.Contains(t)) {
							found=true;
							break;
						}
					}
				}
			}

			if(found) {
				compList.Add(nullptr);
				sidList.Add(sid);
				rsidList.Add(FName());
				indexList.Add(ind++);
			}
		}
		for(FrdInstanceRandomSettings& rs:isa.settingsRandom) {

			bool found=true;
			if(!theme.IsEmpty() && !rs.randomSettings.themes.IsEmpty()) {
				TArray<FString> themes;
				int32 num=rs.randomSettings.themes.ParseIntoArray(themes,TEXT(","));
				if(themes.Num()>0) { 
					found=false;
					for(auto t:sthemes) {
						if(themes.Contains(t)) {
							found=true;
							break;
						}
					}
				}
			}

			if(found) {
				compList.Add(nullptr);
				sidList.Add(FName());
				rsidList.Add(sid);
				indexList.Add(ind++);
			}
		}
	}

	if(compList.Num()>0) {
		int32 ind=round(rdGetRandomStream().FRandRange(0.0f,((float)compList.Num())-1.00f));

		USceneComponent* comp=compList[ind];
		
		if(comp) {

			comp->SetVisibility(true,bRecurseVisibilityChanges);
			comp->bHiddenInGame=false;

			UChildActorComponent* cac=Cast<UChildActorComponent>(comp);
			if(cac && cac->GetChildActor()) {
				AActor* act=cac->GetChildActor();
				ArdActor* ract=Cast<ArdActor>(act);
				if(ract) {
					ract->rdRemoveInstances();
					ract->rdBuild();
					ract->rdOnShowPrefab();
				} else {
					UFunction* func=act->FindFunction(FName(TEXT("rdShowPrefab")));
					if(func) {
						act->ProcessEvent(func,nullptr);
					}
				}
			}

		} else if(!sidList[ind].IsNone()) {

			FName sid=sidList[ind];
			if(indexList[ind]>=0) {
				UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
				FrdInstanceSettingsArray& i=InstanceDataX[sid];
				FrdInstanceFastSettings& j=i.settingsFast[indexList[ind]];

				j.index=rdAddInstanceFastWorld(ismc,j.transform);
				j.bNoVis=false;
				j.bVisible=true;
			}
		} else if(!rsidList[ind].IsNone()) {

			FName sid=rsidList[ind];
			if(indexList[ind]>=0) {
				UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
				FrdInstanceSettingsArray& i=InstanceDataX[sid];
				FrdInstanceRandomSettings& j=i.settingsRandom[indexList[ind]];

				j.index=rdAddInstanceFastWorld(ismc,j.origTransform);
				j.bNoVis=false;
				j.bVisible=true;
			}
		}
	}
}

//.............................................................................
// rdThereCanBeOnlyOne_Folder
//
// This is the main routine for showing only one Folder (USceneComponent) of Children
// for the Actor at any one time.
// A Folder is randomly chosen from all USceneComponents directly attached to the RootComponent.
//
//.............................................................................
void ArdActor::rdThereCanBeOnlyOne_Folder() {

	if(!rdGetBaseActor()) return;
	rdBaseActor->rdRemInstancesForOwner(this);

	if(IsHidden() || Tags.Contains(TEXT("rdNoVis"))) {
		rdHideAllChildren();
		return;
	}

	if(!RootComponent) return;

	rdHideAllChildren();

	TArray<USceneComponent*> comps;
	TArray<USceneComponent*> potentialComps; // This includes folders (references by the instanced static meshes)
	RootComponent->GetChildrenComponents(false,comps);
	for(auto c:comps) {

#if WITH_EDITOR
		if(c->GetName().StartsWith(TEXT("BillboardComponent"))) {
			c->SetVisibility(true,true);
			continue;
		}
#endif
		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		if(cac) { // anything in the root folder is always shown

			cac->SetVisibility(true,bRecurseVisibilityChanges);
			cac->bHiddenInGame=false;

		} else {

			USceneComponent* sc=Cast<USceneComponent>(c);
			if(sc) {
				if(rdFolderIncludedWithCurrentTheme(c)) {
					potentialComps.Add(c);
				}
			}
		}
	}

	USceneComponent* showComp=nullptr;
	if(potentialComps.Num()>0) {
		int32 ind=round(rdGetRandomStream().FRandRange(0.0f,((float)potentialComps.Num())-1.00f));

		showComp=potentialComps[ind];

		showComp->SetVisibility(true,bRecurseVisibilityChanges);
		showComp->bHiddenInGame=false;
		AActor* owner=showComp->GetOwner();
		showFolder=owner->GetName()+TEXT("/")+showComp->GetName();
		//showFolder=showComp->GetName();
	}

	// Hide all but the visible one
	for(auto c:comps) {
#if WITH_EDITOR
		if(c->GetName().StartsWith(TEXT("BillboardComponent"))) continue;
#endif
		UrdTransformAssetUserData* ftag=rdInstLib::GetTransformUserData(c,true);
		ftag->bProcessed=true;
		if(c==showComp) {
			ftag->bHide=false;
			rdBuildFolder(c);
			continue;
		}
		USceneComponent* sc=Cast<USceneComponent>(c);
		if(sc) {
			ftag->bHide=true;
			UChildActorComponent* cac=Cast<UChildActorComponent>(sc);
			if(cac) {
				ArdActor* ract=Cast<ArdActor>(cac->GetChildActor());
				if(ract && ract->rdGetBaseActor()) {
					ract->rdHideAllChildren();
				}
			}

		}
	}

	rdBuildShowOnlyOneISMs_Folder();
}

//.............................................................................
// rdBakeRandomization
//.............................................................................
void ArdActor::rdBakeRandomization() {

	rdBuild();

	Modify();

	// Components
	TArray<USceneComponent*> comps,delComps;
	RootComponent->GetChildrenComponents(false,comps);
	for(auto c:comps) {

#if WITH_EDITOR
		if(c->GetName().StartsWith(TEXT("BillboardComponent"))) {
			continue;
		}
#endif
		if(c->bHiddenInGame) delComps.Add(c);
	}

	for(auto c:delComps) {
		c->UnregisterComponent();
		c->DestroyComponent();
	}

	// Instances
	for(auto& it:InstanceData) {
		UStaticMesh* mesh=it.Key;
		FrdInstanceSettingsArray& isa=it.Value;
		TArray<int32> delList;
		int32 ind=0;
		for(FrdInstanceFastSettings& fs:isa.settingsFast) {
			if(!fs.bVisible) {
				delList.Add(ind);
			}
			ind++;
		}
		for(int32 i=delList.Num()-1;i>=0;i--) {
			isa.settingsFast.RemoveAt(delList[i]);
		}
		delList.Empty();
		ind=0;
		for(FrdInstanceRandomSettings& rs:isa.settingsRandom) {
			if(!rs.bVisible) {
				delList.Add(ind);
			}
			ind++;
		}
		for(int32 i=delList.Num()-1;i>=0;i--) {
			isa.settingsRandom.RemoveAt(delList[i]);
		}
	}
}

//.............................................................................
// rdClearAlteredList
//.............................................................................
void ArdActor::rdClearAlteredList() {
	alterations.Empty();
}

//.............................................................................
// rdBuildAlteredList
//.............................................................................
FString ArdActor::rdBuildAlteredList() {
	FString str;
	for(auto a:alterations) {
		str+=a+TEXT("~");
	}
	return str;
}

//.............................................................................
// rdApplyAlteredList
//.............................................................................
int32 ArdActor::rdApplyAlteredList(const FString& str) {

	TArray<FString> alts;
	int32 num=str.ParseIntoArray(alts,TEXT("~"));
	for(auto a:alts) {
		alterations.Add(a);
	}

	return num;
}

//.............................................................................
// rdMoveArraysToTables
//.............................................................................
void ArdActor::rdMoveArraysToTables() {

	if(!bCreateFromArrays) return;
	if(!rdGetBaseActor()) return;

	InstanceData.Empty();
	InstanceDataX.Empty();

	// Fast Array
	for(FrdAddInstanceFastArray& i:InstanceFastArray) {
		if(!i.meshSetup.mesh && i.mesh) i.meshSetup.mesh=i.mesh;
		if(i.meshSetup.sid.IsNone()) {
			i.meshSetup.sid=rdBaseActor->rdGetInstSid(i.meshSetup);
			if(i.meshSetup.sid.IsNone()) {
				continue;
			}
		}
		FrdInstanceSettingsArray& ia=InstanceDataX.FindOrAdd(i.meshSetup.sid);
		TArray<FrdInstanceFastSettings>& isa=ia.settingsFast;
		for(FTransform& tt:i.transforms) {
			isa.Add(FrdInstanceFastSettings(tt));
		}
	}

	// Randomized Array
	for(FrdAddInstanceRandomizedArray& i:InstanceRandomizedArray) {
		if(!i.meshSetup.mesh && i.mesh) i.meshSetup.mesh=i.mesh;
		if(i.meshSetup.sid.IsNone()) {
			i.meshSetup.sid=rdBaseActor->rdGetInstSid(i.meshSetup);
			if(i.meshSetup.sid.IsNone()) {
				continue;
			}
		}
		FrdInstanceSettingsArray& ia=InstanceDataX.FindOrAdd(i.meshSetup.sid);
		TArray<FrdInstanceRandomSettings>& isa=ia.settingsRandom;
		for(FrdInstanceRandomSettings& tt:i.settings) {
			isa.Add(tt);
		}
	}

	InstanceFastArray.Empty();
	InstanceRandomizedArray.Empty();

	bCreateFromArrays=false;

	Modify();
}

//.............................................................................
// rdMoveTablesToArrays
//.............................................................................
void ArdActor::rdMoveTablesToArrays() {

	if(bCreateFromArrays) return;
	if(!rdGetBaseActor()) return;

	InstanceFastArray.Empty();
	InstanceRandomizedArray.Empty();

	for(auto& it:InstanceData) {
		UStaticMesh* mesh=it.Key;
		FrdInstanceSettingsArray& table=it.Value;
		FName sid=rdBaseActor->rdGetSMsid(mesh);

		TArray<UMaterialInterface*> mats;
		FrdInstanceSetup isa(ErdSpawnType::UseDefaultSpawn,sid,mesh,mats,ErdCollision::UseDefault,0.0f,0.0f,-1,NAME_None);

		if(table.settingsFast.Num()>0) {

			TArray<FTransform> transforms;
			for(int i=0;i<table.settingsFast.Num();i++) {
				transforms.Add(table.settingsFast[i].transform);
			}
			InstanceFastArray.Add(FrdAddInstanceFastArray(isa,transforms));
		}

		if(table.settingsRandom.Num()>0) {

			InstanceRandomizedArray.Add(FrdAddInstanceRandomizedArray(isa,table.settingsRandom) );
		}
	}

	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		FrdInstanceSettingsArray& table=it.Value;

		int32 ver=0;
		TEnumAsByte<ErdSpawnType> type=ErdSpawnType::UseDefaultSpawn;
		TSoftObjectPtr<UStaticMesh> mesh=nullptr;
		TArray<TSoftObjectPtr<UMaterialInterface>> materials;
		bool bReverseCulling=false;
		TEnumAsByte<ErdCollision> collision;
		float startCull,endCull;
		int32 id;
		FName groupID;
		rdBaseActor->rdGetSidDetails(sid,ver,type,mesh,materials,bReverseCulling,collision,startCull,endCull,id,groupID);

		TArray<UMaterialInterface*> mats;
		for(auto m:materials) {
			mats.Add(m.Get());
		}

		FrdInstanceSetup isa(type,sid,mesh,mats,collision,startCull,endCull,id,groupID);

		if(table.settingsFast.Num()>0) {

			TArray<FTransform> transforms;
			for(int i=0;i<table.settingsFast.Num();i++) {
				transforms.Add(table.settingsFast[i].transform);
			}
			InstanceFastArray.Add(FrdAddInstanceFastArray(isa,transforms));
		}

		if(table.settingsRandom.Num()>0) {

			InstanceRandomizedArray.Add(FrdAddInstanceRandomizedArray(isa,table.settingsRandom) );
		}
	}

	InstanceData.Empty();
	InstanceDataX.Empty();

	bCreateFromArrays=true;
	Modify();
}

//.............................................................................
