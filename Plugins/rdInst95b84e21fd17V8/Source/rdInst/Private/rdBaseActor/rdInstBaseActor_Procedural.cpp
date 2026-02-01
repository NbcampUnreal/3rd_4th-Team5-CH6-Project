//
// rdInstBaseActor.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 9th September 2023 (moved from rdInstBaseActor.cpp)
// Last Modified: 26th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "TextureResource.h"
#include "rd_PixelWrappers.h"
#include "rdSpawnActor.h"
//#include "rdProceduralActor.h"
//#include "Landscape.h"
//#if ENGINE_MAJOR_VERSION>4
//#include "LandscapeStreamingProxy.h"
//#endif
//.............................................................................
// rdProceduralGenerate
//.............................................................................
int32 ArdInstBaseActor::rdProceduralGenerate(const ArdSpawnActor* spawner) {

	return rdProceduralGenerateBake(spawner,nullptr);
}

//.............................................................................
// rdProceduralGenerateBake
//.............................................................................
int32 ArdInstBaseActor::rdProceduralGenerateBake(const ArdSpawnActor* spawner,FrdSpawnData* bakeToSpawnData) {
	
	if(!spawner) return 0;
	//((ArdSpawnActor*)spawner)->Modify();

	float mainDensity=1.0f;
	UTexture2D* splatmap=nullptr;

	FrdSpawnData& spawnData=(FrdSpawnData&)spawner->spawnData[spawner->currentSpawnDataIndex];

	if(bakeToSpawnData) {
		spawnData.numBakedMeshes=0;
		spawnData.numBakedInstances=0;
	}

	mainDensity=spawnData.mainDensity;
	int32 gridW=spawnData.gridWidth;
	int32 gridH=spawnData.gridHeight;
	splatmap=spawnData.splatMap;
	int32 distributeFrames=spawnData.distributeFrames;

	int cnt=0;

	FVector sz=spawner->rdGetSpawnerSize();
	float volW=sz.X;
	float volH=sz.Y;

	pgLandscapeZ2=spawner->GetActorLocation().Z-20.0f;
	pgLandscapeZ1=pgLandscapeZ2+(64.0f*spawner->GetActorScale3D().Z)+40.0f;

	TArray<FrdObjectPlacementData>& objects=spawnData.items;
	pgNumObjects=objects.Num();

	if(pgNumObjects==0) {
		UE_LOG(LogTemp,Display,TEXT("rdInst: Error: No objects in list for population."));
		return 0;
	}

	int32 numInsts=(volW/100)*(volH/100)*mainDensity;
	int32 numInstsPerMesh=numInsts/pgNumObjects;

	FRandomStream& pgRandomStream=(FRandomStream&)spawner->randomStream;
	if(spawner->randomizationSeed>=0) {
		pgRandomStream.Initialize(spawner->randomizationSeed);
	} else {
		pgRandomStream.GenerateNewSeed();
	}

	uint8*	splatDat=nullptr;
	int32	splatW=0;
	int32	splatH=0;
	ETextureSourceFormat splatFmt;
	float	txScaleX=1.0f;
	float	txScaleY=1.0f;
	rdPixelWrapper* wrp=nullptr;

	if(splatmap) {

#if WITH_EDITOR
		splatFmt=splatmap->Source.GetFormat();

		splatW=splatmap->Source.GetSizeX();
		splatH=splatmap->Source.GetSizeY();

		splatDat=(uint8*)splatmap->Source.LockMip(0);
#else

#if ENGINE_MAJOR_VERSION>4
		EPixelFormat splatEfmt=splatmap->GetPlatformData()->PixelFormat;
#else
		EPixelFormat splatEfmt=splatmap->PlatformData->PixelFormat;
#endif
		if(splatEfmt!=EPixelFormat::PF_B8G8R8A8) { // 4 channels of splat data
			UE_LOG(LogTemp,Display,TEXT("rdInst: Currently only uncompressed BGRA8 format supported for SplatMaps during runtime"));
			return 0;
		}
		splatFmt=TSF_BGRA8;
#if ENGINE_MAJOR_VERSION>4
		FTexturePlatformData* platformData=splatmap->GetPlatformData();//@@
#else
		FTexturePlatformData* platformData=splatmap->PlatformData;//@@
#endif
		TIndirectArray<struct FTexture2DMipMap> mips=platformData->Mips; //@@

		splatDat=(uint8*)platformData->Mips[0].BulkData.LockReadOnly();
		splatW=platformData->SizeX;
		splatH=platformData->SizeY;
#endif
		UE_LOG(LogTemp,Display,TEXT("rdInst: SplatMap: %x - %dx%d"),splatDat,splatW,splatH);

		txScaleX=(float)splatW/volW;
		txScaleY=(float)splatH/volH;

		wrp=createPixelWrapper(splatDat,splatW,splatH,splatFmt,RD_INTERP_BICUBIC);
	}

	float cellW=(float)gridW;
	float cellH=(float)gridH;

	TMap<FName,FrdBakedSpawnObjects> bakedMeshMap;
	TMap<UClass*,FrdBakedSpawnObjects> bakedActorMap;
	TMap<UNiagaraSystem*,FrdBakedSpawnObjects> bakedVFXMap;

	TMap<int32,TArray<FTransform>> distributedInstanceMap;
	TMap<int32,TArray<FTransform>> distributedActorMap;
	TMap<int32,TArray<FTransform>> distributedVFXMap;

	FLinearColor col;

	// Now loop through each object and add the correct amount to the level
	TArray<float> weightedRands;
	for(float y=0;y<volH-gridH;y+=gridH) {
		for(float x=0;x<volW-gridW;x+=gridW) {

			if(pgRandomStream.GetFraction()>mainDensity) continue; // main density

			weightedRands.Empty();
			for(int32 i=0;i<pgNumObjects;i++) {
				const FrdObjectPlacementData& opd=objects[i];

				float weight=opd.density;
				if(splatmap) {
					float px=x*txScaleX,py=y*txScaleY;
					if(spawnData.splatMapCoversLandscape) {
						px*=spawner->landscapeScale.Z;
						px+=spawner->landscapeScale.X*splatW;
						py*=spawner->landscapeScale.W;
						py+=spawner->landscapeScale.Y*splatH;
					}

					col=wrp->readLinearPixel(px,py);
					float fchan=0.0f;
					switch(opd.splatChannel) {
						case 1: fchan=col.G; break;
						case 2: fchan=col.B; break;
						case 3: fchan=col.A; break;
						default: fchan=col.R; break;
					}
					if(fchan<opd.splatMinRange || fchan>opd.splatMaxRange) {
						weight=0;
					} else if(opd.bUseSplatDensity) {
						weight*=fchan;
					} else if(fchan<0.333f) weight=0.0f;
				}
				weightedRands.Add(pgRandomStream.GetFraction()*weight);
			}
			int m=0;
			float w=0.0f;
			for(int32 i=0;i<pgNumObjects;i++) {
				if(weightedRands[i]>w) { m=i; w=weightedRands[i]; }
			}
			if(w<0.001f) continue;

			FrdObjectPlacementData& opd=objects[m];
			UInstancedStaticMeshComponent* instGen=opd.ismc.Get();

			FVector loc;
			loc.X=x+opd.offset.X+pgRandomStream.FRandRange(0.0f,cellW*opd.gridVarianceW)+opd.gridOffsetX;
			loc.Y=y+opd.offset.Y+pgRandomStream.FRandRange(0.0f,cellH*opd.gridVarianceH)+opd.gridOffsetY;
			loc.Z=opd.offset.Z;

			FVector scale;
			scale.X=pgRandomStream.FRandRange(opd.minScale.X,opd.maxScale.X);
			if(opd.bUniformScale) {
				scale.Y=scale.Z=scale.X;
			} else {
				scale.Y=pgRandomStream.FRandRange(opd.minScale.Y,opd.maxScale.Y);
				scale.Z=pgRandomStream.FRandRange(opd.minScale.Z,opd.maxScale.Z);
			}

			FRotator rot(0.0f,0.0f,0.0f);
			if(opd.bRandomYaw) {
				rot.Yaw=pgRandomStream.FRandRange(-180.0f,180.0f);
			}

			loc=spawner->GetActorRotation().RotateVector(loc)+spawner->rdGetSpawnOrigin();
			if(opd.bPlaceOnGround || ignorePMaps.Num()>0 || opd.filterToPhysicalMatsList.Num()>0) {
				FHitResult hit;
				float zpos=getLandscapeZ(loc.X,loc.Y,opd.slopedRadius,hit,opd.bFixSlopePlacement&&!opd.bAlignToNormal,spawner->bSpawnOnStaticMeshes);
				if(!hit.bBlockingHit) continue;

				if(opd.filterToPhysicalMatsList.Num()>0) {

					bool cntn=true;
					for(auto pmap:opd.filterToPhysicalMatsList) {
						if(hit.PhysMaterial.Get()==pmap) {
							cntn=false;
							break;
						}
					}
					if(cntn) continue;

				} else if(ignorePMaps.Num()>0) {
					bool cntn=false;
					for(auto pmap:ignorePMaps) {
						if(hit.PhysMaterial.Get()==pmap) {
							cntn=true;
							break;
						}
					}
					if(cntn) continue;
				}

				if(opd.bPlaceOnGround) loc.Z=opd.offset.Z+zpos;

				if(opd.minSlope>0.0f) {
					FRotator rt=UKismetMathLibrary::MakeRotFromZY(hit.Normal*180.0f,FVector(0.0f,1.0f,0.0f));
					if(fabs(rt.Pitch)>opd.minSlope || fabs(rt.Roll)>opd.minSlope) continue;
				}

				if(opd.bAlignToNormal) {
					rot=UKismetMathLibrary::MakeRotFromZY(hit.Normal*180.0f,FVector(0.0f,1.0f,0.0f));
				}
			}

			FTransform transform(rot,loc,scale);
			if(bakeToSpawnData) {
				if(opd.mesh.mesh.Get()) {
					FName osid=rdGetInstSid(opd.mesh);
					FrdBakedSpawnObjects* bso=bakedMeshMap.Find(osid);
					if(bso) {
						bso->transformMap.Add(transform);
						bso->itemIndex=m;
					} else {
						FrdBakedSpawnObjects bs;
						bs.sid=osid;
						bs.transformMap.Add(transform);
						bs.proxy=opd.proxy;
						bs.itemIndex=m;
						bso=&bakedMeshMap.Add(bs.sid,bs);
					}
				}

				if(opd.actorClass) {
					FrdBakedSpawnObjects* bso=bakedActorMap.Find(opd.actorClass);
					if(bso) {
						bso->transformMap.Add(transform);
						bso->itemIndex=m;
					} else {
						FrdBakedSpawnObjects bs;
						bs.actorClass=opd.actorClass;
						bs.strProps=opd.strProps;
						bs.transformMap.Add(transform);
						bs.proxy=opd.proxy;
						bs.itemIndex=m;
						bso=&bakedActorMap.Add(bs.actorClass,bs);
					}
				}

				if(opd.vfx) {
					FrdBakedSpawnObjects* bso=bakedVFXMap.Find(opd.vfx);
					if(bso) {
						bso->transformMap.Add(transform);
						bso->itemIndex=m;
					} else {
						FrdBakedSpawnObjects bs;
						bs.vfx=opd.vfx;
						bs.transformMap.Add(transform);
						bs.proxy=opd.proxy;
						bs.itemIndex=m;
						bso=&bakedVFXMap.Add(bs.vfx,bs);
					}
				}

			} else {

				if(opd.mesh.mesh.Get()) {

					if(distributeFrames>1) {
						TArray<FTransform>* ts=distributedInstanceMap.Find(m);
						if(!ts) ts=&distributedInstanceMap.Add(m);
						ts->Add(transform);
					} else {
						int32 instIndex=rdAddInstanceFast(instGen,transform);
						if(instIndex>=0) {

							TArray<int32>& il=((ArdSpawnActor*)spawner)->ismIndexList.FindOrAdd(opd.sid);
							il.Add(instIndex);

							if(opd.proxy.proxyType!=RDPROXYTYPE_NONE) {
								AddProxyForInstance(opd.sid,opd.proxy,instIndex);
							}
							cnt++;
						}
					}
				}

				if(opd.actorClass) {

					if(distributeFrames>1) {
						TArray<FTransform>* ts=distributedActorMap.Find(m);
						if(!ts) ts=&distributedActorMap.Add(m);
						ts->Add(transform);
					} else {
						AActor* actor=nullptr;
						if(opd.pooled) {
							actor=rdGetActorFromPool(opd.actorClass,transform);
						} else {
							actor=rdSpawnActor(opd.actorClass,transform,nullptr,true,false,FName(),nullptr,false,opd.strProps);
						}
						if(actor) {
							opd.actorList.Add(actor);
							cnt++;
							ArdActor* rdActor=Cast<ArdActor>(actor);
							if(rdActor && opd.proxy.proxyType!=RDPROXYTYPE_NONE) {
								AddProxyForActor(rdActor,opd.proxy);
							}
						}
					}
				}

				if(opd.vfx) {

					if(distributeFrames>1) {
						TArray<FTransform>* ts=distributedVFXMap.Find(m);
						if(!ts) ts=&distributedVFXMap.Add(m);
						ts->Add(transform);
					} else {
						UNiagaraComponent* fxc=rdSpawnVFX(opd.vfx,transform);
						if(fxc) {
							opd.vfxList.Add(fxc);
							cnt++;
						}
					}
				}
			}
		}
	}

	if(bakeToSpawnData) {
		UrdBakedDataAsset* bda=rdLoadBakedDataAsset(bakeToSpawnData->bakedDataAsset,true);
		if(bda) {
			TArray<FrdBakedSpawnObjects>& baked=bda->bakedData;
			for(auto bmo:bakedMeshMap) {
				baked.Add(bmo.Value);
			}
			for(auto bma:bakedActorMap) {
				baked.Add(bma.Value);
			}
			for(auto bmv:bakedVFXMap) {
				baked.Add(bmv.Value);
			}
		}
	}
/*
	if(distributedInstanceMap.Num()>0) {

		for(auto& i:distributedInstanceMap) {
			FrdObjectPlacementData& pd=(FrdObjectPlacementData&)objects[i.Key];
			rdAddDistributedInstancesX(pd.sid,i.Value,i.Value.Num()/distributeFrames,pd.ismIndexList,pd.proxy);
		}
	}
*/
	if(splatmap) {
#if WITH_EDITOR
		splatmap->Source.UnlockMip(0);
#else
#if ENGINE_MAJOR_VERSION>4
		splatmap->GetPlatformData()->Mips[0].BulkData.Unlock();	
#else
		splatmap->PlatformData->Mips[0].BulkData.Unlock();	
#endif
#endif
	}

	if(wrp) {
		delete wrp;
	}

	if(bakeToSpawnData) {
		for(auto& it:bakedMeshMap) {
			spawnData.numBakedMeshes++;
			spawnData.numBakedInstances+=it.Value.transformMap.Num();
		}
	}

	return cnt;
}

//.............................................................................
