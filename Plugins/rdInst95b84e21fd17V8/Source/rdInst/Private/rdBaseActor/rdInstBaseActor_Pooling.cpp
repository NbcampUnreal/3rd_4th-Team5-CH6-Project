//
// rdInstBaseActor_Pooling.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 11th April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "rdActor.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "rdSpawnActor.h"

//#define _rdDebugPools

//.............................................................................
// rdPoolActor
//.............................................................................
void ArdInstBaseActor::rdPoolActor(UClass* actorClass,int32 numToPool,bool premake,bool doTick,bool startHidden,bool reuse,bool simplePool,int32 growBy) {

	if(actorClass==nullptr) {
		UE_LOG(LogTemp,Display,TEXT("rdPoolActor() - null class type"));
		return;
	}

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("rdPoolActor(%s,%d)"),(!actorClass->GetName().IsEmpty())?*actorClass->GetName():TEXT("No Class Name"),numToPool);
#endif

	if(poolMap.Contains(actorClass)) {
#ifdef _rdDebugPools
		UE_LOG(LogTemp,Display,TEXT("rdPoolActor called for existing pooled actor"));
#endif
		rdSetActorPoolSize(actorClass,numToPool,premake,doTick,startHidden,reuse,simplePool,growBy);
	
	} else {
	
		rdPoolData data(numToPool,reuse,simplePool,premake,doTick,startHidden,growBy);
		data.reuseLast=reuse;
		data.simple=simplePool;
		if(premake) {
			for(int32 i=0;i<numToPool;i++) {

				rdPoolItem item;
				item.actor=rdSpawnActor(actorClass,FTransform(),nullptr,false,false);
				if(item.actor) {
					item.actor->SetLifeSpan(0.0f);
					if(!simplePool) {
						item.actor->SetActorHiddenInGame(true);
					}
					item.hasTick=doTick;
					item.hasCollision=item.actor->GetActorEnableCollision();
					if(!simplePool) {
						item.actor->SetActorEnableCollision(false);
					}
/*
GetMesh()->PrimaryComponentTick.bCanEverTick=false;
GetMesh()->SetComponentTickEnabled(false);
GetMesh()->SetActive(false);
GetMesh()->SetAutoActivate(false);

SetActorTickEnabled(false);
PrimaryActorTick.bCanEverTick=false;
*/
					if(poolListener) {
						poolListener->rdOnActorDepooled(item.actor);
					}
					ArdActor* rdActor=Cast<ArdActor>(item.actor);
					if(rdActor) rdActor->rdOnActorDepooled(item.actor);
				} else {
					UE_LOG(LogTemp,Display,TEXT("Failed to create actors for rdPool."));
				}
				data.pool.Add(MoveTemp(item));
			}
		}
		poolMap.Add(actorClass,MoveTemp(data));
	}
}

