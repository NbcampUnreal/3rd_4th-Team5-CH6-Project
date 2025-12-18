// GameplayCue_Status_Poisoned.cpp
#include "GAS/GC/Gimmick/GameplayCue_Status_Poisoned.h"

#include "Components/PostProcessComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTags/AbilityGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogPoison, Log, All); // 로그 카테고리 정의

AGameplayCue_Status_Poisoned::AGameplayCue_Status_Poisoned()
{
	/*
		GameplayCueTag 명시적 지정
		꼭 필요한 방어 코드: 생성자에서 명시적으로 태그를 지정 → 안정성 확보
		C++ GameplayCue는 클래스명 + 태그 불일치 시 동작 안 하는 버그가 존재
	*/
	GameplayCueTag = AbilityTags::TAG_GameplayCue_Status_Poisoned;
	
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcessComponent->bEnabled = false;
	PostProcessComponent->bUnbound = false; // true: 전체 화면에 적용
	PostProcessComponent->BlendWeight = 1.0f; // 효과 100%
	
	// Priority를 높게 설정하여 다른 효과보다 우선 적용
	PostProcessComponent->Priority = 10.0f;
}

bool AGameplayCue_Status_Poisoned::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	// 로컬 플레이어만 PostProcess 적용
	if (!IsLocalPlayerTarget(MyTarget))
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogPoison, Log,
			TEXT("[GameplayCue_Poison] 대상이 로컬 플레이어가 아니므로 PostProcess 적용을 건너뜁니다"));
#endif
		return true;
	}
	
	SetupPostProcess(MyTarget); // PostProcess 설정 로직
	StartFadeIn(); // Fade In 시작
	return true;
}

bool AGameplayCue_Status_Poisoned::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	// 현재는 주기적 처리 없음
	return true;
}

bool AGameplayCue_Status_Poisoned::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{// GameplayCue 종료 시 호출
	// 로컬 플레이어만 정리
	if (IsLocalPlayerTarget(MyTarget))
	{
		StartFadeOut(); // Fade Out 시작
		//CleanupPostProcess(); // PostProcess 정리
	}
	return true;
}

void AGameplayCue_Status_Poisoned::UpdateFade()
{
	if (!PostProcessComponent)
	{
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
		return;
	}
	
	FadeElapsedTime += 1.0f / 60.0f;
	
	if (bIsFadingIn)
	{
		// Fade In: 0 → 1
		float Alpha = FMath::Clamp(FadeElapsedTime / FadeInDuration, 0.0f, 1.0f);
		
		// Ease In-Out 곡선 적용 (부드러운 전환)
		Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);
		
		PostProcessComponent->BlendWeight = Alpha;
		
		if (Alpha >= 1.0f)
		{
			// Fade In 완료
			bIsFadingIn = false;
			GetWorldTimerManager().ClearTimer(FadeTimerHandle);
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogPoison, Log, TEXT("[GameplayCue_Poison] Fade In 완료"));
#endif
		}
	}
	else if (bIsFadingOut)
	{
		// Fade Out: 1 → 0
		float Alpha = FMath::Clamp(FadeElapsedTime / FadeOutDuration, 0.0f, 1.0f);
		
		// Ease In-Out 곡선 적용
		Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);
		
		PostProcessComponent->BlendWeight = 1.0f - Alpha;
		
		if (Alpha >= 1.0f)
		{
			// Fade Out 완료
			bIsFadingOut = false;
			GetWorldTimerManager().ClearTimer(FadeTimerHandle);
			CleanupPostProcess();
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogPoison, Log, TEXT("[GameplayCue_Poison] Fade Out 완료, PostProcess 정리"));
#endif
		}
	}
}

void AGameplayCue_Status_Poisoned::StartFadeIn()
{
	if (!PostProcessComponent)
	{
		return;
	}
	
	bIsFadingIn = true;
	bIsFadingOut = false;
	FadeElapsedTime = 0.0f;
	
	// 타이머 시작 (60fps로 업데이트)
	GetWorldTimerManager().SetTimer(
		FadeTimerHandle,
		this,
		&AGameplayCue_Status_Poisoned::UpdateFade,
		1.0f / 60.0f,
		true
	);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogPoison, Log,
		TEXT("[GameplayCue_Poison] Fade In 시작 (Duration: %.2fs)"),
		FadeInDuration);
#endif
}

