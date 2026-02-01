//
// rdActor_Spawning.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 9th September 2023
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"
#include "Engine/StaticMeshActor.h"

//.............................................................................
// rdSpawnActor
//
// This Spawn method can be used to easily add new Child Actors at runtime.
// This can be called from a Blueprints constructor, just ensure that
// rdDestroyAttachedActors() has been called first.
//
//.............................................................................
AActor* ArdActor::rdSpawnActor(TSubclassOf<class AActor> actorClass,const FTransform& transform,const FName label,AActor* parent,bool deferConstruction) {

	if(!rdGetBaseActor()) return nullptr;
	AActor* actor=rdBaseActor->rdSpawnActor(actorClass,transform,nullptr,false,true,label,parent,deferConstruction);

	if(actor) {

		if(actor->GetRootComponent()==nullptr) {
			for(auto c:actor->GetComponents()) {
				USceneComponent* sc=Cast<USceneComponent>(c);
				if(sc && (sc->GetName().StartsWith(TEXT("Default")) || sc->GetName().StartsWith(TEXT("shared")))) {
					actor->SetRootComponent(sc);
					break;
				}
			}
		}

		if(actorClass==AStaticMeshActor::StaticClass()) {
			((AStaticMeshActor*)actor)->SetMobility(EComponentMobility::Movable);
		}
		actor->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
	}
	return actor;
}

//.............................................................................
// rdSpawnActorPos
//
// This Spawn method can be used to easily add new Child Actors at runtime.
// This can be called from a Blueprints constructor, just ensure that
// rdDestroyAttachedActors() has been called first.
//
//.............................................................................
AActor* ArdActor::rdSpawnActorPos(TSubclassOf<class AActor> actorClass,const FTransform& transform,const FrdPositionInfo& pos,const FName label) {

	if(!rdGetBaseActor()) return nullptr;
	AActor* actor=rdBaseActor->rdSpawnActor(actorClass,transform,nullptr,false,true,label);
	if(actor) {
		
		if(actor->GetRootComponent()==nullptr) {
			for(auto c:actor->GetComponents()) {
				USceneComponent* sc=Cast<USceneComponent>(c);
				if(sc && (sc->GetName().StartsWith(TEXT("Default")) || sc->GetName().StartsWith(TEXT("shared")))) {
					actor->SetRootComponent(sc);
					break;
				}
			}
		}

		if(actorClass==AStaticMeshActor::StaticClass()) {
			((AStaticMeshActor*)actor)->SetMobility(EComponentMobility::Movable);
		}

		actor->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
		ArdActor* rdActor=Cast<ArdActor>(actor);
		if(rdActor) {
			rdActor->positionInfo=pos;	
		}
#if WITH_EDITOR
		actor->RerunConstructionScripts();
#endif
	}

	
	return actor;
}

//.............................................................................
// rdSpawnActor
//
// Private SpawnActor used by the Assimilation system
//
//.............................................................................
AActor* ArdActor::rdSpawnActor(AActor* srcActor) {

	if(!rdGetBaseActor()) return nullptr;
	AActor* actor=rdBaseActor->rdSpawnActor(srcActor->GetClass(),srcActor->GetActorTransform(),srcActor);
	if(actor) {
		
		if(actor->GetRootComponent()==nullptr) {
			for(auto c:actor->GetComponents()) {
				USceneComponent* sc=Cast<USceneComponent>(c);
				if(sc && (sc->GetName().StartsWith(TEXT("Default")) || sc->GetName().StartsWith(TEXT("shared")))) {
					actor->SetRootComponent(sc);
					break;
				}
			}
		}
		actor->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
		ArdActor* rdActor=Cast<ArdActor>(actor);
		if(rdActor) {
			rdActor->positionInfo=Cast<ArdActor>(srcActor)->positionInfo;	
		}
#if WITH_EDITOR
		actor->RerunConstructionScripts();
#endif
	}
	
	return actor;
}

