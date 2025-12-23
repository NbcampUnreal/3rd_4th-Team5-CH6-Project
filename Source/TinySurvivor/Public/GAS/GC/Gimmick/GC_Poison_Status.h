// AGC_Poison_Status.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_Poison_Status.generated.h"

/*
	독 상태 시각 효과를 처리하는 GameplayCue
	
	- 로컬 플레이어의 카메라에만 PostProcess 적용
	
	- 독 상태 지속 동안 초록색 톤의 PostProcess 효과 적용
	  (비네팅, 색조 변화, Bloom, MotionBlur, DOF 등으로 중독감 표현)
	- 효과 시작 시 Fade In으로 부드럽게 활성화
	- 효과 종료 시 Fade Out 후 PostProcess 정리
*/
UCLASS()
class TINYSURVIVOR_API AGC_Poison_Status : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

#pragma region AGC_Poison_Status
public:
	/*
		생성자
		- GameplayCueTag 명시적 지정으로 안정성 확보
		- PostProcessComponent 생성 및 초기 설정
	*/
	AGC_Poison_Status();
#pragma endregion
	
#pragma region OnActive
public:
	/*
		GameplayCue가 활성화될 때 호출되는 함수
		- 로컬 플레이어 대상만 처리
		- 로컬 플레이어가 아닌 경우, PostProcess 적용을 건너뜀
		- PostProcessComponent 초기화 및 설정(SetupPostProcess)
		- 페이드 인(StartFadeIn) 시작
		- 항상 true 반환
	*/
	virtual bool OnActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region WhileActive
public:
	/*
		GameplayCue가 활성화되어 있는 동안 호출되는 함수
		- 현재는 별도의 주기적 처리 없이 항상 true 반환
		- 활성 상태 유지 여부를 판단하거나 주기적 효과를 추가할 때 확장 가능
	*/
	virtual bool WhileActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region OnRemove
public:
	/*
		GameplayCue가 종료될 때 호출되는 함수
		- 로컬 플레이어인 Target만 처리
		- 페이드 아웃(StartFadeOut) 시작
		- 필요 시 PostProcess 정리(CleanupPostProcess) 가능
		- 항상 true 반환
	*/
	virtual bool OnRemove_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region PostProcessComponent
private:
	UPROPERTY()
	class UPostProcessComponent* PostProcessComponent;
#pragma endregion
	
