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
	
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	ErosionLightSourceComponent = CreateDefaultSubobject<UErosionLightSourceComponent>(TEXT("ErosionLightSourceComponent"));
	
	if (true == bUsePointLight)
	{
		PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
		PointLightComponent->SetupAttachment(GetRootComponent());
	}
	else
	{
		StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
		StaticMeshComponent->SetupAttachment(GetRootComponent());
	}
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
			if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("침식도 매니저 찾지 못했음."));
			return;
		}
		else
		{
			if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("침식도 매니저 찾음."));
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
	if (true == bUsePointLight)
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
	else
	{
		if (CurrentErosionScale < 30)
			SetLightScale(EmissiveScaleStep.StepLess30);
		else if (30 <= CurrentErosionScale && CurrentErosionScale < 60)
			SetLightScale(EmissiveScaleStep.StepMore30Less60);
		else if (60 <= CurrentErosionScale && CurrentErosionScale < 90)
			SetLightScale(EmissiveScaleStep.StepMore60Less90);
		else if (90 <= CurrentErosionScale && CurrentErosionScale < 100.f)
			SetLightScale(EmissiveScaleStep.StepMore90LessMax);
		else if (100.f <= CurrentErosionScale)
			SetLightScale(EmissiveScaleStep.StepMax);
	}
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("가로등 : 침식도 이벤트 수신 받음"));
}

void AErosionLightSourceSubActor::SetLightScale(float scale)
{
	LightScale = scale + 0.01f;
	
	if (true == bUsePointLight)
	{
		PointLightComponent->SetIntensity(LightScale);
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("호스트 : 가로등 밝기 변화됨 (포인트 라이트)"));
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("호스트 : 가로등 밝기 변화 성공"));
		return;
	}
	else
	{
		int32 MatIndex = StaticMeshComponent->GetMaterialIndex("Light");
		if (MatIndex == INDEX_NONE)
		{
			if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("호스트 : 머티리얼 인덱스 찾지 못함"));
			return;
		}
		
		UMaterialInterface* BaseMat = StaticMeshComponent->GetMaterial(MatIndex);
		if (!BaseMat)
		{
			if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("호스트 : 머티리얼 인터페이스 찾지 못함"));
			return;
		}
		
		if (!LightMID)
		{
			LightMID = UMaterialInstanceDynamic::Create(BaseMat, this);
			StaticMeshComponent->SetMaterial(MatIndex, LightMID);
		}

		LightMID->SetScalarParameterValue("EmissiveIntensity", LightScale);
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("호스트 : 가로등 밝기 변화됨 (머티리얼)"));
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("호스트 : 가로등 밝기 변화 성공"));
		return;
	}
}

void AErosionLightSourceSubActor::OnRep_LightScale()
{
	UE_LOG(ErosionManager, Warning, TEXT("클라이언트 : OnRep_LightScale 호출됨"));
	
	if (true == bUsePointLight)
	{
		PointLightComponent->SetIntensity(LightScale);
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("클라이언트 : 가로등 밝기 변화됨 (포인트 라이트)"));
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("클라이언트 : 가로등 밝기 변화 성공"));
		return;
	}
	else
	{
		int32 MatIndex = StaticMeshComponent->GetMaterialIndex("Light");
		if (MatIndex == INDEX_NONE)
		{
			if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("클라이언트 : 머티리얼 인덱스 찾지 못함"));
			return;
		}
		
		UMaterialInterface* BaseMat = StaticMeshComponent->GetMaterial(MatIndex);
		if (!BaseMat)
		{
			if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("클라이언트 : 머티리얼 인터페이스 찾지 못함"));
			return;
		}
		
		if (!LightMID)
		{
			LightMID = UMaterialInstanceDynamic::Create(BaseMat, this);
			StaticMeshComponent->SetMaterial(MatIndex, LightMID);
		}

		LightMID->SetScalarParameterValue("EmissiveIntensity", LightScale);
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("클라이언트 : 가로등 밝기 변화됨 (머티리얼)"));
		if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("클라이언트 : 가로등 밝기 변화 성공"));
		return;
	}
}

