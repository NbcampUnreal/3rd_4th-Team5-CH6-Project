// GC_ElectricShock_Status.cpp
#include "GAS/GC/Gimmick/GC_ElectricShock_Status.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PostProcessComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameplayTags/GameplayCueTags.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCElectricShockStatus, Log, All); // 로그 카테고리 정의

#pragma region AGC_ElectricShock_Status
AGC_ElectricShock_Status::AGC_ElectricShock_Status()
{
	/*
		GameplayCueTag 명시적 지정
		꼭 필요한 방어 코드: 생성자에서 명시적으로 태그를 지정 -> 안정성 확보
		C++ GameplayCue는 클래스명 + 태그 불일치 시 동작 안 하는 버그가 존재
	*/
	GameplayCueTag = CueTags::TAG_GameplayCue_ElectricShock_Status;
	
	PrimaryActorTick.bCanEverTick = false;
	bAutoDestroyOnRemove = false;
	
	EffectElapsedTime = 0.0f;
	PostProcessComp = nullptr;
	CachedPlayerController = nullptr;
}
#pragma endregion

#pragma region OnActive
bool AGC_ElectricShock_Status::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	Super::OnActive_Implementation(MyTarget, Parameters);

	// 로컬 플레이어인지 확인하고 초기화
	if (InitializeForLocalPlayer(MyTarget))
	{
		CreatePostProcessComponent();
		StartCameraShake();
		
		// 업데이트 타이머 시작
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimerHandle,
			this,
			&AGC_ElectricShock_Status::UpdateEffect,
			0.016f, // ~60fps
			true
		);
		
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGCElectricShockStatus, Log,
			TEXT("[GC_ElectricShock] 로컬 플레이어 대상 → 감전 시각 효과 시작"));
#endif
	}
	
	return true;
}
#pragma endregion

#pragma region OnRemove
bool AGC_ElectricShock_Status::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	CleanupEffect();
	return Super::OnRemove_Implementation(MyTarget, Parameters);
}
#pragma endregion

#pragma region InitializeForLocalPlayer
bool AGC_ElectricShock_Status::InitializeForLocalPlayer(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return false;
	}
	
	// Pawn 확인
	APawn* TargetPawn = Cast<APawn>(TargetActor);
	if (!TargetPawn)
	{
		return false;
	}
	
	// PlayerController 확인
	APlayerController* PC = Cast<APlayerController>(TargetPawn->GetController());
	if (!PC || !PC->IsLocalController())
	{
		return false;
	}
	
	CachedPlayerController = PC;
	EffectElapsedTime = 0.0f;
	
	return true;
}
#pragma endregion

#pragma region CreatePostProcessComponent
void AGC_ElectricShock_Status::CreatePostProcessComponent()
{
	if (!CachedPlayerController)
	{
		return;
	}
	
	// PostProcess 컴포넌트 생성
	PostProcessComp = NewObject<UPostProcessComponent>(this, TEXT("ElectricShockPP"));
	if (!PostProcessComp)
	{
		UE_LOG(LogGCElectricShockStatus, Error,
			TEXT("[GC_ElectricShock] PostProcess 컴포넌트 생성에 실패했습니다"));
		return;
	}
	
	// 컴포넌트 등록 및 활성화
	PostProcessComp->RegisterComponent();
	PostProcessComp->bUnbound = true; // 전체 화면에 적용
	PostProcessComp->Priority = 100.0f; // 높은 우선순위
	
	// 초기 PostProcess 설정
	FPostProcessSettings& Settings = PostProcessComp->Settings;
	
	// Vignette (초기값 0, 나중에 업데이트됨)
	Settings.bOverride_VignetteIntensity = true;
	Settings.VignetteIntensity = 0.0f;
	
	// Scene Color Tint
	Settings.bOverride_SceneColorTint = true;
	
	// Bloom
	Settings.bOverride_BloomIntensity = true;
	Settings.BloomIntensity = 0.5f;
	
	// Chromatic Aberration
	Settings.bOverride_SceneFringeIntensity = true;
	Settings.SceneFringeIntensity = 0.0f;
	
	// Film Grain
	Settings.bOverride_FilmGrainIntensity = true;
	Settings.FilmGrainIntensity = 0.0f;
	
	// Saturation (채도 조정)
	Settings.bOverride_ColorSaturation = true;
	Settings.ColorSaturation = FVector4(1.0, 1.0, 1.0, 1.0);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCElectricShockStatus, Log,
		TEXT("[GC_ElectricShock] PostProcess 컴포넌트 생성 및 설정 완료"));