#pragma region EffectSettings
private:
	// 비네팅 색상 및 강도
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings", meta = (AllowPrivateAccess = "true"))
	FLinearColor VignetteColor =
		FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // 초록색
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings", meta = (AllowPrivateAccess = "true"))
	float VignetteIntensity = 1.0f;
	
	// 색상 보정
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Color", meta = (AllowPrivateAccess = "true"))
	FVector4 ColorSaturation =
		FVector4(0.6f, 0.8f, 0.65f, 1.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Color", meta = (AllowPrivateAccess = "true"))
	FVector4 ColorGain =
		FVector4(0.85f, 0.95f, 0.88f, 1.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Color", meta = (AllowPrivateAccess = "true"))
	FVector4 ColorOffset =
		FVector4(-0.05f, 0.02f, -0.03f, 0.0f);
	
	// 크로매틱 애버레이션
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings", meta = (AllowPrivateAccess = "true"))
	float ChromaticAberration = 3.0f;
	
	// 모션 블러
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|MotionBlur", meta = (AllowPrivateAccess = "true"))
	float MotionBlurAmount = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|MotionBlur", meta = (AllowPrivateAccess = "true"))
	float MotionBlurMax = 30.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|MotionBlur", meta = (AllowPrivateAccess = "true"))
	float MotionBlurPerObjectSize = 2.0f;
	
	// Bloom
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Bloom", meta = (AllowPrivateAccess = "true"))
	float BloomIntensity = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Bloom", meta = (AllowPrivateAccess = "true"))
	float BloomThreshold = -1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Bloom", meta = (AllowPrivateAccess = "true"))
	FLinearColor Bloom1Tint =
		FLinearColor(0.6f, 1.0f, 0.6f, 1.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Bloom", meta = (AllowPrivateAccess = "true"))
	FLinearColor Bloom2Tint =
		FLinearColor(0.5f, 1.0f, 0.5f, 1.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Bloom", meta = (AllowPrivateAccess = "true"))
	FLinearColor Bloom3Tint =
		FLinearColor(0.5f, 0.9f, 0.5f, 1.0f);
	
	// Depth of Field
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|DOF", meta = (AllowPrivateAccess = "true"))
	float DepthOfFieldFstop = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|DOF", meta = (AllowPrivateAccess = "true"))
	float DepthOfFieldMinFstop = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|DOF", meta = (AllowPrivateAccess = "true"))
	int32 DepthOfFieldBladeCount = 8;
	
	// Film settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Film", meta = (AllowPrivateAccess = "true"))
	float FilmSlope = 0.9f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Film", meta = (AllowPrivateAccess = "true"))
	float FilmToe = 0.6f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Film", meta = (AllowPrivateAccess = "true"))
	float FilmShoulder = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Film", meta = (AllowPrivateAccess = "true"))
	float FilmBlackClip = 0.05f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		Category = "Effect Settings|Film", meta = (AllowPrivateAccess = "true"))
	float FilmWhiteClip = 0.95f;
#pragma endregion
	
#pragma region FadeVariables
private:
	// Fade 관련 변수
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings|Fade")
	float FadeInDuration = 0.5f; // Fade In 시간 (초)
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings|Fade")
	float FadeOutDuration = 0.8f; // Fade Out 시간 (초)
	
	FTimerHandle FadeTimerHandle;
	float FadeElapsedTime = 0.0f;
	bool bIsFadingIn = false;
	bool bIsFadingOut = false;
#pragma endregion
	
#pragma region UpdateFade
private:
	/*
		화면 효과를 부드럽게 나타내고 사라지게(Fade In/Out) 하기 위한 업데이트 함수
		
		동작 흐름:
			1. PostProcessComponent 존재 여부 확인.
			2. 시간 누적 (FadeElapsedTime)으로 진행 정도 계산.
			3. bIsFadingIn이면 BlendWeight 증가 → 화면 효과 점진적 표시.
			4. bIsFadingOut이면 BlendWeight 감소 → 화면 효과 점진적 제거.
			5. 완료 시 타이머 정리 + 필요 시 PostProcess 정리.
		
		특징: Ease In-Out 곡선 적용, FPS 독립적 계산, Fade 중 깜빡임 방지.
	*/
	UFUNCTION()
	void UpdateFade();
#pragma endregion
	
#pragma region StartFadeIn
private:
	/*
		Fade In 시작
		
		동작 흐름:
			1. PostProcessComponent 존재 여부 확인
			2. 페이드 상태 플래그 설정 (Fading In, Fading Out 초기화)
			3. 경과 시간 초기화
			4. 60fps 주기로 UpdateFade() 타이머 시작
			
		특징: 타이머를 이용한 지속 업데이트 방식
	*/
	void StartFadeIn();
#pragma endregion
	
#pragma region StartFadeOut
private:
	/*
		Fade Out 시작
		
		동작 흐름:
			1. PostProcessComponent 존재 여부 확인
			2. 페이드 상태 플래그 설정 (Fading Out, Fading In 초기화)
			3. 경과 시간 초기화
			4. 60fps 주기로 UpdateFade() 타이머 시작
			
		특징: 타이머를 이용한 지속 업데이트 방식
	*/
	void StartFadeOut();
#pragma endregion
	
#pragma region SetupPostProcess
private:
	/*
		독 상태 효과를 위한 PostProcessComponent를 초기화하고 설정
		- 비네팅, 색조 보정, 모션 블러, 블룸, DOF, 필름 톤 등 다양한 시각 효과 적용
		- BlendWeight 0으로 초기화하여 페이드 인 준비
		- Target이 캐릭터라면 카메라 또는 RootComponent에 PostProcess 부착
		- BlendRadius 사용 가능 (RootComponent 부착 시 캐릭터 주변만 적용)
	*/
	void SetupPostProcess(AActor* Target);
#pragma endregion
	
#pragma region CleanupPostProcess
private:
	/*
		포이즌 상태 효과용 PostProcessComponent를 정리
		- PostProcessComponent 비활성화 및 부모에서 분리
		- Fade 타이머 종료
		- 재사용 가능하도록 파괴하지 않음
		- GameplayCue 재진입 시 안전하게 호출 가능
	*/
	void CleanupPostProcess();
#pragma endregion
	
#pragma region IsLocalPlayerTarget
private:
	/*
		지정된 Target이 로컬 플레이어인지 확인
		- 로컬 플레이어가 아니면 false 반환
		- 로컬 플레이어에만 적용되는 효과 구분 등에 사용
	*/
	bool IsLocalPlayerTarget(AActor* Target) const;
#pragma endregion
};