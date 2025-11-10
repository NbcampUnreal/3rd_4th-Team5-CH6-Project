//
// rdInstBaseActor_AutoInstancing.cpp
//
// Copyright (c) 2022 Recourse Design ltd. All rights reserved.
//
// Creation Date: 3rd September 2024 (moved from rdInstBaseActor_Instancing.cpp)
// Last Modified: 26th June 2025
//
// More information on the rdInst SourceCode can be found at https://recourse.nz/index.php/rdinst-source-code/
//
#include "rdInstBaseActor.h"
#include "Landscape.h"
#if ENGINE_MAJOR_VERSION>4
#include "LandscapeStreamingProxy.h"
#include "WorldPartition/HLOD/HLODActor.h"
#endif
#include "Engine/CullDistanceVolume.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Pawn.h"
#include "rdActor.h"
#include "AbstractNavData.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameNetworkManager.h"
#include "Particles/ParticleEventManager.h"
#include "GameFramework/GameSession.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/DefaultPhysicsVolume.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Light.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Engine/SphereReflectionCapture.h"
#include "GameFramework/DefaultPawn.h"
#include "InstancedFoliageActor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

//.............................................................................
// rdHarvestSMs
//.............................................................................
FName ArdInstBaseActor::rdGetAutoInstSidIncludingCullVolumes(UStaticMeshComponent* smc) {

	float startCull=-1.0f,endCull=-1.0f;
	float minDraw=smc->MinDrawDistance;
	if(minDraw>0.0f) {
		startCull=endCull=minDraw;
	} else {

		float dist=0.0f,largest=-1.0f;
		float meshSize=smc->Bounds.SphereRadius*2.0f;
		if(bAutoInstanceSetCullFromVolumes && cullVolMap.Num()>0) {
			for(auto& it:cullVolMap) {
				if(it.Value>largest) largest=it.Value;
				if(dist==0.0f && meshSize<it.Key) {
					dist=it.Value;
				}
			}
		}
		if(dist>0.0f) {
			startCull=endCull=dist;
		} else {
			startCull=endCull=largest;
		}
	}

	return rdGetSMCsid(smc,ErdSpawnType::UseDefaultSpawn,startCull,endCull);
}

