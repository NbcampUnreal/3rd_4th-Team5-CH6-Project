//
// rdActor_Visibility.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 24th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "rdInstBaseActor.h"

//.............................................................................
// rdSetInstanceVisibility
//.............................................................................
bool ArdActor::rdSetInstanceVisibility(UStaticMesh* mesh,int32 index,bool vis) {
	return rdSetInstanceVisibilityX(rdGetSMsid(mesh),index,vis);
}
bool ArdActor::rdSetInstanceVisibilityX(const FName sid,int32 index,bool vis) {

	if(!rdGetBaseActor()) return false;
	return rdBaseActor->rdSetInstanceVisibilityX(sid,index,vis);
}

//.............................................................................
// rdSetInstanceVisibilityFast
//.............................................................................
bool ArdActor::rdSetInstanceVisibilityFast(UInstancedStaticMeshComponent* ismc,int32 index,bool vis) {

	if(!rdGetBaseActor()) return false;
	return rdBaseActor->rdSetInstanceVisibilityFast(ismc,index,vis);
}

//.............................................................................
// rdSetInstancesVisibility
//.............................................................................
void ArdActor::rdSetInstancesVisibility(UStaticMesh* mesh,const TArray<int32>& indexes,bool vis) {
	rdSetInstancesVisibilityX(rdGetSMsid(mesh),indexes,vis);
}
void ArdActor::rdSetInstancesVisibilityX(const FName sid,const TArray<int32>& indexes,bool vis) {

	if(!rdGetBaseActor()) return;
	rdBaseActor->rdSetInstancesVisibilityX(sid,indexes,vis);
}

//.............................................................................
// rdSetInstancesVisibilityFast
//.............................................................................
void ArdActor::rdSetInstancesVisibilityFast(UInstancedStaticMeshComponent* ismc,const TArray<int32>& indexes,bool vis) {

	if(!rdGetBaseActor()) return;
	rdBaseActor->rdSetInstancesVisibilityFast(ismc,indexes,vis);
}

//.............................................................................
// rdHideAllChildren
//
// Fastest way of removing everything belonging to this actor from the level.
// Hides Child Components and removes the Instances.
//
//.............................................................................
void ArdActor::rdHideAllChildren() {
	
	if(!rdBaseActor) return; // no base actor? not showing - all good...

	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(true,comps);
	for(auto c:comps) {
		c->SetVisibility(false,bRecurseVisibilityChanges);
		c->bHiddenInGame=true;
		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		if(cac) {
			UrdTransformAssetUserData* ftag=rdInstLib::GetTransformUserData(c,true);
			ftag->bHide=true;
			AActor* act=Cast<ArdActor>(cac->GetChildActor());
			if(act) {
				ArdActor* ract=Cast<ArdActor>(act);
				if(ract) {
					rdBaseActor->rdRemInstancesForOwner(ract);
					for(auto& it:ract->InstanceDataX) {
						FName sid=it.Key;
						FrdInstanceSettingsArray& i=it.Value;
						for(FrdInstanceFastSettings& j:i.settingsFast) j.bVisible=false;
						for(FrdInstanceRandomSettings& j:i.settingsRandom) j.bVisible=false;
					}
					ract->rdOnHidePrefab();
				} else {
					UFunction* func=act->FindFunction(FName(TEXT("rdHidePrefab")));
					if(func) {
						act->ProcessEvent(func,nullptr);
					}
				}
			}
		}
	}

	rdBaseActor->rdRemInstancesForOwner(this);
	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
		if(ismc) {
			FrdInstanceSettingsArray& i=it.Value;
			for(FrdInstanceFastSettings& j:i.settingsFast) j.bVisible=false;
			for(FrdInstanceRandomSettings& j:i.settingsRandom) j.bVisible=false;
		}
	}
}

//.............................................................................
// rdBuildAsHidden
//.............................................................................
void ArdActor::rdBuildAsHidden() {

	rdBuildFolderVisibility(GetRootComponent(),TEXT(""),false);
	Super::SetActorHiddenInGame(true);
}

//.............................................................................
// rdBuildAsVisible
//.............................................................................
void ArdActor::rdBuildAsVisible() {

	rdBuildFolderVisibility(GetRootComponent(),TEXT(""),true);
	Super::SetActorHiddenInGame(false);
}