//.............................................................................
// rdSetActorPoolSize
//.............................................................................
bool ArdInstBaseActor::rdSetActorPoolSize(UClass* actorClass,int32 numToPool,bool premake,bool doTick,bool startHidden,bool reuse,bool simplePool,int32 growBy) {

	if(actorClass==nullptr) {
		UE_LOG(LogTemp,Display,TEXT("rdSetActorPoolSize() - null class"));
		return false;
	}

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("rdSetActorPoolSize(%s,%d)"),(!actorClass->GetName().IsEmpty())?*actorClass->GetName():TEXT("No Class Name"),numToPool);
#endif

	rdPoolData* pool=poolMap.Find(actorClass);
	if(pool) {
		rdPoolData& dat=*pool;
		dat.reuseLast=reuse;
		dat.simple=simplePool;
		dat.growBy=growBy;
		if(numToPool==dat.pool.Num()) {

#ifdef _rdDebugPools
			UE_LOG(LogTemp,Display,TEXT("rdSetActorPoolSize called - pool already that size"));
#endif
			return true;
		}

		if(numToPool>dat.pool.Num()) {
			
			int32 origNum=dat.pool.Num();

			if(numToPool>origNum) {
				dat.numPooled=numToPool;
			}

			if(premake) {
				for(int32 i=0;i<origNum;i++) {
					rdPoolItem& item=dat.pool[i];
					if(!item.actor) {
						item.actor=rdSpawnActor(actorClass,FTransform(),nullptr,false,false);
						if(item.actor) {
							item.actor->SetLifeSpan(0.0f);
							item.actor->SetActorHiddenInGame(true); 
							item.hasTick=doTick;
							item.startHidden=startHidden;
							item.hasCollision=item.actor->GetActorEnableCollision();
							if(!simplePool) {
								item.actor->SetActorEnableCollision(false);
							}
							if(poolListener) {
								poolListener->rdOnActorDepooled(item.actor);
							}
							ArdActor* rdActor=Cast<ArdActor>(item.actor);
							if(rdActor) rdActor->rdOnActorDepooled(item.actor);
						} else {
							UE_LOG(LogTemp,Display,TEXT("Failed to create actors for rdPool2."));
						}
					}
				}
				for(int32 i=origNum;i<numToPool;i++) {

					rdPoolItem item;
					item.actor=rdSpawnActor(actorClass,FTransform(),nullptr,false,false);
					if(item.actor) {
						item.actor->SetLifeSpan(0.0f);
						item.actor->SetActorHiddenInGame(true);
						item.hasTick=doTick;
						item.startHidden=startHidden;
						item.hasCollision=item.actor->GetActorEnableCollision();
						if(!simplePool) {
							item.actor->SetActorEnableCollision(false);
						}
						if(poolListener) {
							poolListener->rdOnActorDepooled(item.actor);
						}
						ArdActor* rdActor=Cast<ArdActor>(item.actor);
						if(rdActor) rdActor->rdOnActorDepooled(item.actor);
					} else {
						UE_LOG(LogTemp,Display,TEXT("Failed to create actors for rdPool4."));
					}
					dat.pool.Add(MoveTemp(item));
				}
			}

			return true;

		} else {
#ifdef _rdDebugPools
			UE_LOG(LogTemp,Display,TEXT("rdSetActorPoolSize called tried to resize pool to smaller than existing size"));
#endif
			return true; // we don't resize to less that the currently used amount of actors in the pool
		}

	} else {

		rdPoolActor(actorClass,numToPool,premake,doTick,startHidden,reuse,simplePool,growBy);
	}
	return true;
}

//.............................................................................
// rdRemoveActorPool
//.............................................................................
void ArdInstBaseActor::rdRemoveActorPool(UClass* actorClass) {

	if(actorClass==nullptr) {
		UE_LOG(LogTemp,Display,TEXT("rdRemoveActorFromPool() - trying to remove from null class"));
		return;
	}

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("rdRemoveActorPool(%s)"),(!actorClass->GetName().IsEmpty())?*actorClass->GetName():TEXT("No Class Name"));
#endif

	rdPoolData* pool=poolMap.Find(actorClass);
	if(pool) {
		rdPoolData& dat=*pool;
		for(auto& item:dat.pool) {
			if(item.actor) item.actor->Destroy();
		}
		poolMap.Remove(actorClass);
	}

}