#endif
}
#pragma endregion

#pragma region UpdateEffect
void AGC_ElectricShock_Status::UpdateEffect()
{
	if (!PostProcessComp || !CachedPlayerController)
	{
		CleanupEffect();
		return;
	}
	
	EffectElapsedTime += 0.016f;
	// 지속 시간 초과 시 정리
	if (EffectElapsedTime >= EffectDuration)
	{
		CleanupEffect();
		return;
	}
	
	// 정규화된 시간 (0.0 ~ 1.0)
	float NormalizedTime = FMath::Clamp(EffectElapsedTime / EffectDuration, 0.0f, 1.0f);
	
	// 깜빡임 효과 (Sin 파형)
	float FlickerPhase = EffectElapsedTime * FlickerFrequency * PI * 2.0f;
	float FlickerValue = FMath::Sin(FlickerPhase);
	FlickerValue = (FlickerValue + 1.0f) * 0.5f; // 0~1 범위
	
	// 시간에 따른 감쇠 (부드러운 종료)
	float DecayFactor = 1.0f - FMath::Pow(NormalizedTime, 2.0f);
	
	// 최종 강도
	float FinalIntensity = FlickerValue * DecayFactor * MaxFlickerIntensity;
	
	// PostProcess 설정 업데이트
	FPostProcessSettings& Settings = PostProcessComp->Settings;
	
	// Vignette
	Settings.VignetteIntensity = FinalIntensity * MaxVignetteIntensity;
	
	// Scene Color Tint (전기 색상)
	Settings.SceneColorTint = FLinearColor::LerpUsingHSV(
		FLinearColor::White,
		ElectricColor,
		FinalIntensity * 0.7f
	);
	
	// Bloom (밝은 섬광)
	Settings.BloomIntensity = 0.5f + (FinalIntensity * BloomBoost);
	
	// Chromatic Aberration (색수차)
	Settings.SceneFringeIntensity = FinalIntensity * ChromaticAberration;
	
	// Film Grain (노이즈)
	Settings.FilmGrainIntensity = FinalIntensity * FilmGrainStrength;
	
	// Saturation (채도 감소 효과)
	float SaturationValue = 1.0f - (FinalIntensity * 0.3f);
	Settings.ColorSaturation = FVector4(SaturationValue, SaturationValue, SaturationValue, 1.0f);
	
	// 변경사항 즉시 적용
	PostProcessComp->MarkRenderStateDirty();
}
#pragma endregion

#pragma region CleanupEffect
void AGC_ElectricShock_Status::CleanupEffect()
{
	// 타이머 정리
	if (GetWorld() && UpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
		UpdateTimerHandle.Invalidate();
	}
	
	// PostProcess 컴포넌트 제거
	if (PostProcessComp)
	{
		PostProcessComp->UnregisterComponent();
		PostProcessComp->DestroyComponent();
		PostProcessComp = nullptr;
	}
	
	// 참조 정리
	CachedPlayerController = nullptr;
	EffectElapsedTime = 0.0f;
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCElectricShockStatus, Log,
		TEXT("[GC_ElectricShock] 전기 감전 효과 정리 완료"));
#endif
}
#pragma endregion

#pragma region StartCameraShake
void AGC_ElectricShock_Status::StartCameraShake()
{
	if (!CachedPlayerController || !ElectricCameraShake)
	{
		return;
	}
	
	CachedPlayerController->ClientStartCameraShake(
		ElectricCameraShake,
		ShakeScale
	);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCElectricShockStatus, Log,
		TEXT("[GC_ElectricShock] 감전 효과에 따른 카메라 셰이크 재생"));
#endif
}
#pragma endregion