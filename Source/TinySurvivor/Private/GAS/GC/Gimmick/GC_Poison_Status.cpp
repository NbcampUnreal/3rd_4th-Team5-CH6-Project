// GC_Poison_Status.cpp
#include "GAS/GC/Gimmick/GC_Poison_Status.h"

#include "Components/PostProcessComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameplayTags/GameplayCueTags.h"

DEFINE_LOG_CATEGORY_STATIC(LogGCPoisonStatus, Log, All); // 로그 카테고리 정의

#pragma region AGC_Poison_Status
AGC_Poison_Status::AGC_Poison_Status()
{
	/*
		GameplayCueTag 명시적 지정
		꼭 필요한 방어 코드: 생성자에서 명시적으로 태그를 지정 -> 안정성 확보
		C++ GameplayCue는 클래스명 + 태그 불일치 시 동작 안 하는 버그가 존재
	*/
	GameplayCueTag = CueTags::TAG_GameplayCue_Poison_Status;
	
	PrimaryActorTick.bCanEverTick = false;
	
	// 객체 생성, 기본 서브오브젝트로 소유. "PostProcess"라는 이름으로 엔진에 등록.
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	
	// 처음에는 이 효과를 비활성화 상태로 둠. 필요할 때만 켤 계획.
	PostProcessComponent->bEnabled = false;
	
	// true로 설정하면 화면 전체에 적용. false: 액터 주변이나 특정 볼륨 안에서만 효과 적용 가능.
	PostProcessComponent->bUnbound = false;
	
	// 효과 강도를 100%로 설정. 0~1 사이 값으로 조절 가능.
	PostProcessComponent->BlendWeight = 1.0f;
	
	// Priority를 높게 설정하여 다른 효과보다 우선 적용
	PostProcessComponent->Priority = 10.0f;
}
#pragma endregion

#pragma region OnActive
bool AGC_Poison_Status::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	// 로컬 플레이어만 PostProcess 적용
	if (!IsLocalPlayerTarget(MyTarget))
	{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		UE_LOG(LogGCPoisonStatus, Log,
			TEXT("[GameplayCue_Poison] 대상이 로컬 플레이어가 아니므로 PostProcess 적용을 건너뜁니다"));
#endif
		return true;
	}
	
	SetupPostProcess(MyTarget); // PostProcess 설정 로직
	StartFadeIn(); // Fade In 시작
	return true;
}
#pragma endregion

#pragma region WhileActive
bool AGC_Poison_Status::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	// 현재는 주기적 처리 없음
	return true;
}
#pragma endregion

#pragma region OnRemove
bool AGC_Poison_Status::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{// GameplayCue 종료 시 호출
	// 로컬 플레이어만 정리
	if (IsLocalPlayerTarget(MyTarget))
	{
		StartFadeOut(); // Fade Out 시작
		//CleanupPostProcess(); // PostProcess 정리
	}
	return true;
}
#pragma endregion

