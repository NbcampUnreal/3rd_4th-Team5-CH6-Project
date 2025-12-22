// GC_ElectricShock_Status.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_ElectricShock_Status.generated.h"

// 전방 선언
class UPostProcessComponent;
class APlayerController;
class UCameraShakeBase;

/*
	전기 감전 시각 효과를 처리하는 GameplayCue
	
	- 로컬 플레이어의 카메라에만 PostProcess 적용
	
	- 짧은 시간 동안 전기 섬광 기반 화면 효과 적용
	  (깜빡이는 비네팅, 색조 변화, Bloom, 색수차, 노이즈 등)
	- Sin 파형 기반 Flicker + 시간에 따른 감쇠로 전기 충격 연출
	- 감전 순간 카메라 셰이크를 함께 재생
*/
UCLASS()
class TINYSURVIVOR_API AGC_ElectricShock_Status : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
#pragma region AGC_ElectricShock_Status
public:
	/*
		생성자
		- GameplayCueTag 명시적 지정으로 안정성 확보
		- Actor Tick 비활성화
		- 내부 변수 초기화
	*/
	AGC_ElectricShock_Status();
#pragma endregion
	
#pragma region OnActive
protected:
	/*
		GameplayCue가 활성화될 때 호출되는 함수
		
		동작 흐름:
			1. InitializeForLocalPlayer -> 로컬 플레이어인지 확인
			2. CreatePostProcessComponent -> 화면 효과용 컴포넌트 생성
			3. StartCameraShake -> 카메라 셰이크 재생
			4. 타이머로 UpdateEffect 반복 호출 (0.016s, 60fps)
	*/
	virtual bool OnActive_Implementation(
	AActor* MyTarget,
	const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region OnRemove
protected:
	/*
		GameplayCue가 종료될 때 호출되는 함수
		- CleanupEffect() 호출 -> PostProcess 및 타이머 정리
	*/
	virtual bool OnRemove_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
#pragma endregion
	
#pragma region EffectSettings
private:
	//===================================
	// 화면 효과 설정
	//===================================
	// 효과 지속 시간
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "5.0"))
	float EffectDuration = 0.7f;
	
	// 화면 깜빡임 주파수 (Hz)
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true", ClampMin = "5.0", ClampMax = "50.0"))
	float FlickerFrequency = 15.0f;
	
	// 최대 깜빡임 강도
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0"))
	float MaxFlickerIntensity = 0.9f;
	
	// 전기 색상 (청백색)
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true"))
	FLinearColor ElectricColor =
		FLinearColor(0.6f,0.85f, 1.0f, 1.0f);
	
	// Vignette 최대 강도
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "3.0"))
	float MaxVignetteIntensity = 1.5f;
	
	// Bloom 추가 강도
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "5.0"))
	float BloomBoost = 2.0f;
	
	// Chromatic Aberration 강도
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "10.0"))
	float ChromaticAberration = 4.0f;
	
	// Film Grain 강도
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings",
		meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "2.0"))
	float FilmGrainStrength = 1.0f;
	
	//===================================
	// 카메라 셰이크
	//===================================
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings|CameraShake")
	TSubclassOf<class UCameraShakeBase> ElectricCameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect Settings|CameraShake",
		meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float ShakeScale = 2.0f;
#pragma endregion
	
#pragma region Variables
private:
	//===================================
	// 내부 변수
	//===================================
	
	// PostProcess 컴포넌트 (동적 생성)
	UPROPERTY()
	UPostProcessComponent* PostProcessComp;
	
	// 캐싱된 플레이어 컨트롤러
	UPROPERTY()
	APlayerController* CachedPlayerController;
	
	// 효과 경과 시간
	float EffectElapsedTime;
	
	// 업데이트 타이머
	FTimerHandle UpdateTimerHandle;
#pragma endregion
	
#pragma region InitializeForLocalPlayer
private:
	/*
		로컬 플레이어 확인 및 초기화
		
		로컬 플레이어만 효과 적용:
			1. TargetActor가 APawn인지 확인
			2. TargetActor를 APawn으로 캐스팅
			3. Pawn의 Controller를 APlayerController로 캐스팅
			4. IsLocalController() 확인
			5. 성공 시 CachedPlayerController에 저장, EffectElapsedTime 초기화
		
		특징: 서버/다른 클라이언트에서는 적용되지 않도록 로컬 플레이어만 처리
	*/
	bool InitializeForLocalPlayer(AActor* TargetActor);
#pragma endregion
	
#pragma region CreatePostProcessComponent
private:
	/*
		PostProcessComponent 생성 및 초기 설정
		
		동작 흐름:
			1. CachedPlayerController 유효성 확인
			2. NewObject<UPostProcessComponent>로 컴포넌트 생성
			3. RegisterComponent 호출
			4. bUnbound = true, Priority = 100 설정
			5. PostProcessSettings 초기값 설정
			
		특징: 동적 생성, 다른 효과와 충돌 방지, 전체 화면 적용
	*/
	void CreatePostProcessComponent();
#pragma endregion
	
#pragma region UpdateEffect
private:
	/*
		매 프레임 감전 효과 업데이트
		
		동작 흐름:
			1. PostProcessComp 및 CachedPlayerController 유효성 확인
			2. EffectElapsedTime 증가 (0.016초)
			3. 지속시간 초과 시 CleanupEffect 호출
			4. 정규화 시간 계산 (0~1)
			5. Sin 기반 FlickerValue 계산 → 0~1 범위
			6. DecayFactor 계산 (시간 감쇠)
			7. FinalIntensity 계산
			8. PostProcessSettings 업데이트
			9. MarkRenderStateDirty() 호출
			
		특징: Sin + 시간 감쇠 기반 자연스러운 깜빡임, 실시간 PostProcess 업데이트
	*/
	UFUNCTION()
	void UpdateEffect();
#pragma endregion
	
#pragma region CleanupEffect
private:
	/*
		감전 효과 정리 및 제거
		
		동작 흐름:
			1. 타이머 Clear 및 Invalidate
			2. PostProcessComp Unregister 및 DestroyComponent
			3. 내부 변수 초기화
			
		특징: 안전한 메모리 정리, 타이머 종료, PostProcess 제거
	*/
	void CleanupEffect();
#pragma endregion
	
#pragma region StartCameraShake
private:
	/*
		카메라 셰이크 시작
		
		동작 흐름:
			1. CachedPlayerController와 ElectricCameraShake 유효성 확인
			2. ClientStartCameraShake 호출, ShakeScale 적용
			
		특징: 전기 감전 느낌을 물리적 반응으로 표현, 로컬 플레이어 전용
	*/
	void StartCameraShake();
#pragma endregion
};