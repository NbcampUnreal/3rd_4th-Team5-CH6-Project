//
// rdActor_Assimilation.cpp
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 2nd October 2023
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

//.............................................................................
// rdAssimilateChildInstances
//
// Copies all the StaticMesh Instance data from the specified rdActor into
// this actors data list.
// This is a very useful method - rdBuildBuddy uses it to build one actor
// with one StaticMeshInstance list from procedurally generated nested children.
//
// You can do similar things yourself with your own code, calling this.
//
// Only rdActors with the bCanBeAssimilated flag set to True will be assimilated.
//
//.............................................................................
void ArdActor::rdAssimilate(ArdActor* rdActorParent,bool justVisible) {

	if(!bCanBeAssimilated) return;

	FTransform deltaTransform(rdActorParent->GetActorLocation()*-1.0f);
	for(auto& it:InstanceDataX) {
		FName sid=it.Key;
		FrdInstanceSettingsArray& i=it.Value;

		for(FrdInstanceFastSettings& j:i.settingsFast) {

			if(!justVisible || j.bVisible) {

				FrdInstanceFastSettings is;
				is.transform=rdInstToWorld(j.transform);
				is.transform.Accumulate(deltaTransform);
				is.index=j.index;

				FrdInstanceSettingsArray& isa=rdActorParent->InstanceDataX.FindOrAdd(sid);
				isa.settingsFast.Add(MoveTemp(is));
			}
		}
		for(FrdInstanceRandomSettings& j:i.settingsRandom) {

			if(!justVisible || j.bVisible) {

				FrdInstanceFastSettings is;
				is.transform=rdInstToWorld(j.actualTransform);
				is.transform.Accumulate(deltaTransform);
				is.index=j.index;

				FrdInstanceSettingsArray& isa=rdActorParent->InstanceDataX.FindOrAdd(sid);
				isa.settingsFast.Add(MoveTemp(is));
			}
		}
	}
}

//.............................................................................
// rdAssimilateRecurse
//.............................................................................
void ArdActor::rdAssimilateRecurse(ArdActor* rdActorParent,bool justVisible) {

	rdAssimilate(rdActorParent,justVisible);

	TArray<ArdActor*> rdList;
	TArray<USceneComponent*> comps;
	RootComponent->GetChildrenComponents(true,comps);

	for(auto c:comps) {
		UChildActorComponent* ca=Cast<UChildActorComponent>(c);
		if(ca) {
			ArdActor* rdAct=Cast<ArdActor>(ca->GetChildActor());
			if(rdAct && rdAct->bCanBeAssimilated) {
				rdAct->rdAssimilateRecurse(rdActorParent,justVisible);
			} else {
				rdActorParent->rdAddChildActorComponent(ca);
			}
		} else {
			USceneComponent* sc=Cast<USceneComponent>(c);
			if(sc) {
				for(auto ac:sc->GetAttachChildren()) {
					ArdActor* rdAct=Cast<ArdActor>(ac->GetOwner());
					if(rdAct && rdAct!=this) {
						rdList.AddUnique(rdAct);
					}
				}
			}
		}
	}

	for(auto ac:RootComponent->GetAttachChildren()) {
		ArdActor* rdAct=Cast<ArdActor>(ac->GetOwner());
		if(rdAct && rdAct!=this) {
			rdList.AddUnique(rdAct);
		}
	}

	for(auto rac:rdList) {
		if(rac->bCanBeAssimilated) {
			rac->rdAssimilateRecurse(rdActorParent,justVisible);
		} else {
			//rdActorParent->rdSpawnActor(rac);
		}
	}
}

//.............................................................................
// rdAssimilateChildInstances
//.............................................................................
void ArdActor::rdAssimilateChildInstances(ArdActor* rdActor,bool justVisible,bool recurse) {

	if(!rdActor || !rdActor->bCanBeAssimilated) return;

	if(recurse) {
		rdActor->rdAssimilateRecurse(this,justVisible);
	} else {
		rdActor->rdAssimilate(this,justVisible);
	}
}

//.............................................................................