#pragma region UpdateFade
void AGC_Poison_Status::UpdateFade()
{
	if (!PostProcessComponent)
	{// 만약 후처리 컴포넌트가 존재하지 않으면, 타이머를 정리하고 함수 종료
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
		return;
	}
	
	/*
		시간 누적
		
		페이드 진행 시간(FadeElapsedTime)을 실제 경과 시간 기준으로 누적.
		프레임 속도에 상관없이,
		일정한 FadeInDuration / FadeOutDuration 동안 페이드가 진행되도록 보장.
		나중에 FadeInDuration이나 FadeOutDuration으로 나눠서 0~1 범위의 알파(Alpha) 계산에 사용
	*/
	//FadeElapsedTime += 1.0f / 60.0f;
	FadeElapsedTime += GetWorld()->GetDeltaSeconds();
	
	if (bIsFadingIn)
	{
		/*
			Fade In: 0 -> 1
			FadeElapsedTime / FadeInDuration로 현재 진행 정도를 0~1 범위로 계산
		*/
		float Alpha = FMath::Clamp(FadeElapsedTime / FadeInDuration, 0.0f, 1.0f);
		
		/*
			SmoothStep으로 Ease In-Out 곡선 적용 (부드러운 전환)
			즉, 천천히 시작 → 중간 빠르게 → 천천히 끝나는 자연스러운 전환.
			BlendWeight = Alpha -> PostProcess 효과 강도를 점진적으로 증가.
		*/
		Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);
		PostProcessComponent->BlendWeight = Alpha;
		
		if (Alpha >= 1.0f)
		{// Alpha가 1 이상이면 Fade In 완료
			/*
				Fade In 완료 처리
				bIsFadingIn = false -> 더 이상 페이드 인 상태 아님.
				타이머 정리: 반복 호출을 멈춤.
			*/
			bIsFadingIn = false;
			GetWorldTimerManager().ClearTimer(FadeTimerHandle);
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogGCPoisonStatus, Log, TEXT("[GameplayCue_Poison] Fade In 완료"));
#endif
		}
	}
	else if (bIsFadingOut)
	{
		/*
			Fade Out: 1 -> 0
			FadeElapsedTime / FadeOutDuration로 0~1 범위 계산.
			SmoothStep으로 부드러운 전환.
			BlendWeight = 1.0f - Alpha -> 1에서 0으로 점진적으로 감소.
		*/
		float Alpha = FMath::Clamp(FadeElapsedTime / FadeOutDuration, 0.0f, 1.0f);
		Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);
		PostProcessComponent->BlendWeight = 1.0f - Alpha;
		
		if (Alpha >= 1.0f)
		{// Alpha가 1 이상이면 Fade Out 완료
			/*
				Fade Out 완료 처리
				
				bIsFadingIn = false → 더 이상 페이드 아웃 상태 아님.
				타이머 정리: 반복 호출을 멈춤.
				
				깜빡임, PP 튀는 현상 방지
				페이드 아웃 중에 바로 제거하면,
				화면 깜빡임이나 후처리 튐 현상이 발생할 수 있으므로,
				완전히 사라진 후 처리.
			*/

			bIsFadingOut = false;
			GetWorldTimerManager().ClearTimer(FadeTimerHandle);
			
			CleanupPostProcess();
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogGCPoisonStatus, Log, TEXT("[GameplayCue_Poison] Fade Out 완료, PostProcess 정리"));
#endif
		}
	}
}
#pragma endregion

#pragma region StartFadeIn
void AGC_Poison_Status::StartFadeIn()
{
	if (!PostProcessComponent)
	{
		return;
	}
	
	// 페이드 상태 설정
	bIsFadingIn = true;
	bIsFadingOut = false;
	FadeElapsedTime = 0.0f;
	
	// 타이머 시작 (60fps로 UpdateFade() 호출)
	GetWorldTimerManager().SetTimer(
		FadeTimerHandle,
		this,
		&AGC_Poison_Status::UpdateFade,
		1.0f / 60.0f,
		true
	);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCPoisonStatus, Log,
		TEXT("[GameplayCue_Poison] Fade In 시작 (Duration: %.2fs)"),
		FadeInDuration);
#endif
}
#pragma endregion

#pragma region StartFadeOut
void AGC_Poison_Status::StartFadeOut()
{
	if (!PostProcessComponent)
	{
		return;
	}
	
	// 페이드 상태 설정
	bIsFadingIn = false;
	bIsFadingOut = true;
	FadeElapsedTime = 0.0f;
	
	// 타이머 시작 (60fps로 UpdateFade() 호출)
	GetWorldTimerManager().SetTimer(
		FadeTimerHandle,
		this,
		&AGC_Poison_Status::UpdateFade,
		1.0f / 60.0f,
		true
	);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UE_LOG(LogGCPoisonStatus, Log,
		TEXT("[GameplayCue_Poison] Fade Out 시작 (Duration: %.2fs)"),
		FadeOutDuration);
#endif
}
#pragma endregion