//.............................................................................
// rdSetActorVisibility
//
// Just Hides/Shows things rather than rebuilding (doesn't recalculate reliance etc)
//
//.............................................................................
void ArdActor::rdSetActorVisibility(bool vis) {

	TArray<USceneComponent*> comps;
	GetComponents(comps,false);
	for(auto c:comps) {

		if(c->ComponentTags.Contains(TEXT("rdNoVis"))) {
			continue;
		}

		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		if(cac) {
			AActor* actor=cac->GetChildActor();
			if(!actor) actor=cac->GetChildActorTemplate();
			ArdActor* rdActor=Cast<ArdActor>(actor);

			if(rdActor) {

				rdActor->rdSetActorVisibility(vis);

			} else if(actor) {

				actor->SetHidden(!vis);
			}

		} else {

			c->SetVisibility(vis,bRecurseVisibilityChanges);
			c->bHiddenInGame=!vis;
		}
	}

	if(!rdGetBaseActor()) return;

	// ISMs
	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
		FrdInstanceSettingsArray& i=it.Value;
		for(FrdInstanceFastSettings& j:i.settingsFast) {
			if(!j.bVisible) continue;
			if(!vis && j.index>=0) {
				rdBaseActor->rdRemoveInstanceFast(ismc,j.index);
				j.index=-1;
			} else if(vis && j.index==-1) {
				j.index=rdAddInstanceFastWorld(ismc,j.transform);
			}
		}

		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
			if(!j.bVisible) continue;
			if(!vis && j.index>=0) {
				rdBaseActor->rdRemoveInstanceFast(ismc,j.index);
				j.index=-1;
			} else if(vis && j.index==-1) {
				j.index=rdAddInstanceFastWorld(ismc,j.actualTransform);
			}
		}
	}

	Super::SetActorHiddenInGame(!vis);
}

//.............................................................................
// rdBuildFolderVisibility
//.............................................................................
void ArdActor::rdBuildFolderVisibility(USceneComponent* comp,const FString& folder,bool vis) {

	TArray<USceneComponent*> comps;
	if(!comp) {
		GetComponents(comps,false);
		for(auto c:comps) {
			if(c->GetName()==folder) {
				comp=c;
				break;
			}
		}
	}
	
	if(!comp) return;

	if(vis) comp->ComponentTags.Remove(TEXT("rdNoVis"));
	else    comp->ComponentTags.AddUnique(TEXT("rdNoVis"));
	UrdTransformAssetUserData* ftag=(UrdTransformAssetUserData*)comp->GetAssetUserDataOfClass(UrdTransformAssetUserData::StaticClass());
	if(ftag) {
		ftag->bHide=!vis;
		ftag->bNoVis=!vis;
	}

	comp->GetChildrenComponents(true,comps);

	for(auto c:comps) {

		c->SetVisibility(vis,bRecurseVisibilityChanges);
		c->bHiddenInGame=!vis;
		if(vis) c->ComponentTags.Remove(TEXT("rdNoVis"));
		else    c->ComponentTags.AddUnique(TEXT("rdNoVis"));
		ftag=(UrdTransformAssetUserData*)c->GetAssetUserDataOfClass(UrdTransformAssetUserData::StaticClass());
		if(ftag) {
			ftag->bHide=!vis;
			ftag->bNoVis=!vis;
		}

		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		if(cac) {
			AActor* actor=cac->GetChildActor();
			if(!actor) actor=cac->GetChildActorTemplate();
			if(actor) actor->SetHidden(!vis);
			ArdActor* rdActor=Cast<ArdActor>(actor);
			if(rdActor) {
				if(vis) {
					rdActor->Tags.Remove(TEXT("rdNoVis"));
					rdActor->rdBuild();
					rdActor->rdOnShowPrefab();
				} else {
					rdActor->Tags.Add(TEXT("rdNoVis"));
					rdActor->rdRemoveInstances();
					rdActor->rdOnHidePrefab();
				}
			} else {
				if(vis) {
					UFunction* func=actor->FindFunction(FName(TEXT("rdShowPrefab")));
					if(func) {
						actor->ProcessEvent(func,nullptr);
					}
				} else {
					UFunction* func=actor->FindFunction(FName(TEXT("rdHidePrefab")));
					if(func) {
						actor->ProcessEvent(func,nullptr);
					}
				}
			}
		}
	}

	if(!rdGetBaseActor()) return;

	// ISMs
	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
		FrdInstanceSettingsArray& i=it.Value;
		//bool loop=true;
		//while(loop) {
			//loop=false;
			for(FrdInstanceFastSettings& j:i.settingsFast) {
				if(j.folder.IsEmpty() || folder.IsEmpty() || j.folder==folder) {
					if(j.bNoVis!=(!vis) || (vis && !j.bVisible) || (!vis && j.bVisible)) {
						j.bNoVis=!vis;
						if(!vis) {
							rdBaseActor->rdRemoveInstanceX(sid,j.index);
							j.bVisible=false;
							j.index=-1;
						} else {
							j.index=rdAddInstanceFastWorld(ismc,j.transform);
							j.bVisible=true;
						}
						//loop=true;
						//break;
					}
				}
			}
		//}
		//loop=true;
		//while(loop) {
			//loop=false;
			for(FrdInstanceRandomSettings& j:i.settingsRandom) {
				if(j.folder.IsEmpty() || folder.IsEmpty() || j.folder==folder) {
					if(j.bNoVis!=(!vis) || (vis && !j.bVisible) || (!vis && j.bVisible)) {
						j.bNoVis=!vis;
						if(!vis) {
							rdBaseActor->rdRemoveInstanceX(sid,j.index);
							j.bVisible=false;
							j.index=-1;
						} else {
							j.index=rdAddInstanceFastWorld(ismc,j.actualTransform);
							j.bVisible=true;
						}
						//loop=true;
						//break;
					}
				}
			}
		//}
	}
}