//.............................................................................
// rdAddChildActorComponent
//
// This method is used internally to add child components when assimilating
//
//.............................................................................
UChildActorComponent* ArdActor::rdAddChildActorComponent(UChildActorComponent* cac) {
/*
#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	UChildActorComponent* cac=(UChildActorComponent*)AddComponentByClass(UChildActorComponent::StaticClass(),false,transform,false);
#else
	UChildActorComponent* cac=NewObject<UChildActorComponent>(this,UChildActorComponent::StaticClass());
    cac->RegisterComponent();
	cac->SetRelativeTransform(transform);
#endif

	cac->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	cac->SetChildActorClass(actorClass);
*/
	return nullptr;
}

//.............................................................................
// rdAddComponent
//.............................................................................
UActorComponent* ArdActor::rdAddComponent(TSubclassOf<class UActorComponent> compClass) {

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	UActorComponent* ac=(UActorComponent*)AddComponentByClass(compClass,false,FTransform(),false);
#else
	UActorComponent* ac=NewObject<UActorComponent>(this,compClass);
    ac->RegisterComponent();
#endif

	return ac;
}

//.............................................................................
// rdAddChildComponent
//
// This method can be used to easily add new Child Actors at runtime.
// This can be called from a Blueprints constructor, just ensure that
// rdDestroyAttachedComponents() has been called first.
//
//.............................................................................
UChildActorComponent* ArdActor::rdAddChildComponent(UClass* actorClass,const FTransform& transform) {

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	UChildActorComponent* cac=(UChildActorComponent*)AddComponentByClass(UChildActorComponent::StaticClass(),false,transform,false);
#else
	UChildActorComponent* cac=NewObject<UChildActorComponent>(this,UChildActorComponent::StaticClass());
    cac->RegisterComponent();
	cac->SetRelativeTransform(transform);
#endif

	cac->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	cac->SetChildActorClass(actorClass);

	return cac;
}

//.............................................................................
// rdAddChildComponent
//
// This method can be used to easily add new Child Actors at runtime.
// This can be called from a Blueprints constructor, just ensure that
// rdDestroyAttachedComponents() has been called first.
//
//.............................................................................
UChildActorComponent* ArdActor::rdAddChildComponentPos(UClass* actorClass,const FTransform& transform,const FrdPositionInfo& pos) {

#if ENGINE_MAJOR_VERSION>4 || ENGINE_MINOR_VERSION>25
	UChildActorComponent* cac=(UChildActorComponent*)AddComponentByClass(UChildActorComponent::StaticClass(),false,transform,false);
#else
	UChildActorComponent* cac=NewObject<UChildActorComponent>(this,UChildActorComponent::StaticClass());
    cac->RegisterComponent();
	cac->SetRelativeTransform(transform);
#endif

	cac->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	cac->SetChildActorClass(actorClass);

	return cac;
}

//.............................................................................
// rdDestroyAttachedActors
//
// Removes all Actors attached with the rdSpawnActor method.
//
//.............................................................................
void ArdActor::rdDestroyAttachedActors() {

	TArray<class AActor*> actors;
#if ENGINE_MAJOR_VERSION>4
	GetAttachedActors(actors,true,false);
#else
	GetAttachedActors(actors,true);
#endif
	for(auto a:actors) {
		ArdActor* rdActor=Cast<ArdActor>(a);
		if(rdActor) {
			rdActor->rdRemoveInstances();
		}
		a->Destroy();
	}
}

//.............................................................................
// rdDestroyAttachedComponents
//
// Removes all ChildActors attached with the rdAddChildComponent method.
//
//.............................................................................
void ArdActor::rdDestroyAttachedComponents() {

	TArray<UActorComponent*> cac;
	GetComponents(UChildActorComponent::StaticClass(),cac,false);
	for(auto c:cac)	{
		
		if(c->ComponentHasTag(TEXT("rdSticky"))) continue;

		AActor* actor=((UChildActorComponent*)c)->GetChildActor();

		ArdActor* rdActor=Cast<ArdActor>(actor);
		if(rdActor) {
			rdActor->rdRemoveInstances();
		}

		c->DestroyComponent();
	}
}


//.............................................................................