//.............................................................................
// rdHarvestSMs
//.............................................................................
void ArdInstBaseActor::rdHarvestSMs(AActor* actor,bool hide,int32& numConverted,int32& numStandard,TMap<FName,int32>& sidMap) {

	if(actor->Tags.Contains("NoInstancing")) {
		numStandard++;
		AStaticMeshActor* sma=Cast<AStaticMeshActor>(actor);

		if(sma && sma->GetStaticMeshComponent()) { // StaticMeshActors in the level can have Proxies, described in their tags
			FrdProxySetup proxy;
			rdGetProxySettings(actor,sma->GetStaticMeshComponent()->GetStaticMesh(),proxy);
			if(proxy.proxyType!=rdProxyType::RDPROXYTYPE_NONE) {
				rdAddActorProxy(actor,proxy);
			}
		}

		return;
	}
	
	TArray<UClass*> ignoreClasses={ USphereComponent::StaticClass(),UBoxComponent::StaticClass(),UCapsuleComponent::StaticClass()};//,USpriteComponent::StaticClass() };

	TArray<UStaticMeshComponent*> smComps;
	FrdProxySetup proxySetup;
	TArray<UActorComponent*> comps;
	actor->GetComponents(comps,true);
	for(auto c:comps) {

		if(c->ComponentTags.Contains("NoInstancing")) {
			numStandard++; // keep as actor
			continue;
		}

		USceneComponent* sc=Cast<USceneComponent>(c);
		if(sc && (sc->bHiddenInGame || !sc->IsVisible())) {
			continue; // don't include at all
		}

		if(ignoreClasses.Contains(c->GetClass())) {
			numStandard++;
			continue; // don't include at all
		}

		UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
		
		if(ismc && ismc->GetStaticMesh()) {

			FName sid=rdGetAutoInstSidIncludingCullVolumes(ismc);
			int32* cnt=sidMap.Find(sid);
			if(cnt && *cnt>=ConvertToISM_threshold) {
				smComps.Add(ismc);
				numConverted++;
			} else {
				numStandard++;
			}

		} else {

			USplineMeshComponent* spmc=Cast<USplineMeshComponent>(c);
			UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(c);
			if(smc && !spmc) {

				FName sid=rdGetAutoInstSidIncludingCullVolumes(smc);
				int32* icnt=sidMap.Find(sid);
				if(!icnt||*icnt>=ConvertToISM_threshold) {
					bool canConvert=smc->GetStaticMesh() && !smc->bHiddenInGame && smc->IsVisible() && !smc->ComponentTags.Contains("NoInstancing");
					if(canConvert && smc->IsSimulatingPhysics()) {
						switch(ConvertToISM_PhysicConversion) {
							case RDAUTOINST_PHYSICS_LEAVE:
								canConvert=false;
								break;
							case RDAUTOINST_PHYSICS_TOMESH:
								break;
							case RDAUTOINST_PHYSICS_ADDPROXY:
								break;
						}
					}
					if(canConvert) {
						smComps.Add(smc);
						numConverted++;
					} else {
						numStandard++;
					}
				} else {
					numStandard++;
				}

			} else if(!spmc) {

				UChildActorComponent* cac=Cast<UChildActorComponent>(c);
				if(cac && cac->GetChildActorClass()==AStaticMeshActor::StaticClass()) {
					
					AStaticMeshActor* ca=Cast<AStaticMeshActor>(cac->GetChildActor());
					if(ca) {
						FName sid=rdGetAutoInstSidIncludingCullVolumes(ca->GetStaticMeshComponent());
						int32* icnt=sidMap.Find(sid);
						if(!icnt||*icnt>=ConvertToISM_threshold) {

							if(!cac->bHiddenInGame && cac->IsVisible() && !cac->ComponentTags.Contains("NoInstancing")) {
								if(ca->GetStaticMeshComponent() && ca->GetStaticMeshComponent()->GetStaticMesh()) {
									smComps.Add(ca->GetStaticMeshComponent());
									numConverted++;
								}
							}
						}
					} else {
						numStandard++;
					}
				} else if(c->GetClass()!=USceneComponent::StaticClass()) {
					numStandard++;
				}
			} else {
				numStandard++;
			}
		}
	}

	for(auto smc:smComps) {

		if(!smc->IsVisible()) continue;
		FName sid=rdGetAutoInstSidIncludingCullVolumes(smc);
		UInstancedStaticMeshComponent* rdIsmc=rdGetPreferredInstanceGenX(sid);
		if(rdIsmc) {

			FrdProxySetup proxy;
			if(ConvertToISM_ProcessAssetProxySettings) {
				AActor* tagActor=smc->GetOwner();
				if(tagActor) {
					rdGetProxySettings(tagActor,smc->GetStaticMesh(),proxy);
				}
			}

			TArray<FTransform> transforms;
			UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(smc);
			if(ismc) {
				int32 cnt=ismc->GetInstanceCount();
				FTransform tran;
				for(int i=0;i<cnt;i++) {
					ismc->GetInstanceTransform(i,tran,true);
					transforms.Add(tran);
				}
			} else {
				FTransform tran=smc->GetComponentTransform();
				transforms.Add(tran);
			}

			TArray<int32> indexList;
			rdAddInstancesFastWithIndexes(rdIsmc,transforms,indexList);

			if(proxy.proxyType!=rdProxyType::RDPROXYTYPE_NONE) {
				if(proxy.bEntireMeshProxy) {
					rdAddInstanceProxyX(sid,-1,proxy);
				} else {
					for(int32 index:indexList) {
						rdAddInstanceProxyX(sid,index,proxy);
					}
				}
			}
			TArray<int32>& il=autoInstSavedIndexes.FindOrAdd(sid);
			il.Append(MoveTemp(indexList));
		}

		if(hide) {
			smc->SetVisibility(false);
		}
	}
}