//.............................................................................
// rdBuildFolderNameVisibility
//.............................................................................
void ArdActor::rdBuildFolderNameVisibility(const FString& folder,bool vis) {

	TArray<USceneComponent*> comps;
	GetComponents(comps,false);
	for(auto c:comps) {
		if(c->GetName()==folder) {
			rdBuildFolderVisibility(c,folder,vis);
			break;
		}
	}
}

//.............................................................................
// rdSetFolderVisibility
//
// This is the main method for showing/hiding sections of the Blueprint
//.............................................................................
void ArdActor::rdSetFolderVisibility(USceneComponent* comp,const FString& folder,bool vis) {

	TArray<USceneComponent*> comps;
	if(!comp) {
		GetComponents(comps,false);
		for(auto c:comps) {
			if(c->GetName()==folder) {
				comp=c;
				break;
			}
		}
	}
	
	if(!comp) return;
	comp->GetChildrenComponents(true,comps);

	for(auto c:comps) {

		if(c->ComponentTags.Contains(TEXT("rdNoVis"))) {
			continue;
		}

		UChildActorComponent* cac=Cast<UChildActorComponent>(c);
		if(cac) {
			AActor* actor=cac->GetChildActor();
			if(!actor) actor=cac->GetChildActorTemplate();
			ArdActor* rdActor=Cast<ArdActor>(actor);
			if(rdActor) {

				rdActor->rdSetActorVisibility(vis);

			} else if(actor) {

				actor->SetHidden(!vis);
			}

		} else {

			c->SetVisibility(vis,bRecurseVisibilityChanges);
			c->bHiddenInGame=!vis;
		}
	}

	if(!rdGetBaseActor()) return;

	// ISMs
	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		UInstancedStaticMeshComponent* ismc=rdGetInstanceGenX(sid);
		FrdInstanceSettingsArray& i=it.Value;
		for(FrdInstanceFastSettings& j:i.settingsFast) {
			if(j.bNoVis) continue;
			if(!vis && j.index>=0) {
				rdBaseActor->rdRemoveInstanceFast(ismc,j.index);
				j.index=-1;
			} else if(vis && j.index==-1) {
				j.index=rdAddInstanceFastWorld(ismc,j.transform);
			}
		}

		for(FrdInstanceRandomSettings& j:i.settingsRandom) {
			if(j.bNoVis) continue;
			if(!vis && j.index>=0) {
				rdBaseActor->rdRemoveInstanceFast(ismc,j.index);
				j.index=-1;
			} else if(vis && j.index==-1) {
				j.index=rdAddInstanceFastWorld(ismc,j.actualTransform);
			}
		}
	}
}

//.............................................................................
// rdSetFolderNameVisibility
//
// This is the main method for showing/hiding sections of the Blueprint
//.............................................................................
void ArdActor::rdSetFolderNameVisibility(const FString& folder,bool vis) {

	TArray<USceneComponent*> comps;
	GetComponents(comps,false);
	for(auto c:comps) {
		if(c->GetName()==folder) {
			rdSetFolderVisibility(c,folder,vis);
			break;
		}
	}
}

//.............................................................................
