// Fill out your copyright notice in the Description page of Project Settings.

#include "System/Erosion/ErosionLightSourceSubActor.h"
#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"
#include "System/Erosion/ErosionLightSourceComponent.h"
#include "System/Erosion/TSErosionSubSystem.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ===============================
	// UErosionLightSourceComponent 라이프 사이클
	// ===============================

AErosionLightSourceSubActor::AErosionLightSourceSubActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);
	SetNetUpdateFrequency(1.f);
	
	ErosionLightSourceComponent = CreateDefaultSubobject<UErosionLightSourceComponent>(TEXT("ErosionLightSourceComponent"));
	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	PointLightComponent->SetupAttachment(GetRootComponent());
}

void AErosionLightSourceSubActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AErosionLightSourceSubActor, LightScale, COND_None, REPNOTIFY_Always)
}

void AErosionLightSourceSubActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
		if (!IsValid(ErosionSubSystem))
		{
			UE_LOG(ErosionManager, Warning, TEXT("침식도 매니저 찾지 못했음."));
			return;
		}
		else
		{
			UE_LOG(ErosionManager, Warning, TEXT("침식도 매니저 찾음."));
		}
	
		// 매니저의 침식도 스테이지 이벤트 구독
		ErosionSubSystem->OnErosionChangedDelegate.AddDynamic(this, &AErosionLightSourceSubActor::ChangeLightScaleByErosion);
	}
}

void AErosionLightSourceSubActor::SetErosionLightSource_Implementation(bool bEnable)
{
	if (bEnable)
	{
		SetLightScale(LightScale);
	}
	else
	{
		SetLightScale(0.f);
	}
}

void AErosionLightSourceSubActor::ChangeLightScaleByErosion(float CurrentErosionScale)
{
	if (CurrentErosionScale < 30)
		SetLightScale(LightScaleStep.StepLess30);
	else if (30 <= CurrentErosionScale && CurrentErosionScale < 60)
		SetLightScale(LightScaleStep.StepMore30Less60);
	else if (60 <= CurrentErosionScale && CurrentErosionScale < 90)
		SetLightScale(LightScaleStep.StepMore60Less90);
	else if (90 <= CurrentErosionScale && CurrentErosionScale < 100.f)
		SetLightScale(LightScaleStep.StepMore90LessMax);
	else if (100.f <= CurrentErosionScale)
		SetLightScale(LightScaleStep.StepMax);
}

void AErosionLightSourceSubActor::SetLightScale(float scale)
{
	LightScale = scale;
	PointLightComponent->SetIntensity(LightScale);
	UE_LOG(ErosionManager, Warning, TEXT("가로등 밝기 변화됨"));
}

void AErosionLightSourceSubActor::OnRep_LightScale()
{
	PointLightComponent->SetIntensity(LightScale);
	UE_LOG(ErosionManager, Warning, TEXT("가로등 밝기 변화됨"));
}