//.............................................................................
// rdResetPooledActor
//.............................................................................
void ArdInstBaseActor::rdResetPooledActor(rdPoolData& dat,rdPoolItem& item) {
/*
	UGeometryCollectionComponent* gcc=(UGeometryCollectionComponent*)item.actor->GetComponentByClass(UGeometryCollectionComponent::StaticClass());
	if(!dat.simple gcc) {
	//	gcc->ResetRepData();
	//	gcc->SetLifeSpan(5);
//						gcc->SetRestCollection(

		const TWeakObjectPtr<UPrimitiveComponent> prevOwnerComp=gcc->BodyInstance.OwnerComponent;
		gcc->BodyInstance.OwnerComponent=nullptr;
		gcc->BodyInstance.SetInstanceSimulatePhysics(true);
		gcc->BodyInstance.OwnerComponent=prevOwnerComp;
		gcc->RegisterAndInitializePhysicsProxy();
		gcc->SetDynamicState(Chaos::EObjectStateType::Dynamic);
		// Set Any Collision Responses here BEFORE calling SetCollisionEnabled()
		gcc->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		gcc->SetNotifyBreaks(true);
		gcc->OnActorEnableCollisionChanged();
		// This is needed to assign our GeometryCollection events to work with our new PhysicsProxy 
		//gcc->RegisterForEvents();
	}
*/
	USphereComponent* cc=(USphereComponent*)item.actor->GetComponentByClass(USphereComponent::StaticClass());
	UProjectileMovementComponent* pmc=(UProjectileMovementComponent*)item.actor->GetComponentByClass(UProjectileMovementComponent::StaticClass());
	if(!dat.simple && pmc) {
		bool stopped=pmc->HasStoppedSimulation();
		bool interpolationComplete=pmc->IsInterpolationComplete();
#if ENGINE_MAJOR_VERSION>4
		TObjectPtr<USceneComponent> uc=pmc->UpdatedComponent;
		TObjectPtr<UPrimitiveComponent> up=pmc->UpdatedPrimitive;
#else
		USceneComponent* uc=pmc->UpdatedComponent;
		UPrimitiveComponent* up=pmc->UpdatedPrimitive;
#endif
		if(!uc) pmc->UpdatedComponent=cc;
		if(!up) pmc->UpdatedPrimitive=cc;

		pmc->ResetInterpolation();
	}
}
//.............................................................................
// rdGetActorFromPool
//.............................................................................
AActor* ArdInstBaseActor::rdGetActorFromPool(TSubclassOf<class AActor> actorClass,const FTransform& tran) {

	if(poolMap.Num()==0) {
		UE_LOG(LogTemp,Display,TEXT("rdGetActorFromPool() - poolMap is Empty"));
		return nullptr;
	}

	if(actorClass==nullptr) {
		UE_LOG(LogTemp,Display,TEXT("rdGetActorFromPool() - trying to get from null class"));
		return nullptr;
	}

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("rdGetActorFromPool(%s)"),(!actorClass->GetName().IsEmpty())?*actorClass->GetName():TEXT("No Class Name"));
#endif

	uint64 lowestInstCnt=0;
	rdPoolItem* lowestPoolItem=nullptr;

	rdPoolData* pool=poolMap.Find(actorClass);
	if(pool) {

		rdPoolData& dat=*pool;
		for(auto& item:dat.pool) {
			if(item.inUse==false) {
				item.inUse=true;
				item.instCnt=dat.currInstCnt++;
				if(item.actor) {

					check(item.actor);
					rdResetPooledActor(dat,item);

					if(OnPoolUsedDelegate.IsBound())	{
						FEditorScriptExecutionGuard ScriptGuard;
						OnPoolUsedDelegate.Broadcast(item.actor);
					}
					if(poolListener) {
						poolListener->rdOnActorPooled(item.actor);
					}
					ArdActor* rdActor=Cast<ArdActor>(item.actor);
					if(rdActor) {
						rdActor->rdOnActorPooled(item.actor);
					}
					ArdSpawnActor* spawnActor=Cast<ArdSpawnActor>(item.actor);
					if(spawnActor) {
						rdRegisterSpawnActor(spawnActor);
					}

//					item.actor->SetActorTransform(tran);
					if(!dat.simple && item.hasTick) {
						item.actor->PrimaryActorTick.bCanEverTick=true;
						item.actor->SetActorTickEnabled(true);
						item.actor->PrimaryActorTick.Target=item.actor;
						item.actor->PrimaryActorTick.RegisterTickFunction(GetLevel());
					}

					if(!item.startHidden) {
						item.actor->SetActorHiddenInGame(false);
						item.actor->SetHidden(false);
					}

					if(!dat.simple) {
						item.actor->SetActorEnableCollision(item.hasCollision);
					}
					item.actor->SetActorTransform(tran,false,nullptr,ETeleportType::ResetPhysics);

					if(rdActor) {
						rdActor->rdBuild();
					}

				} else {
					UE_LOG(LogTemp,Display,TEXT("Tried to get Actor from rdPool but actor is null."));
				}
				return item.actor;

			} else if(item.instCnt<lowestInstCnt) {
				lowestInstCnt=item.instCnt;
				lowestPoolItem=&item;
			}
		}

		if(!dat.reuseLast && dat.pool.Num()>=dat.numPooled && dat.growBy>0) {

			if(!rdSetActorPoolSize(actorClass,dat.pool.Num()+dat.growBy,dat.premake,dat.doTick,dat.startHidden,dat.reuseLast,dat.simple,dat.growBy)) {
				return nullptr;
			}
			return rdGetActorFromPool(actorClass,tran);
		}

		if(dat.pool.Num()<dat.numPooled) {
			rdPoolItem item;
			item.inUse=true;
			item.instCnt=dat.currInstCnt++;
			item.actor=rdSpawnActor(actorClass,tran,nullptr,true,false);
			if(item.actor) {
				dat.pool.Add(item);
				rdResetPooledActor(dat,item);
				if(poolListener) {
					poolListener->rdOnActorPooled(item.actor);
				}
				ArdActor* rdActor=Cast<ArdActor>(item.actor);
				if(rdActor) rdActor->rdOnActorPooled(item.actor);
				ArdSpawnActor* spawnActor=Cast<ArdSpawnActor>(item.actor);
				if(spawnActor) {
					rdRegisterSpawnActor(spawnActor);
				}
				return item.actor;
			} else {
				UE_LOG(LogTemp,Display,TEXT("Tried to create Actor for rdPool but failed."));
			}

		} else if(dat.reuseLast && lowestPoolItem) {

			lowestPoolItem->actor->SetActorTransform(tran,false,nullptr,ETeleportType::ResetPhysics);
			lowestPoolItem->instCnt=dat.currInstCnt++;
			return lowestPoolItem->actor;
		}
	}

	UE_LOG(LogTemp,Display,TEXT("failed to get a pooled actor"));
	return nullptr;
}

