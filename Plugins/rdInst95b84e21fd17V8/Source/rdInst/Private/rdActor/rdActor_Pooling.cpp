//
// rdActor_Pooling.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 11th April 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdActor.h"

//.............................................................................
// rdPoolComponent
//.............................................................................
void ArdActor::rdPoolComponent(UClass* compClass,int32 numToPool,bool premake,bool doTick,bool startHidden,bool reuse,bool simplePool,int32 growBy) {

	rdPoolData* pool=compPoolMap.Find(compClass);
	if(pool) {

		rdSetComponentPoolSize(compClass,numToPool,premake);
	
	} else {
	
		rdPoolData data(numToPool,reuse,simplePool,premake,doTick,startHidden,growBy);
		if(premake) {
			for(int32 i=0;i<numToPool;i++) {

				rdPoolItem item;
				item.comp=rdAddComponent(compClass);
				if(item.comp) {
					USceneComponent* sc=Cast<USceneComponent>(item.comp);
					if(sc) {
						sc->SetVisibility(false);
					}
					item.hasTick=item.comp->IsComponentTickEnabled();
					item.comp->SetComponentTickEnabled(false);
					item.comp->SetActive(false,false) ;
					item.hasCollision=sc->GetCollisionEnabled()!=ECollisionEnabled::NoCollision;
				}
				data.pool.Add(MoveTemp(item));
			}
		}
		compPoolMap.Add(compClass,data);
	}
}

//.............................................................................
// rdSetComponentPoolSize
//.............................................................................
bool ArdActor::rdSetComponentPoolSize(UClass* compClass,int32 numToPool,bool premake,bool doTick,bool startHidden,bool reuse,bool simplePool,int32 growBy) {

	rdPoolData* pool=compPoolMap.Find(compClass);
	if(pool) {
		rdPoolData& dat=*pool;
		dat.reuseLast=reuse;
		dat.simple=simplePool;
		dat.growBy=growBy;

		if(numToPool>=dat.pool.Num()) {
			
			int32 origNum=dat.pool.Num();

			if(numToPool>origNum) {
				dat.numPooled=numToPool;
			}

			if(premake) {
				for(int32 i=0;i<origNum;i++) {
					rdPoolItem& item=dat.pool[i];
					if(!item.comp) {
						item.comp=rdAddComponent(compClass);
						if(item.comp) {
							USceneComponent* sc=Cast<USceneComponent>(item.comp);
							if(sc) {
								sc->SetVisibility(false);
							}
							item.hasTick=item.comp->IsComponentTickEnabled();
							item.startHidden=startHidden;
							item.comp->SetComponentTickEnabled(false);
							item.hasCollision=sc->GetCollisionEnabled()!=ECollisionEnabled::NoCollision;
							item.comp->SetActive(false,false) ;
						}
					}
				}
				for(int32 i=origNum;i<numToPool;i++) {

					rdPoolItem item;
					item.comp=rdAddComponent(compClass);
					if(item.comp) {
						USceneComponent* sc=Cast<USceneComponent>(item.comp);
						if(sc) {
							sc->SetVisibility(false);
						}
						item.hasTick=item.comp->IsComponentTickEnabled();
						item.startHidden=startHidden;
						item.comp->SetComponentTickEnabled(false);
						item.hasCollision=sc->GetCollisionEnabled()!=ECollisionEnabled::NoCollision;
						item.comp->SetActive(false,false) ;
					}
					dat.pool.Add(MoveTemp(item));
				}
			}

			return true;

		} else {
			return false; // we don't resize to less that the currently used amount of components in the pool
		}

	} else {

		rdPoolComponent(compClass,numToPool,premake,doTick,startHidden,reuse,simplePool,growBy);
	}
	return true;
}

//.............................................................................
// rdRemoveComponentPool
//.............................................................................
void ArdActor::rdRemoveComponentPool(UClass* compClass) {

	rdPoolData* pool=compPoolMap.Find(compClass);
	if(!pool) {
		return;
	}

	rdPoolData& dat=*pool;
	for(auto& item:dat.pool) {
		if(item.comp) item.comp->DestroyComponent();
	}
	compPoolMap.Remove(compClass);
}

