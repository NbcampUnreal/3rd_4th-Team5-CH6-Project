// GameplayCue_Status_Poisoned.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCue_Status_Poisoned.generated.h"

/*
	독 상태 시각 효과를 처리하는 GameplayCue
	Duration 동안 초록색 비네팅과 카메라 잔상 효과를 적용
	로컬 플레이어의 카메라에만 PostProcess 적용
*/
UCLASS()
class TINYSURVIVOR_API AGameplayCue_Status_Poisoned : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:
	AGameplayCue_Status_Poisoned();
	
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

private:
	UPROPERTY()
	class UPostProcessComponent* PostProcessComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FLinearColor VignetteColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // 초록색
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float VignetteIntensity = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ChromaticAberration = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float MotionBlurAmount = 0.5f;
	
	// Fade 관련 변수
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Fade")
	float FadeInDuration = 0.5f;  // Fade In 시간 (초)
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Fade")
	float FadeOutDuration = 0.8f; // Fade Out 시간 (초)
	
	FTimerHandle FadeTimerHandle;
	float FadeElapsedTime = 0.0f;
	bool bIsFadingIn = false;
	bool bIsFadingOut = false;
	
	UFUNCTION()
	void UpdateFade();
	
	void StartFadeIn();
	void StartFadeOut();
	
	void SetupPostProcess(AActor* Target);
	void CleanupPostProcess();
	
	// 로컬 플레이어 체크 헬퍼 함수
	bool IsLocalPlayerTarget(AActor* Target) const;
};