void AGameplayCue_Status_Poisoned::StartFadeOut()
{
	if (!PostProcessComponent)
	{
		return;
	}
	
	bIsFadingIn = false;
	bIsFadingOut = true;
	FadeElapsedTime = 0.0f;
	
	// 타이머 시작
	GetWorldTimerManager().SetTimer(
		FadeTimerHandle,
		this,
		&AGameplayCue_Status_Poisoned::UpdateFade,
		1.0f / 60.0f,
		true
	);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogPoison, Log,
		TEXT("[GameplayCue_Poison] Fade Out 시작 (Duration: %.2fs)"),
		FadeOutDuration);
#endif
}

void AGameplayCue_Status_Poisoned::SetupPostProcess(AActor* Target)
{
	if (!Target || !PostProcessComponent)
	{// GC 중, 제거 타이밍 꼬임 방지
		return;
	}
	
	// PostProcess 활성화
	PostProcessComponent->bEnabled = true;
	
	// 초기 BlendWeight를 0으로 설정 (Fade In 준비)
	PostProcessComponent->BlendWeight = 0.0f;
	
	//============================================================================
	// 비네팅 설정
	// 독으로 인한 시야 압박, 테두리 어둡게
	//============================================================================
	PostProcessComponent->Settings.bOverride_VignetteIntensity = true;
	PostProcessComponent->Settings.VignetteIntensity = 1.0f; // 비네팅 강도를 최대로
	
	//============================================================================
	// 초록색 색조
	// 약하게 적용하여 비네팅 방해 안 함
	// ColorSaturation / Gain / Offset
	//============================================================================
	// Green 채널만 강조
	PostProcessComponent->Settings.bOverride_ColorSaturation = true;
	//PostProcessComponent->Settings.ColorSaturation = FVector4(0.8f, 1.2f, 0.8f, 1.0f);
	PostProcessComponent->Settings.ColorSaturation = FVector4(0.6f, 0.8f, 0.65f, 1.0f); // 채도 낮춤
	
	// 미세한 초록 톤
	PostProcessComponent->Settings.bOverride_ColorGain = true;
	//PostProcessComponent->Settings.ColorGain = FVector4(0.9f, 1.12f, 0.9f, 1.0f);
	PostProcessComponent->Settings.ColorGain = FVector4(0.85f, 0.95f, 0.88f, 1.0f); // 어둡고 차분한 초록

	
	// 초록빛 오프셋
	PostProcessComponent->Settings.bOverride_ColorOffset = true;
	//PostProcessComponent->Settings.ColorOffset = FVector4(0.0f, 0.1f, 0.0f, 0.0f);
	PostProcessComponent->Settings.ColorOffset = FVector4(-0.05f, 0.02f, -0.03f, 0.0f); // 약간 어둡게
	
	//============================================================================
	// 크로매틱 애버레이션
	// RGB 분리 잔상 (시각적 어지러움) - 매우 강하게: 독 상태 체감 극대화
	//============================================================================
	PostProcessComponent->Settings.bOverride_SceneFringeIntensity = true;
	//PostProcessComponent->Settings.SceneFringeIntensity = 10.0f; // 최대로 증가
	PostProcessComponent->Settings.SceneFringeIntensity = 3.0f;
	
	//============================================================================
	// 모션 블러
	// 카메라 회전 시 잔상 - 매우 강하게
	// 회전 시 잔상 증가 최대 수준: 컨트롤이 무거워진 느낌 연출
	//============================================================================
	PostProcessComponent->Settings.bOverride_MotionBlurAmount = true;
	//PostProcessComponent->Settings.MotionBlurAmount = 1.5f; //강하게
	PostProcessComponent->Settings.MotionBlurAmount = 0.5f;
	
	PostProcessComponent->Settings.bOverride_MotionBlurMax = true;
	//PostProcessComponent->Settings.MotionBlurMax = 100.0f; //강하게
	PostProcessComponent->Settings.MotionBlurMax = 30.0f;
	
	PostProcessComponent->Settings.bOverride_MotionBlurPerObjectSize = true;
	PostProcessComponent->Settings.MotionBlurPerObjectSize = 2.0f;
	
	//============================================================================
	// Bloom
	// 초록빛 번짐 효과: 독 기운이 퍼지는 연출
	//============================================================================
	PostProcessComponent->Settings.bOverride_BloomIntensity = true;
	PostProcessComponent->Settings.BloomIntensity = 2.0f; // 블룸 강도 증가
	
	PostProcessComponent->Settings.bOverride_BloomThreshold = true;
	PostProcessComponent->Settings.BloomThreshold = -1.5f; // 더 많이 번지게
	
	PostProcessComponent->Settings.bOverride_BloomMethod = true;
	PostProcessComponent->Settings.BloomMethod = EBloomMethod::BM_SOG;
	
	// 블룸 색조를 초록색으로
	PostProcessComponent->Settings.bOverride_Bloom1Tint = true;
	PostProcessComponent->Settings.Bloom1Tint = FLinearColor(0.6f, 1.0f, 0.6f, 1.0f);
	
	PostProcessComponent->Settings.bOverride_Bloom2Tint = true;
	PostProcessComponent->Settings.Bloom2Tint = FLinearColor(0.5f, 1.0f, 0.5f, 1.0f);
	
	PostProcessComponent->Settings.bOverride_Bloom3Tint = true;
	PostProcessComponent->Settings.Bloom3Tint = FLinearColor(0.5f, 0.9f, 0.5f, 1.0f);
	
	//============================================================================
	// DOF
	// 약간의 흐림 효과: 정신이 흐려지는 느낌
	//============================================================================
	PostProcessComponent->Settings.bOverride_DepthOfFieldFstop = true;
	PostProcessComponent->Settings.DepthOfFieldFstop = 1.0f;
	
	PostProcessComponent->Settings.bOverride_DepthOfFieldMinFstop = true;
	PostProcessComponent->Settings.DepthOfFieldMinFstop = 0.5f;
	
	PostProcessComponent->Settings.bOverride_DepthOfFieldBladeCount = true;
	PostProcessComponent->Settings.DepthOfFieldBladeCount = 8;
	
	//============================================================================
	// 화면 왜곡감을 위한 Film 설정
	// 화면 대비 감소, 어두운 영역 강조, 비네팅 보조 역할
	//============================================================================
	PostProcessComponent->Settings.bOverride_FilmSlope = true;
	PostProcessComponent->Settings.FilmSlope = 0.9f; // 약간 어둡게
	
	PostProcessComponent->Settings.bOverride_FilmToe = true;
	PostProcessComponent->Settings.FilmToe = 0.6f;
	
	PostProcessComponent->Settings.bOverride_FilmShoulder = true;
	PostProcessComponent->Settings.FilmShoulder = 0.3f;
	
	PostProcessComponent->Settings.bOverride_FilmBlackClip = true;
	PostProcessComponent->Settings.FilmBlackClip = 0.05f; // 검은색 영역 증가 (비네팅 보조)
	
	PostProcessComponent->Settings.bOverride_FilmWhiteClip = true;
	PostProcessComponent->Settings.FilmWhiteClip = 0.95f;
	
	//============================================================================
	// 캐릭터의 카메라에 붙이기
	//============================================================================
	if (ACharacter* Character = Cast<ACharacter>(Target))
	{
		// 먼저 CameraComponent를 찾아봄
		UCameraComponent* CameraComp = Character->FindComponentByClass<UCameraComponent>();
		
		if (CameraComp)
		{
			// 카메라에 직접 붙이기
			PostProcessComponent->AttachToComponent(
				CameraComp,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogPoison, Log, TEXT("[GameplayCue_Poison] 카메라에 PostProcess 부착 완료"));
#endif
		}
		else
		{// 카메라가 없으면 Root에 붙이되, BlendRadius 설정
			PostProcessComponent->AttachToComponent(
				Character->GetRootComponent(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
			
			// BlendRadius를 작게 설정하여 캐릭터 주변에서만 적용
			PostProcessComponent->BlendRadius = 100.0f;
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogPoison, Log, TEXT("[GameplayCue_Poison] Root에 PostProcess 부착 (BlendRadius 사용)"));
#endif
		}
	}
}

void AGameplayCue_Status_Poisoned::CleanupPostProcess()
{
	if (PostProcessComponent)
	{
		PostProcessComponent->bEnabled = false;
		PostProcessComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	// 타이머 정리
	if (FadeTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
	}
}

bool AGameplayCue_Status_Poisoned::IsLocalPlayerTarget(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}
	
	// Pawn인 경우 해당 Pawn의 Controller가 로컬인지 확인
	if (APawn* TargetPawn = Cast<APawn>(Target))
	{
		APlayerController* PC = Cast<APlayerController>(TargetPawn->GetController());
		if (PC && PC->IsLocalController())
		{
			return true;
		}
	}
	
	return false;
}
