// Fill out your copyright notice in the Description page of Project Settings.

#include "System/Erosion/ErosionLightSourceComponent.h"
#include "System/Erosion/TSErosionSubsystem.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "System/Erosion/ErosionLightSourceInterface.h"

UErosionLightSourceComponent::UErosionLightSourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UErosionLightSourceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetOwner())) return;
	if (GetOwner()->HasAuthority())
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
	
		// 매니저의 이벤트 구독 (1초마다 침식도 증가)
		ErosionSubSystem->OnErosionLightSourceSubDelegate.AddDynamic(this, &UErosionLightSourceComponent::ReceiveErosionCheckTime);
	}
}

void UErosionLightSourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
	if (!IsValid(ErosionSubSystem)) return;

	// 구독 해제 
	ErosionSubSystem->OnErosionLightSourceSubDelegate.RemoveAll(this);
}

void UErosionLightSourceComponent::InitializeFromBuildingData()
{
	// 건축물에서 정적 데이터 받아야 한다.
	// 받아와야 하는 것
	// 1. 이 가로등이 몇 초마다 침식량을 감소하냐.
	// 2. 이 가로등이 1의 경우가 일어나면 침식량을 얼마나 감소하냐.
	// 3. 이후 캐싱한 변수쪽에 넣어줘야 한다.

	// 따라서 두 가지 옵션이 있다.
	// 1. 아이템 (== 건축물)이 자신에게 붙은 라이트 소스 컴포넌트에게 직접 이 함수릃 호출.
	// 2. 아이템 (== 건축물)로부터 이 컴포넌트가 건축물이 라이트인지 물어서 라이트이면 데이터를 가져온다.
	// 2번의 경우 그렇다면 언제 그걸 호출할 건지에 대한 타이밍 문제가 있음.
}

bool UErosionLightSourceComponent::UpdateActiveFlag()
{
	// 건축물 또는 기지? 의 유지비 상태를 받아야 한다.
	// 쉽게 말해서, 유지비가 내어지고 있는지 아닌지 판단해야 한다.
	// 유지비를 내고 있는 상태가 아니라면 이 함수에 의해 감소 로직이 멈춘다.
	// 유지비가 건물 각각인지, 기지 전체인지 아직 잘 모르겟으므로 기획 쪽에 물어봐야 한다.

	return true;
}

void UErosionLightSourceComponent::ReceiveErosionCheckTime()
{
	// 유비지 상태가 적자면 계산하지 않음.
	if (false == UpdateActiveFlag())
	{
		UE_LOG(ErosionManager, Warning, TEXT("유지비 없음"));
		if (!IsValid(GetOwner())) return;
		if (!UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UErosionLightSourceInterface::StaticClass())) return;
		IErosionLightSourceInterface::Execute_SetErosionLightSource(GetOwner(), false);
		return;
	}
	else
	{
		UE_LOG(ErosionManager, Warning, TEXT("유지비 있음"));
		if (!IsValid(GetOwner())) return;
		if (!UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UErosionLightSourceInterface::StaticClass())) return;
		IErosionLightSourceInterface::Execute_SetErosionLightSource(GetOwner(), true);
	}

	// 유지비 디버깅용
	if (false == bIsActiveLightForDebug)
	{
		UE_LOG(ErosionManager, Warning, TEXT("유지비 디버깅용 : 유지비 없음"));
		if (!IsValid(GetOwner())) return;
		if (!UKismetSystemLibrary::DoesImplementInterface(GetOwner(), UErosionLightSourceInterface::StaticClass())) return;
		IErosionLightSourceInterface::Execute_SetErosionLightSource(GetOwner(), false);
		return;
	}

	// 1초씩 누적
	AccumulatedTime += 1.0f;

	// 지정된 시간이 지났을 경우 리셋 및 더하기
	if (AccumulatedTime >= MaintenanceEffectTime)
	{
		AccumulatedTime = 0.0f;
		OnIntervalTriggered();
	}

	if (bShowDebug) UE_LOG(ErosionManager, Log, TEXT("가로등 : 1초 타이머 수신 받음"));
}

void UErosionLightSourceComponent::OnIntervalTriggered()
{
	UTSErosionSubSystem* ErosionSubSystem = UTSErosionSubSystem::GetErosionSubSystem(this);
	if (!IsValid(ErosionSubSystem)) return;
	ErosionSubSystem->OnErosionLightSourceBroadcastDelegate.Broadcast(MaintenanceEffectValue);
	if (bShowDebug) UE_LOG(ErosionManager, Warning, TEXT("가로등 : %f 초 지났음. 침식도 감소 요청 (-2감소)"), MaintenanceEffectTime);
}