//.............................................................................
// rdRemoveActorFromPool
//.............................................................................
void ArdInstBaseActor::rdRemoveActorFromPool(AActor* actor) { rdReturnActorToPool(actor); }
void ArdInstBaseActor::rdReturnActorToPool(AActor* actor) {

	if(actor==nullptr || poolMap.Num()<1) {
#ifdef _rdDebugPools
		UE_LOG(LogTemp,Display,TEXT("rdReturnActorToPool - just returning"));
#endif
		return;
	}

#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("rdReturnActorToPool(%s)"),(!actor->GetClass()->GetName().IsEmpty())?*actor->GetClass()->GetName():TEXT("No Class Name"));
#endif

	rdPoolData* pool=poolMap.Find(actor->GetClass());
	if(pool) {

		rdPoolData& dat=*pool;
		for(auto& item:dat.pool) {
			if(item.actor==actor) {

#ifdef _rdDebugPools
				UE_LOG(LogTemp,Display,TEXT("rdReturnActorToPool - removing actor"));
#endif

				UProjectileMovementComponent* pmc=(UProjectileMovementComponent*)item.actor->GetComponentByClass(UProjectileMovementComponent::StaticClass());
				if(pmc) {
					pmc->bIsSliding=false;
				}

				if(!dat.simple) {
					item.actor->SetActorEnableCollision(false);
				}

				item.actor->SetLifeSpan(0.0f);
				item.actor->SetActorHiddenInGame(true);
				item.actor->SetHidden(true);
				item.actor->SetActorTickEnabled(false);

				if(OnPoolReturnedDelegate.IsBound())	{
					FEditorScriptExecutionGuard ScriptGuard;
					OnPoolReturnedDelegate.Broadcast(actor);
				}
				if(poolListener) {
					poolListener->rdOnActorDepooled(actor);
				}
				ArdActor* rdActor=Cast<ArdActor>(actor);
				if(rdActor) {
					rdActor->rdOnActorDepooled(actor);
					rdActor->rdRemoveInstances();
				}
				ArdSpawnActor* spawnActor=Cast<ArdSpawnActor>(actor);
				if(spawnActor) {
					rdUnregisterSpawnActor(spawnActor);
				}
				item.inUse=false;

				return;
			}
		}
	}
#ifdef _rdDebugPools
	UE_LOG(LogTemp,Display,TEXT("rdReturnActorToPool - failed to find actor"));
#endif
}

//.............................................................................
// rdSetPoolListener
//.............................................................................
void ArdInstBaseActor::rdSetPoolListener(ArdActor* actor) {
	
	poolListener=actor;
}

//.............................................................................
// rdIsActorInPool
//.............................................................................
bool ArdInstBaseActor::rdIsActorInPool(ArdActor* actor) {

	rdPoolData* pool=poolMap.Find(actor->GetClass());
	if(pool) {
		rdPoolData& dat=*pool;
		for(auto& item:dat.pool) {
			if(item.actor==actor) return true;
		}
	}

	return false;
}

//.............................................................................
// rdGetActorPoolSize
//.............................................................................
int32 ArdInstBaseActor::rdGetActorPoolSize(TSubclassOf<class AActor> actorClass) {

	rdPoolData* pool=poolMap.Find(actorClass);
	if(!pool) {
		return 0;
	}

	return pool->pool.Num();
}

//.............................................................................
// rdGrowActorPool
//.............................................................................
int32 ArdInstBaseActor::rdGrowActorPool(TSubclassOf<class AActor> actorClass,int32 growAmount) {

	rdPoolData* pool=poolMap.Find(actorClass);
	if(!pool) {
		return 0;
	}

	rdSetActorPoolSize(actorClass,pool->pool.Num()+growAmount,pool->premake,pool->doTick,pool->startHidden,pool->reuseLast,pool->simple,pool->growBy);
	return pool->pool.Num();
}

//.............................................................................