//.............................................................................
// rdRemoveHarvestedInstances
//.............................................................................
void ArdInstBaseActor::rdRemoveHarvestedInstances(AActor* actor) {

	TArray<UActorComponent*> comps;
	actor->GetComponents(comps,true);
	for(auto c:comps) {
		USceneComponent* scomp=Cast<USceneComponent>(c);
		if(scomp && scomp->ComponentTags.Contains("rdHasHidden")) {
			scomp->SetVisibility(true);
			scomp->ComponentTags.Remove("rdHasHidden");
		}
	}
}

//.............................................................................
// rdGetStaticMeshCounts
//.............................................................................
void ArdInstBaseActor::rdGetStaticMeshCounts(AActor* actor,TMap<FName,int32>& sidMap) {

	TArray<UInstancedStaticMeshComponent*> ismComps;
	TArray<UStaticMeshComponent*> smComps;

	TArray<UActorComponent*> comps;
	actor->GetComponents(comps,true);
	for(auto c:comps) {
		UInstancedStaticMeshComponent* ismc=Cast<UInstancedStaticMeshComponent>(c);
		if(ismc && ismc->GetStaticMesh()) {
			if(!ismc->bHiddenInGame && ismc->IsVisible() && !ismc->ComponentTags.Contains("NoInstancing")) {
				FName sid=rdGetAutoInstSidIncludingCullVolumes(ismc);
				if(!sidMap.Contains(sid)) sidMap.Add(sid,1);
				else sidMap[sid]++;
			}
		} else {
			USplineMeshComponent* spmc=Cast<USplineMeshComponent>(c);
			UStaticMeshComponent* smc=Cast<UStaticMeshComponent>(c);
			if(smc && !spmc) {
				if(smc->GetStaticMesh() && !smc->bHiddenInGame && smc->IsVisible() && !smc->ComponentTags.Contains("NoInstancing")) {
					FName sid=rdGetAutoInstSidIncludingCullVolumes(smc);
					if(!sidMap.Contains(sid)) sidMap.Add(sid,1);
					else sidMap[sid]++;
				}
			} else if(!spmc) {
				UChildActorComponent* cac=Cast<UChildActorComponent>(c);
				if(cac && cac->GetChildActorClass()==AStaticMeshActor::StaticClass()) {
					if(!cac->bHiddenInGame && cac->IsVisible() && !cac->ComponentTags.Contains("NoInstancing")) {
						AStaticMeshActor* ca=Cast<AStaticMeshActor>(cac->GetChildActor());
						if(ca && ca->GetStaticMeshComponent() && ca->GetStaticMeshComponent()->GetStaticMesh()) {
							FName sid=rdGetAutoInstSidIncludingCullVolumes(ca->GetStaticMeshComponent());
							if(!sidMap.Contains(sid)) sidMap.Add(sid,1);
							else sidMap[sid]++;
						}
					}
				}
			}
		}
	}
}

//.............................................................................
// rdSetupAutoInst
//.............................................................................
void ArdInstBaseActor::rdSetupAutoInst(bool autoInst,bool autoFromTags,bool autoMeshActors,bool autoBPs) {

	ConvertToISMAtPlay=autoInst;
	ConvertToISM_FromTags=autoFromTags;
	ConvertToISM_IncMeshActors=autoMeshActors;
	ConvertToISM_IncBPs=autoBPs;
}