//.............................................................................
// rdGetComponentFromPool
//.............................................................................
UActorComponent* ArdActor::rdGetComponentFromPool(TSubclassOf<class UActorComponent> compClass) {

	rdPoolData* pool=compPoolMap.Find(compClass);
	if(!pool) {
		return nullptr;
	}

	rdPoolData& dat=*pool;

	uint64 lowestInstCnt=0;
	rdPoolItem* lowestPoolItem=nullptr;

	for(auto& item:dat.pool) {
		if(item.inUse==false) {
			item.inUse=true;
			item.instCnt=dat.currInstCnt++;
			if(item.comp) {
				USceneComponent* sc=Cast<USceneComponent>(item.comp);
				if(sc && !item.startHidden) {
					sc->SetVisibility(true);
				}
				item.comp->SetComponentTickEnabled(true);
				item.comp->SetActive(true,true) ;
			}
			rdOnCompPooled(this,item.comp);
			return item.comp;

		} else if(item.instCnt<lowestInstCnt) {
			lowestInstCnt=item.instCnt;
			lowestPoolItem=&item;
		}
	}

	if(!dat.reuseLast && dat.pool.Num()>=dat.numPooled && dat.growBy>0) {

		if(!rdSetComponentPoolSize(compClass,dat.pool.Num()+dat.growBy,dat.premake,dat.doTick,dat.startHidden,dat.reuseLast,dat.simple,dat.growBy)) {
			return nullptr;
		}
		return rdGetComponentFromPool(compClass);
	}

	if(dat.pool.Num()<dat.numPooled) {
		rdPoolItem item;
		item.inUse=true;
		item.instCnt=dat.currInstCnt++;
		item.comp=rdAddComponent(compClass);
		if(item.comp) {
			dat.pool.Add(MoveTemp(item));
			return item.comp;
		} else {
			UE_LOG(LogTemp,Display,TEXT("Tried to create Component for rdPool but failed."));
		}
	} else if(dat.reuseLast && lowestPoolItem) {

		lowestPoolItem->instCnt=dat.currInstCnt++;
		return lowestPoolItem->comp;
	}

	return nullptr;
}

//.............................................................................
// rdRemoveComponentFromPool
//.............................................................................
void ArdActor::rdRemoveComponentFromPool(UActorComponent* comp) {

	rdPoolData* pool=compPoolMap.Find(comp->GetClass());
	if(!pool) {
		return;
	}

	rdPoolData& dat=*pool;
	for(auto& item:dat.pool) {
		if(item.comp==comp) {
			rdOnCompDepooled(this,item.comp);
			item.inUse=false;
			USceneComponent* sc=Cast<USceneComponent>(item.comp);
			if(sc) sc->SetVisibility(false);
		}
	}
}

void ArdActor::rdOnActorPooled_Implementation(AActor* actor) {

}

void ArdActor::rdOnActorDepooled_Implementation(AActor* actor) {

}

void ArdActor::rdOnCompPooled_Implementation(AActor* actor,UActorComponent* comp) {

}

void ArdActor::rdOnCompDepooled_Implementation(AActor* actor,UActorComponent* comp) {

}

//.............................................................................
// rdGetComponentPoolSize
//.............................................................................
int32 ArdActor::rdGetComponentPoolSize(TSubclassOf<class UActorComponent> compClass) {

	rdPoolData* pool=compPoolMap.Find(compClass);
	if(!pool) {
		return 0;
	}

	return pool->pool.Num();
}

//.............................................................................
// rdGrowComponentPool
//.............................................................................
int32 ArdActor::rdGrowComponentPool(TSubclassOf<class UActorComponent> compClass,int32 growAmount) {

	rdPoolData* pool=compPoolMap.Find(compClass);
	if(!pool) {
		return 0;
	}

	rdSetComponentPoolSize(compClass,pool->pool.Num()+growAmount,pool->premake,pool->doTick,pool->startHidden,pool->reuseLast,pool->simple,pool->growBy);
	return pool->pool.Num();
}

//.............................................................................