#pragma region SetupPostProcess
void AGC_Poison_Status::SetupPostProcess(AActor* Target)
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
	PostProcessComponent->Settings.VignetteIntensity = VignetteIntensity; // 비네팅 강도를 최대로
	
	//============================================================================
	// 초록색 색조
	// 어둡고 탁한 독 기운 연출
	// 약하게 적용하여 비네팅 방해 안 함
	// ColorSaturation / Gain / Offset
	//============================================================================
	
	/*
		ColorSaturation: 색의 선명도 / 채도
		
		Green 채널만 강조
		현재 값 해석 (R:약하게 / G:살짝 강조 / B:약간 줄임 / A:불투명 유지)
	*/
	PostProcessComponent->Settings.bOverride_ColorSaturation = true;
	//PostProcessComponent->Settings.ColorSaturation = FVector4(0.8f, 1.2f, 0.8f, 1.0f);
	PostProcessComponent->Settings.ColorSaturation = ColorSaturation; // 채도 낮춤
	
	/*
		ColorGain: 색 밝기/강조 보정 (1.0 → 원래 색 유지)
		
		미세한 초록 톤
		현재 값 해석 (R:살짝 어둡게 / G:강조 / B:약간 어둡게 / A:불투명 유지)
	*/
	PostProcessComponent->Settings.bOverride_ColorGain = true;
	//PostProcessComponent->Settings.ColorGain = FVector4(0.9f, 1.12f, 0.9f, 1.0f);
	PostProcessComponent->Settings.ColorGain = ColorGain; // 어둡고 차분한 초록
	
	/*
		ColorOffset: 색상에 덧셈으로 더하는 보정
		
		초록빛 오프셋
		현재 값 해석 (R:살짝 조금 빼서 붉은 느낌 감소 / G:살짝 증가 / B:조금 감소 / A:불투명 유지)
	*/
	PostProcessComponent->Settings.bOverride_ColorOffset = true;
	//PostProcessComponent->Settings.ColorOffset = FVector4(0.0f, 0.1f, 0.0f, 0.0f);
	PostProcessComponent->Settings.ColorOffset = ColorOffset; // 약간 어둡게
	
	//============================================================================
	// 크로매틱 애버레이션
	// RGB 분리 잔상 (시각적 어지러움)
	// 독 상태 체감 극대화 (강하면 멀미 유발)
	//============================================================================
	
	/*
		SceneFringeIntensity: 화면 가장자리의 색수차(Chromatic Aberration) 강도를 설정
		
		값이 클수록 빨강/초록/파랑 색이 가장자리에서 더 많이 번져 시각적 왜곡 효과가 커짐.
		0.0f = 효과 없음, 1.0f = 기본 정도, 3.0f = 강한 색수차 효과
	*/
	PostProcessComponent->Settings.bOverride_SceneFringeIntensity = true;
	//PostProcessComponent->Settings.SceneFringeIntensity = 10.0f; // 최대로 증가
	PostProcessComponent->Settings.SceneFringeIntensity = ChromaticAberration;
	
	//============================================================================
	// 모션 블러
	// 카메라 회전 시 잔상
	// 회전 시 잔상 증가 최대 수준: 컨트롤이 무거워진 느낌 연출
	//============================================================================
	
	/*
		모션 블러의 전체 강도(스케일)
		“얼마나 많이 흐려질 것인가”를 결정
		
		설정 가이드
		값       | 체감
		-------- | -----------------------------
		0.0      | 모션 블러 없음
		0.2~0.4  | 아주 약함 (영화 느낌)
		0.5      | 명확히 느껴짐 (게임 플레이 가능)
		1.0      | 매우 강함
		1.5+     | 어지러움 / 멀미 유발 가능
	*/
	PostProcessComponent->Settings.bOverride_MotionBlurAmount = true;
	//PostProcessComponent->Settings.MotionBlurAmount = 1.5f; // 강하게
	PostProcessComponent->Settings.MotionBlurAmount = MotionBlurAmount;
	
	/*
		모션 블러가 적용될 수 있는 최대 길이(클램프)
		즉, 아무리 빨리 움직여도 이 이상은 흐려지지 않음
		
		설정 가이드
		값        | 체감
		----------|----------------------------
		5~10      | 거의 눈치 못 챔
		20~30     | 자연스러운 상한
		50        | 빠른 회전 시 화면 번짐
		100+      | 화면이 찢어지는 느낌
	*/
	PostProcessComponent->Settings.bOverride_MotionBlurMax = true;
	//PostProcessComponent->Settings.MotionBlurMax = 100.0f; // 강하게
	PostProcessComponent->Settings.MotionBlurMax = MotionBlurMax;
	
	/*
		오브젝트 크기에 따른 모션 블러 민감도
		즉, “작은 물체도 빠르게 움직이면 흐려질까?” 라는 의미
		
		설정 가이드
		값       | 의미
		-------- | ---------------------------
		0.0      | 큰 물체만 블러
		1.0      | 기본값
		2.0      | 작은 물체도 블러 적용
		3.0+     | 파편/손/무기까지 흐려짐
	*/
	PostProcessComponent->Settings.bOverride_MotionBlurPerObjectSize = true;
	PostProcessComponent->Settings.MotionBlurPerObjectSize = MotionBlurPerObjectSize;
	
	//============================================================================
	// Bloom
	// 초록빛 독 안개 번짐 효과: 독 기운이 퍼지는 연출
	//============================================================================
	
	/*
		BloomIntensity: 블룸(빛 번짐) 강도를 설정
		
		값이 클수록 밝은 영역에서 빛이 더 퍼져서 화면이 더 화사하게 보임.
		현재 값 해석: 블룸 강도 증가
	*/
	PostProcessComponent->Settings.bOverride_BloomIntensity = true;
	PostProcessComponent->Settings.BloomIntensity = BloomIntensity;
	
	/*
		BloomThreshold: 블룸이 적용될 밝기 기준을 설정
		
		값이 낮을수록 더 어두운 영역에도 블룸이 적용됨.
		현재 값 해석: 음수로 설정해 어두운 영역도 번지게 함
	*/
	PostProcessComponent->Settings.bOverride_BloomThreshold = true;
	PostProcessComponent->Settings.BloomThreshold = BloomThreshold;
	
	/*
		BloomMethod: 블룸 처리 방식을 선택
		
		BM_SOG = Gaussian 기반 블룸, 일반적으로 자연스럽게 퍼지는 효과
	*/
	PostProcessComponent->Settings.bOverride_BloomMethod = true;
	PostProcessComponent->Settings.BloomMethod = EBloomMethod::BM_SOG;
	
	/*
		블룸 색조 (초록 계열 강조)
		Bloom1Tint: 블룸 색조 1단계 설정
		Bloom2Tint: 블룸 색조 2단계 설정
		Bloom3Tint: 블룸 색조 3단계 설정
	*/
	PostProcessComponent->Settings.bOverride_Bloom1Tint = true;
	PostProcessComponent->Settings.Bloom1Tint = Bloom1Tint;
	
	PostProcessComponent->Settings.bOverride_Bloom2Tint = true;
	PostProcessComponent->Settings.Bloom2Tint = Bloom2Tint;
	
	PostProcessComponent->Settings.bOverride_Bloom3Tint = true;
	PostProcessComponent->Settings.Bloom3Tint = Bloom3Tint;
	
	//============================================================================
	// DOF
	// 약간의 흐림 효과: 정신이 흐려지는 느낌 (과하면 플레이 불가)
	//============================================================================
	
	/*
		DepthOfFieldFstop: 심도(Depth of Field) 블러 강도를 설정
		
		값이 작을수록 피사계 심도가 얕아지고, 배경이 더 흐려짐.
		현재 값 해석: 낮은 값으로 얕은 심도 적용, 피사계 심도가 얕아 배경 흐림이 강함
	*/
	PostProcessComponent->Settings.bOverride_DepthOfFieldFstop = true;
	PostProcessComponent->Settings.DepthOfFieldFstop = DepthOfFieldFstop;
	
	/*
		DepthOfFieldMinFstop: 최소 심도 블러 강도 설정
		
		Fstop이 이 값 이하로 내려가지 않도록 제한
		현재 값 해석: 최소 Fstop 설정, 블러가 너무 강해지지 않도록 제한
	*/
	PostProcessComponent->Settings.bOverride_DepthOfFieldMinFstop = true;
	PostProcessComponent->Settings.DepthOfFieldMinFstop = DepthOfFieldMinFstop;
	
	/*
		DepthOfFieldBladeCount: 심도 블러(보케) 모양의 블레이드 개수
		
		블레이드 수가 많을수록 원형 보케가 부드럽게 나타남
		현재 값 해석: 8개 블레이드, 원형 보케가 자연스럽게 표현됨
	*/
	PostProcessComponent->Settings.bOverride_DepthOfFieldBladeCount = true;
	PostProcessComponent->Settings.DepthOfFieldBladeCount = DepthOfFieldBladeCount;
	
	//============================================================================
	// 화면 왜곡감을 위한 Film 설정
	// 화면 대비 감소, 어두운 영역 강조, 비네팅 보조 역할
	//============================================================================
	
	/*
		FilmSlope: 필름 톤 커브의 기울기 설정
		
		값이 낮으면 전체적으로 화면이 약간 어둡게 표현됨
		현재 값 해석: 약간 어둡게 전체 톤 기울기 적용
	*/
	PostProcessComponent->Settings.bOverride_FilmSlope = true;
	PostProcessComponent->Settings.FilmSlope = FilmSlope;
	
	/*
		FilmToe: 그림자(어두운 영역) 커브의 민감도
		
		값이 높을수록 어두운 영역이 더 강조됨
		현재 값 해석: 그림자 영역 강조, 어두운 부분 더 뚜렷하게 표현
	*/
	PostProcessComponent->Settings.bOverride_FilmToe = true;
	PostProcessComponent->Settings.FilmToe = FilmToe;
	
	/*
		FilmShoulder: 하이라이트(밝은 영역) 커브의 민감도
		
		값이 높을수록 밝은 영역이 더 부드럽게 퍼짐
		현재 값 해석: 밝은 영역을 부드럽게 처리, 하이라이트 퍼짐 감소
	*/
	PostProcessComponent->Settings.bOverride_FilmShoulder = true;
	PostProcessComponent->Settings.FilmShoulder = FilmShoulder;
	
	/*
		FilmBlackClip: 검은색 클리핑 임계값
		
		값이 높을수록 검은색 영역이 더 강하게 표현됨 (비네팅 효과 보조 가능)
		현재 값 해석: 검은 영역 증가, 비네팅 효과 보조
	*/
	PostProcessComponent->Settings.bOverride_FilmBlackClip = true;
	PostProcessComponent->Settings.FilmBlackClip = FilmBlackClip;
	
	/*
		FilmWhiteClip: 흰색 클리핑 임계값
		
		값이 낮을수록 밝은 영역이 제한되어 하이라이트가 덜 날카로움
		현재 값 해석: 하이라이트 영역 제한, 밝은 영역이 날카롭지 않음
	*/
	PostProcessComponent->Settings.bOverride_FilmWhiteClip = true;
	PostProcessComponent->Settings.FilmWhiteClip = FilmWhiteClip;
	
	//============================================================================
	// 캐릭터의 카메라에 PostProcessComponent 부착
	//============================================================================
	if (ACharacter* Character = Cast<ACharacter>(Target))
	{
		// 캐릭터의 카메라 컴포넌트를 찾아봄
		UCameraComponent* CameraComp = Character->FindComponentByClass<UCameraComponent>();
		
		if (CameraComp)
		{// 카메라가 존재하면
			/*
				카메라에 직접 붙이기
				SnapToTargetNotIncludingScale: 위치와 회전은 카메라에 맞추지만 스케일은 맞추지 않음
			*/
			PostProcessComponent->AttachToComponent(
				CameraComp,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogGCPoisonStatus, Log, TEXT("[GameplayCue_Poison] 카메라에 PostProcess 부착 완료"));
#endif
		}
		else
		{// 카메라가 없을 경우
			// 캐릭터의 RootComponent에 PostProcessComponent 부착
			PostProcessComponent->AttachToComponent(
				Character->GetRootComponent(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
			
			/*
				BlendRadius 설정
				캐릭터 주변에서만 포스트 프로세스 효과가 적용되도록 범위를 제한
				BlendRadius를 작게 설정하여 캐릭터 주변에서만 적용
			*/
			PostProcessComponent->BlendRadius = 100.0f;
			
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
			UE_LOG(LogGCPoisonStatus, Log, TEXT("[GameplayCue_Poison] Root에 PostProcess 부착 (BlendRadius 사용)"));
#endif
		}
	}
}
#pragma endregion

#pragma region CleanupPostProcess
void AGC_Poison_Status::CleanupPostProcess()
{
	if (PostProcessComponent)
	{// 파괴하지 않고 재사용 가능, GameplayCue 재진입 시 안전
		PostProcessComponent->bEnabled = false;
		PostProcessComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	// 타이머 정리
	if (FadeTimerHandle.IsValid())
	{// 타이머 유실 방지 및 GC 종료 타이밍 꼬임 방지
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
	}
}
#pragma endregion

#pragma region IsLocalPlayerTarget
bool AGC_Poison_Status::IsLocalPlayerTarget(AActor* Target) const
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
#pragma endregion