//.............................................................................
// rdConvertAutoISMs
//.............................................................................
void ArdInstBaseActor::rdConvertAutoISMs() {
	
	autoInstSavedIndexes.Empty();
	cullVolMap.Empty();
	if(bAutoInstanceSetCullFromVolumes) {
		TArray<AActor*> cullVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACullDistanceVolume::StaticClass(),cullVolumes);
		for(auto a:cullVolumes) {
			ACullDistanceVolume* cdv=(ACullDistanceVolume*)a;
			for(auto cd:cdv->CullDistances) {
				float distance=cd.CullDistance;
				float size=cd.Size;
				if(distance>0.0f && size>0.0f) {
					cullVolMap.Add(size,distance);
				}
			}
		}
		cullVolMap.KeySort([](float a,float b) { return a<b; });
	}

	TArray<AActor*> actors;
	if(ConvertToISM_FromTags) {
		UGameplayStatics::GetAllActorsWithTag(GetWorld(),TEXT("AutoInstance"),actors);
	} else {
		if(ConvertToISM_IncBPs) {
			UGameplayStatics::GetAllActorsOfClass(GetWorld(),AActor::StaticClass(),actors);
		} else if(ConvertToISM_IncMeshActors) {
			UGameplayStatics::GetAllActorsOfClass(GetWorld(),AStaticMeshActor::StaticClass(),actors);
		}
	}

	TMap<FName,int32> sidMap;

	TArray<UClass*> ignoreClasses={ ArdActor::StaticClass(),ArdInstBaseActor::StaticClass(),ALandscape::StaticClass(),AWorldSettings::StaticClass(),
									APlayerStart::StaticClass(),ABrush::StaticClass(),APlayerCameraManager::StaticClass(),APlayerState::StaticClass(),
									APlayerController::StaticClass(),AAbstractNavData::StaticClass(),AGameStateBase::StaticClass(),ASkyAtmosphere::StaticClass(),
									AGameNetworkManager::StaticClass(),AGameSession::StaticClass(),ACullDistanceVolume::StaticClass(),//AParticleEventManager::StaticClass(),
									AGameModeBase::StaticClass(),APlayerStartPIE::StaticClass(),//AGamePlayDebuggerPlayerManager::StaticClass(),
									ADefaultPhysicsVolume::StaticClass(),ALight::StaticClass(),ADirectionalLight::StaticClass(),//AAtmosphericFog::StaticClass(),
									ASkyLight::StaticClass(),ASphereReflectionCapture::StaticClass(),APawn::StaticClass(),ADefaultPawn::StaticClass() };
#if ENGINE_MAJOR_VERSION>4
		ignoreClasses.Add(AWorldPartitionHLOD::StaticClass());
#endif
		// or sky sphere or datalayers or HLOD instancing

	TArray<AActor*> potentialActors;
	for(auto a:actors) {

		ArdActor* prefab=Cast<ArdActor>(a);
		if(prefab) {
			continue;
		}

		if(a->GetClass()==AStaticMeshActor::StaticClass()) {
			if(!((AStaticMeshActor*)a)->GetStaticMeshComponent() || !((AStaticMeshActor*)a)->GetStaticMeshComponent()->GetStaticMesh()) {
				continue;
			}
		}

		if(a->IsHidden() || ignoreClasses.Contains(a->GetClass()) || (a->GetClass()!=AStaticMeshActor::StaticClass() && a->Tags.Contains("NoInstancing"))) {
			continue;
		}

		if(a->GetClass()==AInstancedFoliageActor::StaticClass() && !ConvertToISM_IncFoliage) {
			continue;
		}

		potentialActors.Add(a);
		rdGetStaticMeshCounts(a,sidMap);
	}

	for(auto a:potentialActors) {
		int32 numConverted=0,numStandard=0;
		rdHarvestSMs(a,true,numConverted,numStandard,sidMap);
//		UE_LOG(LogTemp,Display,TEXT("rdHarvestSMs: %s %d converted, %d standard"),*a->GetName(),numConverted,numStandard);

		if(numStandard==0) { // hide entire actors of just StaticMeshes
			a->Destroy();
		}
	}
}

//.............................................................................
// rdRevertFromAutoISMs
//.............................................................................
void ArdInstBaseActor::rdRevertFromAutoISMs() {

	//TODO: remove any proxies created (not really needed as EndPlay is removing them and states don't mean much yet for autoinstancing)

	autoInstSavedIndexes.Empty();
}

//.............................................................................
